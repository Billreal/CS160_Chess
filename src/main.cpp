#include <iostream>
#include <cmath>
// #define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <string>
#include <fstream>
#include <filesystem>
#include <vector>
// #include "./../include/background.h"
#include "./../include/color.h"
#include "./../include/colorScheme.h"
#include "./../include/board.h"
#include "./../include/coordinate.h"
#include "./../include/button.h"
#include "./../include/nanosvg.h"
#include "./../include/nanosvgrast.h"
#include "./../include/gameStateManager.h"
#include "./../include/popup.h"
#include "./../include/sound.h"
#include "./../include/music.h"
using std::cerr;
using std::cout;

SDL_Renderer *renderer;
SDL_Window *window;
SDL_Surface *winSurface;

const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 1000;
const double SVG_SCALE = 1;
const int FONT_SIZE = 32;
const int TOP_MARGIN = 160;
const int BOTOTM_MARGIN = 80;
const int SIDE_MARGIN = 80;
const int SIDE_LENGTH = 80;

int demoGameMode = 0;
int demoGameDifficulty = 0;

void renderText(SDL_Renderer *renderer, TTF_Font *font, const std::string &text, SDL_Color textColor, SDL_Rect rect)
{
    SDL_Surface *textSurface = TTF_RenderText_Blended(font, text.c_str(), textColor);
    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    int textWidth = textSurface->w;
    int textHeight = textSurface->h;
    SDL_FreeSurface(textSurface);
    SDL_Rect textRect = {rect.x + (rect.w - textWidth) / 2, rect.y + (rect.h - textHeight) / 2, textWidth, textHeight};
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
    SDL_DestroyTexture(textTexture);
}

SDL_Texture *loadTexture(const std::string &path)
{
    SDL_Surface *surface = IMG_Load(path.c_str());
    // Check if surface is loaded
    if (!surface)
    {
        SDL_Log("Failed to load texture %s: %s", path.c_str(), SDL_GetError());
        return nullptr;
    }
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

SDL_Texture *loadSVGTexture(std::string filePath, int width, int height, double scale)
{
    struct NSVGimage *image = nsvgParseFromFile(filePath.c_str(), "px", 96);
    if (!image)
    {
        printf("Failed to load SVG file.\n");
        return nullptr;
    }

    // Rasterize SVG
    struct NSVGrasterizer *rast = nsvgCreateRasterizer();
    unsigned char *imageData = (unsigned char *)malloc(width * height * 10); // RGBA buffer
    nsvgRasterize(rast, image, 0, 0, scale, imageData, width, height, width * 4);

    // Create SDL surface and texture
    SDL_Surface *surface = SDL_CreateRGBSurfaceFrom(
        imageData, width, height, 32, width * 4,
        0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

    // Cleanup
    SDL_FreeSurface(surface);
    free(imageData);
    nsvgDeleteRasterizer(rast);
    nsvgDelete(image);

    return texture;
}

void deleteSave(int id)
{
    std::string saveFilePath = "saves/save_0" + std::to_string(id) + ".txt";
    std::ofstream saveFile;
    saveFile.open(saveFilePath);
    saveFile << "";
    std::cerr << "Deleted save file: " << saveFilePath << "\n";
    saveFile.close();
}
void saveGame(Board &board, const std::string &filename)
{
    std::ofstream saveFile;
    saveFile.open(filename);
    if (!saveFile)
    {
        // cerr << "Failed to open file for saving: " << filename << "\n";
        return;
    }
    saveFile << board.getFen();

    saveFile.close();
}

void loadGame(Board &board, const std::string &filename)
{
    std::ifstream saveFile(filename);
    std::string FEN;
    if (!saveFile)
    {
        // cerr << "Failed to open file for loading: " << filename << "\n";
        return;
    }

    if (!std::getline(saveFile, FEN))
    {
        // cerr << "Failed to read from file: " << filename << "\n";
        saveFile.close();
        return;
    }
    demoGameMode = 0;
    demoGameDifficulty = 1;
    saveFile >> demoGameMode;
    if (demoGameMode)
        saveFile >> demoGameDifficulty;
    board.updateFen(FEN);
    saveFile.close();
}
bool loadGame(Board &board, const std::string &filename, bool &isSinglePlayer, Communicator &communicator)
{
    std::ifstream saveFile(filename);
    std::string FEN;
    if (!saveFile)
    {
        cerr << "Failed to open file for loading: " << filename << "\n";
        return false;
    }

    if (!std::getline(saveFile, FEN))
    {
        cerr << "Failed to read from file: " << filename << "\n";
        saveFile.close();
        return false;
    }

    bool tmp = 0;
    if (!saveFile.eof())
        saveFile >> tmp;
    isSinglePlayer = tmp;
    std::cerr << "isSinglePlayer is: " << tmp << "\n";
    if (tmp)
    {
        isSinglePlayer = true;
        int difficulty = 1;
        saveFile >> difficulty;
        switch (difficulty)
        {
        case 1:
            communicator.setDifficulty(Difficulty::EASY);
            break;

        case 2:
            communicator.setDifficulty(Difficulty::MEDIUM);
            break;
        case 3:
            communicator.setDifficulty(Difficulty::HARD);
            break;
        default:
            communicator.setDifficulty(Difficulty::EASY);
            break;
        }
        std::cerr << "Set difficulty to " << (1 <= difficulty && difficulty <= 3) ? difficulty : 1;
    }
    else
    {
        isSinglePlayer = false;
        communicator.setDifficulty(Difficulty::EASY);
        std::cerr << "For clarification, in multiplayer mode difficulty option has no effect\n";
    }
    // std::cerr << FEN << "\n";
    board.updateFen(FEN);

    saveFile.close();
    return true;
}

void saveGame(Board &board, int id, bool &isSinglePlayer, Communicator &communicator)
{
    std::string saveId = std::to_string(id);
    while (saveId.length() < 2)
        saveId = '0' + saveId;
    std::string savePath = "saves/save_" + saveId + ".txt";
    std::ofstream saveFile;
    saveFile.open(savePath);
    std::string FEN = board.boardToFen();
    saveFile << FEN << std::endl;
    if (isSinglePlayer)
    {
        int difficulty = 1;
        if (communicator.getDifficulty() == Difficulty::MEDIUM)
            difficulty = 2;
        if (communicator.getDifficulty() == Difficulty::HARD)
            difficulty = 3;
        saveFile << isSinglePlayer << std::endl
                 << difficulty << std::endl;
    }
    std::cerr << "Saved file to " << savePath << std::endl;
    saveFile.close();
    // std::cerr << "Closing finished\n";
}
std::vector<std::string> getSaveFiles(const std::string &directory)
{
    std::vector<std::string> files;
    for (const auto &entry : std::filesystem::directory_iterator(directory))
    {
        if (entry.is_regular_file())
        {
            files.push_back(entry.path().string());
        }
    }
    return files;
}

void initSaveFiles()
{
    std::ifstream testOpen;
    std::ofstream file;
    for (int i = 1; i <= 6; i++)
    {
        std::string path = "./saves/save_0" + std::to_string(i) + ".txt";
        testOpen.open(path);
        if (!testOpen)
        {
            testOpen.close();
            file.open(path);
            file.close();
        }
        else
            testOpen.close();
    }
}
enum GUI_State
{
    START,
    SAVE,
    LOAD,
    GAME,
    SETTINGS,
    MENU,
};

struct ThemeList
{
    Button boardButton;
    Button pieceButton;
    colorRGBA primaryColor;
    colorRGBA secondaryColor;
    std::string pieceTheme;
};

struct DifficultyList
{
    Button button;
    Difficulty difficulty;
};

int main(int argc, char *args[])
{
    // ! Temporary variable, will change in later version
    bool running = true;
    bool isToHighlightMove = false;
    // ! End of temporary variable

    // Initializing main components
    GUI_State isOn = START;
    GameStateManager gameState;
    renderer = NULL;
    window = NULL;

    // Error handling for SDL_Init
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
        return -1;
    }
    window = SDL_CreateWindow("CS160_Chess",
                              SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED,
                              SCREEN_WIDTH,
                              SCREEN_HEIGHT,
                              SDL_WINDOW_SHOWN);

    // Error handling for window initialization
    if (!window)
    {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    // Yet another error handling
    if (!renderer)
    {
        SDL_Log("Failed to create renderer: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // Initialize SDL_ttf
    TTF_Init();
    if (TTF_Init() == -1)
    {
        SDL_Log("Failed to initialize SDL_ttf: %s", TTF_GetError());
        SDL_Quit();
        return -1;
    }

    TTF_Font *font = TTF_OpenFont("./font/Recursive/static/Recursive_Casual-Light.ttf", 20);
    // TTF_Font *font = TTF_OpenFont("C:/Windows/Fonts/arial.ttf", 20);
    if (!font)
    {
        SDL_Log("Failed to load font: %s", TTF_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return -1;
    }

    if (SDL_Init(SDL_INIT_AUDIO) < 0)
    {
        SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
        return -1;
    }

    Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 1, 4096);

    initSaveFiles();
    // ! For disabling communicator
    bool isCommunicatorEnabled = true;
    bool isEndgameSoundPlayed = false;
    // ! -----------------------------
    Board board(renderer, modernPrimary, modernSecondary, bgColor);
    Communicator communicator(isCommunicatorEnabled);
    Music backgroundMusic("assets/effects/fallen.mp3");
    Sound soundboard;
    std::cerr << "Done setting volume\n";
    std::cerr << "Done setting volume\n";
    soundboard.loadSoundEffect(SoundEffect::PICKUP, "assets/effects/notify.mp3");
    soundboard.loadSoundEffect(SoundEffect::CAPTURE, "assets/effects/capture.mp3");
    soundboard.loadSoundEffect(SoundEffect::MOVE, "assets/effects/move-self.mp3");
    soundboard.loadSoundEffect(SoundEffect::GAMEOVER, "assets/effects/game-end.mp3");
    soundboard.loadSoundEffect(SoundEffect::ILLEGAL, "assets/effects/illegal.mp3");
    soundboard.loadSoundEffect(SoundEffect::PROMOTION, "assets/effects/promote.mp3");
    soundboard.loadSoundEffect(SoundEffect::CHECK, "assets/effects/move-check.mp3");
    soundboard.setVolume(backgroundMusic.getVolume());
    std::cerr << "Done starting soundeffect and music" << std::endl;
    // Music backgroundMusic("assets/itsgoingdownnow.mp3");
    backgroundMusic.play();
    communicator.init();
    communicator.startNewGame();
    std::cerr << "Done initializing communicator\n";
    // Handling SDL_events
    SDL_Event event;

    // Chessboard rendering

    // chessPieces chess(QUEEN, WHITE, 4, 4);
    // auto dbg = chess.listAllMove();
    // for (auto x : dbg)
    //     cerr << x.getX() << " " << x.getY() << "\n";

    // Game States
    bool isLeftMouseHolding = false;
    bool isSinglePlayer = false;
    int currentMoveColor = WHITE;
    bool renderOnce = false;
    bool needPresent = false;
    bool openGameOverPopup = false;

    Coordinate prevCoordinate(-1, -1);
    char pickedPiece = ' ';
    Coordinate pickedPlace(-1, -1);

    // board.setColor(modernPrimary, modernSecondary);
    board.setCommunicator(&communicator);

    vector<Coordinate> possibleMoves;
    vector<Coordinate> possibleCaptures;

    bool isEnded = false;

    //// Initialize start menu
    bool isToRenderChooseDifficulty = false;
    // Initizalize logo
    const int logoWidth = 500;
    const int logoHeight = 177;
    SDL_Rect logoInfos = {(SCREEN_WIDTH - logoWidth) / 2, 120, logoWidth, logoHeight};
    SDL_Texture *logoTexture = loadTexture("./assets/logo.png");
    Popup difficultyChoose(renderer, 300, 300);

    // Initialize buttons
    const int X_COLUMN_BUTTON[2] = {700, 1000};
    const int Y_ROW_BUTTON[7] = {80, 180, 280, 380, 480, 580, 680};
    const int startMenuBtnWidth = 500;
    const int startMenuBtnHeight = 100;
    SDL_Color startMenuBtnColor = {118, 150, 85, 255};
    SDL_Color loadMenuBtnColor = {38, 37, 33, 1};
    SDL_Color white = {255, 255, 255, 255};
    Button startBtn2P(renderer, 240, startMenuBtnHeight, startMenuBtnColor, white, "2 Player", font);
    Button startBtnAI(renderer, 240, startMenuBtnHeight, startMenuBtnColor, white, "vs AI", font);
    Button loadBtn(renderer, startMenuBtnWidth, startMenuBtnHeight, startMenuBtnColor, white, "Load", font);
    Button quitBtn(renderer, startMenuBtnWidth, startMenuBtnHeight, startMenuBtnColor, white, "Quit", font);

    //// Initialize load menu
    SDL_Rect loadInfos = {60, 80, 900, 850};
    SDL_Texture *loadMenuTexture = loadTexture("./assets/load.png");
    SDL_Texture *saveMenuTexture = loadTexture("./assets/save.png");
    TTF_Font *loadMenuFont = TTF_OpenFont("./font/Recursive/static/Recursive_Casual-Light.ttf", 24);
    TTF_Font *loadMenuFontSmall = TTF_OpenFont("./font/Recursive/static/Recursive_Casual-Light.ttf", 18);
    // TTF_Font *loadMenuFont = TTF_OpenFont("C:/Windows/Fonts/arial.ttf", 24);
    if (!loadMenuFont || !loadMenuFontSmall)
    {
        SDL_Log("Failed to load Load_menu font: %s", TTF_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return -1;
    }
    SDL_Rect loadMenuSeperateLine = {360, 240, 5, 600};

    Board demoBoard(renderer, modernPrimary, modernSecondary, bgColor);

    demoBoard.setBoardSize(50);
    demoBoard.setMargin(480, 240);

    bool hoverLoading = false;
    std::vector<std::string> files = getSaveFiles("./saves");
    std::vector<Button> loadFileBtns;
    std::vector<Button> saveFileBtns;
    std::vector<Button> deleteSaveFileBtns;
    for (int i = 0; i < files.size(); i++)
    {
        std::cerr << files[i] << "\n";
        string name = "";
        for (int j = 0; j < files[i].length() - 3; j++)
        {
            if (files[i].substr(j, 4) == ".txt")
            {
                name = files[i].substr(j - 7, 7);
                // // std::cerr << name << "\n";
                break;
            }
        }
        loadFileBtns.push_back(Button(renderer, 250, 60, loadMenuBtnColor, white, name, loadMenuFont));
        saveFileBtns.push_back(Button(renderer, 250, 60, loadMenuBtnColor, white, name, loadMenuFont));
        deleteSaveFileBtns.push_back(Button(renderer, 60, 60, loadMenuBtnColor, white, " ", loadMenuFont));
    }

    Button backBtn(renderer, 120, 50, startMenuBtnColor, white, "Back", loadMenuFont);
    GUI_State previousState;
    //// Initialize game GUI

    // Initialize right panel
    const int panelMargin = 60;
    const int rightPanelWidth = 180;
    const int rightPanelHeight = 640;
    SDL_Rect rightPanelInfos = {SCREEN_WIDTH - panelMargin - rightPanelWidth, TOP_MARGIN, rightPanelWidth, rightPanelHeight};
    SDL_Texture *rightPanelTexture = loadTexture("./assets/right_panel.svg");

    // Initialize bottom panel
    const int bottomPanelWidth = 640;
    const int bottomPanelHeight = 70;
    SDL_Rect bottomPanelInfos = {SIDE_MARGIN, SCREEN_HEIGHT - panelMargin - bottomPanelHeight, bottomPanelWidth, bottomPanelHeight};
    SDL_Texture *bottomPanelTexture = loadTexture("./assets/bottom_panel.svg");

    // Initialize turn indicator
    const int turnIndicatorLogoWidth = 300;
    const int turnIndicatorLogoHeight = 100;
    const int turnIndicatorWidth = 446;
    const int isTurnIndicatorHeight = 50;
    const int notTurnIndicatorHeight = 25;
    SDL_Rect isWhiteTurnIndicatorInfos = {SIDE_MARGIN, 0, turnIndicatorWidth, isTurnIndicatorHeight};
    SDL_Rect isBlackTurnIndicatorInfos = {SCREEN_WIDTH - SIDE_MARGIN - turnIndicatorWidth, 0, turnIndicatorWidth, isTurnIndicatorHeight};
    SDL_Rect notWhiteTurnIndicatorInfos = {SIDE_MARGIN, 0, turnIndicatorWidth, notTurnIndicatorHeight};
    SDL_Rect notBlackTurnIndicatorInfos = {SCREEN_WIDTH - SIDE_MARGIN - turnIndicatorWidth, 0, turnIndicatorWidth, notTurnIndicatorHeight};
    SDL_Rect turnIndicatorLogoInfos = {SCREEN_WIDTH / 2 - turnIndicatorLogoWidth / 2, 0, turnIndicatorLogoWidth, turnIndicatorLogoHeight};
    SDL_Texture *turnIndicatorLogoTexture = loadTexture("./assets/game_turn_indicator_logo.png");
    SDL_Texture *isWhiteTurnIndicatorTexture = loadTexture("./assets/is_white_turn.png");
    SDL_Texture *isBlackTurnIndicatorTexture = loadTexture("./assets/is_black_turn.png");
    SDL_Texture *notWhiteTurnIndicatorTexture = loadTexture("./assets/not_white_turn.png");
    SDL_Texture *notBlackTurnIndicatorTexture = loadTexture("./assets/not_black_turn.png");

    // Inintialize game buttons
    Button saveBtn(renderer, 120, 50, startMenuBtnColor, white, "Save", loadMenuFont);
    Button loadBtnInGame(renderer, 120, 50, startMenuBtnColor, white, "Load", loadMenuFont);
    Button settingBtn(renderer, 120, 50, startMenuBtnColor, white, "Setting", loadMenuFont);
    Button ingameBoardColorSwitchModern(renderer, 100, 50, startMenuBtnColor, white, "Modern", loadMenuFontSmall);
    Button ingameBoardColorSwitchFuturistic(renderer, 100, 50, startMenuBtnColor, white, "Futuristic", loadMenuFontSmall);
    Button ingameBoardColorSwitchClassic(renderer, 100, 50, startMenuBtnColor, white, "Classic", loadMenuFontSmall);

    Button ingamePieceColorSwitchModern(renderer, 100, 50, startMenuBtnColor, white, "Modern", loadMenuFontSmall);
    Button ingamePieceColorSwitchFuturistic(renderer, 100, 50, startMenuBtnColor, white, "Futuristic", loadMenuFontSmall);
    Button ingamePieceColorSwitchClassic(renderer, 100, 50, startMenuBtnColor, white, "Classic", loadMenuFontSmall);

    Button ingameDifficultySwitchEasy(renderer, 120, 50, startMenuBtnColor, white, "Easy", loadMenuFont);
    Button ingameDifficultySwitchMedium(renderer, 120, 50, startMenuBtnColor, white, "Medium", loadMenuFont);
    Button ingameDifficultySwitchHard(renderer, 120, 50, startMenuBtnColor, white, "Hard", loadMenuFont);

    vector<ThemeList> themeList = {{ingameBoardColorSwitchModern, ingamePieceColorSwitchModern, modernPrimary, modernSecondary, "modern"},
                                   {ingameBoardColorSwitchClassic, ingamePieceColorSwitchClassic, classicPrimary, classicSecondary, "classic"},
                                   {ingameBoardColorSwitchFuturistic, ingamePieceColorSwitchFuturistic, futuristicPrimary, futuristicSecondary, "futuristic"}};

    vector<DifficultyList> difficultyList = {{ingameDifficultySwitchEasy, Difficulty::EASY},
                                             {ingameDifficultySwitchMedium, Difficulty::MEDIUM},
                                             {ingameDifficultySwitchHard, Difficulty::HARD}};

    Button undoBtn(renderer, 34, 50, startMenuBtnColor, white, "", loadMenuFont);
    Button redoBtn(renderer, 34, 50, startMenuBtnColor, white, "", loadMenuFont);
    Button beginBtn(renderer, 59, 50, startMenuBtnColor, white, "", loadMenuFont);
    Button endBtn(renderer, 59, 50, startMenuBtnColor, white, "", loadMenuFont);

    int currentBoardThemeIndex = 0;
    int currentPieceThemeIndex = 0;
    int currentDifficultyIndex = 0;
    Button *currentBoardThemeButton = &themeList[currentBoardThemeIndex].boardButton;
    Button *currentPieceThemeButton = &themeList[currentPieceThemeIndex].pieceButton;
    Button *currentDifficultyButton = &difficultyList[currentDifficultyIndex].button;

    board.setColor(themeList[currentBoardThemeIndex].primaryColor, themeList[currentBoardThemeIndex].secondaryColor);
    board.setPieceTheme(themeList[currentPieceThemeIndex].pieceTheme);
    communicator.setDifficulty(difficultyList[currentDifficultyIndex].difficulty);
    bool isUnderPromotion = false;

    //// Initialize settings menu
    Button settingMuteMusic(renderer, 250, 50, startMenuBtnColor, white, "Mute Music", loadMenuFont);
    Button settingUnmuteMusic(renderer, 250, 50, startMenuBtnColor, white, "Unmute Music", loadMenuFont);
    Button settingIncreaseMusicVolume(renderer, 100, 50, startMenuBtnColor, white, "Increase", loadMenuFontSmall);
    Button settingDecreaseMusicVolume(renderer, 100, 50, startMenuBtnColor, white, "Decrease", loadMenuFontSmall);
    Button homeBtn(renderer, 250, 50, startMenuBtnColor, white, "Home", loadMenuFont);
    Button retryBtn(renderer, 250, 50, startMenuBtnColor, white, "Retry", loadMenuFont);

    SDL_Rect settingLogoSeperator = {(SCREEN_WIDTH - 250) / 2, TOP_MARGIN - 60, 250, 5};
    vector<SDL_Rect> volumeRect(16, {-1, -1, -1, -1});
    Button *currentMusicState = &settingMuteMusic;
    //// Popup handling
    Popup confirmationPopup(renderer, (SCREEN_WIDTH - 350) / 2, (SCREEN_HEIGHT - 350) / 2);
    bool isToRenderPopupSave = false;
    bool isToRenderPopupDelete = false;
    int saveFileId, deleteSaveFileId;

    //// Pseudo codes
    auto resetGameState = [&]()
    {
        prevCoordinate = Coordinate(-1, -1);
        pickedPiece = ' ';
        pickedPlace = Coordinate(-1, -1);
        currentMoveColor = WHITE;
        renderOnce = false;
        board.resetBoardState(isEnded);
        gameState.clear();
    };

    auto GameTurnIndicatorLoad = [&]()
    {
        if (currentMoveColor == WHITE)
        {
            SDL_RenderCopy(renderer, isWhiteTurnIndicatorTexture, NULL, &isWhiteTurnIndicatorInfos);
            SDL_RenderCopy(renderer, notBlackTurnIndicatorTexture, NULL, &notBlackTurnIndicatorInfos);
        }
        else
        {
            SDL_RenderCopy(renderer, isBlackTurnIndicatorTexture, NULL, &isBlackTurnIndicatorInfos);
            SDL_RenderCopy(renderer, notWhiteTurnIndicatorTexture, NULL, &notWhiteTurnIndicatorInfos);
        }
        SDL_RenderCopy(renderer, turnIndicatorLogoTexture, NULL, &turnIndicatorLogoInfos);
        // SDL_RenderPresent(renderer);
    };

    auto GameGUILoad = [&]()
    {
        // Render Panels
        SDL_RenderCopy(renderer, rightPanelTexture, NULL, &rightPanelInfos);
        SDL_RenderCopy(renderer, bottomPanelTexture, NULL, &bottomPanelInfos);

        // Render Buttons
        saveBtn.renderSVG("./assets/game_button.svg", SCREEN_WIDTH - (SIDE_MARGIN + 130), TOP_MARGIN + 40, SVG_SCALE);
        loadBtnInGame.renderSVG("./assets/game_button.svg", SCREEN_WIDTH - (SIDE_MARGIN + 130), TOP_MARGIN + 120, SVG_SCALE);
        // homeBtn.renderSVG("./assets/game_button.svg", SCREEN_WIDTH - (SIDE_MARGIN + 130), TOP_MARGIN + 200, SVG_SCALE);
        // retryBtn.renderSVG("./assets/game_button.svg", SCREEN_WIDTH - (SIDE_MARGIN + 130), TOP_MARGIN + 280, SVG_SCALE);
        settingBtn.renderSVG("./assets/game_button.svg", SCREEN_WIDTH - (SIDE_MARGIN + 130), TOP_MARGIN + 200, SVG_SCALE);
        // if (isSinglePlayer)
        //     currentDifficultyButton->renderSVG("./assets/game_button.svg", SCREEN_WIDTH - (SIDE_MARGIN + 130), TOP_MARGIN + 440, SVG_SCALE);

        undoBtn.renderPNG("./assets/undo.png", SIDE_MARGIN + bottomPanelWidth / 2 - 34 - 5, SCREEN_HEIGHT - panelMargin - bottomPanelHeight + 10);
        redoBtn.renderPNG("./assets/redo.png", SIDE_MARGIN + bottomPanelWidth / 2 + 5, SCREEN_HEIGHT - panelMargin - bottomPanelHeight + 10);
        beginBtn.renderPNG("./assets/begin.png", SIDE_MARGIN + bottomPanelWidth / 2 - 5 - 34 - 10 - 59, SCREEN_HEIGHT - panelMargin - bottomPanelHeight + 10);
        endBtn.renderPNG("./assets/end.png", SIDE_MARGIN + bottomPanelWidth / 2 + 5 + 34 + 10, SCREEN_HEIGHT - panelMargin - bottomPanelHeight + 10);
    };

    auto GameGUIButtonsHandling = [&]()
    {
        // currentBoardThemeButton->handleEvent(&event);
        // if (isSinglePlayer)
        //     currentDifficultyButton->handleEvent(&event);
        saveBtn.handleEvent(&event);
        loadBtnInGame.handleEvent(&event);
        // homeBtn.handleEvent(&event);
        // retryBtn.handleEvent(&event);
        settingBtn.handleEvent(&event);
        undoBtn.handleEvent(&event);
        redoBtn.handleEvent(&event);
        beginBtn.handleEvent(&event);
        endBtn.handleEvent(&event);
    };

    auto ResetColor = [&]()
    {
        currentBoardThemeIndex = 0;
        currentPieceThemeIndex = 0;
        currentBoardThemeButton->resetClicked();
        currentPieceThemeButton->resetClicked();
        currentBoardThemeButton = &themeList[currentBoardThemeIndex].boardButton;
        currentPieceThemeButton = &themeList[currentPieceThemeIndex].pieceButton;
        board.setColor(themeList[currentBoardThemeIndex].primaryColor, themeList[currentBoardThemeIndex].secondaryColor);
        board.setPieceTheme(themeList[currentPieceThemeIndex].pieceTheme);
        std::cerr << "Done resetting color\n";
    };

    auto ResetDifficulty = [&]()
    {
        currentDifficultyIndex = 0;
        currentDifficultyButton->resetClicked();
        currentDifficultyButton = &difficultyList[currentDifficultyIndex].button;
        communicator.setDifficulty(difficultyList[currentDifficultyIndex].difficulty);
        std::cerr << "Done reseting difficulty\n";
    };
    auto LoadMenuRefresh = [&]()
    {
        for (int i = 0; i < loadFileBtns.size(); i++)
        {
            loadFileBtns[i].resetHovered();
            loadFileBtns[i].resetClicked();
        }
        demoBoard.updateFen("8/8/8/8/8/8/8/8 w - - 0 0");
    };
    auto GameGUIButtonsClicked = [&]()
    {
        if (saveBtn.clicked())
        {
            SDL_Log("Save clicked!");
            previousState = isOn;
            isOn = SAVE;
            saveBtn.resetClicked(); // Reset button state
        }
        if (loadBtnInGame.clicked())
        {
            SDL_Log("Load clicked!");
            previousState = isOn;
            isOn = LOAD;
            loadBtnInGame.resetClicked(); // Reset button state
            LoadMenuRefresh();
        }
        // if (homeBtn.clicked())
        // {
        //     homeBtn.resetClicked(); // Reset button state
        //     SDL_Log("Home clicked!");
        //     isOn = GUI_State::START;
        //     ResetColor();
        //     ResetDifficulty();
        //     quitBtn.setSize(startMenuBtnWidth, startMenuBtnHeight);
        //     renderOnce = false;
        // }
        // if (retryBtn.clicked())
        // {
        //     SDL_Log("Retry clicked!");
        //     board.startNewGame();
        //     board.resetBoardState(isEnded);
        //     resetGameState();
        //     gameState.pushState(board.getFen());
        //     retryBtn.resetClicked(); // Reset button state
        // }
        // if (currentBoardThemeButton->clicked())
        // {
        //     SDL_Log("Theme clicked");
        //     currentBoardThemeButton->resetClicked(); // Reset button state
        //     // * To next color in circle
        //     currentBoardThemeIndex = (currentBoardThemeIndex + 1) % 3;
        //     currentBoardThemeButton = &themeList[currentBoardThemeIndex].button;
        //     board.setColor(themeList[currentBoardThemeIndex].primaryColor, themeList[currentBoardThemeIndex].secondaryColor);

        //     renderOnce = false;
        // }
        if (currentDifficultyButton->clicked() && isSinglePlayer)
        {
            SDL_Log("Difficulty Clicked");
            currentDifficultyButton->resetClicked();
            currentDifficultyIndex = (currentDifficultyIndex + 1) % 3;
            currentDifficultyButton = &difficultyList[currentDifficultyIndex].button;
            communicator.setDifficulty(difficultyList[currentDifficultyIndex].difficulty);
            renderOnce = false;
        }
        if (settingBtn.clicked())
        {
            SDL_Log("Setting clicked!");
            isOn = SETTINGS;
            previousState = GUI_State::GAME;
            // for (auto &theme : themeList)
            // {
            // std::cerr << theme.button.getWidth() << " " << theme.button.getHeight() << "\n";
            // theme.boardButton.setSize(250, 50);
            // }
            quitBtn.setSize(250, 50);

            settingBtn.resetClicked(); // Reset button state
        }
        if (undoBtn.clicked())
        {
            SDL_Log("Undo clicked!");
            if (gameState.canUndo(isSinglePlayer))
            {
                std::string prevFen = gameState.undo(isSinglePlayer);
                board.updateFen(prevFen);
                board.resetBoardState(isEnded);

                currentMoveColor = board.getMoveColor();
                if (currentMoveColor)
                    std::cerr << "Black\n";
                else
                    std::cerr << "White\n";
                std::cerr << prevFen << "\n";
                renderOnce = false;
            }
            undoBtn.resetClicked(); // Reset button state
        }
        if (redoBtn.clicked())
        {
            SDL_Log("Redo clicked!");
            if (gameState.canRedo(isSinglePlayer))
            {
                std::string nextFen = gameState.redo(isSinglePlayer);
                board.updateFen(nextFen);
                board.resetBoardState(isEnded);

                currentMoveColor = board.getMoveColor();
                if (currentMoveColor)
                    std::cerr << "Black\n";
                else
                    std::cerr << "White\n";
                std::cerr << nextFen << "\n";
                renderOnce = false;
            }
            redoBtn.resetClicked(); // Reset button state
        }
        if (beginBtn.clicked())
        {
            SDL_Log("Start clicked!");
            std::string nextFen = gameState.startState();
            board.updateFen(nextFen);
            board.resetBoardState(isEnded);

            currentMoveColor = board.getMoveColor();
            if (currentMoveColor)
                std::cerr << "Black\n";
            else
                std::cerr << "White\n";
            std::cerr << nextFen << "\n";
            renderOnce = false;
            beginBtn.resetClicked(); // Reset button state
        }
        if (endBtn.clicked())
        {
            SDL_Log("End clicked!");
            std::string nextFen = gameState.finalState();
            board.updateFen(nextFen);
            board.resetBoardState(isEnded);

            currentMoveColor = board.getMoveColor();
            if (currentMoveColor)
                std::cerr << "Black\n";
            else
                std::cerr << "White\n";
            std::cerr << nextFen << "\n";
            renderOnce = false;
            endBtn.resetClicked(); // Reset button state
        }
    };

    auto GameBoardRender = [&]()
    {
        // Background color
        SDL_SetRenderDrawColor(renderer, bgColor.getR(), bgColor.getG(), bgColor.getB(), bgColor.getA());
        SDL_RenderClear(renderer);

        GameGUILoad();
        board.render();
    };

    // Main app
    while (running)
    {
        switch (isOn)
        {
        case START:
        {
            SDL_SetRenderDrawColor(renderer, 49, 46, 43, 1); // Black background
            SDL_RenderClear(renderer);

            while (SDL_PollEvent(&event) != 0)
            {
                if (event.type == SDL_QUIT)
                {
                    running = false;
                }

                // Handle button events
                startBtn2P.handleEvent(&event);
                startBtnAI.handleEvent(&event);
                loadBtn.handleEvent(&event);
                quitBtn.handleEvent(&event);
                if (!difficultyChoose.isClosed())
                    difficultyChoose.handleDiffultyEvent(&event);
            }

            // Render logo
            SDL_RenderCopy(renderer, logoTexture, NULL, &logoInfos);

            // Render button
            startBtn2P.renderSVG("./assets/start_button_small.svg", (SCREEN_WIDTH - 500) / 2, 350, SVG_SCALE);
            startBtnAI.renderSVG("./assets/start_button_small.svg", SCREEN_WIDTH / 2 + 10, 350, SVG_SCALE);
            loadBtn.renderSVG("./assets/start_button.svg", (SCREEN_WIDTH - startMenuBtnWidth) / 2, 500, SVG_SCALE);
            quitBtn.renderSVG("./assets/start_button.svg", (SCREEN_WIDTH - startMenuBtnWidth) / 2, 650, SVG_SCALE);

            if (!difficultyChoose.isClosed())
                difficultyChoose.renderDifficulty("Difficulty", 35);

            // Update screen
            SDL_RenderPresent(renderer);

            // Check if button was clicked
            if (startBtn2P.clicked())
            {
                SDL_Log("Start 2 Players game");
                board.startNewGame();
                board.resetBoardState(isEnded);
                resetGameState();
                gameState.pushState(board.getFen());
                isOn = GAME;
                isSinglePlayer = false;
                startBtn2P.resetClicked(); // Reset button state
                break;
            }
            if (startBtnAI.clicked())
            {
                SDL_Log("Start game with AI");
                isToRenderChooseDifficulty = true;
                difficultyChoose.open();
                startBtnAI.resetClicked(); // Reset button state
                break;
            }
            if (loadBtn.clicked())
            {
                SDL_Log("Button clicked!");
                previousState = isOn;
                isOn = LOAD;
                loadBtn.resetClicked(); // Reset button state
                LoadMenuRefresh();
            }
            if (quitBtn.clicked())
            {
                SDL_Log("Button clicked!");
                running = false;
                quitBtn.resetClicked(); // Reset button state
            }
            if (isToRenderChooseDifficulty && !difficultyChoose.isClosed())
            {
                difficultyChoose.handleDifficultyClicked();
                if (difficultyChoose.getDifficulty() != Difficulty::NONE)
                {
                    communicator.setDifficulty(difficultyChoose.getDifficulty());
                    std::cerr << "Difficulty set to " << (int)difficultyChoose.getDifficulty() << "\n";
                    board.startNewGame();
                    board.resetBoardState(isEnded);
                    resetGameState();
                    gameState.pushState(board.getFen());
                    isOn = GAME;
                    isSinglePlayer = true;
                    difficultyChoose.clearDifficulty();
                    difficultyChoose.close();
                }
            }

            break;
        }
        case LOAD:
        {
            SDL_SetRenderDrawColor(renderer, 49, 46, 43, 1); // Black background
            SDL_RenderClear(renderer);

            while (SDL_PollEvent(&event) != 0)
            {
                if (event.type == SDL_QUIT)
                {
                    running = false;
                    break;
                }
                for (int i = 0; i < loadFileBtns.size(); i++)
                {
                    loadFileBtns[i].handleEvent(&event);
                    // std::cerr << "Done handle " << i << "\n";
                }
                backBtn.handleEvent(&event);
            }

            // ? Handle success
            // Render demo board and infos
            demoBoard.renderFromFen();
            // std::cerr << demoBoard.getMoves() << "\n";
            if (demoBoard.getMoves() != "")
            {

                renderText(renderer, font, "Turn: " + demoBoard.getTurn(), white, {480, 700, 135, 30});
                renderText(renderer, font, "Move: " + demoBoard.getMoves(), white, {780, 700, 100, 30});

                if (demoGameMode)
                {
                    std::string difficultyLevel;
                    if (demoGameDifficulty == 1)
                        difficultyLevel = "Easy";
                    else if (demoGameDifficulty == 2)
                        difficultyLevel = "Medium";
                    else if (demoGameDifficulty == 3)
                        difficultyLevel = "Hard";
                    renderText(renderer, font, "Versus AI", white, {480, 780, 135, 30});
                    renderText(renderer, font, "Difficulty: " + difficultyLevel, white, {780, 780, 100, 30});
                }
                else
                {
                    renderText(renderer, font, "2-Player", white, {490, 780, 135, 30});
                }
            }
            // Render logo
            SDL_RenderCopy(renderer, loadMenuTexture, NULL, &loadInfos);
            SDL_SetRenderDrawColor(renderer, 238, 238, 210, 255);
            SDL_RenderFillRect(renderer, &loadMenuSeperateLine);

            // Render Buttons
            for (int i = 0; i < loadFileBtns.size(); i++)
            {
                loadFileBtns[i].render(60, 240 + i * 90);
            }
            backBtn.renderSVG("assets/game_button.svg", 800, 120, SVG_SCALE);
            SDL_RenderPresent(renderer);

            // Update screen
            hoverLoading = false;
            for (int i = 0; i < loadFileBtns.size(); i++)
            {
                if (loadFileBtns[i].hover())
                {
                    loadGame(demoBoard, files[i]);
                    hoverLoading = true;
                    loadFileBtns[i].updateColor(startMenuBtnColor);
                }
                else
                    loadFileBtns[i].updateColor(loadMenuBtnColor);
            }
            if (!hoverLoading)
                demoBoard.updateFen("8/8/8/8/8/8/8/8 w - - 0 0");

            // Handle button click
            if (backBtn.clicked())
            {
                backBtn.resetClicked();
                isOn = previousState;
                renderOnce = false;
                break;
            }
            for (int i = 0; i < loadFileBtns.size(); i++)
            {
                if (loadFileBtns[i].clicked())
                {
                    // std::cerr << "Button clicked!\n";
                    // if (!isSinglePlayer)
                    // loadGame(board, files[i]);
                    // else
                    loadFileBtns[i].resetClicked();
                    if (!loadGame(board, files[i], isSinglePlayer, communicator))
                        break;
                    resetGameState();
                    board.resetBoardState(isEnded);
                    gameState.clear();
                    gameState.pushState(board.getFen());
                    board.highlightKingStatus(isEnded, WHITE);
                    board.highlightKingStatus(isEnded, BLACK);
                    isOn = GAME;
                    currentDifficultyButton->clear();
                    currentDifficultyIndex = 0;

                    for (int k = 0; k < difficultyList.size(); k++)
                    {
                        std::cerr << "Seaching at: " << k << "\n";
                        if (difficultyList[k].difficulty == communicator.getDifficulty())
                        {
                            std::cerr << "Found current difficulty at | " << k << " |" << "\n";
                            currentDifficultyIndex = k;
                            break;
                        }
                    }
                    currentDifficultyButton = &difficultyList[currentDifficultyIndex].button;
                    GameGUILoad();
                    break;
                }
            }
            // SDL_Delay(1000);
            break;
        }
        case GAME:
        {
            if (isEnded)
            {
                if (!openGameOverPopup)
                {
                    confirmationPopup.open();
                    openGameOverPopup = true;
                }
                needPresent = true;
            }
            else
            {
                if (!confirmationPopup.isClosed())
                {
                    confirmationPopup.close();
                    openGameOverPopup = false;
                }

                needPresent = false;
            }
            // * Computer's turn
            currentMoveColor = board.getMoveColor();
            if (!renderOnce)
            {
                SDL_SetRenderDrawColor(renderer, 49, 46, 43, 1); // background color
                SDL_RenderClear(renderer);

                GameGUILoad();
                board.renderFromFen();

                GameTurnIndicatorLoad();

                std::cerr << "Nvm, its moves are resetted now\n";

                SDL_RenderPresent(renderer);

                renderOnce = true;
                break;
            }

            if (isSinglePlayer && currentMoveColor == BLACK)
            {
                int prevBlackPawnCount = board.getBlackPawnCount();
                int prevWhitePieceCount = board.getWhitePieceCount();
                board.nextMove(currentMoveColor);

                int postBlackPawnCount = board.getBlackPawnCount();
                int postWhitePieceCount = board.getWhitePieceCount();
                board.setRenderCheck(COLOR_NONE);
                board.nextMoveColor();
                currentMoveColor = board.getMoveColor();
                if (board.highlightKingStatus(isEnded, (chessColor)(currentMoveColor)))
                {
                    if (isEnded)
                        soundboard.playSound(SoundEffect::GAMEOVER);
                    else
                        soundboard.playSound(SoundEffect::CHECK);
                }
                else
                {
                    if (prevBlackPawnCount != postBlackPawnCount)
                        soundboard.playSound(SoundEffect::PROMOTION);
                    else if (prevWhitePieceCount != postWhitePieceCount)
                        soundboard.playSound(SoundEffect::CAPTURE);
                    else
                        soundboard.playSound(SoundEffect::MOVE);
                }

                gameState.pushState(board.getFen());
                // board.updateFen(board.boardToFen());
                // board.updateFen(board.boardToFen());
                board.highlightKingStatus(isEnded, (chessColor)currentMoveColor);
                GameBoardRender();
                GameTurnIndicatorLoad();
                std::cerr << "Hello hello black moved\n";
                // Update the screen
                SDL_RenderPresent(renderer);
                break;
            }

            GameGUILoad();
            GameTurnIndicatorLoad();
            // Check if the window is running or not
            while (SDL_PollEvent(&event) != 0)
            {
                switch (event.type)
                {
                case SDL_QUIT:
                {
                    running = false;
                    break;
                }
                case SDL_MOUSEBUTTONDOWN:
                {
                    if (isEnded)
                    {
                        // std::cerr << "Game ended\n";
                        break;
                    }
                    if (event.button.button != SDL_BUTTON_LEFT)
                        break;
                    if (!board.testInbound(event.button))
                    {
                        // Clicked outside of board
                        // * Not pressed inside of chessboard

                        if (isUnderPromotion)
                        {
                            // GameBoardRender();
                            // GameTurnIndicatorLoad();
                            // SDL_RenderPresent(renderer);
                            if (board.handlePawnPromotion(&event))
                            {
                                // gameState.pushState(board.getFen());
                                // SDL_RenderPresent(renderer);
                                isUnderPromotion = false;

                                board.nextMoveColor();
                                currentMoveColor = board.getMoveColor();
                                // the current move color is switched, opposite of promoted piece
                                // board.updateFen(board.boardToFen());
                                if (board.highlightKingStatus(isEnded, (chessColor)currentMoveColor))
                                {
                                    if (isEnded)
                                        soundboard.playSound(SoundEffect::GAMEOVER);
                                    else
                                        soundboard.playSound(SoundEffect::CHECK);
                                }
                                else
                                    soundboard.playSound(SoundEffect::PROMOTION);
                                // Frame handling
                                GameBoardRender();

                                gameState.pushState(board.getFen());
                                GameTurnIndicatorLoad();

                                needPresent = true;
                            }
                        }
                        break;
                    }

                    // Clicked inside of board
                    if (isUnderPromotion)
                        break;

                    // Passing game state conditions
                    Coordinate pickedPlace = board.getPieceCoord(event.button);
                    pickedPiece = board.getPiece(pickedPlace);
                    int pickedColor = board.getPieceColor(pickedPiece);
                    // Correct color
                    if (pickedColor != currentMoveColor)
                        break;

                    // Picked place inside of board
                    prevCoordinate = pickedPlace;
                    if (pickedPlace == Coordinate(-1, -1))
                        break;

                    // Legal movement
                    if (pickedPiece == '0')
                        break;
                    // std::cerr << "Before getting possible moves: \n";
                    // board.debugBoard();
                    isLeftMouseHolding = true;
                    soundboard.playSound(SoundEffect::PICKUP);

                    // * Getting possible moves
                    possibleMoves.clear();
                    possibleCaptures.clear();
                    possibleMoves = board.getPossibleMoves(pickedPiece, prevCoordinate.getX(), prevCoordinate.getY());
                    // std::cerr << "After getting possible moves: \n";
                    // board.debugBoard();
                    possibleCaptures = board.getPossibleCaptures(pickedPiece, prevCoordinate.getX(), prevCoordinate.getY());
                    // std::cerr << "After getting possible captures: \n";
                    // board.debugBoard();

                    // Frame handling
                    GameBoardRender();

                    GameTurnIndicatorLoad();

                    needPresent = true;

                    board.deleteCell(pickedPlace);
                    break;
                }
                case SDL_MOUSEMOTION:
                {
                    if (isEnded)
                    {
                        // std::cerr << "Game ended\n";
                        break;
                    }
                    if (isLeftMouseHolding == false) // Mouse hover
                        break;
                    // Frame handling
                    GameBoardRender();

                    board.renderMove(possibleMoves, possibleCaptures);
                    board.renderPieceByCursor(pickedPiece, event.button.x, event.button.y);
                    GameGUILoad();

                    GameTurnIndicatorLoad();

                    needPresent = true;

                    break;
                }
                case SDL_MOUSEBUTTONUP:
                {
                    if (isEnded)
                    {
                        // std::cerr << "Game ended\n";
                        break;
                    }
                    if (event.button.button != SDL_BUTTON_LEFT)
                        break;

                    if (isLeftMouseHolding)
                    {
                        isLeftMouseHolding = false;
                        Coordinate droppedPlace = board.getPieceCoord(event.button);
                        // * Case player don't move
                        if (droppedPlace == prevCoordinate)
                        {
                            // Dropping at same place
                            board.writeCell(droppedPlace, pickedPiece);

                            // // Frame handling
                            GameBoardRender();
                            // GameTurnIndicatorLoad();
                            board.renderMove(possibleMoves, possibleCaptures);

                            isToHighlightMove = true;

                            GameTurnIndicatorLoad();

                            needPresent = true;
                            break;
                        }
                        // * Case player did a legal move
                        else
                        {
                            int prevPieceCount = board.getPieceCount() + 1;
                            if (board.makeMove(prevCoordinate, droppedPlace, pickedPiece, possibleMoves, possibleCaptures))
                            {
                                // Check if there is any pawn under promotion
                                int postPieceCount = board.getPieceCount();
                                // std::cerr << "Before if clause: \n";
                                // board.debugBoard();
                                // ! -------
                                if ((droppedPlace.getY() == 0 || droppedPlace.getY() == 7) && (board.getPieceName(pickedPiece) == PAWN))
                                {
                                    isUnderPromotion = true;
                                    board.enablePawnPromotion(droppedPlace.getX(), droppedPlace.getY());
                                    if (postPieceCount != prevPieceCount)
                                        soundboard.playSound(SoundEffect::CAPTURE);
                                    else
                                        soundboard.playSound(SoundEffect::MOVE);
                                }
                                else
                                {
                                    if (board.highlightKingStatus(isEnded, chessColor(1 - (int)currentMoveColor)))
                                    {
                                        if (isEnded)
                                            soundboard.playSound(SoundEffect::GAMEOVER);
                                        else
                                            soundboard.playSound(SoundEffect::CHECK);
                                    }
                                    else
                                    {
                                        if (postPieceCount != prevPieceCount)
                                            soundboard.playSound(SoundEffect::CAPTURE);
                                        else
                                            soundboard.playSound(SoundEffect::MOVE);
                                    }
                                }
                                // std::cerr << "After if clause: \n";
                                // ! ----
                                // Check if there is any king being checked
                                // board.debugBoard();
                                board.setRenderCheck(COLOR_NONE);

                                // Update Castling informations
                                // board.debugBoard();
                                board.updateCastlingStatus();

                                prevCoordinate = Coordinate(-1, -1);
                                pickedPiece = ' ';

                                // Frame handling
                                // std::cerr << board.getFen() << "\n";
                                GameBoardRender();
                                GameTurnIndicatorLoad();
                                needPresent = true;
                                // board.debugBoard();
                            }

                            // * Case player did a illegal move
                            else // invalid move
                            {
                                // break;
                                board.writeCell(prevCoordinate, pickedPiece);
                                std::cerr.flush();
                                std::cerr << "Done putting back to original\n";
                                board.debugBoard();
                                // Frame handling
                                GameBoardRender();

                                GameTurnIndicatorLoad();

                                needPresent = true;
                                soundboard.playSound(SoundEffect::ILLEGAL);
                                break;
                            }
                        }
                        // * King status is checked after move and promotion have done
                        if (!isUnderPromotion)
                        {
                            // std::cerr << "Highlighting " << currentMoveColor << "\n";
                            board.nextMoveColor();
                            currentMoveColor = board.getMoveColor();
                            gameState.pushState(board.getFen());
                            // GameGUILoad();
                            GameBoardRender();
                            if (board.highlightKingStatus(isEnded, (chessColor)currentMoveColor))
                            {
                                if (isToHighlightMove)
                                {
                                    isToHighlightMove = false;
                                    board.renderMove(possibleMoves, possibleCaptures);
                                }

                                // GameTurnIndicatorLoad();
                                // SDL_RenderPresent(renderer);
                                // board.renderMove(possibleMoves, possibleCaptures);
                            }
                            board.render();
                            isToHighlightMove = false;
                            GameTurnIndicatorLoad();
                            needPresent = true;
                        }
                    }
                    break;
                }
                    // default:
                    // board.present();
                }
                if (isEnded && !confirmationPopup.isClosed())
                    confirmationPopup.handleButtonEvent(&event);
                else
                    GameGUIButtonsHandling();
            }
            GameGUIButtonsClicked();

            if (isEnded)
            {
                if (!isEndgameSoundPlayed)
                {
                    isEndgameSoundPlayed = true;
                    soundboard.playSound(SoundEffect::GAMEOVER);
                }
                if (confirmationPopup.isClosed())
                {
                    renderOnce = false;
                    break;
                }
                confirmationPopup.handleButtonClicked();
                // std::cerr << "The game ended\n";
                confirmationPopup.render(board.getEndingMessage(), (std::string) "Do you want to restart?", 40);

                if (confirmationPopup.isConfirmed() == Confirmation::YES)
                {
                    confirmationPopup.clearConfirmation();
                    board.startNewGame();
                    board.resetBoardState(isEnded);
                    resetGameState();
                    board.highlightKingStatus(isEnded, (chessColor)currentMoveColor);
                    gameState.pushState(board.getFen());
                    std::cerr << "Popup confirmed\n";
                }
                else if (confirmationPopup.isConfirmed() == Confirmation::NO)
                {
                    confirmationPopup.clearConfirmation();
                    // running = false;
                    isOn = GUI_State::START;
                    ResetColor();
                    ResetDifficulty();
                    renderOnce = false;
                    quitBtn.setSize(startMenuBtnWidth, startMenuBtnHeight);
                    break;
                    std::cerr << "Popup denied\n";
                }
            }
            else
                isEndgameSoundPlayed = false;

            if (needPresent)
            {
                SDL_RenderPresent(renderer);
            }

            break;
        }
        case SAVE:
        {

            SDL_SetRenderDrawColor(renderer, 49, 46, 43, 1); // Black background
            SDL_RenderClear(renderer);

            while (SDL_PollEvent(&event) != 0)
            {
                if (event.type == SDL_QUIT)
                {
                    running = false;
                    break;
                }
                if (isToRenderPopupSave || isToRenderPopupDelete)
                {
                    confirmationPopup.handleButtonEvent(&event);
                }
                else
                {
                    for (Button &button : saveFileBtns)
                    {
                        button.handleEvent(&event);
                        // std::cerr << "Done handle " << i << "\n";
                    }
                    for (Button &deleteButton : deleteSaveFileBtns)
                    {
                        deleteButton.handleEvent(&event);
                    }
                    backBtn.handleEvent(&event);
                }
            }
            // ? Handle success
            // Render demo board and infos
            demoBoard.renderFromFen();
            // std::cerr << demoBoard.getMoves() << "\n";
            if (demoBoard.getMoves() != "")
            {
                renderText(renderer, font, "Turn: " + demoBoard.getTurn(), white, {480, 700, 135, 30});
                renderText(renderer, font, "Move: " + demoBoard.getMoves(), white, {780, 700, 100, 30});

                if (demoGameMode)
                {
                    std::string difficultyLevel;
                    if (demoGameDifficulty == 1)
                        difficultyLevel = "Easy";
                    else if (demoGameDifficulty == 2)
                        difficultyLevel = "Medium";
                    else if (demoGameDifficulty == 3)
                        difficultyLevel = "Hard";
                    renderText(renderer, font, "Versus AI", white, {480, 780, 135, 30});
                    renderText(renderer, font, "Difficulty: " + difficultyLevel, white, {780, 780, 100, 30});
                }
                else
                {
                    renderText(renderer, font, "2-Player", white, {490, 780, 135, 30});
                }
            }
            // Render logo
            SDL_RenderCopy(renderer, saveMenuTexture, NULL, &loadInfos);
            // Render Buttons
            for (int i = 0; i < saveFileBtns.size(); i++)
            {
                saveFileBtns[i].render(60, 240 + i * 90);
            }
            for (int i = 0; i < deleteSaveFileBtns.size(); i++)
            {
                deleteSaveFileBtns[i].renderSVG("./assets/x_circle.svg", 350, 240 + i * 90, SVG_SCALE * 1.25);
            }
            backBtn.renderSVG("assets/game_button.svg", 120, 800, SVG_SCALE);

            // Save confirmation popup
            if (isToRenderPopupSave)
            {
                confirmationPopup.handleButtonClicked();
                confirmationPopup.render("Do you want to overwrite this file?", 40);
                SDL_RenderPresent(renderer);

                if (confirmationPopup.isConfirmed() != Confirmation::NONE)
                {
                    if (confirmationPopup.isConfirmed() == Confirmation::YES)
                    {
                        std::cerr << "Yes pressed" << std::endl;
                        saveGame(board, saveFileId + 1, isSinglePlayer, communicator);
                        std::cerr << "Done saving\n";
                        demoBoard.updateFen("8/8/8/8/8/8/8/8 w - - 0 0");
                        demoBoard.renderFromFen();

                        saveFileBtns[saveFileId].resetHovered();
                        renderOnce = false;

                        isOn = GUI_State::GAME;
                        confirmationPopup.close();
                        isToRenderPopupSave = false;
                        saveFileId = -1;
                    }
                    else if (confirmationPopup.isConfirmed() == Confirmation::NO || confirmationPopup.isClosed())
                    {
                        demoBoard.renderFromFen();
                        demoBoard.updateFen("8/8/8/8/8/8/8/8 w - - 0 0");
                        saveFileBtns[saveFileId].resetHovered();
                        saveFileBtns[saveFileId].updateColor(loadMenuBtnColor);
                        saveFileBtns[saveFileId].render(60, 240 + saveFileId * 90);
                        SDL_RenderPresent(renderer);
                        confirmationPopup.close();

                        isToRenderPopupSave = false;
                        saveFileId = -1;
                    }
                    confirmationPopup.clearConfirmation();
                    confirmationPopup.close();
                    break;
                }
                // * Get
                // * Render confirmationPopup, disable clicking at background
                break;
            }
            if (isToRenderPopupDelete)
            {
                confirmationPopup.render("Do you want to delete this file?", 40);
                SDL_RenderPresent(renderer);
                confirmationPopup.handleButtonClicked();
                if (confirmationPopup.isConfirmed() == Confirmation::YES)
                {
                    std::cerr << "Yes pressed" << std::endl;
                    std::cerr << "saveFileId is: " << deleteSaveFileId + 1 << "\n";
                    deleteSave(deleteSaveFileId + 1);
                    saveFileId = -1;
                    isToRenderPopupDelete = false;
                }
                else if (confirmationPopup.isConfirmed() == Confirmation::NO || confirmationPopup.isClosed())
                {
                    saveFileId = -1;
                    isToRenderPopupDelete = false;
                }
                confirmationPopup.clearConfirmation();
                break;
            }

            if (backBtn.clicked())
            {
                backBtn.resetClicked();
                isOn = previousState;
                renderOnce = false;
                break;
            }

            hoverLoading = false;
            // Handle hovering
            for (int i = 0; i < saveFileBtns.size(); i++)
            {
                if (saveFileBtns[i].hover())
                {
                    // saveFileBtns[i].resetHovered();
                    loadGame(demoBoard, files[i]);
                    hoverLoading = true;
                    saveFileBtns[i].updateColor(startMenuBtnColor);
                }
                else
                    saveFileBtns[i].updateColor(loadMenuBtnColor);
            }
            if (!hoverLoading)
                demoBoard.updateFen("8/8/8/8/8/8/8/8 w - - 0 0");

            // Handle button click
            for (int i = 0; i < saveFileBtns.size(); i++)
            {
                if (saveFileBtns[i].clicked())
                {
                    saveFileBtns[i].resetClicked();
                    saveFileBtns[i].updateColor(loadMenuBtnColor);
                    isToRenderPopupSave = true;
                    confirmationPopup.open();
                    saveFileId = i;
                }
            }
            for (int i = 0; i < saveFileBtns.size(); i++)
            {
                if (deleteSaveFileBtns[i].clicked())
                {
                    deleteSaveFileBtns[i].resetClicked();
                    isToRenderPopupDelete = true;
                    confirmationPopup.open();
                    deleteSaveFileId = i;
                }
            }

            SDL_RenderPresent(renderer);
            // SDL_Delay(1000);
            break;
        }
        case SETTINGS:
        {
            SDL_SetRenderDrawColor(renderer, bgColor.getR(), bgColor.getG(), bgColor.getB(), bgColor.getA()); // Black background
            SDL_RenderClear(renderer);

            backBtn.renderSVG("assets/game_button.svg", 50, 50, SVG_SCALE);

            TTF_Font *settingFont = TTF_OpenFont("./font/Recursive/static/Recursive_Casual-Bold.ttf", 42);
            renderText(renderer, settingFont, "Settings", white, {(SCREEN_WIDTH - 250) / 2, TOP_MARGIN - 80 - 35, 250, 50});
            SDL_SetRenderDrawColor(renderer, modernPrimary.getR(), modernPrimary.getG(), modernPrimary.getB(), modernPrimary.getA());
            SDL_RenderFillRect(renderer, &settingLogoSeperator);

            renderText(renderer, loadMenuFont, "Theme", white, {SCREEN_WIDTH / 2 - 50, TOP_MARGIN - 20, 100, 20});
            renderText(renderer, loadMenuFontSmall, "Board theme", white, {(SCREEN_WIDTH - 250) / 2, TOP_MARGIN + 20, 100, 20});
            renderText(renderer, loadMenuFontSmall, "Piece theme", white, {SCREEN_WIDTH / 2 + 25, TOP_MARGIN + 20, 100, 20});
            currentBoardThemeButton->renderSVG("assets/setting_button_small.svg", (SCREEN_WIDTH - 250) / 2, TOP_MARGIN + 50, SVG_SCALE);
            currentPieceThemeButton->renderSVG("assets/setting_button_small.svg", (SCREEN_WIDTH / 2) + 25, TOP_MARGIN + 50, SVG_SCALE);
            retryBtn.renderSVG("./assets/setting_button.svg", (SCREEN_WIDTH - 250) / 2, TOP_MARGIN + 130, SVG_SCALE);
            renderText(renderer, font, "Music Volume: " + std::to_string(backgroundMusic.getVolume()), white, {SCREEN_WIDTH / 2 - 50, TOP_MARGIN + 200, 100, 30});
            int volume = backgroundMusic.getVolume();
            for (int i = 0; i < volumeRect.size(); i++)
            {
                volumeRect[i] = {SCREEN_WIDTH / 2 - 150 + 19 * i, TOP_MARGIN + 240, 15, 20};
                if (volume)
                {
                    SDL_SetRenderDrawColor(renderer, modernPrimary.getR(), modernPrimary.getG(), modernPrimary.getB(), modernPrimary.getA());
                    SDL_RenderFillRect(renderer, &volumeRect[i]);
                    volume--;
                }
                else
                {
                    SDL_SetRenderDrawColor(renderer, semiDark.getR(), semiDark.getG(), semiDark.getB(), semiDark.getA());
                    SDL_RenderFillRect(renderer, &volumeRect[i]);
                }
            }
            settingDecreaseMusicVolume.renderSVG("assets/setting_button_small.svg", (SCREEN_WIDTH - 250) / 2, TOP_MARGIN + 280, SVG_SCALE);
            settingIncreaseMusicVolume.renderSVG("assets/setting_button_small.svg", (SCREEN_WIDTH / 2) + 25, TOP_MARGIN + 280, SVG_SCALE);
            currentMusicState->renderSVG("assets/setting_button.svg", (SCREEN_WIDTH - 250) / 2, TOP_MARGIN + 360, SVG_SCALE);
            homeBtn.renderSVG("./assets/setting_button.svg", (SCREEN_WIDTH - 250) / 2, TOP_MARGIN + 440, SVG_SCALE);
            quitBtn.renderSVG("assets/setting_button.svg", (SCREEN_WIDTH - 250) / 2, TOP_MARGIN + 520, SVG_SCALE);

            while (SDL_PollEvent(&event) != 0)
            {
                if (event.type == SDL_QUIT)
                {
                    running = false;
                    break;
                }
                currentBoardThemeButton->handleEvent(&event);
                currentPieceThemeButton->handleEvent(&event);
                backBtn.handleEvent(&event);
                homeBtn.handleEvent(&event);
                retryBtn.handleEvent(&event);
                currentMusicState->handleEvent(&event);
                settingIncreaseMusicVolume.handleEvent(&event);
                settingDecreaseMusicVolume.handleEvent(&event);
                quitBtn.handleEvent(&event);
            }

            if (currentBoardThemeButton->clicked())
            {
                SDL_Log("Theme clicked");
                currentBoardThemeButton->resetClicked(); // Reset button state
                // * To next color in circle
                currentBoardThemeIndex = (currentBoardThemeIndex + 1) % 3;
                currentBoardThemeButton = &themeList[currentBoardThemeIndex].boardButton;
                board.setColor(themeList[currentBoardThemeIndex].primaryColor, themeList[currentBoardThemeIndex].secondaryColor);

                renderOnce = false;
            }

            if (currentPieceThemeButton->clicked())
            {
                SDL_Log("Piece theme clicked");
                currentPieceThemeButton->resetClicked(); // Reset button state
                // * To next color in circle
                currentPieceThemeIndex = (currentPieceThemeIndex + 1) % 3;
                currentPieceThemeButton = &themeList[currentPieceThemeIndex].pieceButton;
                board.setPieceTheme(themeList[currentPieceThemeIndex].pieceTheme);
                std::cerr << "Piece theme setted to " << board.getTheme() << "\n";

                renderOnce = false;
            }

            if (backBtn.clicked())
            {
                backBtn.resetClicked();
                isOn = previousState;
                renderOnce = false;

                // for (auto &theme : themeList)
                // {
                //     theme.button.setSize(120, 50);
                // }
                quitBtn.setSize(120, 50);

                break;
            }
            if (homeBtn.clicked())
            {
                homeBtn.resetClicked(); // Reset button state
                SDL_Log("Home clicked!");
                isOn = GUI_State::START;
                ResetColor();
                ResetDifficulty();
                quitBtn.setSize(startMenuBtnWidth, startMenuBtnHeight);
            }
            if (retryBtn.clicked())
            {
                SDL_Log("Retry clicked!");
                board.startNewGame();
                board.resetBoardState(isEnded);
                resetGameState();
                gameState.pushState(board.getFen());
                renderOnce = false;
                isOn = GUI_State::GAME;
                retryBtn.resetClicked(); // Reset button state
            }

            if (currentMusicState->clicked())
            {
                currentMusicState->resetClicked();
                if (!backgroundMusic.isPaused())
                {
                    std::cerr << "Music paused\n";
                    backgroundMusic.pause();
                    soundboard.mute();
                    currentMusicState = &settingUnmuteMusic;
                }
                else
                {
                    std::cerr << "Music resumed\n";
                    backgroundMusic.resume();
                    soundboard.unmute();
                    currentMusicState = &settingMuteMusic;
                }
            }

            if (settingIncreaseMusicVolume.clicked())
            {
                std::cerr << "Increase music volume\n";
                settingIncreaseMusicVolume.resetClicked();
                backgroundMusic.increaseVolume();
                soundboard.setVolume(backgroundMusic.getVolume());
            }

            if (settingDecreaseMusicVolume.clicked())
            {
                std::cerr << "Decrease music volume\n";
                settingDecreaseMusicVolume.resetClicked();
                backgroundMusic.decreaseVolume();
                soundboard.setVolume(backgroundMusic.getVolume());
            }

            if (quitBtn.clicked())
            {
                std::cerr << "Quit\n";
                quitBtn.resetClicked();
                running = false;
                break;
            }

            // Update screen
            SDL_RenderPresent(renderer);
            TTF_CloseFont(settingFont);
            break;
        }
        }
    }

    // system("pause");
    backgroundMusic.stop();
    TTF_CloseFont(font);
    TTF_CloseFont(loadMenuFont);
    TTF_CloseFont(loadMenuFontSmall);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}
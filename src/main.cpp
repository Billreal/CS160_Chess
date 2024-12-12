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
using std::cerr;
using std::cout;

SDL_Renderer *renderer;
SDL_Window *window;
SDL_Surface *winSurface;

const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 1000;
const int SVG_SCALE = 1;
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

void saveGame(Board &board, const std::string &filename)
{
    std::ofstream saveFile;
    saveFile.open(filename);
    if (!saveFile)
    {
        cerr << "Failed to open file for saving: " << filename << "\n";
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
        cerr << "Failed to open file for loading: " << filename << "\n";
        return;
    }

    if (!std::getline(saveFile, FEN))
    {
        cerr << "Failed to read from file: " << filename << "\n";
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
void loadGame(Board &board, const std::string &filename, bool &isSinglePlayer, Communicator &communicator)
{
    std::ifstream saveFile(filename);
    std::string FEN;
    if (!saveFile)
    {
        cerr << "Failed to open file for loading: " << filename << "\n";
        return;
    }

    if (!std::getline(saveFile, FEN))
    {
        cerr << "Failed to read from file: " << filename << "\n";
        saveFile.close();
        return;
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
    Button button;
    colorRGBA primaryColor;
    colorRGBA secondaryColor;
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
    // ! For disabling communicator
    bool isCommunicatorEnabled = true;
    // ! -----------------------------
    Board board(renderer, modernPrimary, modernSecondary, bgColor);
    Communicator communicator(isCommunicatorEnabled);
    Music backgroundMusic("assets/fallen.mp3");
    backgroundMusic.play();
    communicator.init();
    communicator.startNewGame();

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

    Coordinate prevCoordinate(-1, -1);
    char pickedPiece = ' ';
    Coordinate pickedPlace(-1, -1);

    // board.setColor(modernPrimary, modernSecondary);
    board.setCommunicator(&communicator);

    vector<Coordinate> possibleMoves;
    vector<Coordinate> possibleCaptures;

    bool isEnded = false;

    //// Initialize start menu
    // Initizalize logo
    const int logoWidth = 500;
    const int logoHeight = 177;
    SDL_Rect logoInfos = {(SCREEN_WIDTH - logoWidth) / 2, 120, logoWidth, logoHeight};
    SDL_Texture *logoTexture = loadTexture("./assets/logo.png");

    // Initialize buttons
    const int X_COLUMN_BUTTON[2] = {700, 1000};
    const int Y_ROW_BUTTON[7] = {80, 180, 280, 380, 480, 580, 680};
    const int startMenuBtnWidth = 500;
    const int startMenuBtnHeight = 100;
    SDL_Color startMenuBtnColor = {118, 150, 85, 255};
    SDL_Color loadMenuBtnColor = {38, 37, 33, 1};
    SDL_Color white = {255, 255, 255, 255};
    Button startBtn2P(renderer, (SCREEN_WIDTH - 500) / 2, 350, 240, startMenuBtnHeight, startMenuBtnColor, white, "2 Player", font);
    Button startBtnAI(renderer, SCREEN_WIDTH / 2 + 10, 350, 240, startMenuBtnHeight, startMenuBtnColor, white, "vs AI", font);
    Button loadBtn(renderer, (SCREEN_WIDTH - startMenuBtnWidth) / 2, 500, startMenuBtnWidth, startMenuBtnHeight, startMenuBtnColor, white, "Load", font);
    Button quitBtn(renderer, (SCREEN_WIDTH - startMenuBtnWidth) / 2, 650, startMenuBtnWidth, startMenuBtnHeight, startMenuBtnColor, white, "Quit", font);

    //// Initialize load menu
    SDL_Rect loadInfos = {60, 80, 900, 850};
    SDL_Texture *loadMenuTexture = loadTexture("./assets/load.png");
    TTF_Font *loadMenuFont = TTF_OpenFont("./font/Recursive/static/Recursive_Casual-Light.ttf", 24);
    // TTF_Font *loadMenuFont = TTF_OpenFont("C:/Windows/Fonts/arial.ttf", 24);
    if (!loadMenuFont)
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
        loadFileBtns.push_back(Button(renderer, 60, 240 + i * 90, 250, 60, loadMenuBtnColor, white, name, loadMenuFont));
        saveFileBtns.push_back(Button(renderer, 60, 240 + i * 90, 250, 60, loadMenuBtnColor, white, name, loadMenuFont));
    }

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
    Button saveBtn(renderer, SCREEN_WIDTH - (SIDE_MARGIN + 130), TOP_MARGIN + 40, 120, 50, startMenuBtnColor, white, "Save", loadMenuFont);
    Button loadBtnInGame(renderer, SCREEN_WIDTH - (SIDE_MARGIN + 130), TOP_MARGIN + 120, 120, 50, startMenuBtnColor, white, "Load", loadMenuFont);
    Button homeBtn(renderer, SCREEN_WIDTH - (SIDE_MARGIN + 130), TOP_MARGIN + 200, 120, 50, startMenuBtnColor, white, "Home", loadMenuFont);
    Button retryBtn(renderer, SCREEN_WIDTH - (SIDE_MARGIN + 130), TOP_MARGIN + 280, 120, 50, startMenuBtnColor, white, "Retry", loadMenuFont);
    Button settingBtn(renderer, SCREEN_WIDTH - (SIDE_MARGIN + 130), TOP_MARGIN + 360, 120, 50, startMenuBtnColor, white, "Setting", loadMenuFont);
    Button ingameColorSwitchModern(renderer, SCREEN_WIDTH - (SIDE_MARGIN + 130), TOP_MARGIN + 440, 120, 50, startMenuBtnColor, white, "Modern", loadMenuFont);
    Button ingameColorSwitchFuturistic(renderer, SCREEN_WIDTH - (SIDE_MARGIN + 130), TOP_MARGIN + 440, 120, 50, startMenuBtnColor, white, "Futuristic", loadMenuFont);
    Button ingameColorSwitchClassic(renderer, SCREEN_WIDTH - (SIDE_MARGIN + 130), TOP_MARGIN + 440, 120, 50, startMenuBtnColor, white, "Classic", loadMenuFont);

    Button ingameDifficultySwitchEasy(renderer, SCREEN_WIDTH - (SIDE_MARGIN + 130), TOP_MARGIN + 520, 120, 50, startMenuBtnColor, white, "Easy", loadMenuFont);
    Button ingameDifficultySwitchMedium(renderer, SCREEN_WIDTH - (SIDE_MARGIN + 130), TOP_MARGIN + 520, 120, 50, startMenuBtnColor, white, "Medium", loadMenuFont);
    Button ingameDifficultySwitchHard(renderer, SCREEN_WIDTH - (SIDE_MARGIN + 130), TOP_MARGIN + 520, 120, 50, startMenuBtnColor, white, "Hard", loadMenuFont);

    vector<ThemeList> themeList = {{ingameColorSwitchModern, modernPrimary, modernSecondary},
                                   {ingameColorSwitchClassic, classicPrimary, classicSecondary},
                                   {ingameColorSwitchFuturistic, futuristicPrimary, futuristicSecondary}};

    vector<DifficultyList> difficultyList = {{ingameDifficultySwitchEasy, Difficulty::EASY},
                                             {ingameDifficultySwitchMedium, Difficulty::MEDIUM},
                                             {ingameDifficultySwitchHard, Difficulty::HARD}};

    Button undoBtn(renderer, SIDE_MARGIN + bottomPanelWidth / 2 - 34 - 5, SCREEN_HEIGHT - panelMargin - bottomPanelHeight + 10, 34, 50, startMenuBtnColor, white, "", loadMenuFont);
    Button redoBtn(renderer, SIDE_MARGIN + bottomPanelWidth / 2 + 5, SCREEN_HEIGHT - panelMargin - bottomPanelHeight + 10, 34, 50, startMenuBtnColor, white, "", loadMenuFont);
    Button beginBtn(renderer, SIDE_MARGIN + bottomPanelWidth / 2 - 5 - 34 - 10 - 59, SCREEN_HEIGHT - panelMargin - bottomPanelHeight + 10, 59, 50, startMenuBtnColor, white, "", loadMenuFont);
    Button endBtn(renderer, SIDE_MARGIN + bottomPanelWidth / 2 + 5 + 34 + 10, SCREEN_HEIGHT - panelMargin - bottomPanelHeight + 10, 59, 50, startMenuBtnColor, white, "", loadMenuFont);

    int currentThemeIndex = 0;
    int currentDifficultyIndex = 0;
    Button *currentThemeButton = &themeList[currentThemeIndex].button;
    Button *currentDifficultyButton = &difficultyList[currentDifficultyIndex].button;

    board.setColor(themeList[currentThemeIndex].primaryColor, themeList[currentThemeIndex].secondaryColor);
    communicator.setDifficulty(difficultyList[currentDifficultyIndex].difficulty);
    bool isUnderPromotion = false;

    //// Popup handling
    Popup popup(renderer, POPUP_MODE::CONFIRM, (SCREEN_WIDTH - 350) / 2, (SCREEN_HEIGHT - 350) / 2);
    bool isToRenderPopup = false;
    int saveFileId;

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
        saveBtn.renderSVG("./assets/game_button.svg", SVG_SCALE);
        loadBtnInGame.renderSVG("./assets/game_button.svg", SVG_SCALE);
        homeBtn.renderSVG("./assets/game_button.svg", SVG_SCALE);
        retryBtn.renderSVG("./assets/game_button.svg", SVG_SCALE);
        currentThemeButton->renderSVG("./assets/game_button.svg", SVG_SCALE);
        if (isSinglePlayer)
            currentDifficultyButton->renderSVG("./assets/game_button.svg", SVG_SCALE);
        undoBtn.renderPNG("./assets/undo.png");
        redoBtn.renderPNG("./assets/redo.png");
        beginBtn.renderPNG("./assets/begin.png");
        endBtn.renderPNG("./assets/end.png");
    };

    auto GameGUIButtonsHandling = [&]()
    {
        currentThemeButton->handleEvent(&event);
        if (isSinglePlayer)
            currentDifficultyButton->handleEvent(&event);
        saveBtn.handleEvent(&event);
        loadBtnInGame.handleEvent(&event);
        homeBtn.handleEvent(&event);
        retryBtn.handleEvent(&event);
        undoBtn.handleEvent(&event);
        redoBtn.handleEvent(&event);
        beginBtn.handleEvent(&event);
        endBtn.handleEvent(&event);
    };

    auto ResetColor = [&]()
    {
        currentThemeIndex = 0;
        currentThemeButton->resetClicked();
        currentThemeButton = &themeList[currentThemeIndex].button;
        board.setColor(themeList[currentThemeIndex].primaryColor, themeList[currentThemeIndex].secondaryColor);
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
            isOn = SAVE;
            saveBtn.resetClicked(); // Reset button state
        }
        if (loadBtnInGame.clicked())
        {
            SDL_Log("Load clicked!");
            isOn = LOAD;
            loadBtnInGame.resetClicked(); // Reset button state
            LoadMenuRefresh();
        }
        if (homeBtn.clicked())
        {
            homeBtn.resetClicked(); // Reset button state
            SDL_Log("Home clicked!");
            isOn = GUI_State::START;
            ResetColor();
            ResetDifficulty();
            renderOnce = false;
        }
        if (retryBtn.clicked())
        {
            SDL_Log("Retry clicked!");
            board.startNewGame();
            board.resetBoardState(isEnded);
            resetGameState();
            gameState.pushState(board.getFen());
            retryBtn.resetClicked(); // Reset button state
        }
        if (currentThemeButton->clicked())
        {
            SDL_Log("Theme clicked");
            currentThemeButton->resetClicked(); // Reset button state
            // * To next color in circle
            currentThemeIndex = (currentThemeIndex + 1) % 3;
            currentThemeButton = &themeList[currentThemeIndex].button;
            board.setColor(themeList[currentThemeIndex].primaryColor, themeList[currentThemeIndex].secondaryColor);

            renderOnce = false;
        }

        if (currentDifficultyButton->clicked() && isSinglePlayer)
        {
            SDL_Log("Difficulty Clicked");
            currentDifficultyButton->resetClicked();
            currentDifficultyIndex = (currentDifficultyIndex + 1) % 3;
            currentDifficultyButton = &difficultyList[currentDifficultyIndex].button;
            communicator.setDifficulty(difficultyList[currentDifficultyIndex].difficulty);
            renderOnce = false;
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
            }

            // Render logo
            SDL_RenderCopy(renderer, logoTexture, NULL, &logoInfos);

            // Render button
            startBtn2P.renderSVG("./assets/start_button_small.svg", SVG_SCALE);
            startBtnAI.renderSVG("./assets/start_button_small.svg", SVG_SCALE);
            loadBtn.renderSVG("./assets/start_button.svg", SVG_SCALE);
            quitBtn.renderSVG("./assets/start_button.svg", SVG_SCALE);

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
                board.startNewGame();
                board.resetBoardState(isEnded);
                resetGameState();
                gameState.pushState(board.getFen());
                isOn = GAME;
                isSinglePlayer = true;
                startBtnAI.resetClicked(); // Reset button state
                break;
            }
            if (loadBtn.clicked())
            {
                SDL_Log("Button clicked!");
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

            startBtn2P.clear();
            startBtnAI.clear();
            loadBtn.clear();
            quitBtn.clear();
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
            for (Button btn : loadFileBtns)
            {
                btn.render();
            }
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
            for (int i = 0; i < loadFileBtns.size(); i++)
            {
                if (loadFileBtns[i].clicked())
                {
                    // std::cerr << "Button clicked!\n";
                    // if (!isSinglePlayer)
                    // loadGame(board, files[i]);
                    // else
                    loadGame(board, files[i], isSinglePlayer, communicator);
                    resetGameState();
                    board.resetBoardState(isEnded);
                    gameState.clear();
                    gameState.pushState(board.getFen());
                    board.highlightKingStatus(isEnded, WHITE);
                    board.highlightKingStatus(isEnded, BLACK);
                    isOn = GAME;
                    loadFileBtns[i].resetClicked();
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
                needPresent = true;
            else
            {
                if (popup.isClosed())
                {
                    popup.resetClose();
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

                SDL_RenderPresent(renderer);

                renderOnce = true;
                break;
            }

            if (isSinglePlayer && currentMoveColor == BLACK)
            {
                board.nextMove(currentMoveColor);

                board.setRenderCheck(COLOR_NONE);
                board.highlightKingStatus(isEnded, (chessColor)(currentMoveColor));

                board.nextMoveColor();
                currentMoveColor = board.getMoveColor();
                gameState.pushState(board.getFen());
                // board.updateFen(board.boardToFen());
                // board.updateFen(board.boardToFen());
                board.highlightKingStatus(isEnded, (chessColor)currentMoveColor);
                GameBoardRender();
                GameTurnIndicatorLoad();

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
                                board.highlightKingStatus(isEnded, (chessColor)currentMoveColor);
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
                        else if (board.makeMove(prevCoordinate, droppedPlace, pickedPiece, possibleMoves, possibleCaptures))
                        {
                            // Check if there is any pawn under promotion
                            if ((droppedPlace.getY() == 0 || droppedPlace.getY() == 7) && (board.getPieceName(pickedPiece) == PAWN))
                            {
                                isUnderPromotion = true;
                                board.enablePawnPromotion(droppedPlace.getX(), droppedPlace.getY());
                            }

                            // Check if there is any king being checked
                            board.setRenderCheck(COLOR_NONE);

                            // Update Castling informations
                            board.updateCastlingStatus();

                            prevCoordinate = Coordinate(-1, -1);
                            pickedPiece = ' ';

                            // Frame handling
                            // std::cerr << board.getFen() << "\n";
                            GameBoardRender();
                            GameTurnIndicatorLoad();
                            needPresent = true;
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
                            break;
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
                if (isEnded && !popup.isClosed())
                    popup.handleButtonEvent(&event);
                else
                    GameGUIButtonsHandling();
            }
            GameGUIButtonsClicked();

            if (isEnded)
            {
                if (popup.isClosed())
                {
                    renderOnce = false;
                    break;
                }
                popup.handleButtonClicked();
                // std::cerr << "The game ended\n";
                popup.render((std::string) "White win", (std::string) "Do you want to restart?", 40);

                if (popup.isConfirmed() == Confirmation::YES)
                {
                    popup.clearConfirmation();
                    board.startNewGame();
                    board.resetBoardState(isEnded);
                    resetGameState();
                    board.highlightKingStatus(isEnded, (chessColor)currentMoveColor);
                    gameState.pushState(board.getFen());
                    std::cerr << "Popup confirmed\n";
                }
                else if (popup.isConfirmed() == Confirmation::NO)
                {
                    popup.clearConfirmation();
                    running = false;
                    break;
                    std::cerr << "Popup denied\n";
                }
            }

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
                if (isToRenderPopup)
                {
                    popup.handleButtonEvent(&event);
                }
                else
                {
                    for (Button &button : saveFileBtns)
                    {
                        button.handleEvent(&event);
                        // std::cerr << "Done handle " << i << "\n";
                    }
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
            SDL_RenderCopy(renderer, loadMenuTexture, NULL, &loadInfos);
            SDL_SetRenderDrawColor(renderer, 238, 238, 210, 255);
            SDL_RenderFillRect(renderer, &loadMenuSeperateLine);

            // Render Buttons
            for (Button btn : saveFileBtns)
            {
                btn.render();
            }

            // Update screen
            if (isToRenderPopup)
            {
                popup.render("Do you want to overwrite this file?", 40);
                SDL_RenderPresent(renderer);
                popup.handleButtonClicked();
                if (popup.isConfirmed() == Confirmation::YES)
                {
                    std::cerr << "Yes pressed" << std::endl;
                    saveGame(board, saveFileId + 1, isSinglePlayer, communicator);
                    std::cerr << "Done saving\n";
                    demoBoard.updateFen("8/8/8/8/8/8/8/8 w - - 0 0");
                    demoBoard.renderFromFen();
                    saveFileBtns[saveFileId].resetHovered();
                    renderOnce = false;
                    isOn = GUI_State::GAME;
                    isToRenderPopup = false;
                    saveFileId = -1;
                }
                else if (popup.isConfirmed() == Confirmation::NO || popup.isClosed())
                {
                    demoBoard.renderFromFen();
                    demoBoard.updateFen("8/8/8/8/8/8/8/8 w - - 0 0");
                    saveFileBtns[saveFileId].resetHovered();
                    saveFileBtns[saveFileId].updateColor(loadMenuBtnColor);
                    saveFileBtns[saveFileId].render();
                    SDL_RenderPresent(renderer);

                    isToRenderPopup = false;
                    saveFileId = -1;
                }
                popup.resetClose();
                popup.clearConfirmation();
                // * Get
                // * Render popup, disable clicking at background
            }
            else // * Allow interaction with elements
            {
                SDL_RenderPresent(renderer);
                hoverLoading = false;
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
                        isToRenderPopup = true;
                        saveFileId = i;
                    }
                }
            }
            // SDL_Delay(1000);
            break;
        }
        case SETTINGS:
        {
            renderOnce = false;
            isOn = START;
            std::cerr << "Currently in Settings ";
            break;
        }
        }
    }

    // system("pause");
    backgroundMusic.stop();
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}
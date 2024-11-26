#include <iostream>
// #define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <string>
#include "./test.cpp"
#include <fstream>
#include <filesystem>
#include <vector>
// #include "./../include/background.h"
#include "./../include/color.h"
#include "./../include/colorScheme.h"
#include "./../include/board.h"
#include "./../include/pieces.h"
#include "./../include/coordinate.h"
#include "./../include/button.h"
#include "./../include/nanosvg.h"
#include "./../include/nanosvgrast.h"

using std::cerr, std::cout;

SDL_Renderer *renderer;
SDL_Window *window;
SDL_Surface *winSurface;

const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 1000;
const int SVG_SCALE = 1;
const int FONT_SIZE = 32;
const int TOP_MARGIN = 200;
const int BOTOTM_MARGIN = 80;
const int SIDE_MARGIN = 80;
const int SIDE_LENGTH = 80;

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
    std::ifstream saveFile;
    saveFile.open(filename);
    std::string FEN;
    if (!saveFile)
    {
        cerr << "Failed to open file for loading: " << filename << "\n";
        return;
    }

    std::getline(std::cin, FEN);
    board.updateFen(FEN);

    saveFile.close();
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
    LOAD,
    GAME
};

int main(int argc, char *args[])
{
    // ! Temporary variable, will change in later version
    bool running = true;
    bool isPlayer1White = false;
    bool isAgainstBot = true;
    bool isToRotate = (isPlayer1White == false) && isAgainstBot;
    // ! End of temporary variable
    debug();

    // Initializing main components
    GUI_State isOn = START;
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

    TTF_Font *font = TTF_OpenFont("C:/Windows/Fonts/arial.ttf", FONT_SIZE);
    if (!font)
    {
        SDL_Log("Failed to load font: %s", TTF_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return -1;
    }

    // Background background(renderer);
    // background.render(bgColor);
    // if (SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE) < 0)
    // {
    //     cerr << "Blending failure\n";
    //     cerr << SDL_GetError() << "\n";
    // }

    Board board(renderer, board1Primary, board2Primary, bgColor);

    // Handling SDL_events
    SDL_Event event;

    // Chessboard rendering

    // chessPieces chess(QUEEN, WHITE, 4, 4);
    // auto dbg = chess.listAllMove();
    // for (auto x : dbg)
    //     cerr << x.getX() << " " << x.getY() << "\n";

    bool isLeftMouseHolding = false;
    Coordinate prevCoordinate(-1, -1);
    char pickedPiece = ' ';
    board.setColor(board1Primary, board2Primary);
    vector<Coordinate> possibleMoves;
    vector<Coordinate> possibleCaptures;

    // Init logo
    const int logoWidth = 500;
    const int logoHeight = 177;
    SDL_Rect logoInfos = {(SCREEN_WIDTH - logoWidth) / 2, 120, logoWidth, logoHeight};
    SDL_Texture *logoTexture = loadTexture("./assets/logo.png");

    bool isOnStartMenu = false;
    bool renderOnce = false;
    // board.renderPieces();
    // board.render();
    // board.present();

    // Initialize buttons
    const int startMenuBtnWidth = 500;
    const int startMenuBtnHeight = 100;
    SDL_Color startMenuBtnColor = {118, 150, 85, 255};
    SDL_Color loadMenuBtnColor = {38, 37, 33, 1};
    SDL_Color white = {255, 255, 255, 255};
    Button startBtn(renderer, (SCREEN_WIDTH - startMenuBtnWidth) / 2, 350, startMenuBtnWidth, startMenuBtnHeight, startMenuBtnColor, white, "Start", font);
    Button loadBtn(renderer, (SCREEN_WIDTH - startMenuBtnWidth) / 2, 500, startMenuBtnWidth, startMenuBtnHeight, startMenuBtnColor, white, "Load", font);
    Button quitBtn(renderer, (SCREEN_WIDTH - startMenuBtnWidth) / 2, 650, startMenuBtnWidth, startMenuBtnHeight, startMenuBtnColor, white, "Quit", font);

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

    // Initialize load menu
    SDL_Rect loadInfos = {60, 80, 900, 850};
    SDL_Texture *loadMenuTexture = loadTexture("./assets/load.png");
    TTF_Font *loadMenuFont = TTF_OpenFont("C:/Windows/Fonts/arial.ttf", 24);
    SDL_Rect loadMenuSeperateLine = {360, 240, 5, 600};

    Board demoBoard(renderer, board1Primary, board2Primary, bgColor);
    demoBoard.setBoardSize(50);
    demoBoard.setMargin(480, 240);

    if (!font)
    {
        SDL_Log("Failed to load Load_menu font: %s", TTF_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return -1;
    }

    std::vector<std::string> files = getSaveFiles("./saves");

    std::vector<Button> loadFileBtns;

    for (int i = 0; i < files.size(); i++)
    {
        string name = "";
        for (int j = 0; j < files[i].length() - 3; j++)
        {
            if (files[i].substr(j, 4) == ".txt")
            {
                name = files[i].substr(j - 7, 7);
                // std::cerr << name << "\n";
                break;
            }
        }

        loadFileBtns.push_back(Button(renderer, 60, 240 + i * 90, 250, 60, loadMenuBtnColor, white, name, loadMenuFont));
        // std::cerr << "name: " << loadFileBtns[i].getText() << "\n";
    }

    // for (Button btn : loadFileBtns)
    // {
    //     std::cerr << "Adress: " << btn << "\n";
    //     std::cerr << btn.getX() << " " << btn.getY() << " " << btn.getText() << " " << btn.getPrevText() << "\n";
    // }

    while (running)
    {
        // Start menu
        // Clear screen
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
                startBtn.handleEvent(&event);
                loadBtn.handleEvent(&event);
                quitBtn.handleEvent(&event);
            }

            // Render logo
            SDL_RenderCopy(renderer, logoTexture, NULL, &logoInfos);

            // Render button
            startBtn.renderSVG("./assets/start_menu_button.svg", SVG_SCALE);
            startBtn.renderSVG("./assets/start_menu_button.svg", SVG_SCALE);
            loadBtn.renderSVG("./assets/start_menu_button.svg", SVG_SCALE);
            quitBtn.renderSVG("./assets/start_menu_button.svg", SVG_SCALE);

            // Update screen
            SDL_RenderPresent(renderer);

            // Check if button was clicked
            if (startBtn.clicked())
            {
                SDL_Log("Button clicked!");
                isOn = GAME;
                startBtn.reset(); // Reset button state
            }
            if (loadBtn.clicked())
            {
                SDL_Log("Button clicked!");
                isOn = LOAD;
                loadBtn.reset(); // Reset button state
            }
            if (quitBtn.clicked())
            {
                SDL_Log("Button clicked!");
                running = false;
                quitBtn.reset(); // Reset button state
            }

            startBtn.clear();
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
                }

                //     // Handle button events
                for (Button btn : loadFileBtns)
                {
                    btn.handleEvent(&event);
                }
            }

            // Render logo
            SDL_RenderCopy(renderer, loadMenuTexture, NULL, &loadInfos);
            SDL_SetRenderDrawColor(renderer, 238, 238, 210, 255);
            SDL_RenderFillRect(renderer, &loadMenuSeperateLine);
            demoBoard.renderChessboard();

            // Render Buttons
            for (Button btn : loadFileBtns)
            {
                btn.render();
                // std::cerr << loadFileBtns[i].getX() << " " << loadFileBtns[i].getY() << "\n";
            }

            // Update screen
            SDL_RenderPresent(renderer);

            // Handle button hover

            // for (int i = 0; i < loadFileBtns.size(); i++)
            // {
            //     if (loadFileBtns[i].hover())
            //     {
            //         loadGame(demoBoard, files[i]);
            //         SDL_Log("Button hovering!");
            //         demoBoard.renderFromFen();
            //         demoBoard.present();
            //         loadFileBtns[i].reset(); // Reset button state
            //     }
            // }

            // Handle button click
            for (int i = 0; i < loadFileBtns.size(); i++)
            {
                if (loadFileBtns[i].clicked())
                {
                    std::cerr<< "Button clicked!\n";
                    loadGame(demoBoard, files[i]);
                    isOn = GAME;
                    loadFileBtns[i].reset(); // Reset button state
                }
            }

            // Clear buttons
            for (Button btn : loadFileBtns)
            {
                btn.clear();
            }

            demoBoard.clear();
            // SDL_Delay(2000);
            break;
        }
        case GAME:
        {
            SDL_RenderCopy(renderer, rightPanelTexture, NULL, &rightPanelInfos);
            SDL_RenderCopy(renderer, bottomPanelTexture, NULL, &bottomPanelInfos);
            // std::cerr << isOnStartMenu << "\n";

            // // Clear screen
            // SDL_RenderClear(renderer);

            // Update screen
            SDL_RenderPresent(renderer);

            if (!renderOnce)
            {
                board.renderPieces();
                board.render();
                board.present();
                renderOnce = true;
            }
            // Check if the window is running or not do
            do
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
                    if (event.button.button != SDL_BUTTON_LEFT)
                        break;
                    if (!board.testInbound(event.button))
                        break;
                    Coordinate pickedPlace = board.getPieceCoord(event.button);
                    pickedPiece = board.getPiece(pickedPlace);
                    prevCoordinate = pickedPlace;
                    if (pickedPlace == Coordinate(-1, -1))
                        break;
                    if (pickedPiece == '0')
                        break;
                    isLeftMouseHolding = true;
                    cerr << pickedPiece << " " << pickedPlace.getX() << " " << pickedPlace.getY() << "\n";
                    // board.clear();
                    board.render();
                    board.present();
                    // board.debugBoard();
                    board.deleteCell(pickedPlace);
                    break;
                }
                case SDL_MOUSEMOTION:
                {
                    if (isLeftMouseHolding == false) // Mouse hover
                        break;
                    // board.clear();
                    // board.clear();
                    board.render();
                    board.renderMove(possibleMoves, possibleCaptures);
                    board.renderPieceByCursor(pickedPiece, event.button.x, event.button.y);
                    board.present();
                    break;
                }
                case SDL_MOUSEBUTTONUP:
                {
                    if (event.button.button != SDL_BUTTON_LEFT)
                        break;
                    if (!isLeftMouseHolding)
                        break;
                    isLeftMouseHolding = false;
                    Coordinate droppedPlace = board.getPieceCoord(event.button);
                    vector<Coordinate> possibleMoves = board.getPossibleMoves(pickedPiece, prevCoordinate.getX(), prevCoordinate.getY());
                    vector<Coordinate> possibleCaptures = board.getPossibleCaptures(pickedPiece, prevCoordinate.getX(), prevCoordinate.getY());
                    // Coordinate selectedPlace = board.getPieceCoord(event.button);

                    if (droppedPlace != Coordinate(-1, -1) && board.isKingSafe(prevCoordinate, droppedPlace, pickedPiece) && board.isValidMove(possibleMoves, possibleCaptures, droppedPlace))
                        board.writeCell(droppedPlace, pickedPiece);
                    else
                        board.writeCell(prevCoordinate, pickedPiece);
                    // board.clear();
                    board.render();
                    if (droppedPlace == prevCoordinate)
                    {
                        // board.clear();
                        board.renderMove(possibleMoves, possibleCaptures);
                    }
                    prevCoordinate = Coordinate(-1, -1);
                    pickedPiece = ' ';
                    std::cerr << "Dropped at " << droppedPlace.getX() << " " << droppedPlace.getY() << "\n";
                    board.log(event.button, "released");
                    board.present();
                    break;
                }
                    // default:
                    // board.present();
                }
            } while (SDL_PollEvent(&event) != 0);
            break;
        }
        }
    }

    // system("pause");
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}
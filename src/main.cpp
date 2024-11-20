#pragma once
#include <iostream>
// #define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_ttf.h>
#include "./test.cpp"
// #include "./../include/background.h"
#include "./../include/color.h"
#include "./../include/colorScheme.h"
#include "./../include/board.h"
#include "./../include/pieces.h"
#include "./../include/coordinate.h"
#include "./../include/button.h"
using std::cerr, std::cout;

SDL_Renderer *renderer;
SDL_Window *window;
SDL_Surface *winSurface;

const int SCREEN_WIDTH = 700;
const int SCREEN_HEIGHT = 700;

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

    TTF_Font *font = TTF_OpenFont("C:/Windows/Fonts/arial.ttf", 20);
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

    bool isOnStartMenu = false;
    bool renderOnce = false;
    // board.renderPieces();
    // board.render();
    // board.present();

    Button startBtn(renderer, SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 100, 200, 50, {118, 150, 85, 255}, {255, 255, 255, 255}, "Start", font);
    Button loadBtn(renderer, SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 25, 200, 50, {118, 150, 85, 255}, {255, 255, 255, 255}, "Load", font);
    Button quitBtn(renderer, SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 + 50, 200, 50, {118, 150, 85, 255}, {255, 255, 255, 255}, "Quit", font);

    while (running)
    {
        // Start menu
        if (!isOnStartMenu)
        {
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

            // Clear screen
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black background
            SDL_RenderClear(renderer);

            // Render button
            startBtn.render();
            loadBtn.render();
            quitBtn.render();

            // Update screen
            SDL_RenderPresent(renderer);

            // Check if button was clicked
            if (startBtn.clicked())
            {
                SDL_Log("Button clicked!");
                isOnStartMenu = true;
                startBtn.reset(); // Reset button state
            }
            if (loadBtn.clicked())
            {
                SDL_Log("Button clicked!");
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
        }
        else
        {
            // std::cerr << isOnStartMenu << "\n";
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
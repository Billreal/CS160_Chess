#pragma once
#include <iostream>
#include <cmath>
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

const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 750;

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

    TTF_Font *font = TTF_OpenFont("./font/Recursive/static/Recursive_Casual-Light.ttf", 20);
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

    Board board(renderer, modernPrimary, modernSecondary, bgColor);

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
    SDL_SetRenderDrawColor(renderer, bgColor.getR(), bgColor.getG(), bgColor.getB(), bgColor.getA());
    SDL_RenderClear(renderer);
    board.setColor(modernPrimary, modernSecondary);
    // board.renderPieces();
    // board.render();
    // board.present();
    vector<Coordinate> possibleMoves;
    vector<Coordinate> possibleCaptures;
    int currentMoveColor = WHITE;
    Coordinate enPassantCoord;
    bool isEnded = false;

    bool isOnStartMenu = true;
    bool renderOnce = false;
    // board.renderPieces();
    // board.render();
    // board.present();

    Button startBtn(renderer, SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 100, 200, 50, {118, 150, 85, 255}, {255, 255, 255, 255}, "Start", font);
    Button loadBtn(renderer, SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 25, 200, 50, {118, 150, 85, 255}, {255, 255, 255, 255}, "Load", font);
    Button quitBtn(renderer, SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 + 50, 200, 50, {118, 150, 85, 255}, {255, 255, 255, 255}, "Quit", font);
    Button ingameColorSwitchModern(renderer, 800, SCREEN_HEIGHT / 2 - 100, 100, 50, {118, 150, 85, 255}, {255, 255, 255, 255}, "Modern", font);
    Button ingameColorSwitchFuturistic(renderer, 800, SCREEN_HEIGHT / 2 - 100, 100, 50, {118, 150, 85, 255}, {255, 255, 255, 255}, "Futuristic", font);
    Button ingameColorSwitchClassic(renderer, 800, SCREEN_HEIGHT / 2 - 100, 100, 50, {118, 150, 85, 255}, {255, 255, 255, 255}, "Classic", font);
    Button *currentThemeButton = &ingameColorSwitchModern;
    while (running)
    {
        // Check if the window is running or not

        // Start menu
        if (isOnStartMenu)
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
                isOnStartMenu = false;
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
                currentThemeButton->render();
                // board.present();
                renderOnce = true;
                continue;
            }
            // Check if the window is running or not do
            do
            {
                if (!isEnded)
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
                        {
                            SDL_Log("Pressed theme changing button");
                            // * Not pressed inside of chessboard
                            currentThemeButton->handleEvent(&event);
                            if (currentThemeButton->clicked())
                            {
                                currentThemeButton->reset();
                                // * To next color in circle
                                if (currentThemeButton == &ingameColorSwitchModern)
                                {
                                    currentThemeButton = &ingameColorSwitchFuturistic;
                                    board.setColor(futuristicPrimary, futuristicSecondary);
                                }
                                else if (currentThemeButton == &ingameColorSwitchFuturistic)
                                {
                                    currentThemeButton = &ingameColorSwitchClassic;
                                    board.setColor(classicPrimary, classicSecondary);
                                }
                                else if (currentThemeButton == &ingameColorSwitchClassic)
                                {
                                    currentThemeButton = &ingameColorSwitchModern;
                                    board.setColor(modernPrimary, modernSecondary);
                                }
                                currentThemeButton->render();
                                board.render();
                                // board.present();
                            }
                            break;
                        }
                        Coordinate pickedPlace = board.getPieceCoord(event.button);
                        pickedPiece = board.getPiece(pickedPlace);
                        int pickedColor = board.getPieceColor(pickedPiece);
                        if (pickedColor != currentMoveColor)
                            break;
                        prevCoordinate = pickedPlace;
                        if (pickedPlace == Coordinate(-1, -1))
                            break;
                        if (pickedPiece == '0')
                            break;
                        isLeftMouseHolding = true;
                        cerr << pickedPiece << " " << pickedPlace.getX() << " " << pickedPlace.getY() << "\n";
                        possibleMoves.clear();
                        possibleCaptures.clear();
                        possibleMoves = board.getPossibleMoves(pickedPiece, prevCoordinate.getX(), prevCoordinate.getY());

                        possibleCaptures = board.getPossibleCaptures(pickedPiece, prevCoordinate.getX(), prevCoordinate.getY());
                        board.render();
                        currentThemeButton->render();
                        // board.log("Done present");
                        board.deleteCell(pickedPlace);
                        // board.log("Done delete");
                        break;
                    }
                    case SDL_MOUSEMOTION:
                    {
                        if (isLeftMouseHolding == false) // Mouse hover
                            break;
                        SDL_SetRenderDrawColor(renderer, bgColor.getR(), bgColor.getG(), bgColor.getB(), bgColor.getA());
                        SDL_RenderClear(renderer);
                        board.render();
                        board.renderMove(possibleMoves, possibleCaptures);
                        board.renderPieceByCursor(pickedPiece, event.button.x, event.button.y);
                        currentThemeButton->render();
                        // board.log("Done render animation");
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

                        if (droppedPlace == prevCoordinate)
                        {
                            // Dropping at same place
                            board.writeCell(droppedPlace, pickedPiece);
                            board.render();
                            board.renderMove(possibleMoves, possibleCaptures);
                        }
                        else if (board.makeMove(prevCoordinate, droppedPlace, pickedPiece, possibleMoves, possibleCaptures))
                        {
                            board.render();
                            // currentThemeButton -> render();
                            board.updateCastlingStatus();
                            prevCoordinate = Coordinate(-1, -1);
                            pickedPiece = ' ';
                            if (currentMoveColor == WHITE)
                                currentMoveColor = BLACK;
                            else if (currentMoveColor == BLACK)
                                currentMoveColor = WHITE;
                            if (board.isStatemate(WHITE) || board.isStatemate(BLACK))
                            {
                                isEnded = true;
                                SDL_Log("End game: Statemate");
                            }
                            int opponentColor = 1 - currentMoveColor;
                            if (board.isCheckmate(opponentColor))
                            {
                                isEnded = true;
                                SDL_Log("End game: Checkmate");
                            }

                            std::cerr << "Statemate status: " << board.isStatemate(WHITE) << " and " << board.isStatemate(BLACK) << "\n";
                        }
                        else // invalid move
                        {
                            board.writeCell(prevCoordinate, pickedPiece);
                            board.render();
                            currentThemeButton->render();
                        }
                        board.log(event.button, "released");
                        break;
                    }
                        // default:
                        // board.present();
                    }
                }
                else
                {
                    board.setBackground(black);
                    board.clear();
                    switch (event.type)
                    {
                    case SDL_QUIT:
                        running = false;
                        break;
                    }
                }
                board.present();
                // board.log("Done checking stalemate");
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
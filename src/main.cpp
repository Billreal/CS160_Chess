#pragma once
#include <iostream>
#include <cmath>
// #define SDL_MAIN_HANDLED
#include <SDL.h>
#include "./test.cpp"
// #include "./../include/background.h"
#include "./../include/color.h"
#include "./../include/colorScheme.h"
#include "./../include/board.h"
#include "./../include/pieces.h"
#include "./../include/coordinate.h"
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

    renderer = SDL_CreateRenderer(window, -1, 0);
    // Yet another error handling
    if (!renderer)
    {
        SDL_Log("Failed to create renderer: %s", SDL_GetError());
        SDL_DestroyWindow(window);
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

    Board board(renderer, classicboard1Primary, classicBoard2Primary, bgColor);

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
    board.setColor(classicboard1Primary, classicBoard2Primary);
    board.renderPieces();
    board.render();
    board.present();
    vector<Coordinate> possibleMoves;
    vector<Coordinate> possibleCaptures;
    int currentMoveColor = WHITE;
    Coordinate enPassantCoord;
    bool isEnded = false;
    while (running)
    {
        // Check if the window is running or not
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
                        break;
                    Coordinate pickedPlace = board.getPieceCoord(event.button);
                    pickedPiece = board.getPiece(pickedPlace);
                    int pickedColor = board.getPieceColor(pickedPiece);
                    // if (pickedColor != currentMoveColor)
                    //     break;
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

                    // board.log("Done getting moves");
                    possibleCaptures = board.getPossibleCaptures(pickedPiece, prevCoordinate.getX(), prevCoordinate.getY());
                    // board.log("Done getting captures");
                    board.render();
                    // board.log("Done render");
                    board.present();
                    // board.log("Done present");
                    board.deleteCell(pickedPlace);
                    // board.log("Done delete");
                    break;
                }
                case SDL_MOUSEMOTION:
                {
                    if (isLeftMouseHolding == false) // Mouse hover
                        break;
                    board.render();
                    // board.log("Done render");
                    board.renderMove(possibleMoves, possibleCaptures);
                    board.renderPieceByCursor(pickedPiece, event.button.x, event.button.y);
                    // board.log("Done render animation");
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
                    }
                    // board.clear();
                    // std::cerr << "Dropped at " << droppedPlace.getX() << " " << droppedPlace.getY() << "\n";
                    board.log(event.button, "released");
                    board.present();
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
            }
            // board.log("Done checking stalemate");
        } while (SDL_PollEvent(&event) != 0);
    }

    // system("pause");
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
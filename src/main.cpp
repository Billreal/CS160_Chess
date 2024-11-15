#pragma once
#include <iostream>
// #define SDL_MAIN_HANDLED
#include <SDL.h>
#include "./test.cpp"
// #include "./../include/background.h"
#include "./../include/color.h"
#include "./../include/colorScheme.h"
#include "./../include/board.h"
// #include "./../include/pieces.h"
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

    Board board(renderer, board1Primary, board2Primary, bgColor);
    // ChessPieces pieces(renderer);

    // Handling SDL_events
    SDL_Event event;

    // Chessboard rendering

    // chessPieces chess(QUEEN, WHITE, 4, 4);
    // auto dbg = chess.listAllMove();
    // for (auto x : dbg)
    //     cerr << x.getX() << " " << x.getY() << "\n";

    board.renderChessboard(board1Primary, board2Primary);
    board.ConvertFEN();
    board.renderPieces(board1Primary, board2Primary, false);
    bool isLeftMouseHolding = false;
    Coordinate prevCoordinate(-1, -1);
    char pickedPiece = ' ';
    board.setColor(board1Primary, board2Primary);
    // board.renderPieces();
    board.render();
    board.present();
    while (running)
    {
        // Check if the window is running or not
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
                if (pickedPiece == '0')
                    break;
                isLeftMouseHolding = true;
                // cerr << pickedPiece << " " << selectedPlace.getX() << " " << selectedPlace.getY() << "\n";
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
                // Coordinate selectedPlace = board.getPieceCoord(event.button);
                Coordinate droppedPlace = board.getPieceCoord(event.button);
                board.writeCell(droppedPlace, pickedPiece);
                // board.clear();
                board.render();
                if (droppedPlace == prevCoordinate)
                {
                    // board.clear();
                    board.renderMove(pickedPiece, droppedPlace.getX(), droppedPlace.getY());
                }
                prevCoordinate = Coordinate(-1, -1);
                pickedPiece = ' ';
                // std::cerr << "Dropped at " << selectedPlace.getX() << " " << selectedPlace.getY() << "\n";
                // board.log(event.button, "released");
                board.present();
                break;
            }
                // default:
                // board.present();
            }
        } while (SDL_PollEvent(&event) != 0);
    }

    // system("pause");
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
#pragma once
#include <iostream>
// #define SDL_MAIN_HANDLED
#include <SDL.h>
#include "./test.cpp"
#include "./../include/background.h"
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

    Background background(renderer);
    background.render(bgColor);

    Board board(renderer);

    // Handling SDL_events
    SDL_Event event;

    // Chessboard rendering

    // chessPieces chess(QUEEN, WHITE, 4, 4);
    // auto dbg = chess.listAllMove();
    // for (auto x : dbg)
    //     cerr << x.getX() << " " << x.getY() << "\n";

    board.renderChessboard(board1Primary, board2Primary);
    board.renderPieces();
    bool isLeftMouseHolding = false;
    while (running)
    {
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
                if (event.button.button != SDL_BUTTON_LEFT)
                    break;
                isLeftMouseHolding = true;
                Coordinate selectedPiece = board.getPressedPieceCoord(event.button);
                std::cerr << "Picked up at " << selectedPiece.getX() << " " << selectedPiece.getY() << "\n";
                // board.log(event.button, "pressed");
                break;
            }
            case SDL_MOUSEMOTION:
            {
                if (isLeftMouseHolding == false) // Mouse hover
                {
                    // board.log(event.button, "hovering");

                    break;
                }
                // board.log(event.button, "dragging"); // Mouse drag

                break;
            }
            case SDL_MOUSEBUTTONUP:
            {
                if (event.button.button != SDL_BUTTON_LEFT)
                    break;
                isLeftMouseHolding = false;
                Coordinate selectedPiece = board.getPressedPieceCoord(event.button);
                std::cerr << "Dropped at " << selectedPiece.getX() << " " << selectedPiece.getY() << "\n";
                // board.log(event.button, "released");
                break;
            }
            }
        }
        // board.flush();
    }

    // system("pause");
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    // TODO: Render column and row indexing correctly
    return 0;
}
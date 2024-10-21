#pragma once
#include <iostream>
// #define SDL_MAIN_HANDLED
#include <SDL.h>
#include "./test.cpp"
#include "./../include/background.h"
#include "./../include/color.h"
#include "./../include/colorScheme.h"
#include "./../include/board.h"
using std::cerr, std::cout;
SDL_Renderer *renderer;
SDL_Window *window;
SDL_Surface *winSurface;
int main(int argc, char *args[])
{
    // ! Temporary variable, will change in later version
    bool isPlayer1White = false;
    bool isAgainstBot   = true;
    bool isToRotate     = (isPlayer1White == false) && isAgainstBot;
    // ! End of temporary variable
    debug();

    renderer = NULL;
    window = NULL;
    SDL_Init(SDL_INIT_EVERYTHING);

    window = SDL_CreateWindow("VLXD", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 700, 700, SDL_WINDOW_SHOWN);

    renderer = SDL_CreateRenderer(window, -1, 0);

    Background background(renderer);
    background.render(bgColor);

    Board board(renderer);
    board.renderChessboard(board1Primary, board2Primary);
    system("pause");
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    // TODO: Render column and row indexing correctly
    return 0;
}
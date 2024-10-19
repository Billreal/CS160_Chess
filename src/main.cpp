#pragma once
#include <iostream>
// #define SDL_MAIN_HANDLED
#include <SDL.h>
#include "./test.cpp"
#include "./../include/board.h"
#include "./../include/color.h"
#include "./../include/colorScheme.h"
using std::cerr, std::cout;
SDL_Renderer *renderer;
SDL_Window *window;
SDL_Surface *winSurface;
int main(int argc, char *args[])
{
    debug();

    renderer = NULL;
    window = NULL;
    SDL_Init(SDL_INIT_EVERYTHING);

    window = SDL_CreateWindow("VLXD", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 600, 600, SDL_WINDOW_SHOWN);

    renderer = SDL_CreateRenderer(window, -1, 0);

    Background background(renderer);
    background.render(board1Primary);

    system("pause");
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
#pragma once
#include <iostream>
// #define SDL_MAIN_HANDLED
#include <SDL.h>
#include "./test.cpp"
#include "./../include/board.h"
#include "./../include/color.h"
#include "./../include/colorScheme.h"
// #include "./color.cpp"
using std::cerr, std::cout;
SDL_Surface *winSurface;
SDL_Window *window;
// void initSDL()
int main(int argc, char *args[])
{
    debug();
    // return 0;

    winSurface = NULL;
    window = NULL;
    SDL_Init(SDL_INIT_EVERYTHING);
 
    window = SDL_CreateWindow("VLXD", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 800, SDL_WINDOW_SHOWN);
 
    // Board chessboard(window);
    winSurface = SDL_GetWindowSurface(window);

    Background background(winSurface);
 
    // SDL_FillRect(winSurface, NULL, SDL_MapRGB(winSurface->format, 0, 255, 255));

    // background.render(bgColor);
    // Update the window display
    SDL_UpdateWindowSurface(window);

    // Wait
    system("pause");

    // Destroy the window. This will also destroy the surface
    SDL_DestroyWindow(window);

    // Quit SDL
    SDL_Quit();
    return 0;
}
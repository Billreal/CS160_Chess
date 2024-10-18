#include <iostream>
// #define SDL_MAIN_HANDLED
#include <SDL.h>

using std::cerr, std::cout;
SDL_Surface *winSurface;
SDL_Window *window;
// void initSDL()
int main(int argc, char *args[])
{
    winSurface = NULL;
    window = NULL;
    SDL_Init(SDL_INIT_EVERYTHING);
    window = SDL_CreateWindow("Example", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1280, 720, SDL_WINDOW_SHOWN);
    winSurface = SDL_GetWindowSurface(window);
    SDL_FillRect(winSurface, NULL, SDL_MapRGB(winSurface->format, 0, 255, 255));

    // Update the window display
    SDL_UpdateWindowSurface(window);

    // Wait
    system("pause");

    // Destroy the window. This will also destroy the surface
    SDL_DestroyWindow(window);

    // Quit SDL
    SDL_Quit();
    return 0;
    // if (SDL_Init(SDL_INIT_EVERYTHING) == -1)
    // {
    //     cerr << "Initialization failed\n";
    // }
}
// {
//     initSDL();
//     std::cout << "succ\n";
//     return 0;
// }
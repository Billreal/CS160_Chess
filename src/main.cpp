#include <iostream>
// #define SDL_MAIN_HANDLED
#include <SDL.h>

using std::cerr, std::cout;
SDL_Surface* winSurface;
SDL_Window* window;
// void initSDL()
int main(int argc, char* args[])
{
    winSurface = NULL;
    window = NULL;
    SDL_Init(SDL_INIT_EVERYTHING);
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
#pragma once
#include <SDL.h>
#include "./colorScheme.h"
class Board
{
private:
    SDL_Renderer *renderer = NULL;
public:
    Board(SDL_Renderer *renderer);
    void renderChessboard(colorRGBA primary, colorRGBA secondary);
};
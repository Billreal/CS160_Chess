#pragma once
#include <SDL.h>
#include "./colorScheme.h"
class Board
{
private:
    SDL_Renderer *renderer = NULL;
    const int MARGIN = 80;
    const int SIDE_LENGTH = 70;
    const int BOARD_SIZE = 8;

public:
    Board(SDL_Renderer *renderer);
    void renderChessboard(colorRGBA primary, colorRGBA secondary);
    void renderIndex(colorRGBA primary, colorRGBA secondary, bool rotationFlag);
    int getMargin();
    int getSideLength();
};
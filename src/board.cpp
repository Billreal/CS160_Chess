#pragma once
#include "./../include/board.h"

Board::Board(SDL_Renderer *renderer)
{
    this -> renderer = renderer;
}

void Board::renderChessboard(colorRGBA primary, colorRGBA secondary)
{
    SDL_SetRenderDrawColor(renderer, primary.getR(), primary.getG(), primary.getB(), primary.getA());
    for (int i = 1; i <= 8; i++)
        for (int j = 1; j <= 8; j++)
        {
            bool cellType = (i + j) % 2;
            int currentX = margin + sideLength * (i - 1);
            int currentY = margin + sideLength * (j - 1);
            SDL_Rect currentCell{currentX, currentY, sideLength, sideLength};

            if (cellType) SDL_SetRenderDrawColor(renderer, primary.getR(), primary.getG(), primary.getB(), primary.getA());
            else SDL_SetRenderDrawColor(renderer, secondary.getR(), secondary.getG(), secondary.getB(), secondary.getA());
            SDL_RenderFillRect(renderer, &currentCell);
        }
    SDL_RenderPresent(renderer);
}
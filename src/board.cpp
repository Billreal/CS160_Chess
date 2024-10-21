#pragma once
#include "./../include/board.h"

Board::Board(SDL_Renderer *renderer)
{
    this -> renderer = renderer;
}

void Board::renderChessboard(colorRGBA primary, colorRGBA secondary)
{
    SDL_SetRenderDrawColor(renderer, primary.getR(), primary.getG(), primary.getB(), primary.getA());
    for (int i = 1; i <= BOARD_SIZE; i++)
        for (int j = 1; j <= BOARD_SIZE; j++)
        {
            bool cellType = (i + j) % 2;
            int currentX = MARGIN + SIDE_LENGTH * (i - 1);
            int currentY = MARGIN + SIDE_LENGTH * (j - 1);
            SDL_Rect currentCell{currentX, currentY, SIDE_LENGTH, SIDE_LENGTH};
            // cellType = cellType ^ rotationFlag;
            if (cellType) SDL_SetRenderDrawColor(renderer, primary.getR(), primary.getG(), primary.getB(), primary.getA());
            else SDL_SetRenderDrawColor(renderer, secondary.getR(), secondary.getG(), secondary.getB(), secondary.getA());
            SDL_RenderFillRect(renderer, &currentCell);
        }
    SDL_RenderPresent(renderer);
}

void Board::renderIndex(colorRGBA primary, colorRGBA secondary, bool rotationFlag)
{
    // i is row index
    // j is column index
    // * Bottom to up, left to right
    for (int i = 1; i <= BOARD_SIZE; i++)
        for (int j = 1; j <= BOARD_SIZE; j++)
        {
            if (j != 1 || i != BOARD_SIZE) continue;
            bool cellType = (i + j) % 2 ^ 1; // As to contrast the cell's color
            int currentX = MARGIN + SIDE_LENGTH * (i - 1);
            int currentY = MARGIN + SIDE_LENGTH * (j - 1);
            // if (i == BOARD_SIZE)

            // if (j == 1)

        }
}

int Board::getMargin()
{
    return MARGIN;
}

int Board::getSideLength()
{
    return SIDE_LENGTH;
}
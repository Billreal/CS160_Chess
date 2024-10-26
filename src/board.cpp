#pragma once
#include "./../include/board.h"

Board::Board(SDL_Renderer *renderer):renderer(renderer){
    loadTextures();
}

void Board::loadTextures() {
    pieces[0][0] = loadTexture("assets/white_pawn.png");
    pieces[1][0] = loadTexture("assets/black_pawn.png");
}

void Board::render() {
    drawTexture(pieces[0][1], MARGIN, MARGIN, SIDE_LENGTH, SIDE_LENGTH);
    // drawTexture(pieces[0][1], MARGIN, MARGIN, SIDE_LENGTH, SIDE_LENGTH);
}

void Board::renderChessboard(colorRGBA primary, colorRGBA secondary)
{
    //Initialize color for 1st cell as primary color
    SDL_SetRenderDrawColor(renderer, primary.getR(), primary.getG(), primary.getB(), primary.getA());
    for (int i = 1; i <= BOARD_SIZE; i++)
        for (int j = 1; j <= BOARD_SIZE; j++)
        {
            bool cellType = (i + j) % 2;
            int currentX = MARGIN + SIDE_LENGTH * (i - 1);
            int currentY = MARGIN + SIDE_LENGTH * (j - 1);

            //A rectangle, with the origin at the upper left (integer).
            SDL_Rect currentCell{currentX, currentY, SIDE_LENGTH, SIDE_LENGTH};

            // cellType = cellType ^ rotationFlag;
            if (cellType) SDL_SetRenderDrawColor(renderer, primary.getR(), primary.getG(), primary.getB(), primary.getA());
            else SDL_SetRenderDrawColor(renderer, secondary.getR(), secondary.getG(), secondary.getB(), secondary.getA());
            //Fill a rectangle on the current rendering target with the drawing color.
            SDL_RenderFillRect(renderer, &currentCell);
        }
    // Update the screen with any rendering performed since the previous call.
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
#pragma once
#include <iostream>
#include "./../include/board.h"

Board::Board(SDL_Renderer *renderer) : renderer(renderer)
{
    loadTextures();
}

void Board::loadTextures()
{
    pieces[0][0] = loadTexture("./assets/white_pawn.png");
    pieces[0][1] = loadTexture("./assets/white_rook.png");
    pieces[0][2] = loadTexture("./assets/white_knight.png");
    pieces[0][3] = loadTexture("./assets/white_bishop.png");
    pieces[0][4] = loadTexture("./assets/white_queen.png");
    pieces[0][5] = loadTexture("./assets/white_king.png");
    pieces[1][0] = loadTexture("./assets/black_pawn.png");
    pieces[1][1] = loadTexture("./assets/black_rook.png");
    pieces[1][2] = loadTexture("./assets/black_knight.png");
    pieces[1][3] = loadTexture("./assets/black_bishop.png");
    pieces[1][4] = loadTexture("./assets/black_queen.png");
    pieces[1][5] = loadTexture("./assets/black_king.png");
}

void Board::renderPieces()
{
    // std::cerr << "Begin rendering\n";
    // drawTexture(pieces[0][1], MARGIN, MARGIN, SIDE_LENGTH, SIDE_LENGTH);
    drawTexture(pieces[0][0], MARGIN, MARGIN, SIDE_LENGTH, SIDE_LENGTH);
    drawTexture(pieces[1][0], MARGIN + 70, MARGIN + 70, SIDE_LENGTH, SIDE_LENGTH);
    // std::cerr << "Rendering Done\n";
    // flush();
}

void Board::renderChessboard(colorRGBA primary, colorRGBA secondary)
{
    // Initialize color for 1st cell as primary color
    setRendererColor(renderer, primary);
    for (int i = 1; i <= BOARD_SIZE; i++)
        for (int j = 1; j <= BOARD_SIZE; j++)
        {
            bool cellType = (i + j) % 2;
            int currentX = MARGIN + SIDE_LENGTH * (i - 1);
            int currentY = MARGIN + SIDE_LENGTH * (j - 1);

            // A rectangle, with the origin at the upper left (integer).
            SDL_Rect currentCell{currentX, currentY, SIDE_LENGTH, SIDE_LENGTH};

            // cellType = cellType ^ rotationFlag;
            if (cellType)
                setRendererColor(renderer, primary);
            else
                setRendererColor(renderer, secondary);
            // Fill a rectangle on the current rendering target with the drawing color.
            SDL_RenderFillRect(renderer, &currentCell);
        }
    // Update the screen with any rendering performed since the previous call.
    // flush();
}

void Board::renderIndex(colorRGBA primary, colorRGBA secondary, bool rotationFlag)
{
    // i is row index
    // j is column index
    // * Bottom to up, left to right
    for (int i = 1; i <= BOARD_SIZE; i++)
        for (int j = 1; j <= BOARD_SIZE; j++)
        {
            if (j != 1 || i != BOARD_SIZE)
                continue;
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

void Board::setRendererColor(SDL_Renderer *renderer, colorRGBA color)
{
    SDL_SetRenderDrawColor(renderer, color.getR(), color.getG(), color.getB(), color.getA());
}

void Board::flush()
{
    SDL_RenderPresent(renderer);
}
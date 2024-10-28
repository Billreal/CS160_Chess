#pragma once
#include <iostream>
#include "./../include/board.h"
#include <math.h>
#include <string.h>
#include <stdio.h>

#define NANOSVG_IMPLEMENTATION
#define NANOSVGRAST_IMPLEMENTATION
#include "./../include/nanosvg.h"
#include "./../include/nanosvgrast.h"

Board::Board(SDL_Renderer *renderer) : renderer(renderer)
{
    loadTextures();
}

SDL_Texture *Board::loadTexture(const char *filePath, int width, int height)
{
    struct NSVGimage *image = nsvgParseFromFile(filePath, "px", 96);
    if (!image)
    {
        printf("Failed to load SVG file.\n");
        return nullptr;
    }

    // Rasterize SVG
    struct NSVGrasterizer *rast = nsvgCreateRasterizer();
    unsigned char *imageData = (unsigned char *)malloc(width * height * 4); // RGBA buffer
    nsvgRasterize(rast, image, 0, 0, 1, imageData, width, height, width * 4);

    // Create SDL surface and texture
    SDL_Surface *surface = SDL_CreateRGBSurfaceFrom(
        imageData, width, height, 32, width * 4,
        0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

    // Cleanup
    SDL_FreeSurface(surface);
    free(imageData);
    nsvgDeleteRasterizer(rast);
    nsvgDelete(image);

    return texture;
}

// SDL_Texture *Board::loadTexture(const std::string &path)
// {
//     SDL_Surface *surface = IMG_Load(path.c_str());
//     // Check if surface is loaded
//     if (!surface)
//     {
//         SDL_Log("Failed to load texture %s: %s", path.c_str(), SDL_GetError());
//         return nullptr;
//     }
//     TextureList.push_back(SDL_CreateTextureFromSurface(renderer, surface));
//     SDL_Texture *texture = TextureList.back();
//     SDL_FreeSurface(surface);
//     return texture;
// }

void Board::loadTextures()
{
    pieces[0][0] = loadTexture("./assets/white_pawn.svg", SIDE_LENGTH, SIDE_LENGTH);
    pieces[1][0] = loadTexture("./assets/black_pawn.svg", SIDE_LENGTH, SIDE_LENGTH);
    // pieces[0][0] = loadTexture("./assets/white_pawn.png");
    // pieces[1][0] = loadTexture("./assets/black_pawn.png");
}

void Board::render()
{
    std::cerr << "Begin rendering\n";
    // drawTexture(pieces[0][1], MARGIN, MARGIN, SIDE_LENGTH, SIDE_LENGTH);
    drawTexture(pieces[0][0], MARGIN, MARGIN, SIDE_LENGTH, SIDE_LENGTH);
    drawTexture(pieces[1][0], MARGIN + 70, MARGIN + 70, SIDE_LENGTH, SIDE_LENGTH);
    std::cerr << "Rendering Done\n";
    flush();
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
    flush();
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
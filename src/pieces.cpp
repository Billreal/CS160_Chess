#pragma once

#include <algorithm>
#include "./../include/pieces.h"
#include <string.h>
#include <stdio.h>

#define NANOSVG_IMPLEMENTATION
#define NANOSVGRAST_IMPLEMENTATION
#include "./../include/nanosvg.h"
#include "./../include/nanosvgrast.h"

ChessPieces::ChessPieces(SDL_Renderer *renderer) : renderer(renderer)
{
    loadTextures();
}

// void ChessPieces::loadTextures()
// {
// textures[WHITE][PAWN] = loadTexture("./assets/white_pawn.svg", SIDE_LENGTH, SIDE_LENGTH);
// textures[BLACK][PAWN] = loadTexture("./assets/black_pawn.svg", SIDE_LENGTH, SIDE_LENGTH);
// textures[WHITE][ROOK] = loadTexture("./assets/white_rook.svg", SIDE_LENGTH, SIDE_LENGTH);
// textures[BLACK][ROOK] = loadTexture("./assets/black_rook.svg", SIDE_LENGTH, SIDE_LENGTH);
// textures[WHITE][KNIGHT] = loadTexture("./assets/white_knight.svg", SIDE_LENGTH, SIDE_LENGTH);
// textures[BLACK][KNIGHT] = loadTexture("./assets/black_knight.svg", SIDE_LENGTH, SIDE_LENGTH);
// textures[WHITE][BISHOP] = loadTexture("./assets/white_bishop.svg", SIDE_LENGTH, SIDE_LENGTH);
// textures[BLACK][BISHOP] = loadTexture("./assets/black_bishop.svg", SIDE_LENGTH, SIDE_LENGTH);
// textures[WHITE][QUEEN] = loadTexture("./assets/white_queen.svg", SIDE_LENGTH, SIDE_LENGTH);
// textures[BLACK][QUEEN] = loadTexture("./assets/black_queen.svg", SIDE_LENGTH, SIDE_LENGTH);
// textures[WHITE][KING] = loadTexture("./assets/white_king.svg", SIDE_LENGTH, SIDE_LENGTH);
// textures[BLACK][KING] = loadTexture("./assets/black_king.svg", SIDE_LENGTH, SIDE_LENGTH);
// }

void ChessPieces::update(chessName name, chessColor color, int row, int column)
{
    ChessPieces::name = name;
    ChessPieces::color = color;
    const string PATH = "./assets/" + nameMap[name] + "_" + colorMap[color] + ".svg";
    texture = loadTexture(PATH.c_str(), SIDE_LENGTH, SIDE_LENGTH);
    coordinate = Coordinate(row, column);
}

ChessPieces::ChessPieces(chessName name, chessColor color, int row, int column) : name(name), color(color)
{
    // string path = "./assets/" + color + "_" + name + ".svg";
    // texture = loadTexture(path.c_str(), SIDE_LENGTH, SIDE_LENGTH);
    coordinate = Coordinate(row, column);
}

SDL_Texture *ChessPieces::loadTexture(const char *filePath, int width, int height)
{
    struct NSVGimage *image = nsvgParseFromFile(filePath, "px", 96);
    if (!image)
    {
        printf("Failed to load SVG file.\n");
        return nullptr;
    }

    // Rasterize SVG
    struct NSVGrasterizer *rast = nsvgCreateRasterizer();
    unsigned char *imageData = (unsigned char *)malloc(width * height * 10); // RGBA buffer
    nsvgRasterize(rast, image, 0, 0, IMG_SCALE, imageData, width, height, width * 4);

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

SDL_Texture ChessPieces::*getTexture(){
    // SDL_Texture *texture = loadTexture("./assets/black_king.svg", SIDE_LENGTH, SIDE_LENGTH);
    return ChessPieces::texture;
}

int ChessPieces::getX()
{
    return coordinate.getX();
}

int ChessPieces::getY()
{
    return coordinate.getY();
}

bool ChessPieces::isInRange(Coordinate coord)
{
    if (coord.getX() < 1 || coord.getX() > 8 || coord.getY() < 1 || coord.getY() > 8)
        return false;
    return true;
}

void ChessPieces::addCell(vector<Coordinate> &vect, const Coordinate &coord)
{
    if (isInRange(coord))
        vect.push_back(coord);
}
vector<Coordinate> ChessPieces::listAllMove()
{
    // * This piece of code is temporary, further complex move of first pawn movement, pawn capturing, castling, and en passant
    // * This only list possible cell in next move, regardless of cell's occupation
    vector<Coordinate> res;
    switch (name)
    {
    case PAWN: // PAWN
        if (color == WHITE)
        {
            addCell(res, coordinate + Coordinate(0, 1));
        }
        else if (color == BLACK)
        {
            addCell(res, coordinate + Coordinate(0, -1));
        }
        break;
    case ROOK: // ROOK
        for (int i = -8; i <= 8; i++)
        {
            if (i == 0)
                continue;
            addCell(res, coordinate + Coordinate(0, i));
            addCell(res, coordinate + Coordinate(i, 0));
        }
        break;
    case KNIGHT: // KNIGHT
        for (int i = -2; i <= 2; i++)
            for (int j = -2; j <= 2; j++)
            {
                if (abs(i) + abs(j) != 3)
                    continue;
                addCell(res, coordinate + Coordinate(i, j));
            }
        break;
    case BISHOP:
        for (int i = -8; i <= 8; i++)
        {
            if (i == 0)
                continue;
            addCell(res, coordinate + Coordinate(1, 1) * i);
            addCell(res, coordinate + Coordinate(1, -1) * i);
        }
        break;
    case QUEEN:
        for (int i = -8; i <= 8; i++)
        {
            if (i == 0)
                continue;
            addCell(res, coordinate + Coordinate(1, 1) * i);
            addCell(res, coordinate + Coordinate(-1, 1) * i);
            addCell(res, coordinate + Coordinate(0, 1) * i);
            addCell(res, coordinate + Coordinate(1, 0) * i);
        }
        break;
    case KING:
        for (int i = -1; i <= 1; i++)
            for (int j = -1; j <= 1; j++)
                if (!(i == 0 && j == 0))
                    addCell(res, coordinate + Coordinate(i, j));
        break;
    default:

        break;
    }
    std::sort(res.begin(), res.end());
    return res;
}
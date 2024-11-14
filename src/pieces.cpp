#pragma once

#include <algorithm>
#include <iostream>
#include "./../include/pieces.h"
#include <string.h>
#include <stdio.h>

// ChessPieces::ChessPieces(SDL_Renderer *renderer) : renderer(renderer)
// {
//     initMap();
// }

void ChessPieces::update(chessColor color, chessName name, int row, int column)
{
    ChessPieces::name = name;
    ChessPieces::color = color;
    coordinate = Coordinate(row, column);
}

void ChessPieces::update(chessColor color, chessName name)
{
    ChessPieces::name = name;
    ChessPieces::color = color;
}

void ChessPieces::update(SDL_Renderer *renderer)
{
    ChessPieces::renderer = renderer;
    // std::cerr << "Renderer updated for " << getColor() << " " << getName() << "\n";
}

const std::string ChessPieces::getTexturePath()
{
    if (color == COLOR_NONE || name == CHESS_NONE)
        return "";

    std::string PATH = "./assets/";

    if (color == WHITE)  
    {
        PATH += "white_";
    }
    else
        PATH += "black_";


    switch (name)
    {
    case ROOK:
        PATH += "rook";
        break;
    case KNIGHT:
        PATH += "knight";
        break;
    case BISHOP:
        PATH += "bishop";
        break;
    case QUEEN:
        PATH += "queen";
        break;
    case KING:
        PATH += "king";
        break;
    case PAWN:
        PATH += "pawn";
        break;
    }
    PATH += ".svg";

    return PATH;
}

chessColor ChessPieces::getColor()
{
    return color;
}

chessName ChessPieces::getName()
{
    return name;
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
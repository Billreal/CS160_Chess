#pragma once

#include <algorithm>
#include <iostream>
#include "./../include/pieces.h"
#include <string.h>
#include <stdio.h>
#include <vector>

// chessPieces::chessPieces(chessName name, chessColor color, int x, int y) : name(name), color(color)
// {
//     coordinate = Coordinate(x, y);
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

// chessPieces::chessPieces()
// {
//     name = CHESS_NONE;
//     color = COLOR_NONE;
//     coordinate = Coordinate(-1, -1);
// }

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
    if (coord.getX() < 0 || coord.getX() > 7 || coord.getY() < 0 || coord.getY() > 7)
        return false;
    return true;
}

void ChessPieces::addCell(vector<Coordinate> &vect, const Coordinate &coord)
{
    if (isInRange(coord))
        vect.push_back(coord);
}
vector<vector<Coordinate>> chessPieces::listAllMove()
{
    // * This piece of code is temporary, further complex move of first pawn movement, pawn capturing, castling, and en passant
    // * This only list possible cell in next move, regardless of cell's occupation
    vector<vector<Coordinate>> res;
    switch (name)
    {
    case PAWN:
    {
        res.resize(1);
        if (color == BLACK)
        {
            addCell(res[0], coordinate + Coordinate(0, 1));
            if (coordinate.getY() == 1)
                addCell(res[0], coordinate + Coordinate(0, 2));
        }
        else
        {
            addCell(res[0], coordinate + Coordinate(0, -1));
            if (coordinate.getY() == 6)
                addCell(res[0], coordinate + Coordinate(0, -2));
        }
        break;
    }
    case ROOK:
    {
        res.resize(4);
        for (int i = 1; i <= 8; i++)
        {
            addCell(res[0], coordinate + Coordinate(0, i));
            addCell(res[1], coordinate + Coordinate(i, 0));
            addCell(res[2], coordinate + Coordinate(0, -i));
            addCell(res[3], coordinate + Coordinate(-i, 0));
        }
        break;
    }
    case KNIGHT:
    {
        res.resize(8);
        int idx = 0;
        int dx[8] = {-2, -1, 1, 2, 2, 1, -1, -2};
        int dy[8] = {-1, -2, -2, -1, 1, 2, 2, 1};
        for (int i = 0; i < 8; i++)
            addCell(res[i], coordinate + Coordinate(dx[i], dy[i]));
        break;
    }

    case BISHOP:
    {
        res.resize(4);
        for (int i = 1; i <= 8; i++)
        {
            addCell(res[0], coordinate + Coordinate(i, i));
            addCell(res[1], coordinate + Coordinate(i, -i));
            addCell(res[2], coordinate + Coordinate(-i, i));
            addCell(res[3], coordinate + Coordinate(-i, -i));
        }
        break;
    }
    case QUEEN:
    {
        res.resize(8);
        for (int i = 1; i <= 8; i++)
        {
            addCell(res[0], coordinate + Coordinate(0, i));
            addCell(res[1], coordinate + Coordinate(i, i));
            addCell(res[2], coordinate + Coordinate(i, 0));
            addCell(res[3], coordinate + Coordinate(i, -i));
            addCell(res[4], coordinate + Coordinate(0, -i));
            addCell(res[5], coordinate + Coordinate(-i, -i));
            addCell(res[6], coordinate + Coordinate(-i, 0));
            addCell(res[7], coordinate + Coordinate(-i, i));
        }
        break;
    }
    case KING:
    {
        res.resize(8);
        addCell(res[0], coordinate + Coordinate(0, 1));
        addCell(res[1], coordinate + Coordinate(1, 1));
        addCell(res[2], coordinate + Coordinate(1, 0));
        addCell(res[3], coordinate + Coordinate(1, -1));
        addCell(res[4], coordinate + Coordinate(0, -1));
        addCell(res[5], coordinate + Coordinate(-1, -1));
        addCell(res[6], coordinate + Coordinate(-1, 0));
        addCell(res[7], coordinate + Coordinate(-1, 1));
    }
    }
    return res;
}

vector<vector<Coordinate>> chessPieces::listAllMove(Coordinate current)
{
    Coordinate prevCoord = coordinate;
    coordinate = current;
    vector<vector<Coordinate>> result = listAllMove();
    coordinate = prevCoord;
    return result;
}
vector<vector<Coordinate>> chessPieces::listAllMove(int name, int color, int coordX, int coordY)
{
    Coordinate prevCoord = coordinate;
    chessName prevName = this->name;
    chessColor prevColor = this->color;
    
    coordinate = Coordinate(coordX, coordY);
    this->name = chessName(name);
    this->color = chessColor(color);
    
    vector<vector<Coordinate>> result = listAllMove();
    
    coordinate = prevCoord;
    this->name = prevName;
    this->color = prevColor;
    
    return result;
}

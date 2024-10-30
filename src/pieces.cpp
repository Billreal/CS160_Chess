#include <algorithm>
#include "./../include/pieces.h"

chessPieces::chessPieces(chessName name, chessColor color, int x, int y) : name(name), color(color)
{
    coordinate = Coordinate(x, y);
}
chessPieces::chessPieces(int x, int y)
{
    name = CHESS_NONE;
    color = COLOR_NONE;
    coordinate = Coordinate(x, y);
}

int chessPieces::getX()
{
    return coordinate.getX();
}

int chessPieces::getY()
{
    return coordinate.getY();
}

bool chessPieces::isInRange(Coordinate coord)
{
    if (coord.getX() < 1 || coord.getX() > 8 || coord.getY() < 1 || coord.getY() > 8)
        return false;
    return true;
}

void chessPieces::addCell(vector<Coordinate> &vect, const Coordinate &coord)
{
    if (isInRange(coord))
        vect.push_back(coord);
}
vector<Coordinate> chessPieces::listAllMove()
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
            if (i == 0) continue;
            addCell(res, coordinate + Coordinate(1, 1) * i);
            addCell(res, coordinate + Coordinate(1, -1) * i);
        }
        break;
    case QUEEN:
        for (int i = -8; i <= 8; i++)
        {
            if (i == 0) continue;
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
#pragma once

#include <SDL_image.h>
#include <SDL.h>
#include <string>
#include <vector>
#include "coordinate.h"
using std::string;
using std::vector;
enum chessName
{
    CHESS_NONE = -1,
    ROOK,
    KNIGHT,
    BISHOP,
    QUEEN,
    KING,
    PAWN,
};

enum chessColor
{
    COLOR_NONE = -1,
    WHITE,
    BLACK,
};

class ChessPieces
{
private:
    std::string nameMap[7];
    std::string colorMap[3];

    SDL_Renderer *renderer = NULL;

    Coordinate coordinate;
    chessName name;
    chessColor color;

    bool isInRange(Coordinate coord);
    void addCell(vector<Coordinate> &vect, const Coordinate &coord);

public:
    // ChessPieces(SDL_Renderer *renderer);

    SDL_Texture *loadTexture(const char *filePath, int width, int height);

    void update(chessColor color, chessName name, int row, int column);
    void update(chessColor color, chessName name);
    void update(SDL_Renderer *renderer);

    // Get Infos
    const std::string getTexturePath();
    chessColor getColor();
    chessName getName();
    int getX();
    int getY();
    vector<vector<Coordinate>> listAllMove();
    vector<vector<Coordinate>> listAllMove(Coordinate current);
    vector<vector<Coordinate>> listAllMove(int name, int color, int coordX, int coordY);
};
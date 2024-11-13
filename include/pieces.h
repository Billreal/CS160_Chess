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
class chessPieces
{
private:
    chessName name;
    Coordinate coordinate;
    chessColor color;
    bool isInRange(Coordinate coord);
    void addCell(vector<Coordinate> &vect, const Coordinate &coord);

public:
    chessPieces(chessName name, chessColor color, int x, int y);
    chessPieces(int x, int y);
    int getX();
    int getY();
    vector<vector<Coordinate>> listAllMove();
    vector<vector<Coordinate>> listAllMove(Coordinate current);
};
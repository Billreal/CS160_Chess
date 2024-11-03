#include <string>
#include <vector>
#include "coordinate.h"
using std::string;
using std::vector;
enum chessName 
{
    CHESS_NONE,
    PAWN,
    ROOK,
    KNIGHT,
    BISHOP,
    QUEEN,
    KING,
};

enum chessColor
{
    COLOR_NONE,
    BLACK,
    WHITE,
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
    vector<Coordinate> listAllMove(); 
    vector<Coordinate> listAllMove(Coordinate current);

};
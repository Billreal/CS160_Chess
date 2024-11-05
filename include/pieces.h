#include <SDL_image.h>
#include <SDL.h>
#include <string>
#include <vector>
#include <map>
#include "coordinate.h"
#include "board.h"
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
    
    std::map<int, std::string> nameMap;
    std::map<int, string> colorMap;

    SDL_Renderer *renderer = NULL;
    Coordinate coordinate;
    chessName name;
    chessColor color;
    const int MARGIN = 80;
    const int SIDE_LENGTH = 70;
    const int BOARD_SIZE = 8;

    const float IMG_SCALE = 1.5;

    bool isInRange(Coordinate coord);
    void addCell(vector<Coordinate> &vect, const Coordinate &coord);

public:
    ChessPieces(SDL_Renderer *renderer);


    SDL_Texture *loadTexture(const char *filePath, int width, int height);

    void update(chessColor color, chessName name, int row, int column);
    void update(chessColor color, chessName name);
    void update(SDL_Renderer *renderer);
    void initMap();
    
    // Get Infos
    SDL_Texture *getTexture();
    chessColor getColor();
    chessName getName();
    int getX();
    int getY();
    vector<Coordinate> listAllMove();
};
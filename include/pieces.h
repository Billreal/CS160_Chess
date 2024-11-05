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

class ChessPieces
{
private:
    
    std::map<int, std::string> nameMap;
    // nameMap[0] = "NONE";
    nameMap[PAWN] = "PAWN";
    nameMap[ROOK] = "ROOK";
    nameMap[KNIGHT] = "KNIGHT";
    nameMap[BISHOP] = "BISHOP";
    nameMap[QUEEN] = "QUEEN";
    nameMap[KING] = "KING";
    std::map<int, string> colorMap;
    // colorMap[COLOR_NONE] = "NONE";
    // colorMap[BLACK] = "BLACK";
    // colorMap[WHITE] = "WHITE";

    SDL_Renderer *renderer = NULL;
    Coordinate coordinate;
    chessName name;
    chessColor color;
    SDL_Texture *texture;
    const int MARGIN = 80;
    const int SIDE_LENGTH = 70;
    const int BOARD_SIZE = 8;

    const float IMG_SCALE = 1.5;

    bool isInRange(Coordinate coord);
    void addCell(vector<Coordinate> &vect, const Coordinate &coord);

public:
    ChessPieces(SDL_Renderer *renderer);
    ChessPieces(chessName name, chessColor color, int row, int column);

    SDL_Texture *loadTexture(const char *filePath, int width, int height);

    void update(chessName name, chessColor color, int row, int column);
    void loadTextures();
    
    // Get Infos
    SDL_Texture *getTexture();
    int getX();
    int getY();
    vector<Coordinate> listAllMove();
};
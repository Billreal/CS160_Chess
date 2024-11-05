#pragma once

#include <SDL_image.h>
#include <SDL.h>
#include <math.h>
#include <utility>
#include <string.h>
#include <vector>
#include <map>
#include "colorScheme.h"
#include "coordinate.h"
#include "pieces.h"

class Board
{
private:
    SDL_Renderer *renderer = NULL;
    const int MARGIN = 80;
    const int SIDE_LENGTH = 70;
    const int BOARD_SIZE = 8;

    std::map<int, ChessPieces> BOARD;

    // Pieces
    std::map<int, ChessPieces> pieces;

    // Board status, contains playerTurn, Castling, En Passant, half and total moves.
    int isPlayerTurn; // contains either 0 or 1, whiÍÍÍch 1 stands for white turn, 0 for black turn
    bool whiteKingSide;
    bool whiteQueenSide;
    bool blackKingSide;
    bool blackQueenSide;
    bool enPassant;
    int halfmoves = 0;
    int totalmoves = 0;

    // FEN Notation
    std::string boardSequence[6] = {""};
    std::string STARTING_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    // std::string STARTING_FEN = "rnb1kbnr/ppppqppp/8/4N3/4P3/8/PPP2PPP/R1BQKB1R w KQkq - 0 1";
    std::vector<std::string> MOVES;

    // std::vector<SDL_Texture *> TextureList;
    // SDL_Texture *boardTexture;
    void initPieces();
    void initPiecesRenderer();

public:

    Board(SDL_Renderer *renderer);

    // Clear renderer
    void clear() const { SDL_RenderClear(renderer); }

    // Update renderer
    void present() const { SDL_RenderPresent(renderer); }

    // SDL_Texture *loadTexture(const std::string &path);

    void drawTexture(SDL_Texture *texture, int x, int y, int w, int h) const
    {
        SDL_Rect infos = {x, y, w, h};
        SDL_RenderCopy(renderer, texture, nullptr, &infos);
    }

    // Game Update, with data taken in as boardSequence[0 -> 5], which are strings
    void convertStartingPosition(std::string seq);
    void updatePlayerStatus(std::string player);
    void updateCastlingStatus(std::string seq);
    void updateEnPassantStatus(std::string seq);
    void countHalfmove(std::string num); // used to enforce the 50-move draw rule
    void countTotalMove(std::string num);

    // Main functions
    void renderChessboard(colorRGBA primary, colorRGBA secondary);
    void renderPieces(colorRGBA primary, colorRGBA secondary, bool rotationFlag);
    void ConvertFEN();

    // Infos
    bool checkBoardSeq();
    int getMargin();
    int getSideLength();
    void setRendererColor(colorRGBA color);

    // Utilities

    /*
        Split FEN Notation in sequences, with
        boardSequence[0] represents the placement of pieces (lower case is black, uppercase is white)
        boardSequence[1] indicates who moves next (w is white, b is black)
        boardSequence[2] tells if the players can castle and to what side.
        boardSequence[3] tells a possible target for an en passant capture
        boardSequence[4] informs how many moves both players have made since the last pawn advance or piece capture (known as halfmoves)
        boardSequence[5] shows the number of completed turns in the game
    */
    void splitSequence(std::string str); // Input FEN Notation

    bool isChessPiece(char c, int &indicator);

    bool isNum(char c)
    {
        return 0 <= (c - '0') && (c - '0') <= 9;
    }

    int stringToNum(std::string str);

    void flush();

    bool testInbound(SDL_MouseButtonEvent ev);

    void log(SDL_MouseButtonEvent ev, std::string status);

    Coordinate getPressedPieceCoord(SDL_MouseButtonEvent ev);
};
#pragma once

#include <SDL_image.h>
#include <SDL.h>
#include <math.h>
#include <string.h>
#include <vector>
// #include "background.h"
#include "colorScheme.h"
#include "coordinate.h"
#include "pieces.h"
class Board
{
private:
    SDL_Renderer *renderer = NULL;
    // Background background;
    const int MARGIN = 80;
    const int SIDE_LENGTH = 70;
    static const int BOARD_SIZE = 8;
    char board[BOARD_SIZE][BOARD_SIZE];
    bool isMoved[BOARD_SIZE][BOARD_SIZE];
    Coordinate enPassantCoord;
    colorRGBA primaryColor;
    colorRGBA secondaryColor;
    colorRGBA backgroundColor;
    chessPieces chessPiece;
    // Board status, contains playerTurn, Castling, En Passant, half and total moves.
    int isPlayerTurn; // contains either 0 or 1, which 1 stands for white turn, 0 for black turn
    bool whiteKingSide;
    bool whiteQueenSide;
    bool blackKingSide;
    bool blackQueenSide;
    bool enPassant;
    int halfmoves = 0;
    int totalmoves = 0;

    // Castling status

    // FEN Notation
    std::string boardSequence[6] = {""};
    std::string STARTING_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    // std::string STARTING_FEN = "rnb1kbnr/ppppqppp/8/4N3/4P3/8/PPP2PPP/R1BQKB1R w KQkq - 0 1";
    std::vector<std::string> MOVES;

    const double IMG_SCALE = 1.5;
    const double MOVE_INDICATOR_SCALE = 0.75;
    const double CAPTURE_INDICATOR_SCALE = 1.5;

    std::vector<SDL_Texture *> TextureList;
    SDL_Texture *boardTexture;
    SDL_Texture *pieces[12];
    SDL_Texture *possibleMoveIndicator;
    SDL_Texture *possibleCaptureIndicator;
    void loadTextures();
    void renderPiece(int pieceName, int color, int x, int y);

public:
    Board(SDL_Renderer *renderer);

    Board(SDL_Renderer *renderer, colorRGBA primaryColor, colorRGBA secondaryColor, colorRGBA backgroundColor);

    // Clear renderer
    void clear() const { SDL_RenderClear(renderer); }

    // Update renderer
    void present() const { SDL_RenderPresent(renderer); }

    SDL_Texture *loadTexture(const char *filePath, int width, int height, double scale);

    // SDL_Texture *loadTexture(const std::string &path);

    void drawTexture(SDL_Texture *texture, int x, int y, int w, int h) const
    {
        SDL_Rect infos = {x, y, w, h};
        SDL_RenderCopy(renderer, texture, nullptr, &infos);
        // std::cerr << "Rendering at " << x << " " << y << "\n";
    }

    // Game Update, with data taken in as boardSequence[1 -> 5], which are strings
    void updatePlayerStatus(std::string player);

    void updateCastlingStatus(std::string seq);
    void updateCastlingStatus();

    void updateEnPassantStatus(std::string seq);

    void countHalfmove(std::string num); // used to enforce the 50-move draw rule

    void countTotalMove(std::string num);

    // Main functions
    void renderChessboard(colorRGBA primary, colorRGBA secondary);

    void renderChessboard();

    void renderStartingPosition(std::string seq);

    void renderIndex(colorRGBA primary, colorRGBA secondary, bool rotationFlag);

    void renderPieces();

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

    bool isNum(char c);

    int stringToNum(std::string str);

    bool testInbound(SDL_MouseButtonEvent ev);

    void log(SDL_MouseButtonEvent ev, std::string status);

    Coordinate getPieceCoord(SDL_MouseButtonEvent ev);

    Coordinate getPieceCoord(int x, int y);

    void renderPieceByCoordinate(int pieceName, int color, int x, int y);

    void renderPieceByCursor(int pieceName, int color, int x, int y);

    void renderPieceByCursor(char piece, int x, int y);

    void parseFENToBoard(std::string fenConfig);

    void renderFromBoard();

    int getPieceName(char piece);

    int getPieceColor(char piece);

    char getPiece(int x, int y);

    char getPiece(Coordinate coord);

    void deleteCell(int x, int y);

    void deleteCell(Coordinate coord);

    void writeCell(int x, int y, char piece);

    void writeCell(Coordinate coord, char piece);

    void debugBoard();

    void setColor(colorRGBA primary, colorRGBA secondary);

    void render();

    void setBackground(colorRGBA bg);

    vector<Coordinate> getPossibleMoves(int pieceName, int color, int coordX, int coordY);
    vector<Coordinate> getPossibleMoves(char piece, int coordX, int coordY);

    vector<Coordinate> getPossibleCaptures(int pieceName, int color, int coordX, int coordY);
    vector<Coordinate> getPossibleCaptures(char piece, int coordX, int coordY);

    void renderMove(const vector<Coordinate> &moveList, const vector<Coordinate> &captureList);

    bool isValidMove(const vector<Coordinate> &moveList, const vector<Coordinate> &captureList, Coordinate dest);
    bool isKingSafe(int color);
    bool testMovesKingSafety(Coordinate dest, char movingPiece);

    bool isInBound(Coordinate coord);
    void markMoved(Coordinate dest);

    bool canWhiteCastlingKing();
    bool canWhiteCastlingQueen();
    bool canBlackCastlingKing();
    bool canBlackCastlingQueen();

    char getPieceFromInfo(int pieceName, int color);
};
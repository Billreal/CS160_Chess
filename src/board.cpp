#include <iostream>
#include "./../include/board.h"
#include "./../include/colorScheme.h"
#include "./../include/button.h"
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <algorithm>
#include <sstream>
#define NANOSVG_IMPLEMENTATION
#define NANOSVGRAST_IMPLEMENTATION
#include "./../include/nanosvg.h"
#include "./../include/nanosvgrast.h"

Board::Board()
{
}
Board::Board(SDL_Renderer *renderer) : renderer(renderer)
{
    loadTextures();
    for (int i = 0; i < BOARD_SIZE; i++)
        for (int j = 0; j < BOARD_SIZE; j++)
            isMoved[i][j] = false;
}
Board::Board(SDL_Renderer *renderer, colorRGBA primaryColor, colorRGBA secondaryColor, colorRGBA backgroundColor) : renderer(renderer), primaryColor(primaryColor), secondaryColor(secondaryColor), backgroundColor(backgroundColor)
{
    loadTextures();
    for (int i = 0; i < BOARD_SIZE; i++)
        for (int j = 0; j < BOARD_SIZE; j++)
            isMoved[i][j] = false;
}

Board::~Board()
{
    TTF_CloseFont(font);
}

SDL_Texture *Board::loadTexture(const char *filePath, int width, int height, double scale)
{
    struct NSVGimage *image = nsvgParseFromFile(filePath, "px", 96);
    if (!image)
    {
        printf("Failed to load SVG file.\n");
        return nullptr;
    }

    // Rasterize SVG
    struct NSVGrasterizer *rast = nsvgCreateRasterizer();
    unsigned char *imageData = (unsigned char *)malloc(width * height * 10); // RGBA buffer
    nsvgRasterize(rast, image, 0, 0, scale, imageData, width, height, width * 4);

    // Create SDL surface and texture
    SDL_Surface *surface = SDL_CreateRGBSurfaceFrom(
        imageData, width, height, 32, width * 4,
        0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

    // Cleanup
    SDL_FreeSurface(surface);
    free(imageData);
    nsvgDeleteRasterizer(rast);
    nsvgDelete(image);

    return texture;
}

void Board::setMargin(int sideMargin, int topMargin)
{
    SIDE_MARGIN = sideMargin;
    TOP_MARGIN = topMargin;
}

void Board::setBoardSize(int boardSize)
{
    SIDE_LENGTH = boardSize;
}

void Board::loadTextures()
{
    /*
        0 -> 5: white
        6 -> 11: black
    */
    pieces[0] = loadTexture("./assets/white_rook.svg", SIDE_LENGTH, SIDE_LENGTH, IMG_SCALE);
    pieces[1] = loadTexture("./assets/white_knight.svg", SIDE_LENGTH, SIDE_LENGTH, IMG_SCALE);
    pieces[2] = loadTexture("./assets/white_bishop.svg", SIDE_LENGTH, SIDE_LENGTH, IMG_SCALE);
    pieces[3] = loadTexture("./assets/white_queen.svg", SIDE_LENGTH, SIDE_LENGTH, IMG_SCALE);
    pieces[4] = loadTexture("./assets/white_king.svg", SIDE_LENGTH, SIDE_LENGTH, IMG_SCALE);
    pieces[5] = loadTexture("./assets/white_pawn.svg", SIDE_LENGTH, SIDE_LENGTH, IMG_SCALE);
    pieces[6] = loadTexture("./assets/black_rook.svg", SIDE_LENGTH, SIDE_LENGTH, IMG_SCALE);
    pieces[7] = loadTexture("./assets/black_knight.svg", SIDE_LENGTH, SIDE_LENGTH, IMG_SCALE);
    pieces[8] = loadTexture("./assets/black_bishop.svg", SIDE_LENGTH, SIDE_LENGTH, IMG_SCALE);
    pieces[9] = loadTexture("./assets/black_queen.svg", SIDE_LENGTH, SIDE_LENGTH, IMG_SCALE);
    pieces[10] = loadTexture("./assets/black_king.svg", SIDE_LENGTH, SIDE_LENGTH, IMG_SCALE);
    pieces[11] = loadTexture("./assets/black_pawn.svg", SIDE_LENGTH, SIDE_LENGTH, IMG_SCALE);
    possibleMoveIndicator = loadTexture("./assets/move_indicator.svg", SIDE_LENGTH, SIDE_LENGTH, MOVE_INDICATOR_SCALE);
    possibleCaptureIndicator = loadTexture("./assets/capture_indicator.svg", SIDE_LENGTH, SIDE_LENGTH, CAPTURE_INDICATOR_SCALE);
}

void Board::startNewGame()
{
    CURRENT_FEN = STARTING_FEN;
    renderFen();
}
void Board::renderFen()
{
    splitSequence(CURRENT_FEN);
    reloadFen();
}
void Board::reloadFen()
{
    splitSequence(CURRENT_FEN);

    if (checkBoardSeq())
    {
        parseFENToBoard(boardSequence[0]);
        renderFromBoard();
        updatePlayerStatus(boardSequence[1]);
        updateCastlingStatus(boardSequence[2]);
        updateEnPassantStatus(boardSequence[3]);
        countHalfmove(boardSequence[4]);
        countTotalMove(boardSequence[5]);
    }
}

void Board::renderChessboard(colorRGBA primary, colorRGBA secondary)
{
    // Initialize color for 1st cell as primary color
    setRendererColor(primary);
    for (int i = 1; i <= BOARD_SIZE; i++)
        for (int j = 1; j <= BOARD_SIZE; j++)
        {
            bool cellType = (i + j) % 2;
            int currentX = SIDE_MARGIN + SIDE_LENGTH * (i - 1);
            int currentY = TOP_MARGIN + SIDE_LENGTH * (j - 1);

            // A rectangle, with the origin at the upper left (integer).
            SDL_Rect currentCell{currentX, currentY, SIDE_LENGTH, SIDE_LENGTH};

            // cellType = cellType ^ rotationFlag;
            if (cellType)
                setRendererColor(primary);
            else
                setRendererColor(secondary);
            // Fill a rectangle on the current rendering target with the drawing color.
            SDL_RenderFillRect(renderer, &currentCell);
        }
    // Update the screen with any rendering performed since the previous call.
}


bool Board::checkBoardSeq()
{
    // Check if FEN Notation sequences are available. If not, end the function
    for (int i = 0; i < 6; i++)
    {
        if (boardSequence[i].empty())
            return false;
    }
    return true;
}

// Game update fuction
void Board::updateFen(std::string fen)
{
    if (fen.empty())
    {
        std::cerr << "Empty FEN Notation\n";
        return;
    }
    CURRENT_FEN = fen;
    reloadFen();
}

void Board::updatePlayerStatus(std::string player)
{
    isPlayerTurn = player == "w" ? 1 : 0; // 1 stands for white turn, 0 for black turn
}

void Board::updateCastlingStatus(std::string seq)
{
    whiteKingSide = seq.find('K') != std::string::npos ? true : false;
    whiteQueenSide = seq.find('Q') != std::string::npos ? true : false;
    blackKingSide = seq.find('k') != std::string::npos ? true : false;
    blackQueenSide = seq.find('q') != std::string::npos ? true : false;
}

// Check if en passant move is available
void Board::updateEnPassantStatus(std::string seq)
{
    enPassant = seq == "-" ? false : true;
    enPassantCoord = Coordinate(seq[0] - 'a', seq[1] - '1');
}

// Halfmoves, used to enforce the 50-move draw rule
void Board::countHalfmove(std::string num)
{
    halfmoves = stringToNum(boardSequence[4]);
}

// Total moves
void Board::countTotalMove(std::string num)
{
    totalmoves = stringToNum(boardSequence[5]);
}

std::string Board::getFen()
{
    return CURRENT_FEN;
}

int Board::getMargin()
{
    return SIDE_MARGIN;
}

int Board::getSideLength()
{
    return SIDE_LENGTH;
}

void Board::setRendererColor(colorRGBA color)
{
    SDL_SetRenderDrawColor(renderer, color.getR(), color.getG(), color.getB(), color.getA());
}

// Utilities

void Board::splitSequence(std::string str) // Input FEN Notation
{
    for (int i = 0; i <= 5; i++)
        boardSequence[i] = "";
    for (int i = 0, j = 0; i <= str.length(); i++)
    {
        if (str[i] == ' ' || str[i] == '\0')
        {
            j++;
        }
        else
            boardSequence[j] += str[i];
    }
}

bool Board::isChessPiece(char c, int &indicator)
{
    std::string ChessPiece = "RNBQKPrnbqkp";
    bool isPiece = false;
    for (int i = 0; i < ChessPiece.length(); i++)
    {
        if (c == ChessPiece[i])
        {
            indicator = i;
            isPiece = true;
        }
    }
    return isPiece;
}

int Board::stringToNum(std::string str)
{
    int num = 0;
    for (int i = 0; i < str.length(); i++)
    {
        num = num * 10 + (str[i] - '0');
    }
    return num;
}

bool Board::testInbound(SDL_MouseButtonEvent ev)
{
    // placeholder
    int maxX = SIDE_MARGIN + 8 * SIDE_LENGTH;
    int maxY = TOP_MARGIN + 8 * SIDE_LENGTH;
    return SIDE_MARGIN <= ev.x && ev.x <= maxX && TOP_MARGIN <= ev.y && ev.y <= maxY;
}

void Board::log(SDL_MouseButtonEvent ev, std::string status)
{
    double mouseX = ev.x;
    double mouseY = ev.y;
    printf("The mouse is %s, its position is: %f %f\n", status.c_str(), mouseX, mouseY);
}

Coordinate Board::getPieceCoord(int x, int y)
{
    Coordinate invalidMoves(-1, -1);
    int mouseX = x;
    int mouseY = y;
    int horizontalCell = (mouseX - SIDE_MARGIN) / SIDE_LENGTH;
    int verticalCell = (mouseY - TOP_MARGIN) / SIDE_LENGTH;
    // Fix out of bound cell
    if (horizontalCell < 0 || mouseX - SIDE_MARGIN < 0)
        return invalidMoves;
    if (horizontalCell > 7 || mouseX - (SIDE_MARGIN + 8 * SIDE_LENGTH) > 0)
        return invalidMoves;
    if (verticalCell < 0 || mouseY - TOP_MARGIN < 0)
        return invalidMoves;
    if (verticalCell > 7 || mouseY - (TOP_MARGIN + 8 * SIDE_LENGTH) > 0)
        return invalidMoves;
    return Coordinate(horizontalCell, verticalCell);
}

Coordinate Board::getPieceCoord(SDL_MouseButtonEvent ev)
{
    return getPieceCoord(ev.x, ev.y);
}

void Board::renderPiece(int pieceName, int color, int x, int y)
{
    if (color < 0 || pieceName < 0)
        return;
    int pieceIndex = pieceName + color * 6;
    drawTexture(pieces[pieceIndex], x, y, SIDE_LENGTH, SIDE_LENGTH);
}

void Board::renderPieceByCursor(int pieceName, int color, int x, int y)
{
    renderPiece(pieceName, color, x - SIDE_LENGTH / 2, y - SIDE_LENGTH / 2);
    int leftEdge = x - SIDE_LENGTH / 2;
    int rightEdge = x + SIDE_LENGTH / 2;
    int upperEdge = y - SIDE_LENGTH / 2;
    int lowerEdge = y + SIDE_LENGTH / 2;
    SDL_SetRenderDrawColor(renderer, bgColor.getR(), bgColor.getG(), bgColor.getB(), bgColor.getA());
    if (upperEdge < TOP_MARGIN)
    {
        SDL_Rect fillRect = {leftEdge, upperEdge, SIDE_LENGTH, TOP_MARGIN - upperEdge};
        SDL_RenderFillRect(renderer, &fillRect);
    }
    if (lowerEdge > TOP_MARGIN + SIDE_LENGTH * 8)
    {
        SDL_Rect fillRect = {leftEdge, TOP_MARGIN + SIDE_LENGTH * 8, SIDE_LENGTH, lowerEdge - (TOP_MARGIN + SIDE_LENGTH * 8)};
        SDL_RenderFillRect(renderer, &fillRect);
    }
    if (leftEdge < SIDE_MARGIN)
    {
        SDL_Rect fillRect = {leftEdge, upperEdge, SIDE_MARGIN - leftEdge, SIDE_LENGTH};
        SDL_RenderFillRect(renderer, &fillRect);
    }
    if (rightEdge > SIDE_MARGIN + SIDE_LENGTH * 8)
    {
        SDL_Rect fillRect = {SIDE_MARGIN + SIDE_LENGTH * 8, upperEdge, rightEdge - (SIDE_MARGIN + SIDE_LENGTH * 8), SIDE_LENGTH};
        SDL_RenderFillRect(renderer, &fillRect);
    }
}

void Board::renderPieceByCursor(char piece, int x, int y)
{
    renderPieceByCursor(getPieceName(piece), getPieceColor(piece), x, y);
}

void Board::renderPieceByCoordinate(int pieceName, int color, int x, int y)
{
    renderPiece(pieceName, color, SIDE_MARGIN + x * SIDE_LENGTH, TOP_MARGIN + y * SIDE_LENGTH);
}

void Board::parseFENToBoard(std::string fenConfig)
{
    int row = 0;
    int col = 0;
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            board[i][j] = '0';
    for (auto chr : fenConfig)
    {
        if ('0' <= chr && chr <= '9')
        {
            col = col + chr - '0';
            continue;
        }
        if (chr == '/')
        {
            row++;
            col = 0;
            continue;
        }

        board[row][col] = chr;
        col++;
    }
}

void Board::renderFromBoard()
{
    for (int row = 0; row < BOARD_SIZE; row++)
    {

        for (int col = 0; col < BOARD_SIZE; col++)
        {
            if (board[row][col] == '0')
                continue;
            int name = getPieceName(board[row][col]);
            int color = getPieceColor(board[row][col]);
            renderPieceByCoordinate(name, color, col, row);
        }
    }
}

int Board::getPieceColor(char piece)
{
    if (piece == '0')
        return -1;
    if ('A' <= piece && piece <= 'Z')
        return 0;
    if ('a' <= piece && piece <= 'z')
        return 1;
    return -1;
}

int Board::getPieceName(char piece)
{
    if ('A' <= piece && piece <= 'Z')
        piece = piece - 'A' + 'a';
    switch (piece)
    {
    case 'r':
        return 0;
    case 'n':
        return 1;
    case 'b':
        return 2;
    case 'q':
        return 3;
    case 'k':
        return 4;
    case 'p':
        return 5;
    default:
        return -1;
    }
}

// TODO: Implement chess piece moving

char Board::getPiece(int x, int y)
{
    std::swap(x, y);
    return board[x][y];
}

char Board::getPiece(Coordinate coord)
{
    return getPiece(coord.getX(), coord.getY());
}

void Board::deleteCell(int x, int y)
{
    std::swap(x, y);
    board[x][y] = '0';
}

void Board::writeCell(int x, int y, char piece)
{
    std::swap(x, y);
    board[x][y] = piece;
}

void Board::deleteCell(Coordinate coord)
{
    deleteCell(coord.getX(), coord.getY());
}

void Board::writeCell(Coordinate coord, char piece)
{
    writeCell(coord.getX(), coord.getY(), piece);
}

void Board::debugBoard()
{
    for (int i = 0; i <= 7; i++)
        for (int j = 0; j <= 7; j++)
            std::cerr << board[i][j] << " \n"[j == 7];
    std::cerr << "\n";
}

void Board::setColor(colorRGBA primary, colorRGBA secondary)
{
    primaryColor = primary;
    secondaryColor = secondary;
}

void Board::renderChessboard()
{
    renderChessboard(primaryColor, secondaryColor);
}

void Board::render()
{
    renderChessboard();
    renderLastMove();
    renderCheck();
    renderCheckmate();
    renderStalemate();
    renderFromBoard();
    // std::cerr << dangerCoordinate.getX() << " " << dangerCoordinate.getY() << "\n";
    if (isUnderPromotion)
        renderPawnPromotion();
}

void Board::renderFromFen()
{
    renderChessboard();
    renderFen();
}

void Board::setBackground(colorRGBA bg)
{
    backgroundColor = bg;
}

bool Board::isNum(char c)
{
    return 0 <= (c - '0') && (c - '0') <= 9;
}

void Board::renderMove(const vector<Coordinate> &moveList, const vector<Coordinate> &captureList)
{
    for (Coordinate cell : moveList)
        drawTexture(possibleMoveIndicator, cell.getX() * SIDE_LENGTH + SIDE_MARGIN + (SIDE_LENGTH * (1 - MOVE_INDICATOR_SCALE)) / 2, cell.getY() * SIDE_LENGTH + TOP_MARGIN, SIDE_LENGTH, SIDE_LENGTH);
    for (Coordinate cell : captureList)
    {
        drawTexture(possibleCaptureIndicator, cell.getX() * SIDE_LENGTH + SIDE_MARGIN, cell.getY() * SIDE_LENGTH + TOP_MARGIN, SIDE_LENGTH, SIDE_LENGTH);
    }
}

vector<Coordinate> Board::getPossibleMoves(char piece, int coordX, int coordY)
{
    int originCol = coordX, originRow = coordY;
    int pieceName = getPieceName(piece);
    int color = getPieceColor(piece);
    vector<vector<Coordinate>> allMoves = chessPiece.listAllMove(pieceName, color, coordX, coordY);
    vector<Coordinate> res;
    // Processing of ordinary move, take king's check status in consider
    for (vector<Coordinate> direction : allMoves)
        for (Coordinate cell : direction)
        {
            int row = cell.getY();
            int col = cell.getX();
            if (board[row][col] != '0')
                break;
            if (isSafeMove(color, piece, Coordinate(coordX, coordY), cell))
                res.push_back(cell);
        }
    if (pieceName == KING)
    {
        if (color == BLACK)
        {
            if (canBlackCastlingKing())
                res.push_back({coordX + 2, coordY});
            if (canBlackCastlingQueen())
                res.push_back({coordX - 2, coordY});
        }
        if (color == WHITE)
        {
            if (canWhiteCastlingKing())
                res.push_back({coordX + 2, coordY});
            if (canWhiteCastlingQueen())
                res.push_back({coordX - 2, coordY});
        }
    }

    return res;
}

vector<Coordinate> Board::getPossibleCaptures(int pieceName, int pieceColor, int coordX, int coordY)
{
    return getPossibleCaptures(getPieceFromInfo(pieceName, pieceColor), coordX, coordY);
}
vector<Coordinate> Board::getPossibleCaptures(char piece, int coordX, int coordY)
{
    Coordinate currentCoordinate(coordX, coordY);
    int pieceName = getPieceName(piece);
    int color = getPieceColor(piece);
    vector<vector<Coordinate>> allMoves = chessPiece.listAllMove(pieceName, color, coordX, coordY);
    vector<Coordinate> res;

    if (pieceName != PAWN)
    {
        for (vector<Coordinate> direction : allMoves)
        {
            for (Coordinate cell : direction)
            {
                int row = cell.getY();
                int col = cell.getX();
                if (board[row][col] == '0')
                    continue;

                if (getPieceColor(board[row][col]) != color && isSafeMove(color, piece, currentCoordinate, cell))
                    res.push_back(cell);
                break;
            }
        }
    }
    else
    {
        Coordinate leftDiagonalCapture, rightDiagonalCapture;
        if (color == WHITE)
        {
            leftDiagonalCapture = Coordinate(coordX - 1, coordY - 1);
            rightDiagonalCapture = Coordinate(coordX + 1, coordY - 1);
        }
        else
        {
            leftDiagonalCapture = Coordinate(coordX - 1, coordY + 1);
            rightDiagonalCapture = Coordinate(coordX + 1, coordY + 1);
        }

        int leftColor = getPieceColor(board[leftDiagonalCapture.getY()][leftDiagonalCapture.getX()]);
        int rightColor = getPieceColor(board[rightDiagonalCapture.getY()][rightDiagonalCapture.getX()]);
        if (isInBound(leftDiagonalCapture))
            if (leftColor != color && leftColor != COLOR_NONE && isSafeMove(color, piece, currentCoordinate, leftDiagonalCapture))
                res.push_back(leftDiagonalCapture);
        if (isInBound(rightDiagonalCapture))
            if (rightColor != color && rightColor != COLOR_NONE && isSafeMove(color, piece, currentCoordinate, rightDiagonalCapture))
                res.push_back(rightDiagonalCapture);
    }
    // Generating en passant capture
    if (getPieceName(piece) == PAWN)
    {
        Coordinate curr(coordX, coordY);
        if (!enPassant)
            return res;
        Coordinate difference = curr - enPassantCoord;
        difference = Coordinate(abs(difference.getX()), abs(difference.getY()));
        if (!(difference.getX() == 1 && difference.getY() == 0))
            return res;
        char piece = getPiece(curr);
        char EPSPiece = getPiece(enPassantCoord);
        int color = getPieceColor(piece);
        if (color == getPieceColor(EPSPiece))
            return res;
        if (color == WHITE)
        {
            Coordinate enPassantMove = enPassantCoord - Coordinate(0, 1);
            if (isSafeMove(color, piece, currentCoordinate, enPassantMove))
                res.push_back(enPassantMove);
        }
        if (color == BLACK)
        {
            Coordinate enPassantMove = enPassantCoord + Coordinate(0, 1);
            if (isSafeMove(color, piece, currentCoordinate, enPassantMove))
                res.push_back(enPassantMove);
        }
    }
    return res;
}

bool Board::isValidMove(const vector<Coordinate> &moveList, const vector<Coordinate> &captureList, Coordinate dest)
{
    // * Check if moving the piece doesn't put the king in danger and the destination is valid
    for (Coordinate cell : moveList)
    {
        if (dest == cell)
            return true;
    }
    for (Coordinate cell : captureList)
    {
        if (dest == cell)
            return true;
    }
    return false;
}
bool Board::testMovesKingSafety(Coordinate src, Coordinate dest, char movingPiece)
{
    char currentBoard[BOARD_SIZE][BOARD_SIZE];
    int destRow = dest.getY();
    int destCol = dest.getX();
    int srcRow = src.getY();
    int srcCol = src.getX();
    for (int i = 0; i < BOARD_SIZE; i++)
        for (int j = 0; j < BOARD_SIZE; j++)
            currentBoard[i][j] = board[i][j];

    board[destRow][destCol] = movingPiece;
    board[srcRow][srcCol] = '0';
    bool res = isKingSafe(getPieceColor(movingPiece));
    for (int i = 0; i < BOARD_SIZE; i++)
        for (int j = 0; j < BOARD_SIZE; j++)
            board[i][j] = currentBoard[i][j];

    return res;
}
bool Board::isKingSafe(int color)
{
    using std::cerr;

    int kingRow, kingCol;
    // Always have
    for (int i = 0; i < BOARD_SIZE; i++)
        for (int j = 0; j < BOARD_SIZE; j++)
        {

            if (getPieceName(board[i][j]) == KING && getPieceColor(board[i][j]) == color)
            {
                kingRow = i;
                kingCol = j;
                break;
            }
        }
    int oppositeColor = 1 - color;
    bool res = true;
    int cardinalRow[8] = {-1, -1, -1, 0, 1, 1, 1, 0};
    int cardinalCol[8] = {-1, 0, 1, 1, 1, 0, -1, -1};

    int horseRow[8] = {-1, -2, -2, -1, 1, 2, 2, 1};
    int horseCol[8] = {-2, -1, 1, 2, 2, 1, -1, -2};

    bool isWhite = color == WHITE;
    bool pawnDirection[8] = {isWhite, false, isWhite, false, !isWhite, false, !isWhite, false};
    bool rookDirection[8] = {false, true, false, true, false, true, false, true};
    bool bishopDirection[8] = {true, false, true, false, true, false, true, false};
    bool queenDirection[8] = {true, true, true, true, true, true, true, true};
    bool kingDirection[8] = {true, true, true, true, true, true, true, true};

    for (int direction = 0; direction < 8; direction++)
    {
        for (int distance = 1; distance < 8; distance++)
        {
            int currentRow = kingRow + distance * cardinalRow[direction];
            int currentCol = kingCol + distance * cardinalCol[direction];
            if (currentCol < 0 || currentCol >= 8 || currentRow < 0 || currentRow >= 8)
                break;
            char piece = board[currentRow][currentCol];
            if (piece != '0')
            {
                if (getPieceColor(piece) == color)
                    break;
                // opposite color
                if (distance == 1)
                {
                    if (pawnDirection[direction] && getPieceName(piece) == PAWN)
                        return false;
                    if (kingDirection[direction] && getPieceName(piece) == KING)
                        return false;
                }
                if (bishopDirection[direction] && getPieceName(piece) == BISHOP)
                    return false;
                if (rookDirection[direction] && getPieceName(piece) == ROOK)
                    return false;
                if (queenDirection[direction] && getPieceName(piece) == QUEEN)
                    return false;
                break;
            }
        }
    }
    // Checking horse movement
    for (int direction = 0; direction < 8; direction++)
    {
        int currentRow = kingRow + horseRow[direction];
        int currentCol = kingCol + horseCol[direction];
        if (currentRow < 0 || currentRow > 7 || currentCol < 0 || currentCol > 7)
            continue;
        char piece = board[currentRow][currentCol];
        if (getPieceColor(piece) == color)
            continue;
        if (getPieceName(piece) == KNIGHT)
            return false;
    }
    return true;
}

bool Board::isInBound(Coordinate coord)
{
    if (coord.getX() < 0)
        return false;
    if (coord.getX() >= BOARD_SIZE)
        return false;
    if (coord.getY() < 0)
        return false;
    if (coord.getY() >= BOARD_SIZE)
        return false;
    return true;
}

void Board::updateCastlingStatus()
{
    char leftWhiteRook = board[BOARD_SIZE - 1][0];
    char rightWhiteRook = board[BOARD_SIZE - 1][BOARD_SIZE - 1];
    char whiteKing = board[BOARD_SIZE - 1][4];
    char leftBlackRook = board[0][0];
    char rightBlackRook = board[0][BOARD_SIZE - 1];
    char blackKing = board[0][4];
    if (getPieceColor(leftWhiteRook) != WHITE || getPieceName(leftWhiteRook) != ROOK)
        whiteQueenSide = false;
    if (getPieceColor(rightWhiteRook) != WHITE || getPieceName(rightWhiteRook) != ROOK)
        whiteKingSide = false;
    if (getPieceColor(leftBlackRook) != BLACK || getPieceName(leftBlackRook) != ROOK)
        blackQueenSide = false;
    if (getPieceColor(rightBlackRook) != BLACK || getPieceName(rightBlackRook) != ROOK)
        blackKingSide = false;
    if (getPieceColor(whiteKing) != WHITE || getPieceName(whiteKing) != KING)
    {
        whiteKingSide = false;
        whiteQueenSide = false;
    }
    if (getPieceColor(blackKing) != BLACK || getPieceName(blackKing) != KING)
    {
        blackKingSide = false;
        blackQueenSide = false;
    }
}

bool Board::canWhiteCastlingKing()
{
    if (!whiteKingSide)
        return false;
    bool res = true;
    board[BOARD_SIZE - 1][4] = '0';
    board[BOARD_SIZE - 1][BOARD_SIZE - 1] = '0';
    for (int col = 4; col <= 6 && res; col++)
    {
        if (getPieceName(board[BOARD_SIZE - 1][col]) != -1)
        {
            res = false;
            continue;
        }
        board[BOARD_SIZE - 1][col] = 'K';
        if (!isKingSafe(WHITE))
            res = false;
        board[BOARD_SIZE - 1][col] = '0';
    }
    board[BOARD_SIZE - 1][4] = 'K';
    board[BOARD_SIZE - 1][BOARD_SIZE - 1] = 'R';
    return res;
}
bool Board::canWhiteCastlingQueen()
{
    if (!whiteQueenSide)
        return false;
    bool res = true;
    board[BOARD_SIZE - 1][4] = '0';
    board[BOARD_SIZE - 1][0] = '0';
    for (int col = 4; col >= 2 && res; col--)
    {
        if (getPieceName(board[BOARD_SIZE - 1][col]) != -1)
        {
            res = false;
            continue;
        }
        board[BOARD_SIZE - 1][col] = 'K';
        if (!isKingSafe(WHITE))
            res = false;
        board[BOARD_SIZE - 1][col] = '0';
    }
    board[BOARD_SIZE - 1][4] = 'K';
    board[BOARD_SIZE - 1][0] = 'R';
    if (getPieceName(board[BOARD_SIZE - 1][1]) != -1)
        res = false;
    return res;
}
bool Board::canBlackCastlingKing()
{
    if (!blackKingSide)
        return false;
    bool res = true;
    board[0][4] = '0';
    board[0][BOARD_SIZE - 1] = '0';
    for (int col = 4; col <= 6 && res; col++)
    {
        if (getPieceName(board[0][col]) != -1)
        {
            res = false;
            continue;
        }

        board[0][col] = 'k';
        if (!isKingSafe(BLACK))
            res = false;
        board[0][col] = '0';
    }
    board[0][4] = 'k';
    board[0][BOARD_SIZE - 1] = 'r';
    return res;
}
bool Board::canBlackCastlingQueen()
{
    if (!blackQueenSide)
        return false;
    bool res = true;
    board[0][4] = '0';
    board[0][0] = '0';
    for (int col = 4; col >= 2 && res; col--)
    {
        if (getPieceName(board[0][col]) != -1)
        {
            res = false;
            continue;
        }

        board[0][col] = 'k';
        if (!isKingSafe(BLACK))
        {
            res = false;
            continue;
        }
        board[0][col] = '0';
    }
    board[0][4] = 'k';
    board[0][0] = 'r';
    if (getPieceName(board[0][1]) != -1)
    {
        res = false;
    }
    return res;
}

char Board::getPieceFromInfo(int pieceName, int color)
{
    char res;
    switch (pieceName)
    {
    case PAWN:
        res = 'p';
        break;
    case ROOK:
        res = 'r';
        break;
    case KNIGHT:
        res = 'n';
        break;
    case BISHOP:
        res = 'b';
        break;
    case QUEEN:
        res = 'q';
        break;
    case KING:
        res = 'k';
        break;
    case CHESS_NONE:
        return '0';
    default:
        return '0';
    }
    switch (color)
    {
    case WHITE:
        res = res - 'a' + 'A';
        break;
    case BLACK:
        break;
    case COLOR_NONE:
        return '0';
    default:
        return '0';
    }
    return res;
}

bool Board::makeMove(Coordinate src, Coordinate dest, char piece, const vector<Coordinate> &moveList, const vector<Coordinate> &captureList)
{
    if (dest == Coordinate(-1, -1))
        return false;
    if (!isValidMove(moveList, captureList, dest))
        return false;
    if (!testMovesKingSafety(src, dest, piece))
        return false;
    Coordinate displacement = dest - src;
    int prevTotalPiece = 0;
    // * Counting total piece before
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            if (board[i][j] != '0')
                prevTotalPiece++;
    // * Consider special moves: En passant
    if (enPassant && getPieceName(piece) == PAWN && getPieceColor(piece) != getPieceColor(getPiece(enPassantCoord)) && getPiece(dest) == '0')
    {
        if (abs(displacement.getX()) == 1 && abs(displacement.getY()) == 1)
        {
            deleteCell(enPassantCoord);
        }
    }
    if (getPieceName(piece) == PAWN && abs(displacement.getX() == 0) && abs(displacement.getY()) == 2)
    {
        enPassant = true;
        enPassantCoord = dest;
    }
    else
    {
        enPassant = false;
        enPassantCoord = Coordinate(-1, -1);
    }
    // * Consider special moves: Castling
    if (getPieceName(piece) == KING)
    {
        if (abs(displacement.getX()) == 2 && abs(displacement.getY()) == 0)
        {
            // * Can already castling
            int startKingX = src.getX();
            int finalKingX = dest.getX();
            int startRookX, finalRookX;
            int startRookY = src.getY();
            int finalRookY = dest.getY();
            if (finalKingX < startKingX)
                startRookX = 0;
            else
                startRookX = BOARD_SIZE - 1;
            finalRookX = (startKingX + finalKingX) / 2;
            writeCell(Coordinate(finalRookX, finalRookY), getPieceColor(piece) == WHITE ? 'R' : 'r');
            deleteCell(Coordinate(startRookX, startRookY));
        }
    }
    writeCell(dest, piece);
    deleteCell(src);
    recordMove(src, dest);
    if (getPieceColor(piece) == BLACK)
        totalmoves++;
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            if (board[i][j] != '0')
                prevTotalPiece--;
    if (prevTotalPiece != 0 || getPieceName(piece) == PAWN)
        halfmoves = 0;
    else
        halfmoves++;
    return true;
}

void Board::log(std::string message)
{
    if (false)
        std::cerr << message << std::endl;
}

bool Board::isStalemate(int color)
{
    if (!isKingSafe(color))
        return false;
    for (int row = 0; row < BOARD_SIZE; row++)
        for (int col = 0; col < BOARD_SIZE; col++)
        {
            if (getPieceColor(board[row][col]) == color)
            {
                int coordX = col;
                int coordY = row;
                if (getPossibleMoves(board[row][col], coordX, coordY).size() != 0)
                    return false;
                if (getPossibleCaptures(board[row][col], coordX, coordY).size() != 0)
                    return false;
            }
        }
    return true;
}

bool Board::isSafeMove(int color, char piece, Coordinate src, Coordinate dest)
{
    int rowDest = dest.getY();
    int colDest = dest.getX();
    int rowSrc = src.getY();
    int colSrc = src.getX();
    char originDestPiece = getPiece(dest);
    writeCell(dest, piece);
    deleteCell(src);
    bool res = isKingSafe(color);
    writeCell(src, piece);
    writeCell(dest, originDestPiece);
    return res;
}

bool Board::isCheckmate(int color)
{
    if (isKingSafe(color))
        return false;
    for (int row = 0; row < BOARD_SIZE; row++)
        for (int col = 0; col < BOARD_SIZE; col++)
        {
            if (getPieceColor(board[row][col]) == color)
            {
                int coordX = col;
                int coordY = row;
                if (getPossibleMoves(board[row][col], coordX, coordY).size() != 0)
                    return false;
                if (getPossibleCaptures(board[row][col], coordX, coordY).size() != 0)
                    return false;
            }
        }
    return true;
}

void Board::recordMove(Coordinate src, Coordinate dest)
{
    previousCoordinate = Coordinate(src.getX(), src.getY());
    currentCoordinate = Coordinate(dest.getX(), dest.getY());
}
void Board::renderLastMove()
{
    renderBlendCell(previousCoordinate, moveIndicator);
    renderBlendCell(currentCoordinate, moveIndicator);
}

void Board::renderBlendCell(Coordinate coordinate, colorRGBA color)
{
    if (coordinate == Coordinate(-1, -1))
        return;
    coordinate = coordinate * SIDE_LENGTH + Coordinate(SIDE_MARGIN, TOP_MARGIN);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    bool cellType = (coordinate.getX() + coordinate.getY()) % 2;
    setRendererColor(color);
    SDL_Rect renderRect = {coordinate.getX(), coordinate.getY(), SIDE_LENGTH, SIDE_LENGTH};
    SDL_RenderFillRect(renderer, &renderRect);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}

void Board::setRenderCheck(chessColor color)
{
    dangerCoordinate = Coordinate(-1, -1);
    if (color != COLOR_NONE)
        for (int row = 0; row < BOARD_SIZE; row++)
            for (int col = 0; col < BOARD_SIZE; col++)
                if (board[row][col] == getPieceFromInfo(KING, color))
                {
                    dangerCoordinate = Coordinate(col, row); // col as x, row as y
                    return;
                }
}

void Board::renderCheck()
{
    if (dangerCoordinate == Coordinate(-1, -1))
        return;
    renderBlendCell(dangerCoordinate, checkIndicator);
}

void Board::enablePawnPromotion(int x, int y) // In cell coordinate
{
    isUnderPromotion = true;
    if (!isInBound(Coordinate(x, y)))
        return;
    int row = y;
    int col = x;
    int menuOriginY;
    promotionColor = chessColor(getPieceColor(board[row][col]));
    if (y == 0)
        menuOriginY = y - 1;
    else if (y == 7)
        menuOriginY = y + 1;
    else
        return;
    if (x < 4)
    {
        queenButtonCoordinate = Coordinate(x * SIDE_LENGTH + SIDE_MARGIN, menuOriginY * SIDE_LENGTH + TOP_MARGIN);
        knightButtonCoordinate = Coordinate((x + 1) * SIDE_LENGTH + SIDE_MARGIN, menuOriginY * SIDE_LENGTH + TOP_MARGIN);
        rookButtonCoordinate = Coordinate((x + 2) * SIDE_LENGTH + SIDE_MARGIN, menuOriginY * SIDE_LENGTH + TOP_MARGIN);
        bishopButtonCoordinate = Coordinate((x + 3) * SIDE_LENGTH + SIDE_MARGIN, menuOriginY * SIDE_LENGTH + TOP_MARGIN);
    }
    else
    {
        queenButtonCoordinate = Coordinate(x * SIDE_LENGTH + SIDE_MARGIN, menuOriginY * SIDE_LENGTH + TOP_MARGIN);
        knightButtonCoordinate = Coordinate((x - 1) * SIDE_LENGTH + SIDE_MARGIN, menuOriginY * SIDE_LENGTH + TOP_MARGIN);
        rookButtonCoordinate = Coordinate((x - 2) * SIDE_LENGTH + SIDE_MARGIN, menuOriginY * SIDE_LENGTH + TOP_MARGIN);
        bishopButtonCoordinate = Coordinate((x - 3) * SIDE_LENGTH + SIDE_MARGIN, menuOriginY * SIDE_LENGTH + TOP_MARGIN);
    }
    if ((queenButtonCoordinate.getX() + queenButtonCoordinate.getY()) % 2 == 1)
    {
        queenPromotionCellColor = rookPromotionCellColor = {secondaryColor.getR(), secondaryColor.getG(), secondaryColor.getB(), secondaryColor.getA()};
        knightPromotionCellColor = bishopPromotionCellColor = {primaryColor.getR(), primaryColor.getG(), primaryColor.getB(), primaryColor.getA()};
    }
    else
    {
        knightPromotionCellColor = bishopPromotionCellColor = {secondaryColor.getR(), secondaryColor.getG(), secondaryColor.getB(), secondaryColor.getA()};
        queenPromotionCellColor = rookPromotionCellColor = {primaryColor.getR(), primaryColor.getG(), primaryColor.getB(), primaryColor.getA()};
    }
    SDL_Color blackColor = {black.getR(), black.getG(), black.getA(), black.getB()};
    queenPromotion = Button(renderer, queenButtonCoordinate.getX(), queenButtonCoordinate.getY(), SIDE_LENGTH, SIDE_LENGTH, queenPromotionCellColor, blackColor, ".", font);
    knightPromotion = Button(renderer, knightButtonCoordinate.getX(), knightButtonCoordinate.getY(), SIDE_LENGTH, SIDE_LENGTH, knightPromotionCellColor, blackColor, ".", font);
    rookPromotion = Button(renderer, rookButtonCoordinate.getX(), rookButtonCoordinate.getY(), SIDE_LENGTH, SIDE_LENGTH, rookPromotionCellColor, blackColor, ".", font);
    bishopPromotion = Button(renderer, bishopButtonCoordinate.getX(), bishopButtonCoordinate.getY(), SIDE_LENGTH, SIDE_LENGTH, bishopPromotionCellColor, blackColor, ".", font);
    promotionCoord = Coordinate(x, y);
}
void Board::renderPawnPromotion()
{
    Button *arr[4] = {&queenPromotion, &knightPromotion, &rookPromotion, &bishopPromotion};
    Coordinate coord[4] = {queenButtonCoordinate, knightButtonCoordinate, rookButtonCoordinate, bishopButtonCoordinate};
    for (int i = 0; i < 4; i++)
    {
        int colorX = (coord[i].getX() - SIDE_MARGIN) / SIDE_LENGTH;
        int colorY = (coord[i].getY() - TOP_MARGIN) / SIDE_LENGTH;
        if ((colorX + colorY) % 2 == 1)
            arr[i]->setColor(secondaryColor);
        else
            arr[i]->setColor(primaryColor);
        arr[i]->render();
    }
    renderPiece(QUEEN, promotionColor, queenButtonCoordinate.getX(), queenButtonCoordinate.getY());
    renderPiece(KNIGHT, promotionColor, knightButtonCoordinate.getX(), knightButtonCoordinate.getY());
    renderPiece(ROOK, promotionColor, rookButtonCoordinate.getX(), rookButtonCoordinate.getY());
    renderPiece(BISHOP, promotionColor, bishopButtonCoordinate.getX(), bishopButtonCoordinate.getY());
}

bool Board::handlePawnPromotion(SDL_Event *ev)
{
    int row = promotionCoord.getY();
    int col = promotionCoord.getX();
    vector<Button *> button = {&queenPromotion, &rookPromotion, &bishopPromotion, &knightPromotion};
    for (Button *currentButton : button)
        currentButton->handleEvent(ev);
    if (queenPromotion.clicked() || rookPromotion.clicked() || bishopPromotion.clicked() || knightPromotion.clicked())
    {
        if (queenPromotion.clicked())
        {
            board[row][col] = getPieceFromInfo(QUEEN, promotionColor);
        }
        else if (rookPromotion.clicked())
        {
            board[row][col] = getPieceFromInfo(ROOK, promotionColor);
        }
        else if (bishopPromotion.clicked())
        {
            board[row][col] = getPieceFromInfo(BISHOP, promotionColor);
        }
        else if (knightPromotion.clicked())
        {
            board[row][col] = getPieceFromInfo(KNIGHT, promotionColor);
        }

        for (Button *currentButton : button)
        {
            currentButton->resetClicked();
            currentButton->clear();
        }
        isUnderPromotion = false;
        return true;
    }
    return false;
}

void Board::setRenderCheckmate(chessColor color)
{
    for (int row = 0; row < BOARD_SIZE; row++)
        for (int col = 0; col < BOARD_SIZE; col++)
            if (board[row][col] == getPieceFromInfo(KING, color))
            {
                checkmateCoordinate = Coordinate(col, row);
                dangerCoordinate = stalemateCoordinate = Coordinate(-1, -1);
            }
}
void Board::setRenderStalemate(chessColor color)
{
    for (int row = 0; row < BOARD_SIZE; row++)
        for (int col = 0; col < BOARD_SIZE; col++)
            if (board[row][col] == getPieceFromInfo(KING, color))
            {
                stalemateCoordinate = Coordinate(col, row);
                dangerCoordinate = checkmateCoordinate = Coordinate(-1, -1);
            }
}
void Board::renderCheckmate()
{
    if (checkmateCoordinate == Coordinate(-1, -1))
        return;
    renderBlendCell(checkmateCoordinate, checkmateIndicator);
}
void Board::renderStalemate()
{
    if (stalemateCoordinate == Coordinate(-1, -1))
        return;
    renderBlendCell(stalemateCoordinate, stalemateIndicator);
}

int Board::getCurrentTurn()
{
    return !isPlayerTurn;
}

string Board::boardstateToFEN()
{
    std::stringstream returnStr;
    for (int row = 0; row < BOARD_SIZE; row++)
    {
        int countBlank = 0;
        for (int col = 0; col < BOARD_SIZE; col++)
        {
            if (board[row][col] == '0')
                countBlank++;
            else
            {
                if (countBlank != 0)
                    returnStr << countBlank;
                returnStr << board[row][col];
                countBlank = 0;
            }
        }
        if (countBlank != 0)
            returnStr << countBlank;
        if (row != BOARD_SIZE - 1)
            returnStr << '/';
    }
    returnStr << " ";
    returnStr << (isPlayerTurn ? 'w' : 'b');
    returnStr << ' ';
    if (!blackKingSide && !blackQueenSide && !whiteQueenSide && !whiteKingSide)
        returnStr << "-";
    else
    {
        if (whiteKingSide)
            returnStr << 'K';
        if (whiteQueenSide)
            returnStr << 'Q';
        if (blackKingSide)
            returnStr << 'k';
        if (blackQueenSide)
            returnStr << 'q';
    }
    returnStr << " ";
    if (enPassant)
    {
        char x = enPassantCoord.getX() + 'a';
        char y = enPassantCoord.getY() + '1';
        returnStr << x << y;
    }
    else
        returnStr << '-';
    returnStr << " ";
    returnStr << halfmoves << " ";
    returnStr << totalmoves << " ";
    return returnStr.str();
}
string Board::boardstateToFEN(int color)
{
    isPlayerTurn = !color;
    return boardstateToFEN();
}

bool Board::nextMove(int color)
{
    std::string fen = boardstateToFEN(color);
    std::string bestMove = communicator -> getMove(fen);
    if (bestMove == "(none)")
    {
        debugBoard();
        return false;
    }
    int srcCol = bestMove[0] - 'a';
    int srcRow = '8' - bestMove[1];
    int destCol = bestMove[2] - 'a';
    int destRow = '8' - bestMove[3];

    char promotionPiece = '0';
    if (bestMove.length() > 4)
        promotionPiece = bestMove[4];
    if (color == WHITE)
        promotionPiece = promotionPiece - 'a' + 'A';
    char piece = getPiece(srcCol, srcRow);

    auto moveList = getPossibleMoves(piece, srcCol, srcRow);
    for (auto cell : moveList)
        if (false)
            std::cerr << cell.getX() << " " << cell.getY() << "\n";
    auto captureList = getPossibleCaptures(piece, srcCol, srcRow);
    for (auto cell : captureList)
        if (false)
            std::cerr << cell.getX() << " " << cell.getY() << "\n";
    makeMove(Coordinate(srcCol, srcRow), Coordinate(destCol, destRow), piece, moveList, captureList);
    if (bestMove.length() != 4)
    {
        piece = promotionPiece;
        writeCell(destCol, destRow, piece);
    }
    updateCastlingStatus();
    if (false)
        std::cerr << "after: \n";
    debugBoard();
    if (false)
        std::cerr << "Reached end of nextMove\n";
    return true;
}

bool Board::highlightKingStatus(bool &isEnded)
{
    if (isCheckmate(WHITE) || isCheckmate(BLACK))
    {
        isEnded = true;
        if (isCheckmate(WHITE))
            setRenderCheckmate(WHITE);
        else
            setRenderCheckmate(BLACK);
        SDL_Log("End game: Checkmate");

        return true;
    }
    if (!isKingSafe(WHITE) || !isKingSafe(BLACK))
    {
        if (!isKingSafe(WHITE))
            setRenderCheck(WHITE);
        else
            setRenderCheck(BLACK);
        return true;
    }
    if (isStalemate(WHITE) || isStalemate(BLACK))
    {
        isEnded = true;
        if (isStalemate(WHITE))
            setRenderStalemate(WHITE);
        else
            setRenderStalemate(BLACK);
        SDL_Log("End game: Statemate");
        return true;
    }
    return false;
    if (false)
        std::cerr << "Statemate status: " << isStalemate(WHITE) << " and " << isStalemate(BLACK) << "\n";
}

void Board::clear()
{
    setRendererColor(bgColor);
}

bool Board::resetBoardState(bool &isEnded)
{

    isEnded = false;
    Coordinate nullCell = Coordinate(-1, -1);
    dangerCoordinate = nullCell;
    checkmateCoordinate = nullCell;
    stalemateCoordinate = nullCell;
    previousCoordinate = nullCell;
    currentCoordinate = nullCell;
    communicator -> startNewGame();
    return highlightKingStatus(isEnded);
}

void Board::setCommunicator(Communicator* communicator)
{
    this -> communicator = communicator;
}

void Board::nextMoveColor()
{
    isPlayerTurn = !isPlayerTurn;
}

chessColor Board::getMoveColor()
{
    return (chessColor)(!isPlayerTurn);
}
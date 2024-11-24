#pragma once
#include <iostream>
#include "./../include/board.h"
#include "./../include/colorScheme.h"
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <algorithm>
// #include "./../include/colorScheme.h"
#define NANOSVG_IMPLEMENTATION
#define NANOSVGRAST_IMPLEMENTATION
#include "./../include/nanosvg.h"
#include "./../include/nanosvgrast.h"

// #include "colorScheme.cpp"
Board::Board(SDL_Renderer *renderer) : renderer(renderer)
{
    loadTextures();
    for (int i = 0; i < BOARD_SIZE; i++)
        for (int j = 0; j < BOARD_SIZE; j++)
            isMoved[i][j] = false;
    // background = Background(renderer);
}
Board::Board(SDL_Renderer *renderer, colorRGBA primaryColor, colorRGBA secondaryColor, colorRGBA backgroundColor) : renderer(renderer), primaryColor(primaryColor), secondaryColor(secondaryColor), backgroundColor(backgroundColor)
{
    loadTextures();
    for (int i = 0; i < BOARD_SIZE; i++)
        for (int j = 0; j < BOARD_SIZE; j++)
            isMoved[i][j] = false;
    // background = Background(renderer);
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

// SDL_Texture *Board::loadTexture(const std::string &path)
// {
//     SDL_Surface *surface = IMG_Load(path.c_str());
//     // Check if surface is loaded
//     if (!surface)
//     {
//         SDL_Log("Failed to load texture %s: %s", path.c_str(), SDL_GetError());
//         return nullptr;
//     }
//     TextureList.push_back(SDL_CreateTextureFromSurface(renderer, surface));
//     SDL_Texture *texture = TextureList.back();
//     SDL_FreeSurface(surface);
//     return texture;
// }

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
    // pieces[0][0] = loadTexture("./assets/white_pawn.png");
    // pieces[1][0] = loadTexture("./assets/black_pawn.png");
}

void Board::renderPieces()
{
    // std::cerr << "Begin rendering\n";
    // drawTexture(pieces[0][1], MARGIN, MARGIN, SIDE_LENGTH, SIDE_LENGTH);
    // drawTexture(pieces[0], MARGIN, MARGIN, SIDE_LENGTH, SIDE_LENGTH);
    // drawTexture(pieces[6], MARGIN + 70 * 7, MARGIN + 70 * 7, SIDE_LENGTH, SIDE_LENGTH);

    splitSequence(STARTING_FEN);

    if (checkBoardSeq())
    {
        parseFENToBoard(boardSequence[0]);
        renderFromBoard();
        // renderStartingPosition(boardSequence[0]);
        updatePlayerStatus(boardSequence[1]);
        updateCastlingStatus(boardSequence[2]);
        updateEnPassantStatus(boardSequence[3]);
        countHalfmove(boardSequence[4]);
        countTotalMove(boardSequence[5]);
    }

    // std::cerr << "Rendering Done\n";
    // flush();
}

void Board::renderChessboard(colorRGBA primary, colorRGBA secondary)
{
    // Initialize color for 1st cell as primary color
    setRendererColor(primary);
    for (int i = 1; i <= BOARD_SIZE; i++)
        for (int j = 1; j <= BOARD_SIZE; j++)
        {
            bool cellType = (i + j) % 2;
            int currentX = MARGIN + SIDE_LENGTH * (i - 1);
            int currentY = MARGIN + SIDE_LENGTH * (j - 1);

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
    // flush();
}

void Board::renderIndex(colorRGBA primary, colorRGBA secondary, bool rotationFlag)
{
    // i is row index
    // j is column index
    // * Bottom to up, left to right
    for (int i = 1; i <= BOARD_SIZE; i++)
        for (int j = 1; j <= BOARD_SIZE; j++)
        {
            if (j != 1 || i != BOARD_SIZE)
                continue;
            bool cellType = (i + j) % 2 ^ 1; // As to contrast the cell's color
            int currentX = MARGIN + SIDE_LENGTH * (i - 1);
            int currentY = MARGIN + SIDE_LENGTH * (j - 1);
        }
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

void Board::renderStartingPosition(std::string seq)
{
    // Translate FEN notation's chess placements into an 8x8 array
    // Direction: Left to Right, Top down
    // std::cerr << seq << "\n";
    for (int i = 0, row = 0, column = 0; i <= seq.length(); i++)
    {
        char currentChar = seq[i];

        // If there's a chess piece, place it on the board
        int pieceIndicator;
        if (isChessPiece(currentChar, pieceIndicator))
        {
            // board[row][column++] = currentChar;
            if (0 <= pieceIndicator && pieceIndicator < 12)
            {
                // Draw chess piece
                drawTexture(pieces[pieceIndicator],
                            MARGIN + SIDE_LENGTH * column,
                            MARGIN + SIDE_LENGTH * row,
                            SIDE_LENGTH,
                            SIDE_LENGTH);
                column++;
                // std::cerr << row << " " << column << "\n";
            }
            continue;
        }

        // If there's no available piece, place a '*' onto the board instead
        if (isNum(currentChar))
        {
            int blankLength = int(currentChar) - '0';
            column = column + blankLength;
            // while (length--)
            // {
            //     board[row][column++] = '*';
            // }
            continue;
        }

        // Moves to the next row
        if (currentChar == '/')
        {
            row++;
            // Reset column initial position on a new row
            column = 0;
            continue;
        }
    }
}

// Game update fuction
void Board::updatePlayerStatus(std::string player)
{
    isPlayerTurn = player == "w" ? 1 : 0; // 1 stands for white turn, 0 for black turn
    // std::cerr << "Player " << isPlayerTurn << " is playing\n";
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

int Board::getMargin()
{
    return MARGIN;
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
    int maxX = MARGIN + 8 * SIDE_LENGTH;
    int maxY = MARGIN + 8 * SIDE_LENGTH;
    return MARGIN <= ev.x && ev.x <= maxX && MARGIN <= ev.y && ev.y <= maxY;
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
    int horizontalCell = (mouseX - MARGIN) / SIDE_LENGTH;
    int verticalCell = (mouseY - MARGIN) / SIDE_LENGTH;
    // Fix out of bound cell
    if (horizontalCell < 0 || mouseX - MARGIN < 0)
        return invalidMoves;
    if (horizontalCell > 7 || mouseX - (MARGIN + 8 * SIDE_LENGTH) > 0)
        return invalidMoves;
    if (verticalCell < 0 || mouseY - MARGIN < 0)
        return invalidMoves;
    if (verticalCell > 7 || mouseY - (MARGIN + 8 * SIDE_LENGTH) > 0)
        return invalidMoves;
    return Coordinate(horizontalCell, verticalCell);
}

Coordinate Board::getPieceCoord(SDL_MouseButtonEvent ev)
{
    return getPieceCoord(ev.x, ev.y);
}

void Board::renderPiece(int pieceName, int color, int x, int y)
{
    // std::swap(x, y);
    // Swapped x and y for real positioning
    if (color < 0 || pieceName < 0)
        return;
    int pieceIndex = pieceName + color * 6;
    drawTexture(pieces[pieceIndex], x, y, SIDE_LENGTH, SIDE_LENGTH);
    // SDL_Log("Rendering chess pieces at %d %d", x, y);
}

void Board::renderPieceByCursor(int pieceName, int color, int x, int y)
{
    // std::swap(x, y);
    // Swapped x and y
    renderPiece(pieceName, color, x - SIDE_LENGTH / 2, y - SIDE_LENGTH / 2);
    int leftEdge = x - SIDE_LENGTH / 2;
    int rightEdge = x + SIDE_LENGTH / 2;
    int upperEdge = y - SIDE_LENGTH / 2;
    int lowerEdge  = y + SIDE_LENGTH / 2;
    SDL_SetRenderDrawColor(renderer, bgColor.getR(), bgColor.getG(), bgColor.getB(), bgColor.getA());
    if (upperEdge < MARGIN)
    {
        SDL_Rect fillRect = {leftEdge, upperEdge, SIDE_LENGTH, MARGIN - upperEdge};
        SDL_RenderFillRect(renderer, &fillRect);
    } 
    if (lowerEdge > MARGIN + SIDE_LENGTH * 8)
    {
        SDL_Rect fillRect = {leftEdge, MARGIN + SIDE_LENGTH * 8, SIDE_LENGTH, lowerEdge - (MARGIN + SIDE_LENGTH * 8)};
        SDL_RenderFillRect(renderer, &fillRect);
    }
    if (leftEdge < MARGIN)
    {
        SDL_Rect fillRect = {leftEdge, upperEdge, MARGIN - leftEdge, SIDE_LENGTH};
        SDL_RenderFillRect(renderer, &fillRect);
    }
    if (rightEdge > MARGIN + SIDE_LENGTH * 8)
    {
        SDL_Rect fillRect = {MARGIN + SIDE_LENGTH * 8, upperEdge, rightEdge - (MARGIN + SIDE_LENGTH * 8), SIDE_LENGTH};
        SDL_RenderFillRect(renderer, &fillRect);
    }

    // SDL_Log("Rendering chess pieces at %d %d", x, y);
}

void Board::renderPieceByCursor(char piece, int x, int y)
{
    renderPieceByCursor(getPieceName(piece), getPieceColor(piece), x, y);
}

void Board::renderPieceByCoordinate(int pieceName, int color, int x, int y)
{
    renderPiece(pieceName, color, MARGIN + x * SIDE_LENGTH, MARGIN + y * SIDE_LENGTH);
}

void Board::parseFENToBoard(std::string fenConfig)
{
    // std::cerr << fenConfig << "\n";
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
    // for (int i = 0; i < 8; i++)
    //     for (int j = 0; j < 8; j++)
    //         std::cerr << board[i][j] << " \n"[j == 7];
}

void Board::renderFromBoard()
{
    for (int row = 0; row < BOARD_SIZE; row++)
    {

        for (int col = 0; col < BOARD_SIZE; col++)
        {
            // std::cerr << board[row][col] << " ";
            if (board[row][col] == '0')
                continue;
            int name = getPieceName(board[row][col]);
            int color = getPieceColor(board[row][col]);
            renderPieceByCoordinate(name, color, col, row);
        }
        // std::cerr << std::endl;
    }
    // std::cerr << std::endl;
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
    // Coordinate boardCoordinate = getPieceCoord(x, y);
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
    // setRendererColor(backgroundColor);
    // clear();
    // background.render(backgroundColor);
    renderChessboard();
    renderLastMove();
    renderCheck();
    renderFromBoard();
}

void Board::setBackground(colorRGBA bg)
{
    backgroundColor = bg;
}

bool Board::isNum(char c)
{
    return 0 <= (c - '0') && (c - '0') <= 9;
}
// TODO: render next move possible for a chess

// colorRGBA indicator(75, 72, 71, 127);
void Board::renderMove(const vector<Coordinate> &moveList, const vector<Coordinate> &captureList)
{
    for (Coordinate cell : moveList)
        drawTexture(possibleMoveIndicator, (cell.getX() + 1 - MOVE_INDICATOR_SCALE) * SIDE_LENGTH + MARGIN, (cell.getY() + 1 - MOVE_INDICATOR_SCALE) * SIDE_LENGTH + MARGIN, SIDE_LENGTH, SIDE_LENGTH);
    for (Coordinate cell : captureList)
    {
        // std::cerr << "Rendering capture move at " << cell.getX() << " " << cell.getY() << "\n";
        drawTexture(possibleCaptureIndicator, cell.getX() * SIDE_LENGTH + MARGIN, cell.getY() * SIDE_LENGTH + MARGIN, SIDE_LENGTH, SIDE_LENGTH);
    }
}

// * renderMove is gonna be replaced

vector<Coordinate> Board::getPossibleMoves(char piece, int coordX, int coordY)
{
    // std::cerr << "Begin getting possible moves at: " << coordX << " " << coordY << "\n";
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
    // std::cerr << "Done getting possible moves at: " << coordX << " " << coordY << "\n";

    // Generating castling
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
    // std::cerr << "Begin checking ordinary moves of: " << piece << " at " << coordX << " " << coordY << "\n";

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
                // std::cerr << "Can capture at " << row << " " << col << "\n";
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
        // std::cerr << leftColor << " " << board[leftDiagonalCapture.getY()][leftDiagonalCapture.getX()] << "\n";
        int rightColor = getPieceColor(board[rightDiagonalCapture.getY()][rightDiagonalCapture.getX()]);
        // std::cerr << rightColor << " " << board[rightDiagonalCapture.getY()][rightDiagonalCapture.getX()] << "\n";
        if (isInBound(leftDiagonalCapture))
            if (leftColor != color && leftColor != COLOR_NONE && isSafeMove(color, piece, currentCoordinate, leftDiagonalCapture))
                res.push_back(leftDiagonalCapture);
        if (isInBound(rightDiagonalCapture))
            if (rightColor != color && rightColor != COLOR_NONE && isSafeMove(color, piece, currentCoordinate, rightDiagonalCapture))
                res.push_back(rightDiagonalCapture);
    }
    // std::cerr << "Done ordinary moves of: " << piece << " at: " << coordX << " " << coordY << std::endl;
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
        // std::cerr << "Can enpassant at: " << enPassantCoord.getX() << " " << enPassantCoord.getY() << "\n";
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
        // std::cerr << "Done checking en passant at: " << coordX << " " << coordY << "\n";
    }

    // for (auto cell : res)
    //     std::cerr << "Can capture at " << cell.getX() << " " << cell.getY() << "\n";
    return res;
}

bool Board::isValidMove(const vector<Coordinate> &moveList, const vector<Coordinate> &captureList, Coordinate dest)
{
    // ! Piece at src doesn't exist
    // * Check if moving the piece doesn't put the king in danger and the destination is valid
    // Generating the next board
    for (Coordinate cell : moveList)
    {
        // std::cerr << cell.getX() << " " << cell.getY() << " " << dest.getX() << " " << dest.getY() << "\n";
        if (dest == cell)
            return true;
    }
    for (Coordinate cell : captureList)
    {
        // std::cerr << cell.getX() << " " << cell.getY() << " " << dest.getX() << " " << dest.getY() << "\n";
        if (dest == cell)
            return true;
    }
    return false;

    // * Check if the king is in check after moving the piece
}
bool Board::testMovesKingSafety(Coordinate dest, char movingPiece)
{
    char currentBoard[BOARD_SIZE][BOARD_SIZE];
    int destRow = dest.getY();
    int destCol = dest.getX();
    for (int i = 0; i < BOARD_SIZE; i++)
        for (int j = 0; j < BOARD_SIZE; j++)
            currentBoard[i][j] = board[i][j];

    board[destRow][destCol] = movingPiece;
    bool res = isKingSafe(getPieceColor(movingPiece));
    for (int i = 0; i < BOARD_SIZE; i++)
        for (int j = 0; j < BOARD_SIZE; j++)
            board[i][j] = currentBoard[i][j];

    return res;
}
bool Board::isKingSafe(int color)
{
    using std::cerr;
    // cerr << "Entering isKingSafe\n";

    int kingRow, kingCol;
    // Always have
    for (int i = 0; i < BOARD_SIZE; i++)
        for (int j = 0; j < BOARD_SIZE; j++)
        {
            // cerr << board[i][j] << " ";

            if (getPieceName(board[i][j]) == KING && getPieceColor(board[i][j]) == color)
            {
                kingRow = i;
                kingCol = j;
                break;
            }
        }
    // cerr << "\n";
    // std::cerr << kingRow << " " << kingCol << " " << color << "\n";
    int oppositeColor = 1 - color;
    bool res = true;
    // cerr << "Current Color is " << color << "\n";
    // cerr << "Opposite Color is " << oppositeColor << "\n";
    // * 3 - getPieceColor(movingPiece) return the opposite color to movingPiece
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
            if (currentCol < 0 || currentCol >= 8 || currentRow < 0 || currentRow >= 8) break;
            char piece = board[currentRow][currentCol];
            if (piece != '0')
            {
                if (getPieceColor(piece) == color) break;
                // opposite color
                if (distance == 1)
                {
                    if (pawnDirection[direction] && getPieceName(piece) == PAWN) return false;
                    if (kingDirection[direction] && getPieceName(piece) == KING) return false;
                }
                if (bishopDirection[direction] && getPieceName(piece) == BISHOP) return false;
                if (rookDirection[direction] && getPieceName(piece) == ROOK) return false;
                if (queenDirection[direction] && getPieceName(piece) == QUEEN) return false;
                break;
            }
        }
    }
    // Checking horse movement
    for (int direction = 0; direction < 8; direction++)
    {
        int currentRow = kingRow + horseRow[direction];
        int currentCol = kingCol + horseCol[direction];
        if (currentRow < 0 || currentRow > 7 || currentCol < 0 || currentCol > 7) continue;
        char piece = board[currentRow][currentCol];
        if (getPieceColor(piece) == color) continue;
        if (getPieceName(piece) == KNIGHT) return false;
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
    // std::cerr << whiteKingSide << whiteQueenSide << blackKingSide << blackQueenSide << "\n";
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
        // std::cerr << "Checking black queen castling at: " << 0 << " " << col << ": ";
        // std::cerr << getPieceName(board[0][col]) << "\n";
        if (getPieceName(board[0][col]) != -1)
        {
            res = false;
            // std::cerr << "Exist piece at " << 0 << " " << col << "\n";
            continue;
        }

        board[0][col] = 'k';
        if (!isKingSafe(BLACK))
        {
            // std::cerr << "King is not safe at: " << 0 << " " << col << "\n";
            res = false;
        }
        board[0][col] = '0';
    }
    board[0][4] = 'k';
    board[0][0] = 'r';
    if (getPieceName(board[0][1]) != -1)
    {
        // std::cerr << "Exist piece at " << 0 << " " << 1 << "\n";
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
    log("Done checking out of bound");
    if (!isValidMove(moveList, captureList, dest))
        return false;
    log("Done checking violation of moving rules");
    // * Check king's safety
    if (!testMovesKingSafety(dest, piece))
        return false;
    log("done checking King's safety");
    // * Record change in position
    log("Done updating current piece");
    Coordinate displacement = dest - src;
    // * Consider special moves: En passant
    // std::cerr << enPassant << " "
    //          << getPieceName(piece) << " "
    //          << getPieceColor(piece) << " "
    //          << getPieceColor(getPiece(enPassantCoord)) << " "
    //          << getPiece(dest) << std::endl;
    // std::cerr << displacement.getX() << " " << displacement.getY() << std::endl;
    if (enPassant && getPieceName(piece) == PAWN && getPieceColor(piece) != getPieceColor(getPiece(enPassantCoord)) && getPiece(dest) == '0')
    {
        if (abs(displacement.getX()) == 1 && abs(displacement.getY()) == 1)
            deleteCell(enPassantCoord);
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
    return true;
}

void Board::log(std::string message)
{
    std::cerr << message << std::endl;
}

bool Board::isStatemate(int color)
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
    // return true;
    int rowDest = dest.getY();
    int colDest = dest.getX();
    int rowSrc = src.getY();
    int colSrc = src.getX();
    char originDestPiece = getPiece(dest);
    writeCell(dest, piece);
    deleteCell(src);
    // std::cerr << "Checking move of " << piece << " to " << dest.getX() << " " << dest.getY() << "\n";
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
    // if (previousCoordinate == Coordinate(-1, -1) || currentCoordinate == Coordinate(-1, -1)) return;
    renderBlendCell(previousCoordinate, moveIndicator);
    renderBlendCell(currentCoordinate, moveIndicator);
}

void Board::renderBlendCell(Coordinate coordinate, colorRGBA color)
{
    if (coordinate == Coordinate(-1, -1)) return;
    coordinate = coordinate * SIDE_LENGTH + Coordinate(MARGIN, MARGIN);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    // SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    bool cellType = (coordinate.getX() + coordinate.getY()) % 2;
    setRendererColor(color);
    // if (cellType)
    //     setRendererColor(primaryColor);
    // else setRendererColor(secondaryColor);
    SDL_Rect renderRect = {coordinate.getX(), coordinate.getY(), SIDE_LENGTH, SIDE_LENGTH};
    SDL_RenderFillRect(renderer, &renderRect);
    // SDL_Log(SDL_GetError());
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

}

void Board::setRenderCheck(chessColor color)
{
    std::cerr << "Entering set render check\n";
    Coordinate kingPlace = Coordinate(-1, -1);
    if (color != COLOR_NONE)
    for (int row = 0; row < BOARD_SIZE; row++)
        for (int col = 0; col < BOARD_SIZE; col++)
            if (board[row][col] == getPieceFromInfo(KING, color))
            {
                kingPlace = Coordinate(col, row); // col as x, row as y
                std::cerr << "Found " << board[row][col] << " at " << row << " " << col << "\n";
            }
    dangerCoordinate = kingPlace;
}

void Board::renderCheck()
{
    if (dangerCoordinate == Coordinate(-1, -1)) return;
    // std::cerr << "Rendering check indicator at" << dangerCoordinate.getX() << " " << dangerCoordinate.getY() << "\n";
    renderBlendCell(dangerCoordinate, checkIndicator);
}

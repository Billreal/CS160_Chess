#pragma once
#include <iostream>
#include "./../include/board.h"
#include <math.h>
#include <string.h>
#include <stdio.h>

#define NANOSVG_IMPLEMENTATION
#define NANOSVGRAST_IMPLEMENTATION
#include "./../include/nanosvg.h"
#include "./../include/nanosvgrast.h"

Board::Board(SDL_Renderer *renderer) : renderer(renderer)
{
    loadTextures();
    background = Background(renderer);
}
Board::Board(SDL_Renderer *renderer, colorRGBA primaryColor, colorRGBA secondaryColor, colorRGBA backgroundColor) : renderer(renderer), primaryColor(primaryColor), secondaryColor(secondaryColor), backgroundColor(backgroundColor)
{
    loadTextures();
    background = Background(renderer);
}

SDL_Texture *Board::loadTexture(const char *filePath, int width, int height)
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
    nsvgRasterize(rast, image, 0, 0, IMG_SCALE, imageData, width, height, width * 4);

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
    pieces[0] = loadTexture("./assets/white_rook.svg", SIDE_LENGTH, SIDE_LENGTH);
    pieces[1] = loadTexture("./assets/white_knight.svg", SIDE_LENGTH, SIDE_LENGTH);
    pieces[2] = loadTexture("./assets/white_bishop.svg", SIDE_LENGTH, SIDE_LENGTH);
    pieces[3] = loadTexture("./assets/white_queen.svg", SIDE_LENGTH, SIDE_LENGTH);
    pieces[4] = loadTexture("./assets/white_king.svg", SIDE_LENGTH, SIDE_LENGTH);
    pieces[5] = loadTexture("./assets/white_pawn.svg", SIDE_LENGTH, SIDE_LENGTH);
    pieces[6] = loadTexture("./assets/black_rook.svg", SIDE_LENGTH, SIDE_LENGTH);
    pieces[7] = loadTexture("./assets/black_knight.svg", SIDE_LENGTH, SIDE_LENGTH);
    pieces[8] = loadTexture("./assets/black_bishop.svg", SIDE_LENGTH, SIDE_LENGTH);
    pieces[9] = loadTexture("./assets/black_queen.svg", SIDE_LENGTH, SIDE_LENGTH);
    pieces[10] = loadTexture("./assets/black_king.svg", SIDE_LENGTH, SIDE_LENGTH);
    pieces[11] = loadTexture("./assets/black_pawn.svg", SIDE_LENGTH, SIDE_LENGTH);
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
            // if (i == BOARD_SIZE)

            // if (j == 1)
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
    std::cerr << seq << "\n";
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
                std::cerr << row << " " << column << "\n";
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
    int mouseX = x;
    int mouseY = y;
    int horizontalCell = (mouseX - MARGIN) / SIDE_LENGTH;
    int verticalCell = (mouseY - MARGIN) / SIDE_LENGTH;
    // Fix out of bound cell
    if (horizontalCell < 0)
        horizontalCell = 0;
    if (horizontalCell > 7)
        horizontalCell = 7;
    if (verticalCell < 0)
        verticalCell = 0;
    if (verticalCell > 7)
        verticalCell = 7;
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
        for (int col = 0; col < BOARD_SIZE; col++)
        {
            if (board[row][col] == '0')
                continue;
            int name = getPieceName(board[row][col]);
            int color = getPieceColor(board[row][col]);
            renderPieceByCoordinate(name, color, col, row);
        }
}

int Board::getPieceColor(char piece)
{
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
    background.render(backgroundColor);
    renderChessboard();
    renderFromBoard();
}

void Board::setBackground(colorRGBA bg)
{
    backgroundColor = bg;
}
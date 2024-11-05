#pragma once
#include <iostream>
#include "./../include/board.h"

Board::Board(SDL_Renderer *renderer) : renderer(renderer)
{
    initPieces();
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

void Board::initPiecesRenderer(){
}

void Board::initPieces()
{
    /*
        0 -> 5: white
        6 -> 11: black
    */
    for (int i = 0; i <= 11; i++) pieces[i].update(renderer);
    // for (int color = WHITE; color <= BLACK; color++){
    //     for(int name = ROOK; name <= PAWN; name++){
    //         pieces[color * 6 + name].update(color, name);
    //     }
    // }
    pieces[0].update(WHITE, ROOK);
    pieces[1].update(WHITE, KNIGHT);
    pieces[2].update(WHITE, BISHOP);
    pieces[3].update(WHITE, QUEEN);
    pieces[4].update(WHITE, KING);
    pieces[5].update(WHITE, PAWN);
    pieces[6].update(BLACK, ROOK);
    pieces[7].update(BLACK, KNIGHT);
    pieces[8].update(BLACK, BISHOP);
    pieces[9].update(BLACK, QUEEN);
    pieces[10].update(BLACK, KING);
    pieces[11].update(BLACK, PAWN);
}

void Board::ConvertFEN()
{
    // std::cerr << "Begin rendering\n";
    // drawTexture(pieces[0][1], MARGIN, MARGIN, SIDE_LENGTH, SIDE_LENGTH);
    // drawTexture(pieces[0], MARGIN, MARGIN, SIDE_LENGTH, SIDE_LENGTH);
    // drawTexture(pieces[6], MARGIN + 70 * 7, MARGIN + 70 * 7, SIDE_LENGTH, SIDE_LENGTH);

    splitSequence(STARTING_FEN);

    if (checkBoardSeq())
    {
        convertStartingPosition(boardSequence[0]);
        updatePlayerStatus(boardSequence[1]);
        updateCastlingStatus(boardSequence[2]);
        updateEnPassantStatus(boardSequence[3]);
        countHalfmove(boardSequence[4]);
        countTotalMove(boardSequence[5]);
    }

    std::cerr << "Converting Done\n";
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

void Board::renderPieces(colorRGBA primary, colorRGBA secondary, bool rotationFlag)
{
    // * Bottom to up, left to right
    for (int row = 1; row <= BOARD_SIZE; row++)
        for (int column = 1; column <= BOARD_SIZE; column++)
        {
            // Draw chess piece
            drawTexture(BOARD[row * 8 + column].getTexture(),
                        MARGIN + SIDE_LENGTH * column,
                        MARGIN + SIDE_LENGTH * row,
                        SIDE_LENGTH,
                        SIDE_LENGTH);
            // if (row == BOARD_SIZE)

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

void Board::convertStartingPosition(std::string seq)
{
    // Translate FEN notation's chess placements into an 8x8 array
    // Direction: Left to Right, Top down
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
                BOARD[row * 8 + column].update(pieces[pieceIndicator].getColor(), 
                                        pieces[pieceIndicator].getName(), 
                                        row, 
                                        column);
                // drawTexture(pieces[pieceIndicator],
                //             MARGIN + SIDE_LENGTH * column,
                //             MARGIN + SIDE_LENGTH * row,
                //             SIDE_LENGTH,
                //             SIDE_LENGTH);
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
    std::cerr << "Player " << isPlayerTurn << " is playing\n";
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

void Board::flush()
{
    SDL_RenderPresent(renderer);
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

Coordinate Board::getPressedPieceCoord(SDL_MouseButtonEvent ev)
{
    int mouseX = ev.x;
    int mouseY = ev.y;
    int horizontalCell = (mouseX - MARGIN) / SIDE_LENGTH;
    int verticalCell = (mouseY - MARGIN) / SIDE_LENGTH;
    // Fix out of bound cell
    if (horizontalCell < 0) horizontalCell = 0;
    if (horizontalCell > 7) horizontalCell = 7;
    if (verticalCell < 0) verticalCell = 0;
    if (verticalCell > 7) verticalCell = 7;
    return Coordinate(horizontalCell, verticalCell); 
}
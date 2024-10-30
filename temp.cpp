#include <iostream>
#include <cctype>

using namespace std;

string s, boardSequence[6];

/*
    boardSequence[0] represents the placement of pieces (lower case is black, uppercase is white)
    boardSequence[1] indicates who moves next (w is white, b is black)
    boardSequence[2] tells if the players can castle and to what side.
    boardSequence[3] tells a possible target for an en passant capture
    boardSequence[4] informs how many moves both players have made since the last pawn advance or piece capture (known as halfmoves)
    boardSequence[5] shows the number of completed turns in the game
*/
int seqInitial = 0;

char board[8][8];

bool isChessPiece(char c)
{
    string ChessPiece = "rnbqkpRNBQKP";
    bool isPiece = false;
    for (char piece : ChessPiece)
    {
        if (c == piece)
            isPiece = true;
    }
    return isPiece;
}

bool isNum(char c)
{
    return 0 <= (c - '0') && (c - '0') <= 9;
}

int stringToNum(string str)
{
    int num = 0;
    for (int i = 0; i < str.length(); i++)
    {
        num = num * 10 + (str[i] - '0');
    }
    return num;
}

int main()
{
    // freopen("input.INP", "read", stdin);
    // freopen("output.OUT", "write", stdout);
    getline(cin, s);

    for (int i = 0, j = 0; i <= s.length(); i++)
    {
        if (s[i] == ' ' || s[i] == '\0')
        {
            j++;
        }
        else
            boardSequence[j] += s[i];
    }
    // for(int i = 0; i < 6; i++){
    //     cout << boardSequence[i] << "\n";
    // }

    // Translate FEN notation's chess placements into an 8x8 array
    for (int k = 0, i = 0, j = 0; k <= boardSequence[0].length(); k++)
    {
        char currentChar = boardSequence[0][k];

        // If there's a chess piece, place it on the board
        if (isNum(currentChar) == false && isChessPiece(currentChar))
        {
            board[i][j++] = currentChar;
            continue;
        }

        // If there's no available piece, place a '*' onto the board instead
        if (isNum(currentChar))
        {
            int length = int(currentChar) - '0';
            while (length--)
            {
                board[i][j++] = '*';
            }
            continue;
        }

        // Moves to the next row
        if (currentChar == '/')
        {
            i++;
            j = 0;
            continue;
        }
    }

    int isPlayerTurn = boardSequence[1] == "w" ? 1 : 0; // 1 stands for white turn, 0 for black turn

    // Check if any castling moves are available
    bool whiteKingSide = boardSequence[2].find('K') != string::npos ? true : false;
    bool whiteQueenSide = boardSequence[2].find('Q') != string::npos ? true : false;
    bool blackKingSide = boardSequence[2].find('k') != string::npos ? true : false;
    bool blackQueenSide = boardSequence[2].find('q') != string::npos ? true : false;

    // Check if en passant move is available
    bool enPassant = boardSequence[3] == "-" ? false : true;

    // Halfmoves, used to enforce the 50-move draw rule
    int halfmoves = stringToNum(boardSequence[4]);

    // Total moves
    int totalmoves = stringToNum(boardSequence[5]);

    cout << "Board: " << "\n";
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            cout << board[i][j] << " ";
        }
        cout << "\n";
    }

    string tmp = isPlayerTurn == 1 ? "white" : "black";
    cout << "This is " << tmp << "'s turn\n";

    cout << "White king Castling move available: " << whiteKingSide << "\n";
    cout << "White queen Castling move available: " << whiteQueenSide << "\n";
    cout << "Black king Castling move available: " << blackKingSide << "\n";
    cout << "Black queen Castling move available: " << blackQueenSide << "\n";

    cout << "En passant move available: " << enPassant << "\n";

    cout << "Halfmoves done: " << halfmoves << "\n";
    cout << "Total moves done: " << totalmoves << "\n";
}
#include "./../include/communicator.h"
#include "./../include/board.h"
int main(int argc, char *args[])
{
    freopen("log.log", "w", stderr);
    Communicator communicator;
    Board board;
    std::string testFen = "1Q6/8/R5k1/6p1/2p5/2P1K3/8/8 b - - 3 53 ";
    // std::string testFen = "4k3/8/8/8/8/8/3r3r/4K2R w - - 0 1";
    board.updateFen(testFen);
    
    board.debugBoard();
    int currentMoveColor = board.getCurrentTurn();
    communicator.init();
    communicator.startNewGame();
    for (int i = 1; i <= 200; i++)
    {
        std::string fen = board.boardstateToFEN(currentMoveColor);
        std::string bestMove = communicator.getMove(fen, Difficulty::MEDIUM);
        std::cerr << bestMove << std::endl;
        if (bestMove == "(none)") 
        {
            std::cerr << "End game" << std::endl;
            board.debugBoard();
            break;
        }
        int srcCol = bestMove[0] - 'a';
        int srcRow = '8' - bestMove[1];
        int destCol = bestMove[2] - 'a';
        int destRow = '8' - bestMove[3];

        char promotionPiece = '0';
        if (bestMove.length() > 4) promotionPiece = bestMove[4];
        if (currentMoveColor == WHITE) promotionPiece = promotionPiece - 'a' + 'A';
        // std::cerr << "previous: " << std::endl;
        // board.debugBoard();
        // std::cerr << fen << std::endl;
        char piece = board.getPiece(srcCol, srcRow);
        
        std::cerr << "Moved " << piece << " from " << srcCol << " " << srcRow << " to " << destCol << " " << destRow << "\n"; 
        
        // std::cerr << "Deleted: \n";
        // board.debugBoard();
        
        auto moveList = board.getPossibleMoves(piece, srcCol, srcRow);
        for (auto cell: moveList) std::cerr << cell.getX() << " " << cell.getY() << "\n";
        auto captureList = board.getPossibleCaptures(piece, srcCol, srcRow);
        for (auto cell: captureList) std::cerr << cell.getX() << " " << cell.getY() << "\n";
        board.makeMove(Coordinate(srcCol, srcRow), Coordinate(destCol, destRow), piece, moveList, captureList);
        if (bestMove.length() != 4) 
        {
            piece = promotionPiece;
            board.writeCell(destCol, destRow, piece);
        }
        board.updateCastlingStatus();
        std::cerr << "after: \n";
        board.debugBoard();
        currentMoveColor = 1 - currentMoveColor;
    }
    communicator.stop();
    return 0;
}
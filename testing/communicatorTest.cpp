#include "./../include/communicator.h"
// #include "./../include/board.h"
int main(int argv, char** args)
{
    // Board board;
    freopen("log.log", "w+", stderr);
    Communicator communicator;
    if (communicator.isRunning == false) return 1;
    communicator.init();
    communicator.startNewGame();
    // communicator.waitForReady();
    std::cerr << "Done communication\n";
    std::cerr << communicator.isRunning << "\n";
    // board.loadStartingPosition();
    // auto currentMove = board.getCurrentTurn();
    // for (int i = 1; i <= 10; i++)
    // {
    //     std::string fen = board.boardstateToFEN(currentMove);
    //     std::string temp = communicator.getMove(fen, Difficulty::EASY);
    //     currentMove = 1 - currentMove;
    //     board.debugBoard();
    //     std::cerr << temp << std::endl;
    // }
    communicator.~Communicator();
    return 0;
}
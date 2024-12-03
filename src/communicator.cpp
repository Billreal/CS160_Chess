#include "./../include/communicator.h"

Communicator::Communicator()
{
    inputFile.open("tmp/stockfish_output.txt");
    process = _popen("stockfish.exe > tmp/stockfish_output.txt", "w");
    isRunning = true;
}

Communicator::~Communicator()
{
    stop();
}
void Communicator::stop()
{
    pclose(process);
    isRunning = false;
}

void Communicator::writeCommand(const std::string &command)
{
    // std::cerr << "written to stockfish: \n" << command << "\n";
    fprintf(process, (command + "\n").c_str());
    fflush(process);
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
}

void Communicator::init()
{
    writeCommand("uci");
    writeCommand("setoption name Hash value 128");
    writeCommand("setoption name Threads value 1");
    writeCommand("isready");
}

void Communicator::startNewGame()
{
    writeCommand("ucinewgame");
    writeCommand("isready");
}

std::string Communicator::getLineStockfishOutput()
{
    std::string res;
    std::getline(inputFile, res);
    return res;
}

std::string Communicator::getMove(const std::string &fen, Difficulty difficulty)
{
    if (difficulty == Difficulty::EASY)
        return getBestMove(fen, 2, 100);
    if (difficulty == Difficulty::MEDIUM)
        return getBestMove(fen, 5, 500);
    if (difficulty == Difficulty::HARD)
        return getBestMove(fen, 10, 2000);
    return getBestMove(fen, 20, 5000);
}

std::string Communicator::getBestMove(const std::string &fen, int depth, int maximumTime)
{
    std::string fenSet = "position fen " + fen;
    std::string goCommand = "go depth " + std::to_string(depth) + " movetime " + std::to_string(maximumTime);
    writeCommand(fenSet);
    writeCommand(goCommand);

    std::this_thread::sleep_for(std::chrono::milliseconds(maximumTime + 500));
    std::string response = readResponse();
    if (response == "checkmate") return response;
    size_t pos = response.find("bestmove");
    if (pos != std::string::npos)
    {
        std::istringstream iss(response.substr(pos));
        std::string token, bestMove;
        iss >> token >> bestMove;
        return bestMove;
    }
    return "";
}

std::string Communicator::readResponse()
{
    std::string line;
    std::string response;
    while (std::getline(inputFile, line))
    {
        response += line + "\n";
        if (line.find("bestmove") != std::string::npos)
            break;
    }
    return response;
}
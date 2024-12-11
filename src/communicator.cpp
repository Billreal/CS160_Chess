#include "./../include/communicator.h"

Communicator::Communicator()
{
    inputFile.open("tmp/stockfish_output.txt");
    process = popen("stockfish.exe > tmp/stockfish_output.txt", "w");
    isRunning = true;
}

Communicator::Communicator(bool isRunning): isRunning(isRunning)
{   
    if (isRunning)
    {
        inputFile.open("tmp/stockfish_output.txt");
        process = popen("stockfish.exe > tmp/stockfish_output.txt", "w");
        std::cerr << "Communicator is enabled" << std::endl;
    }
    else
        std::cerr << "Communicator is disabled" << std::endl;

    if (process)
        std::cerr << "Process started" << std::endl;
    else std::cerr << "Process is not started" << std::endl;

}

Communicator::~Communicator()
{
    stop();
}
void Communicator::stop()
{
    if (!isRunning || !process) return;
    pclose(process);
    isRunning = false;
}

void Communicator::writeCommand(const std::string &command)
{
    // std::cerr << "written to stockfish: \n" << command << "\n";
    if (!isRunning || !process) return;

    fprintf(process, (command + "\n").c_str());
    fflush(process);
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
}

void Communicator::init()
{
    if (!isRunning || !process) return;

    writeCommand("uci");
    writeCommand("setoption name Hash value 128");
    writeCommand("setoption name Threads value 1");
    setDifficulty(Difficulty::EASY);
    writeCommand("isready");
    // writeCommand("setoption name UCI_LimitStrength value true");
}

void Communicator::startNewGame()
{
    if (!isRunning || !process) return;
    writeCommand("ucinewgame");
    writeCommand("isready");
}

std::string Communicator::getLineStockfishOutput()
{
    if (!isRunning || !process) return "";
    std::string res;
    std::getline(inputFile, res);
    return res;
}

std::string Communicator::getMove(const std::string &fen)
{
    if (!isRunning || !process) return "";
    return getBestMove(fen);
}

std::string Communicator::getBestMove(const std::string &fen)
{
    if (!isRunning || !process) return "";
    std::string fenSet = "position fen " + fen;
    std::string goCommand = "go depth " + std::to_string(searchDepth) + " movetime " + std::to_string(timeAllowed);
    writeCommand(fenSet);
    writeCommand(goCommand);

    std::this_thread::sleep_for(std::chrono::milliseconds(timeAllowed + 500));
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
    if (!isRunning || !process) return "";
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

void Communicator::setDifficulty(Difficulty difficulty)
{
    if (!isRunning || !process) return;
    std::cerr << "Set difficulty called\n";
    currentDifficulty = difficulty;
    if (difficulty == Difficulty::EASY)
    {
        std::cerr << "Switch to easy\n";
        stockfishLevel = 0;
        searchDepth = 1;
        timeAllowed = 100;
        PVValue = 10;
        hashLimit = 16;
        threadLimit = 1;
    }
    else if (difficulty == Difficulty::MEDIUM)
    {
        std::cerr << "Switch to medium\n";
        stockfishLevel = 3;
        searchDepth = 3;
        timeAllowed = 400;
        PVValue = 5;
        hashLimit = 64;
        threadLimit = 2;
    }
    else 
    {
        std::cerr << "Switch to hard\n";
        stockfishLevel = 20;
        searchDepth = 20;
        timeAllowed = 2000;
        PVValue = 1;
        hashLimit = 2058;
        threadLimit = 8;
    }
    writeCommand("setoption name Skill level value " + std::to_string(stockfishLevel));
    writeCommand("setoption name Threads value " + std::to_string(threadLimit));
    writeCommand("setoption name Hash level value " + std::to_string(hashLimit));
    writeCommand("setoption name MultiPV level value " + std::to_string(PVValue));
}

Difficulty Communicator::getDifficulty()
{
    return currentDifficulty;
}
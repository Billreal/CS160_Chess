#include "./../include/communicator.h"

Communicator::Communicator()
{
    inputFile.open("tmp/stockfish_output.txt");
    process = popen("stockfish.exe > tmp/stockfish_output.txt", "w");
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
    if (!process) return;
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
    // writeCommand("setoption name UCI_LimitStrength value true");
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

std::string Communicator::getMove(const std::string &fen)
{
    return getBestMove(fen);
}

std::string Communicator::getBestMove(const std::string &fen)
{
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
    if (difficulty == Difficulty::EASY)
    {
        stockfishLevel = 0;
        searchDepth = 1;
        timeAllowed = 100;
    }
    else if (difficulty == Difficulty::MEDIUM)
    {
        stockfishLevel = 3;
        searchDepth = 3;
        timeAllowed = 400;
    }
    else 
    {
        stockfishLevel = 5;
        searchDepth = 10;
        timeAllowed = 1000;
    }
    writeCommand("setoption name Skill level value " + std::to_string(stockfishLevel));
}
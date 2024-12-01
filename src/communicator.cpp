#include "./../include/communicator.h"
#include <filesystem>
#include <direct.h>
#include <chrono>
#include <thread>
Communicator::Communicator()
{
    // Get current working directory
    char cwd[1024];
    if (_getcwd(cwd, sizeof(cwd)) == nullptr)
    {
        std::cerr << "Failed to get current working directory" << std::endl;
        isRunning = false;
        return;
    }

    // Build path relative to executable
    std::filesystem::path exePath = std::filesystem::absolute(
        std::filesystem::path(cwd) /
        "stockfish.exe");

    // Debug output
    std::cerr << "Looking for Stockfish at: " << exePath << std::endl;

    // Verify executable exists
    if (!std::filesystem::exists(exePath))
    {
        std::cerr << "Cannot find Stockfish at: " << exePath << std::endl;
        isRunning = false;
        return;
    }

    // Open process with verified path
    process = _popen(exePath.string().c_str(), "w+");
    if (!process)
    {
        std::cerr << "Engine initialization failure: " << strerror(errno) << std::endl;
        isRunning = false;
        return;
    }

    isRunning = true;
    std::cerr << "Successfully started Stockfish at: " << exePath << std::endl;
}

void Communicator::init()
{
    writeCommand("uci");
}
void Communicator::writeCommand(const std::string &command)
{
    if (!process) return;
    std::cerr << int(fprintf(process, "%s\n", (command + "\n").c_str())) << " ";
    fflush(process);
    // fclose(process);
    std::cerr << command << "\n";
}

void Communicator::waitForReady()
{
    writeCommand("isready");
    std::string line;
    do
        line = readLine();
    while (line.find("readyok") == std::string::npos);
}

std::string Communicator::readLine()
{
    char buffer[8192];
    if (fgets(buffer, sizeof buffer, process) != nullptr)
        return std::string(buffer);
    return "";
}

void Communicator::stop()
{
    if (!isRunning)
        return;

    std::cerr << "Stopping engine..." << std::endl;

    // Send quit command

    // Flush all buffers
    fflush(process);

    // Read any remaining output
    std::cerr << process << "\n";
    char buffer[8192];
    writeCommand("quit");
    while (fgets(buffer, sizeof(buffer), process) != nullptr)
    {
        std::cerr << buffer << "\n";
        // Discard remaining output
    }

    // Force immediate termination
    // system("taskkill /F /IM stockfish_windows-x86-64.exe > nul 2>&1");
    std::cerr << "Done killing";
    // Close pipe without waiting
    _pclose(process);

    isRunning = false;
    process = nullptr;
    std::cerr << "Engine stopped" << std::endl;

    isRunning = false;
    process = nullptr;
    std::cerr << "Engine stopped" << std::endl;
}

Communicator::~Communicator()
{
    stop();
}

std::string Communicator::getBestMove(const std::string &fen, int depth)
{
    std::stringstream str;
    str << "position " << fen;
    writeCommand(str.str());
    str.clear();
    str << "go depth " << depth;
    writeCommand(str.str());
    std::string line;
    std::string bestMove;
    while (true)
    {
        line = readLine();
        if (line.find("bestmove") != std::string::npos)
        {
            bestMove = line.substr(9, 5);
            if (bestMove.back() == ' ')
                bestMove = bestMove.substr(0, 4);
            return bestMove;
        }
    }
}

std::string Communicator::getMove(const std::string &fen, Difficulty difficulty)
{
    if (difficulty == Difficulty::EASY)
        return getBestMove(fen, 2);

    if (difficulty == Difficulty::MEDIUM)
        return getBestMove(fen, 5);

    if (difficulty == Difficulty::HARD)
        return getBestMove(fen, 10);

    return getBestMove(fen, 20);
}

bool Communicator::startNewGame()
{
    writeCommand("ucinewgame");
    return true;
}
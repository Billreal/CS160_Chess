#pragma once

#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <thread>
#include <chrono>
#include <sstream>
#include <fstream>

enum class Difficulty
{
    NONE = -1,
    EASY,
    MEDIUM,
    HARD
};
class Communicator
{
    FILE *process = nullptr;
    bool isRunning;
    std::ifstream inputFile;
    int stockfishLevel;
    int searchDepth;
    int timeAllowed;
    int PVValue;
    int hashLimit;
    int threadLimit;
    std::string getBestMove(const std::string &fen); // maximumTime is in miliseconds
    Difficulty currentDifficulty;

public:
    Communicator();
    Communicator(bool isRunning);
    void init();
    void startNewGame();

    void writeCommand(const std::string &command);

    void waitForReady();
    std::string readLine();
    void stop();
    ~Communicator();
    std::string getMove(const std::string &fen);
    std::string getLineStockfishOutput();
    std::string readResponse();
    void setDifficulty(Difficulty difficulty);
    Difficulty getDifficulty();
};
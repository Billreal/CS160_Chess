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
    std::string getBestMove(const std::string &fen); // maximumTime is in miliseconds

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
};
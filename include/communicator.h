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

public:
    Communicator();
    void init();
    void startNewGame();

    void writeCommand(const std::string &command);

    void waitForReady();
    std::string readLine();
    std::string getBestMove(const std::string &fen, int depth, int maximumTime); // maximumTime is in miliseconds
    void stop();
    ~Communicator();
    std::string getMove(const std::string &fen, Difficulty difficulty);
    std::string getLineStockfishOutput();
    std::string readResponse();
};
#pragma once

#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <thread>
#include <chrono>
#include <sstream>

enum class Difficulty
{
    EASY,
    MEDIUM,
    HARD
};
class Communicator
{
FILE* process = nullptr;

public:
    bool isRunning;
    Communicator();
    void init();
    bool startNewGame();

    void writeCommand(const std::string &command);

    void waitForReady();
    std::string readLine();
    std::string getBestMove(const std::string &fen, int depth);
    void stop();
    ~Communicator();
    std::string getMove(const std::string &fen, Difficulty difficulty);
};
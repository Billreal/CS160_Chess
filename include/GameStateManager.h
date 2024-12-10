#include <iostream>
#include <stack>
#include <string>

class GameStateManager
{
private:
    std::stack<std::string> undoStack;
    std::stack<std::string> redoStack;

public:
    // Update game state
    void pushState(const std::string &fen);
    std::string undo(bool isSinglePlayer);
    std::string redo(bool isSinglePlayer);
    std::string getFirstMove();
    std::string getLatestMove();
    void clear();
    void clearRedo();

    // Get infos
    void getLen();
    bool canUndo(bool isSinglePlayer);
    bool canRedo(bool isSinglePlayer);
    std::string startState();
    std::string finalState();
    
};
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
    std::string undo();
    std::string redo();
    std::string getFirstMove();
    std::string getLatestMove();
    void clear();
    void clearRedo();

    // Get infos
    void getLen();
    bool canUndo();
    bool canRedo();
};
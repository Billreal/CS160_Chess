#include "./../include/GameStateManager.h"

void GameStateManager::pushState(const std::string &fen)
{
    undoStack.push(fen);
    std::cerr << "Pushed" << fen << std::endl;
    // Clear the redo stack whenever a new state is pushed
    while (!redoStack.empty())
    {
        redoStack.pop();
    }
}

void GameStateManager::clear()
{
    while (!undoStack.empty())
    {
        undoStack.pop();
    }
    while (!redoStack.empty())
    {
        redoStack.pop();
    }
}

void GameStateManager::clearRedo()
{
    while (!redoStack.empty())
    {
        redoStack.pop();
    }
}

std::string GameStateManager::undo(bool isSinglePlayer)
{
    if (canUndo(isSinglePlayer))
    {
        int steps;
        if (isSinglePlayer) steps = 2;
        else steps = 1;
        for (int i = 1; i <= steps; i++)
        {
            redoStack.push(undoStack.top());
            undoStack.pop();
        } 
        std::cerr << "Undo: " << undoStack.top() << "\n";
        return undoStack.empty() ? "" : undoStack.top();
    }
    return "";
}

std::string GameStateManager::redo(bool isSinglePlayer)
{
    if (canRedo(isSinglePlayer))
    {
        int steps;
        if (isSinglePlayer) steps = 2;
        else steps = 1;
        for (int i = 1; i <= steps; i++)
        {
            undoStack.push(redoStack.top());
            redoStack.pop();
        }
        return undoStack.top();
    }
    return "";
}

void GameStateManager::getLen()
{
    std::cerr << "unoStack size: " << undoStack.size() << "\nredoStack size: " << redoStack.size() << "\n";
}

bool GameStateManager::canUndo(bool isSinglePlayer)
{
    return isSinglePlayer ? undoStack.size() >= 3 : undoStack.size() >= 2;
}

bool GameStateManager::canRedo(bool isSinglePlayer)
{
    return isSinglePlayer ? redoStack.size() >= 2 : redoStack.size() >= 1;
}
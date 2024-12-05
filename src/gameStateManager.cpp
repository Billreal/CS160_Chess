#include "./../include/GameStateManager.h"

void GameStateManager::pushState(const std::string &fen)
{
    undoStack.push(fen);
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

std::string GameStateManager::undo()
{
    if (canUndo())
    {
        std::string currentState = undoStack.top();
        undoStack.pop();
        redoStack.push(currentState);
        return undoStack.empty() ? "" : undoStack.top();
    }
    return "";
}

std::string GameStateManager::redo()
{
    if (canRedo())
    {
        std::string currentState = redoStack.top();
        redoStack.pop();
        undoStack.push(currentState);
        return currentState;
    }
    return "";
}

void GameStateManager::getLen()
{
    std::cerr << "unoStack size: " << undoStack.size() << "\nredoStack size: " << redoStack.size() << "\n";
}

bool GameStateManager::canUndo()
{
    return !undoStack.empty();
}

bool GameStateManager::canRedo()
{
    return !redoStack.empty();
}
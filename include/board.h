#pragma once

#include <SDL.h>
#include "colorScheme.h"
#include <string>

class Board
{
private:
    SDL_Renderer *renderer = NULL;
    const int MARGIN = 80;
    const int SIDE_LENGTH = 70;
    const int BOARD_SIZE = 8;

    SDL_Texture *boardTexture;
    SDL_Texture *pieces[2][6];
    void loadTextures();

public:
    Board(SDL_Renderer *renderer);

    // Clear renderer
    void clear() const { SDL_RenderClear(renderer); }

    // Update renderer
    void present() const { SDL_RenderPresent(renderer); }

    SDL_Texture *loadTexture(const std::string &path)
    {
        SDL_Surface *surface = SDL_LoadBMP(path.c_str());

        // Check if surface is loaded
        if (!surface)
        {
            SDL_Log("Failed to load texture %s: %s", path.c_str(), SDL_GetError());
            return nullptr;
        }

        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
        return texture;
    }

    void drawTexture(SDL_Texture *texture, int x, int y, int w, int h) const
    {
        SDL_Rect infos = {x, y, w, h};
        SDL_RenderCopy(renderer, texture, nullptr, &infos);
    }
    
    // Main functions
    void renderChessboard(colorRGBA primary, colorRGBA secondary);
    void renderIndex(colorRGBA primary, colorRGBA secondary, bool rotationFlag);
    void render();

    // Infos
    int getMargin();
    int getSideLength();
};
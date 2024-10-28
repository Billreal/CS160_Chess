#pragma once

#include <SDL_image.h>
#include <SDL.h>
#include <math.h>
#include <string.h>
#include <vector>
#include "colorScheme.h"

class Board
{
private:
    SDL_Renderer *renderer = NULL;
    const int MARGIN = 80;
    const int SIDE_LENGTH = 70;
    const int BOARD_SIZE = 8;

    std::vector<SDL_Texture *> TextureList;
    SDL_Texture *boardTexture;
    SDL_Texture *pieces[2][6];
    void loadTextures();

public:
    Board(SDL_Renderer *renderer);

    // Clear renderer
    void clear() const { SDL_RenderClear(renderer); }

    // Update renderer
    void present() const { SDL_RenderPresent(renderer); }

    SDL_Texture *loadTexture(const char *filePath, int width, int height);

    // SDL_Texture *loadTexture(const std::string &path);
    
    void drawTexture(SDL_Texture *texture, int x, int y, int w, int h) const
    {
        SDL_Rect infos = {x, y, w, h};
        SDL_RenderCopy(renderer, texture, nullptr, &infos);
    }

    // Main functions
    void renderChessboard(colorRGBA primary, colorRGBA secondary);
    void renderIndex(colorRGBA primary, colorRGBA secondary, bool rotationFlag);
    void renderPieces();

    // Infos
    int getMargin();
    int getSideLength();
    void setRendererColor(SDL_Renderer *renderer, colorRGBA color);

    void flush();
};
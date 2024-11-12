#pragma once
#include <SDL.h>
#include "color.h"
class Background
{
private:
    SDL_Renderer *renderer;

public:
    void render(colorRGBA color);
    Background(SDL_Renderer *renderer);
    Background();
};
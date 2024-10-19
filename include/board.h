#pragma once
#include <SDL.h>
#include "color.h"
class Background
{
private:
    SDL_Surface *surface;

public:
    void render(colorRGB color);
    Background(SDL_Surface *surface);
};
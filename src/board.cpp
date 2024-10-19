#pragma once
#include <SDL.h>
#include "./../include/board.h"
#include "./../include/color.h"
#include "./../include/colorScheme.h"

Background::Background(SDL_Surface *surface)
{
    this -> surface = surface;
}
void Background::render(colorRGB color)
{
    SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, color.getR(), color.getG(), color.getB()));
}

#pragma once
#include <iostream>
#include <SDL.h>
#include "./../include/background.h"
#include "./../include/color.h"
#include "./../include/colorScheme.h"

Background::Background(SDL_Renderer *renderer):renderer(renderer){}
Background::Background(){}
void Background::render(colorRGBA color)
{
    SDL_SetRenderDrawColor(renderer, color.getR(), color.getG(), color.getB(), color.getA());
    // SDL_RenderClear(renderer);
    SDL_RenderFillRect(renderer, NULL);
    // SDL_RenderPresent(renderer);
}

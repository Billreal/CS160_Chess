#pragma once

#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include "color.h"
#include "colorScheme.h"
class Button
{
private:
    SDL_Rect rect;
    SDL_Color color;
    SDL_Color textColor;
    std::string text;
    TTF_Font *font;
    SDL_Renderer *renderer;
    SDL_Surface *surface;
    bool isClicked;

    void renderRect(SDL_Rect rect, SDL_Color color);
    void renderRect(SDL_Rect rect, colorRGBA color);

public:
    Button(SDL_Renderer *renderer);
    Button();
    Button(SDL_Renderer *renderer, int x, int y, int w, int h, SDL_Color color, SDL_Color textColor, const std::string &text, TTF_Font *font);

    void render();
    // Update renderer
    void present() const { SDL_RenderPresent(renderer); }
    // Clear renderer
    void clear();
    
    void handleEvent(SDL_Event *e);
    bool clicked() const;
    void reset();

    void setColor(colorRGBA color);
    void setColor(SDL_Color color);
};
#pragma once

#include <SDL.h>
#include <SDL_ttf.h>
#include <string>

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

    void renderRect(SDL_Renderer *renderer, SDL_Rect rect, SDL_Color color);

public:
    Button(SDL_Renderer *renderer);
    Button(SDL_Renderer *renderer, int x, int y, int w, int h, SDL_Color color, SDL_Color textColor, const std::string &text, TTF_Font *font);

    void render();
    // Update renderer
    void present() const { SDL_RenderPresent(renderer); }
    // Clear renderer
    void clear() const { SDL_RenderClear(renderer); }
    
    void handleEvent(SDL_Event *e);
    bool clicked() const;
    void reset();
};
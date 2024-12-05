#pragma once

#include <SDL2/SDL.h>
#include <SDL_ttf.h>
#include <string>
#include "color.h"
#include "colorScheme.h"
class Button
{
private:
    SDL_Rect rect = {-1, -1, -1, -1};
    SDL_Rect buttonArea = {-1 , -1, -1, -1};
    SDL_Color color;
    SDL_Color textColor;
    std::string text;
    TTF_Font *font;
    SDL_Renderer *renderer;
    SDL_Surface *surface;
    bool isClicked;
    bool isHovered;

    void renderRect(SDL_Rect rect, SDL_Color color);
    void renderRect(SDL_Rect rect, colorRGBA color);

public:
    Button(SDL_Renderer *renderer);
    Button();
    Button(SDL_Renderer *renderer, int x, int y, int w, int h, SDL_Color color, SDL_Color textColor, std::string text, TTF_Font *font);
    void updateColor(SDL_Color);

    void render();
    void renderSVG(std::string svgFilePath, double scale);

    // Clear renderer
    void clear() const { SDL_RenderClear(renderer); }

    // Update renderer
    void present() const { SDL_RenderPresent(renderer); }

    void handleEvent(SDL_Event *e);
    bool clicked() const;
    bool hover() const;
    void resetClicked();
    void resetHovered();

    void setColor(colorRGBA color);
    void setColor(SDL_Color color);

    // Get infos
    int getY() {
        return rect.y;
    }
    int getX() {
        return rect.x;
    }
    std::string getText() {
        return text;
    }

};
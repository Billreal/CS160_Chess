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
    bool isInside;

    void renderRect(SDL_Renderer *renderer, SDL_Rect rect, SDL_Color color);
    // SDL_Texture *loadTexture(std::string filePath, int width, int height, double scale);

public:
    Button(SDL_Renderer *renderer);
    Button(SDL_Renderer *renderer, int x, int y, int w, int h, SDL_Color color, SDL_Color textColor, std::string text, TTF_Font *font);

    void render();
    void renderSVG(std::string svgFilePath, double scale);
    // Update renderer
    void present() const { SDL_RenderPresent(renderer); }
    // Clear renderer
    void clear() const { SDL_RenderClear(renderer); }

    void handleEvent(SDL_Event *e);
    bool clicked() const;
    bool hover() const;
    void reset();

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
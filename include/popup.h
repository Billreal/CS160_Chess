#include <SDL2/SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <iostream>
#include "colorScheme.h"
#include "color.h"

enum POPUP_MODE {
    POPUP_MODE_YES_NO,
    POPUP_MODE_GAME_OVER
};

class Popup {
private:
    SDL_Renderer* renderer;
    POPUP_MODE mode;
    SDL_Rect popupRect;
    SDL_Rect yesButtonRect;
    SDL_Rect noButtonRect;
    std::string text;
    TTF_Font* textFont = TTF_OpenFont("path/to/font.ttf", 24);
    TTF_Font* buttonFont = TTF_OpenFont("path/to/font.ttf", 24);
    SDL_Texture* textTexture;
    SDL_Texture* agreeButtonTexture;
    SDL_Texture* dontButtonTexture;

    void renderBlurredBackground();

    void renderText();

    void renderButtons();

public:
    Popup(SDL_Renderer* renderer, POPUP_MODE mode);

    ~Popup();

    void render(std::string text);
    void clear() const {SDL_RenderClear(renderer);};

    bool handleEvent(SDL_Event* e);
};
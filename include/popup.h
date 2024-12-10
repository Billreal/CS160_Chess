#include <SDL2/SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <iostream>
#include "colorScheme.h"
#include "color.h"
#include "button.h"

enum POPUP_MODE {
    CONFIRM,
    INFO
};

class Popup {
private:
    SDL_Renderer* renderer;
    POPUP_MODE mode;
    SDL_Rect popupInfos;
    std::string text;
    TTF_Font* textFont = TTF_OpenFont("./font/Recursive/static/Recursive_Casual-Light.ttf", 24);
    TTF_Font* buttonFont = TTF_OpenFont("./font/Recursive/static/Recursive_Casual-Light.ttf", 24);
    SDL_Color buttonColor = {118, 150, 85, 255};
    SDL_Color white = {255, 255, 255, 255};
    SDL_Texture* textTexture;
    SDL_Texture* agreeButtonTexture;
    SDL_Texture* dontButtonTexture;

    const double SVG_SCALE = 1;
    const int POPUP_LENGTH = 350;
    Button yesBtn;
    Button noBtn;

    void renderBlurredBackground();
    void renderText(std::string text);
    void renderButtons();

public:
    Popup(SDL_Renderer* renderer, POPUP_MODE mode, int x, int y);
    ~Popup();

    SDL_Texture *loadTexture(const char *filePath, int width, int height, double scale);

    bool render(std::string text);
    void clear() const {SDL_RenderClear(renderer);};

    void handleButtonEvent(SDL_Event* e);
};
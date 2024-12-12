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

enum Confirmation {
    NONE = -1,
    NO,
    YES,
};

class Popup {
private:
    SDL_Renderer* renderer;
    POPUP_MODE mode;
    SDL_Rect popupInfos;
    std::string text;
    int prevTextHeight = 0;
    TTF_Font* textFont = TTF_OpenFont("./font/Recursive/static/Recursive_Casual-Light.ttf", 32);
    TTF_Font* buttonFont = TTF_OpenFont("./font/Recursive/static/Recursive_Casual-Light.ttf", 24);
    SDL_Color buttonColor = {118, 150, 85, 255};
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color popupBg = {bgColor.getR(), bgColor.getG(), bgColor.getB(), bgColor.getA()};
    SDL_Texture* textTexture;
    SDL_Texture* agreeButtonTexture;
    SDL_Texture* dontButtonTexture;

    const double SVG_SCALE = 1;
    const int POPUP_LENGTH = 350;
    const int CLOSE_BUTTON_LENGTH = 24;
    Button yesBtn;
    Button noBtn;
    Button closeBtn;
    Confirmation confirmation = NONE;
    bool isClose = false;

    void renderBlurredBackground();
    void renderText(std::string text, int prevHeight, int padding);
    void renderButtons();

public:
    Popup(SDL_Renderer* renderer, POPUP_MODE mode, int x, int y);
    ~Popup();

    SDL_Texture *loadTexture(std::string filePath, int width, int height, double scale);

    void render(std::string text, int padding);
    void render(std::string textPrimary, std::string textSecondary, int padding);
    void clear() const {SDL_RenderClear(renderer);};
    void handleButtonEvent(SDL_Event* e);
    void handleButtonClicked();
    Confirmation isConfirmed();
    bool isClosed() {return isClose;};
    void resetClose() {isClose = false;};
    void clearConfirmation() {confirmation = NONE;};
};
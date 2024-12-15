#include <SDL2/SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <iostream>
#include "colorScheme.h"
#include "color.h"
#include "button.h"
#include "communicator.h"
enum Confirmation
{
    NONE = -1,
    NO,
    YES,
};

class Popup
{
private:
    SDL_Renderer *renderer;
    SDL_Rect popupInfos;
    std::string text;
    int prevTextHeight = 0;
    TTF_Font *textFont = TTF_OpenFont("./font/Recursive/static/Recursive_Casual-Light.ttf", 32);
    TTF_Font *buttonFont = TTF_OpenFont("./font/Recursive/static/Recursive_Casual-Light.ttf", 24);
    SDL_Color buttonColor = {118, 150, 85, 255};
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color popupBg = {bgColor.getR(), bgColor.getG(), bgColor.getB(), bgColor.getA()};
    SDL_Texture *textTexture;
    SDL_Texture *agreeButtonTexture;
    SDL_Texture *dontButtonTexture;
    bool isClose = true;

    const double SVG_SCALE = 1;
    const int POPUP_LENGTH = 350;
    const int CLOSE_BUTTON_LENGTH = 24;
    Button yesBtn;
    Button noBtn;
    Button closeBtn;
    Confirmation confirmation = Confirmation::NONE;

    Button DifficultyEasy;
    Button DifficultyMedium;
    Button DifficultyHard;
    Difficulty difficulty = Difficulty::NONE; 

    void renderBlurredBackground();
    void renderText(std::string text, int prevHeight, int padding);
    void renderButtons();
    void renderDifficultyButtons();

public:
    Popup(SDL_Renderer *renderer, int x, int y);
    ~Popup();
    bool isClosed() { return isClose; };
    void open() { isClose = false; };
    void close() { isClose = true; };

    SDL_Texture *loadTexture(std::string filePath, int width, int height, double scale);

    void render(std::string text, int padding);
    void render(std::string textPrimary, std::string textSecondary, int padding);
    void clear() const { SDL_RenderClear(renderer); };
    void handleButtonEvent(SDL_Event *e);
    void handleButtonClicked();
    Confirmation isConfirmed() { return confirmation; };
    void clearConfirmation() { confirmation = Confirmation::NONE; };

    void renderDifficulty(std::string text, int padding);
    void handleDiffultyEvent(SDL_Event *e);
    void handleDifficultyClicked();
    Difficulty getDifficulty() { return difficulty; };
    void clearDifficulty() { difficulty = Difficulty::NONE; };

};
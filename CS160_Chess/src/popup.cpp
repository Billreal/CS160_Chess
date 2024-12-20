#include <iostream>
#include "./../include/popup.h"
#include "./../include/nanosvg.h"
#include "./../include/nanosvgrast.h"

Popup::Popup(SDL_Renderer *renderer, int x, int y) : renderer(renderer)
{
    popupInfos = {x, y, POPUP_LENGTH, POPUP_LENGTH};
    closeBtn = Button(renderer, CLOSE_BUTTON_LENGTH, CLOSE_BUTTON_LENGTH, buttonColor, {popupBg.getR(), popupBg.getG(), popupBg.getB(), popupBg.getA()}, ".s", buttonFont);
    yesBtn = Button(renderer, 120, 50, buttonColor, white, "Yes", buttonFont);
    noBtn = Button(renderer, 120, 50, buttonColor, white, "No", buttonFont);
    DifficultyEasy = Button(renderer, 150, 50, buttonColor, white, "Easy", buttonFont);
    DifficultyMedium = Button(renderer, 150, 50, buttonColor, white, "Medium", buttonFont);
    DifficultyHard = Button(renderer, 150, 50, buttonColor, white, "Hard", buttonFont);
}

Popup::~Popup()
{
    TTF_CloseFont(textFont);
    TTF_CloseFont(buttonFont);
}

SDL_Texture *Popup::loadTexture(std::string filePath, int width, int height, double scale)
{
    struct NSVGimage *image = nsvgParseFromFile(filePath.c_str(), "px", 96);
    if (!image)
    {
        printf("Failed to load SVG file.\n");
        return nullptr;
    }

    // Rasterize SVG
    struct NSVGrasterizer *rast = nsvgCreateRasterizer();
    unsigned char *imageData = (unsigned char *)malloc(width * height * 10); // RGBA buffer
    nsvgRasterize(rast, image, 0, 0, scale, imageData, width, height, width * 4);

    // Create SDL surface and texture
    SDL_Surface *surface = SDL_CreateRGBSurfaceFrom(
        imageData, width, height, 32, width * 4,
        0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

    // Cleanup
    SDL_FreeSurface(surface);
    free(imageData);
    nsvgDeleteRasterizer(rast);
    nsvgDelete(image);

    return texture;
}

void Popup::renderText(std::string text, int prevHeight, int padding)
{
    SDL_Surface *textSurface = TTF_RenderText_Blended_Wrapped(textFont, text.c_str(), white, POPUP_LENGTH - 80);
    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    int textWidth = textSurface->w;
    int textHeight = textSurface->h;
    SDL_FreeSurface(textSurface);

    prevTextHeight = textHeight + padding;
    SDL_Rect textRect = {popupInfos.x + (POPUP_LENGTH - textWidth) / 2,
                         popupInfos.y + prevHeight + padding,
                         textWidth,
                         textHeight};
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
    SDL_DestroyTexture(textTexture);
}

void Popup::renderButtons()
{
    closeBtn.renderSVG("./assets/close_button.svg", popupInfos.x + POPUP_LENGTH - CLOSE_BUTTON_LENGTH - 40, popupInfos.y + 40, SVG_SCALE);
    yesBtn.renderSVG("./assets/game_button.svg", popupInfos.x + 35, popupInfos.y + 260, SVG_SCALE);
    noBtn.renderSVG("./assets/game_button.svg", popupInfos.x + 35 + 120 + 40, popupInfos.y + 260, SVG_SCALE);
}

void Popup::renderDifficultyButtons()
{
    closeBtn.renderSVG("./assets/close_button.svg", popupInfos.x + POPUP_LENGTH - CLOSE_BUTTON_LENGTH - 40, popupInfos.y + 40, SVG_SCALE);
    DifficultyEasy.renderSVG("./assets/difficulty_button.svg", popupInfos.x + 100, popupInfos.y + 100, SVG_SCALE);
    DifficultyMedium.renderSVG("./assets/difficulty_button.svg", popupInfos.x + 100, popupInfos.y + 175, SVG_SCALE);
    DifficultyHard.renderSVG("./assets/difficulty_button.svg", popupInfos.x + 100, popupInfos.y + 250, SVG_SCALE);
}

void Popup::render(std::string textPrimary, std::string textSecondary, int padding)
{
    // SDL_Texture *popupTexture = loadTexture((std::string) "./assets/popup.svg", POPUP_LENGTH, POPUP_LENGTH, SVG_SCALE);
    // if (popupBg == semiDark)
    SDL_Texture *popupTexture = loadTexture((std::string) "./assets/popup_semidark.svg", POPUP_LENGTH, POPUP_LENGTH, SVG_SCALE);

    if (!popupTexture)
    {
        std::cerr << "Failed to load popup texture\n";
        return;
    }
    // std::cerr << popupInfos.h << " " << popupInfos.w << "\n";
    SDL_RenderCopy(renderer, popupTexture, NULL, &popupInfos);
    // SDL_DestroyTexture(popupTexture);
    renderText(textPrimary, 0, padding);
    renderText(textSecondary, prevTextHeight, padding);
    renderButtons();
}

void Popup::render(std::string text, int padding)
{
    SDL_Texture *popupTexture = loadTexture((std::string) "./assets/popup.svg", POPUP_LENGTH, POPUP_LENGTH, SVG_SCALE);
    // if (popupBg == semiDark)
    // SDL_Texture *popupTexture = loadTexture((std::string) "./assets/popup_semidark.svg", POPUP_LENGTH, POPUP_LENGTH, SVG_SCALE);

    if (!popupTexture)
    {
        std::cerr << "Failed to load popup texture\n";
        return;
    }
    // std::cerr << popupInfos.h << " " << popupInfos.w << "\n";
    SDL_RenderCopy(renderer, popupTexture, NULL, &popupInfos);
    SDL_DestroyTexture(popupTexture);
    renderText(text, 0, padding);
    renderButtons();
}

void Popup::renderDifficulty(std::string text, int padding)
{
    // SDL_Texture *popupTexture = loadTexture((std::string) "./assets/popup.svg", POPUP_LENGTH, POPUP_LENGTH, SVG_SCALE);
    SDL_Texture *popupTexture = loadTexture((std::string) "./assets/popup_semidark.svg", POPUP_LENGTH, POPUP_LENGTH, SVG_SCALE);
    if (!popupTexture)
    {
        std::cerr << "Failed to load popup texture\n";
        return;
    }
    // std::cerr << popupInfos.h << " " << popupInfos.w << "\n";
    SDL_RenderCopy(renderer, popupTexture, NULL, &popupInfos);
    // SDL_DestroyTexture(popupTexture);
    renderText(text, 0, padding);
    renderDifficultyButtons();
}

void Popup::handleButtonClicked()
{
    if (closeBtn.clicked())
    {
        isClose = true;
        closeBtn.resetClicked();
        return;
    }
    if (yesBtn.clicked())
    {
        // Handle yes button
        confirmation = YES;
        yesBtn.resetClicked();
        return;
    }
    if (noBtn.clicked())
    {
        // Handle no button
        confirmation = NO;
        noBtn.resetClicked();
        return;
    }
}

void Popup::handleButtonEvent(SDL_Event *e)
{
    closeBtn.handleEvent(e);
    yesBtn.handleEvent(e);
    noBtn.handleEvent(e);
}

void Popup::handleDiffultyEvent(SDL_Event *e)
{
    closeBtn.handleEvent(e);
    DifficultyEasy.handleEvent(e);
    DifficultyMedium.handleEvent(e);
    DifficultyHard.handleEvent(e);
}
void Popup::handleDifficultyClicked()
{
    if (closeBtn.clicked())
    {
        close();
        closeBtn.resetClicked();
        return;
    }
    if (DifficultyEasy.clicked())
    {
        difficulty = Difficulty::EASY;
        DifficultyEasy.resetClicked();
        return;
    }
    if (DifficultyMedium.clicked())
    {
        difficulty = Difficulty::MEDIUM;
        DifficultyMedium.resetClicked();
        return;
    }
    if (DifficultyHard.clicked())
    {
        difficulty = Difficulty::HARD;
        DifficultyHard.resetClicked();
        return;
    }
}
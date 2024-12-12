#include <iostream>
#include "./../include/popup.h"
#include "./../include/nanosvg.h"
#include "./../include/nanosvgrast.h"

Popup::Popup(SDL_Renderer *renderer, POPUP_MODE mode, int x, int y) : renderer(renderer), mode(mode)
{
    popupInfos = {x, y, POPUP_LENGTH, POPUP_LENGTH};
    closeBtn = Button(renderer, popupInfos.x + POPUP_LENGTH - CLOSE_BUTTON_LENGTH - 40, popupInfos.y + 40, CLOSE_BUTTON_LENGTH, CLOSE_BUTTON_LENGTH, buttonColor, popupBg, ".s", buttonFont);
    yesBtn = Button(renderer, popupInfos.x + 35, popupInfos.y + 260, 120, 50, buttonColor, white, "Yes", buttonFont);
    noBtn = Button(renderer, popupInfos.x + 35 + 120 + 40, popupInfos.y + 260, 120, 50, buttonColor, white, "No", buttonFont);
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
    closeBtn.renderSVG("./assets/close_button.svg", SVG_SCALE);
    yesBtn.renderSVG("./assets/game_button.svg", SVG_SCALE);
    noBtn.renderSVG("./assets/game_button.svg", SVG_SCALE);
}

void Popup::render(std::string textPrimary, std::string textSecondary, int padding)
{
    SDL_Texture *popupTexture = loadTexture((std::string) "./assets/popup.svg", POPUP_LENGTH, POPUP_LENGTH, SVG_SCALE);
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
    if (!popupTexture)
    {
        std::cerr << "Failed to load popup texture\n";
        return;
    }
    // std::cerr << popupInfos.h << " " << popupInfos.w << "\n";
    SDL_RenderCopy(renderer, popupTexture, NULL, &popupInfos);
    // SDL_DestroyTexture(popupTexture);
    renderText(text, 0, padding);
    renderButtons();
}

void Popup::handleButtonClicked()
{
    if(closeBtn.clicked())
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

Confirmation Popup::isConfirmed()
{
    return confirmation;
}
#include <iostream>
#include "./../include/popup.h"
#include "./../include/nanosvg.h"
#include "./../include/nanosvgrast.h"

void Popup::renderText()
{
    SDL_Color textColor = {255, 255, 255, 255}; // White color
    SDL_Surface *textSurface = TTF_RenderText_Blended(font, text.c_str(), textColor);
    textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_FreeSurface(textSurface);

    int textWidth, textHeight;
    SDL_QueryTexture(textTexture, NULL, NULL, &textWidth, &textHeight);
    SDL_Rect textRect = {popupRect.x + 20, popupRect.y + 20, textWidth, textHeight};
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
}

void Popup::renderButtons()
{
    // Render agree button
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Green color
    SDL_RenderFillRect(renderer, &agreeButtonRect);
    // Render don't button
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red color
    SDL_RenderFillRect(renderer, &dontButtonRect);
}

Popup::Popup(SDL_Renderer *renderer, POPUP_MODE mode): renderer(renderer), mode(mode){}

Popup::~Popup()
{
    SDL_DestroyTexture(textTexture);
    SDL_DestroyTexture(agreeButtonTexture);
    SDL_DestroyTexture(dontButtonTexture);
    TTF_CloseFont(textFont);
    TTF_CloseFont(buttonFont);
}

void Popup::render(std::string text)
{
    SDL_SetRenderDrawColor(renderer, bgColor.getR(), bgColor.getG(), bgColor.getB(), bgColor.getA());
    SDL_RenderFillRect(renderer, &popupRect);
    renderText();
    renderButtons();
}

bool Popup::handleEvent(SDL_Event *e)
{
    if (e->type == SDL_MOUSEBUTTONDOWN)
    {
        int x, y;
        SDL_GetMouseState(&x, &y);
        if (x >= agreeButtonRect.x && x <= agreeButtonRect.x + agreeButtonRect.w &&
            y >= agreeButtonRect.y && y <= agreeButtonRect.y + agreeButtonRect.h)
        {
            std::cout << "Agree button clicked" << std::endl;
            return true;
        }
        if (x >= dontButtonRect.x && x <= dontButtonRect.x + dontButtonRect.w &&
            y >= dontButtonRect.y && y <= dontButtonRect.y + dontButtonRect.h)
        {
            std::cout << "Don't button clicked" << std::endl;
            return false;
        }
    }
    return false;
}

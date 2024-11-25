#include "../include/button.h"
#include <iostream>
Button::Button(SDL_Renderer *renderer) : renderer(renderer) {}

Button::Button(SDL_Renderer *renderer, int x, int y, int w, int h, SDL_Color color, SDL_Color textColor, const std::string &text, TTF_Font *font)
    : renderer(renderer), color(color), textColor(textColor), text(text), font(font), isClicked(false)
{
    rect = {x, y, w, h};
}

Button::Button() {};
void Button::renderRect(SDL_Rect rect, SDL_Color color)
{
    // std::cerr << "Rendeing rect at: " << rect.x << " " << rect.y << "\n";
    SDL_Rect fillRect = {rect.x, rect.y, rect.w, rect.h};
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &fillRect);
}
void Button::renderRect(SDL_Rect rect, colorRGBA color)
{
    SDL_Rect fillRect = {rect.x, rect.y, rect.w, rect.h};
    SDL_SetRenderDrawColor(renderer, color.getR(), color.getG(), color.getB(), color.getA());
    SDL_RenderFillRect(renderer, &fillRect);
}

void Button::render()
{
    // Render button with rounded corners
    renderRect(rect, color);

    // // Render text
    SDL_Surface *textSurface = TTF_RenderText_Solid(font, text.c_str(), textColor);
    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    int textWidth = textSurface->w;
    int textHeight = textSurface->h;
    SDL_FreeSurface(textSurface);
    SDL_Rect textRect = {rect.x + (rect.w - textWidth) / 2, rect.y + (rect.h - textHeight) / 2, textWidth, textHeight};
    std::cerr << textRect.x << " " << textRect.y << " " << textRect.w << " " << textRect.h << "\n";
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
    SDL_DestroyTexture(textTexture);
}

void Button::handleEvent(SDL_Event *e)
{
    if (e->type == SDL_MOUSEBUTTONDOWN)
    {
        int x, y;
        SDL_GetMouseState(&x, &y);
        bool inside = true;

        if (x < rect.x)
        {
            inside = false;
        }
        else if (x > rect.x + rect.w)
        {
            inside = false;
        }
        else if (y < rect.y)
        {
            inside = false;
        }
        else if (y > rect.y + rect.h)
        {
            inside = false;
        }

        if (inside)
        {
            isClicked = true;
        }
    }
}

bool Button::clicked() const
{
    return isClicked;
}

void Button::reset()
{
    isClicked = false;
}

void Button::clear()
{
    renderRect(rect, bgColor);
}

void Button::setColor(colorRGBA color)
{
    SDL_Color toSet = {color.getR(), color.getG(), color.getB(), color.getA()};
    setColor(toSet);
}

void Button::setColor(SDL_Color color)
{
    this -> color = color;
}
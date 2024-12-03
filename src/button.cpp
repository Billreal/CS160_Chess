#include <iostream>
#include "../include/button.h"
#include "./../include/nanosvg.h"
#include "./../include/nanosvgrast.h"

Button::Button(SDL_Renderer *renderer) : renderer(renderer) {}

Button::Button(SDL_Renderer *renderer, int x, int y, int w, int h, SDL_Color color, SDL_Color textColor, std::string text, TTF_Font *font)
    : renderer(renderer), color(color), textColor(textColor), text(text), font(font), isClicked(false)
{
    rect = {x, y, w, h};
}

void Button::updateColor(SDL_Color newColor)
{
    color = newColor;
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
    // std::cerr << "Rendeing rect at: " << rect.x << " " << rect.y << "\n";
    SDL_Color newColor = {color.getR(), color.getG(), color.getB(), color.getA()};
    renderRect(rect, newColor);
}

// Function to render SVG with text in the middle
void Button::renderSVG(std::string svgFilePath, double scale)
{
    const int width = rect.w;
    const int height = rect.h;
    // std::cerr<< "Width: " << width << " Height: "<< height << "\n";
    if (!width || !height)
    {
        printf("Width or height is 0.\n");
        return;
    }

    // Load SVG image
    NSVGimage *image = nsvgParseFromFile(svgFilePath.c_str(), "px", 96);
    if (!image)
    {
        printf("Could not open SVG image.\n");
        return;
    }

    // Create rasterizer
    NSVGrasterizer *rast = nsvgCreateRasterizer();
    if (!rast)
    {
        printf("Could not create rasterizer.\n");
        nsvgDelete(image);
        return;
    }

    // Allocate memory for image
    unsigned char *imageData = (unsigned char *)malloc(width * height * 4);
    if (imageData == NULL)
    {
        printf("Could not allocate memory for image.\n");
        nsvgDeleteRasterizer(rast);
        nsvgDelete(image);
        return;
    }

    // Rasterize SVG image
    nsvgRasterize(rast, image, 0, 0, scale, imageData, width, height, width * 4);

    // Create SDL texture from rasterized image
    SDL_Surface *svgSurface = SDL_CreateRGBSurfaceFrom(
        imageData,
        width, height, 32, width * 4,
        0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
    SDL_Texture *svgTexture = SDL_CreateTextureFromSurface(renderer, svgSurface);

    // Cleanup
    SDL_FreeSurface(svgSurface);
    free(imageData);
    nsvgDeleteRasterizer(rast);
    nsvgDelete(image);

    // Render button rect
    // std::cerr << rect.x << rect.y <<  width << height << "\n";
    // renderRect(rect, color);

    // Render SVG texture
    SDL_RenderCopy(renderer, svgTexture, NULL, &rect);
    SDL_DestroyTexture(svgTexture);

    // Render text
    if (text.empty())
    {
        std::cerr << "Text is empty\n";
        return;
    }
    SDL_Surface *textSurface = TTF_RenderText_Solid(font, text.c_str(), textColor);
    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    int textWidth = textSurface->w;
    int textHeight = textSurface->h;
    SDL_FreeSurface(textSurface);
    SDL_Rect textRect = {rect.x + (width - textWidth) / 2, rect.y + (height - textHeight) / 2, textWidth, textHeight};
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
    SDL_DestroyTexture(textTexture);
}

void Button::render()
{
    // Render button rect
    renderRect(rect, color);

    // Render rectBefore
    renderRect({rect.x, rect.y, 20, rect.h}, SDL_Color({238, 238, 210, 255}));

    // Render text
    SDL_Surface *textSurface = TTF_RenderText_Solid(font, text.c_str(), textColor);
    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    int textWidth = textSurface->w;
    int textHeight = textSurface->h;
    SDL_FreeSurface(textSurface);
    SDL_Rect textRect = {rect.x + (rect.w - textWidth) / 2, rect.y + (rect.h - textHeight) / 2, textWidth, textHeight};
    // std::cerr << textRect.x << " " << textRect.y << " " << textRect.w << " " << textRect.h << "\n";
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
    SDL_DestroyTexture(textTexture);
}

void Button::handleEvent(SDL_Event *e)
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

    switch (e->type)
    {
    case SDL_MOUSEMOTION:
    {
        if (inside)
        {
            isHovered = true;
        }
        else
        {
            isHovered = false;
        }
        break;
    }
    case SDL_MOUSEBUTTONDOWN:
    {
        if (inside)
        {
            isClicked = true;
        }
    }
    }
}

bool Button::clicked() const
{
    return isClicked;
}
bool Button::hover() const
{
    return isHovered;
}

void Button::resetClicked()
{
    isClicked = false;
}
void Button::resetHovered()
{
    isHovered = false;
}

void Button::setColor(colorRGBA color)
{
    SDL_Color newColor = {color.getR(), color.getG(), color.getB(), color.getA()};
    updateColor(newColor);
}

// void Button::clear()
// {
//     if (rect.x == -1 || rect.y == -1 || rect.w == -1 || rect.h == -1) return;
//     renderRect(rect, bgColor);
// }

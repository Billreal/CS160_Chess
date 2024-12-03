#pragma once
#include <SDL2/SDL.h>
class Coordinate
{
    private:
    int x;
    int y;

    public:
    Coordinate();
    Coordinate(int x, int y);
    Coordinate(SDL_MouseButtonEvent ev);

    int getX();
    int getY();
    Coordinate operator+(Coordinate x);
    Coordinate operator-(Coordinate x);
    Coordinate operator*(int x);
    bool operator<(Coordinate x);
    bool operator==(Coordinate coord);
};
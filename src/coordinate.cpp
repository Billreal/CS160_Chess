#include "./../include/coordinate.h"
#include <SDL.h>
Coordinate::Coordinate()
{
    x = 0;
    y = 0;
}

Coordinate::Coordinate(int x, int y)
{
    this -> x = x;
    this -> y = y;
}

Coordinate::Coordinate(SDL_MouseButtonEvent ev)
{
    Coordinate(ev.x, ev.y);
}

int Coordinate::getX()
{
    return x;
}

int Coordinate::getY()
{
    return y;
}

Coordinate Coordinate::operator+(Coordinate x)
{
    return Coordinate(this -> x + x.x, this -> y + x.y);
}

Coordinate Coordinate::operator-(Coordinate x)
{
    return Coordinate(this -> x - x.x, this -> y - x.y);
}

Coordinate Coordinate::operator*(int x)
{
    return Coordinate(this -> x * x, this -> y * x);
}

bool Coordinate::operator<(Coordinate x)
{
    if (this -> x == x.x)
        return (bool)(this -> y < x.y);
    return (bool)(this -> x < x.x);
}

bool Coordinate::operator==(Coordinate coord)
{
    if (x == coord.x && y == coord.y) return true;
    return false;
}
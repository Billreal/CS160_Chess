#include "./../include/color.h"
colorRGBA ::colorRGBA(int r, int g, int b, int a)
{
    this->r = r;
    this->g = g;
    this->b = b;
    this->a = a;
}

int colorRGBA::getR()
{
    return r;
}

int colorRGBA::getG()
{
    return g;
}

int colorRGBA::getB()
{
    return b;
}

int colorRGBA::getA()
{
    return a;
}

colorRGBA::colorRGBA()
{
    r = g = b = a = 0;
}
#pragma once
#include "./../include/color.h"
colorRGB :: colorRGB(int r, int g, int b)
{
    this->r = r;
    this->g = g;
    this->b = b;
}

int colorRGB::getR()
{
    return r;
}

int colorRGB::getG()
{
    return g;
}

int colorRGB::getB()
{
    return b;
}
#pragma once
#include "./../include/color.h"
colorRGBA ::colorRGBA(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
    this->r = r;
    this->g = g;
    this->b = b;
    this->a = a;
}

unsigned char colorRGBA::getR()
{
    return r;
}

unsigned char colorRGBA::getG()
{
    return g;
}

unsigned char colorRGBA::getB()
{
    return b;
}

unsigned char colorRGBA::getA()
{
    return a;
}

colorRGBA::colorRGBA()
{
    r = g = b = a = 0;
}
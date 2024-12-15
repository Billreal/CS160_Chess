#pragma once
class colorRGBA
{
private:
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;

public:
    colorRGBA(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
    unsigned char getR();
    unsigned char getG();
    unsigned char getB();
    unsigned char getA();
    
    bool operator==(colorRGBA color)
    {
        if (r == color.r && g == color.g && b == color.b && a == color.a) return true;
        return false;
    }

    colorRGBA();
};

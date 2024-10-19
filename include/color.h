#pragma once
class colorRGBA
{
private:
    int r;
    int g;
    int b;
    int a;

public:
    colorRGBA(int r, int g, int b, int a);
    int getR();
    int getG();
    int getB();
    int getA();
    
};

class Coordinate
{
    private:
    int x;
    int y;

    public:
    Coordinate();
    Coordinate(int x, int y);
    int getX();
    int getY();
    Coordinate operator+(Coordinate x);
    Coordinate operator-(Coordinate x);
    Coordinate operator*(int x);
    bool operator<(Coordinate x);
};
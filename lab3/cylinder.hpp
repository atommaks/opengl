#include <vector>

class Point
{
public:
    float x, y, z;
    Point (float x, float y, float z);
    Point ();
};

class Cylinder
{
private:
    std::vector<std::vector<std::vector<Point> > > linesArr, fEllArr, sEllArr;
public:
    float xCenter, yCenter, rx, ry, h, ddx, ddy;
    Cylinder (float xCenter, float yCenter, float rx, float ry, float h, float ddx, float ddy);
    void recalc (int n, int k, int kr);
    std::vector<std::vector<std::vector<Point> > >& getLinesArr();
    std::vector<std::vector<std::vector<Point> > >& getfEllArr();
    std::vector<std::vector<std::vector<Point> > >& getsEllArr();
};

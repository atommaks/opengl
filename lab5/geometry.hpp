#include <vector>

class Point
{
public:
    double x, y, z;
    Point (double x, double y, double z);
    Point ();
};

class Cylinder
{
private:
    std::vector<std::vector<std::vector<Point> > > linesArr, fEllArr, sEllArr;
public:
    double xCenter, yCenter, rx, ry, h, ddx, ddy;
    Cylinder (double xCenter, double yCenter, double rx, double ry, double h, double ddx, double ddy);
    void recalc (int n, int k);
    std::vector<std::vector<std::vector<Point> > >& getLinesArr();
    std::vector<std::vector<std::vector<Point> > >& getfEllArr();
    std::vector<std::vector<std::vector<Point> > >& getsEllArr();
};

class Segment
{
private:
    std::vector<Point> pointsIn, pointsOut;
    double x1, x2, y1, y2, z1, z2;
public:
    double getX1 ();
    double getX2 ();
    double getY1 ();
    double getY2 ();
    double getZ1 ();
    double getZ2 ();
    void setX1 (double);
    void setX2 (double);
    void setY1 (double);
    void setY2 (double);
    void setZ1 (double);
    void setZ2 (double);
    void setPointsIn(std::vector<Point>);
    void setPointsOut(std::vector<Point>);
    void setDefault ();
    void changePoints ();
    Segment(double, double, double, double, double, double);
    std::vector<Point>& getPointsIn();
    std::vector<Point>& getPointsOut();
};

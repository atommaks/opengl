#include <vector>

class Point
{
public:
    double x, y, z;
    Point (double, double, double);
    Point ();
};

class Cylinder
{
private:
    std::vector<std::vector<std::vector<Point>>> linesArr, fEllArr, sEllArr;
    std::vector<std::vector<std::vector<std::vector<Point>>>> normals;
    std::vector<std::vector<std::vector<std::vector<std::pair<double, double>>>>> textCoords;
    void countTextCoords(int, int);
public:
    double xCenter, yCenter, rx, ry, h, ddx, ddy;
    Cylinder (double, double, double, double, double, double, double);
    void recalc (int n, int k);
    std::vector<std::vector<std::vector<Point>>>& getLinesArr();
    std::vector<std::vector<std::vector<Point>>>& getfEllArr();
    std::vector<std::vector<std::vector<Point>>>& getsEllArr();
    std::vector<std::vector<std::vector<std::vector<Point>>>>& getNormals();
    std::vector<std::vector<std::vector<std::vector<std::pair<double, double>>>>>& getTextCoords();
};

class CutCylinder
{
private:
    std::vector<std::vector<std::vector<Point>>> linesArr, fEllArr, sEllArr;
    std::vector<std::vector<std::vector<std::vector<Point>>>> normals;
    //std::vector<std::vector<std::vector<std::vector<std::pair<double, double>>>>> textCoords;
    //void countTextCoords(int, int);
public:
    double xCenter, yCenter, rxU, ryU, rxD, ryD, height, ddx, ddy;
    CutCylinder (double, double, double, double, double, double, double);
    void recalc (int n, int k);
    std::vector<std::vector<std::vector<Point>>>& getfEllArr ();
    std::vector<std::vector<std::vector<Point>>>& getsEllArr ();
    std::vector<std::vector<std::vector<Point>>>& getLinesArr ();
    std::vector<std::vector<std::vector<std::vector<Point>>>>& getNormals();
    //std::vector<std::vector<std::vector<std::vector<std::pair<double, double>>>>>& getTextCoords();
};

typedef struct s_stage
{
    double rotationX, rotationY, rotationZ;
    int n, k;
    bool reDraw, LightModel, lighted, animation, animated, textured;
} t_stage;

Point multiVector (Point, Point);
Point countNormal (int, int, int, int,
                   std::vector<std::vector<std::vector<std::vector<Point>>>>*);
double dot (Point, Point);
void normalize (Point*);
void countNormals (std::vector<std::vector<std::vector<Point>>>*,
                   std::vector<std::vector<std::vector<Point>>>*,
                   std::vector<std::vector<std::vector<Point>>>*,
                   std::vector<std::vector<std::vector<std::vector<Point>>>>*,
                   double, double, double, double, double);


#include <iostream>
#include <fstream>
#include <time.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "./cylinder.cpp"

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

void keyCallback (GLFWwindow*, int, int, int, int);
void draw (std::vector<std::vector<std::vector<Point>>>*,
           std::vector<std::vector<std::vector<Point>>>*,
           std::vector<std::vector<std::vector<Point>>>*,
           std::vector<std::vector<std::vector<std::vector<Point>>>>*);
void tweeningAnimation ();
void prepareAnimation ();
void initTexture (char*);
void saveFile ();
void loadFile ();
void clearFile ();
Point cubeEq (Point, Point, Point, Point, float);

Cylinder* c;
CutCylinder* cc;
t_stage stage;
bool reDraw = false, LightModel = false, lighted = true, animation = false, animated = false,
textured = false, localViewer = true, twoSide = true;
int n = 1500, k = 1, screenWidth = 1280, screenHeight = 720, sign = 1,
     initialTime = time(NULL), finalTime, frameCount;
float t = 0.f;
double rotationY = 0.f, rotationX = 0.f, rotationZ = 0.f, ambV = 0.2, size = 2.0;
std::vector<std::vector<std::vector<std::vector<Point>>>> arr1, arr2, arr3, arr4, arr, aNorm;
std::vector<std::vector<std::vector<std::vector<std::pair<double, double>>>>> textCoords;
GLuint texture;
int frames = 0;
int delta = 0;

int main(int argc, const char * argv[])
{
    GLFWwindow *window;
    
    if (!glfwInit())
        return -1;
    
    window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Lab6", nullptr, nullptr);
    glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    
    glfwSetKeyCallback(window, keyCallback);
    glfwMakeContextCurrent(window);
    
    double a = asin(tanf(M_PI / 6.f)), b = M_PI_4, ratio = (double) screenWidth / screenHeight;
    double m[16] =
    {
        cos(b) / ratio, sin(b) * sin(a), 0.0, 0.0,
        0.0, cos(a), 0.0, 0.0,
        sin(b) / ratio, -sin(a) * cos(b), 0.0, 0.0,
        0.0, 0.0, 0.0, 1.0
    };
    
    initTexture("/Users/ATOM/Documents/Bomonka/IV Semestr/Алгоритмы компьютерной графики/лабы/лаба 6/lab6/lab6/textures/texture.bmp");
    
    c = new Cylinder(0.0, 0.0, 0.25, 0.25, 0.4, 0.0, 0.0);
    c->recalc(n, k);
    textCoords = c->getTextCoords();
    cc = new CutCylinder(0.0, 0.0, 0.15, 0.15, 0.25, 0.25, 0.3);
    cc->recalc(n, k);
    prepareAnimation();
    arr.push_back(c->getfEllArr());
    arr.push_back(c->getsEllArr());
    arr.push_back(c->getLinesArr());
    aNorm = c->getNormals();
    while (!glfwWindowShouldClose(window))
    {
        glViewport(0.f, 0.f, screenWidth, screenHeight);
        glClear(GL_COLOR_BUFFER_BIT);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glLoadMatrixd(m);
        
        if (lighted)
        {
            glEnable(GL_NORMALIZE);
            glEnable(GL_LIGHTING);
            glEnable(GL_COLOR_MATERIAL);
            glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
            glEnable(GL_LIGHT0);
            glShadeModel(GL_SMOOTH);
            if (!LightModel)
            {
                float Ambient[4] = {static_cast<float>(ambV), static_cast<float>(ambV), static_cast<float>(ambV), 1.0};
                float Diffuse[4] = {0.8, 0.8, 0.8, 1.0};
                float Specular[4] = {0.0, 0.0, 0.0, 1.0};
                float Color[4] = {0.2, 0.2, 0.2, 1};
                float Position[4] = {1.5, -1.2, 15.0, 1};
                float Emission[4] = {0.0, 0.0, 0.0, 1.0};
                glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, twoSide);
                glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, localViewer);
                glLightModelfv(GL_LIGHT_MODEL_AMBIENT, Color);
                glMaterialf(GL_FRONT, GL_SHININESS, 40.0);
                glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, Emission);
                glLightfv(GL_LIGHT0, GL_AMBIENT, Ambient);
                glLightfv(GL_LIGHT0, GL_DIFFUSE, Diffuse);
                glLightfv(GL_LIGHT0, GL_SPECULAR, Specular);
                glLightfv(GL_LIGHT0, GL_POSITION, Position);
            }
            else
            {
                float Ambient[4] = {0.1, 0.1, 0.1, 1.0};
                float Diffuse[4] = {0.5, 0.5, 0.5, 1.0};
                float Specular[4] = {0.7, 0.7, 0.7, 1.0};
                float Color[4] = {0.1, 0.1, 0.1, 1};
                float Position[4] = {1.5, -1.2, 15.0, 1};
                float Emission[4] = {0.075, 0.05, 0.0, 1.0};
                glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
                glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_FALSE);
                glLightModelfv(GL_LIGHT_MODEL_AMBIENT, Color);
                glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, Specular);
                glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 82.0);
                glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, Emission);
                glLightfv(GL_LIGHT0, GL_AMBIENT, Ambient);
                glLightfv(GL_LIGHT0, GL_DIFFUSE, Diffuse);
                glLightfv(GL_LIGHT0, GL_SPECULAR, Specular);
                glLightfv(GL_LIGHT0, GL_POSITION, Position);
            }
        }
        else
        {
            glDisable(GL_LIGHTING);
            glDisable(GL_LIGHT0);
            glDisable(GL_NORMALIZE);
        }
        
        if (textured)
        {
            glEnable(GL_TEXTURE_2D);
            glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
        }
        else
            glDisable(GL_TEXTURE_2D);
        
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glRotated(rotationX, 1, 0, 0);
        glRotated(rotationY, 0, 1, 0);
        glRotated(rotationZ, 0, 0, 1);
        glScaled(size, size, size);
        
                if (!animation && !animated)
                    draw(&c->getfEllArr(), &c->getsEllArr(), &c->getLinesArr(), &c->getNormals());
                else if ((!animation && animated) || (animation && !animated))
                {
                    tweeningAnimation();
                    draw(&arr[0], &arr[1], &arr[2], &aNorm);
                }
                else
                    draw(&cc->getfEllArr(), &cc->getsEllArr(), &cc->getLinesArr(), &cc->getNormals());
//        if (animation)
//        {
//            tweeningAnimation();
//            draw(&arr[0], &arr[1], &arr[2], &aNorm);
//        }
//        else
//        {
//            draw(&arr[0], &arr[1], &arr[2], &aNorm);
//        }
        
        glPopMatrix();
        
        glfwSwapBuffers(window);
        glfwPollEvents();
        
        frameCount++;
        finalTime = time(NULL);
//        if (finalTime - initialTime > 0)
//        {
//            std::cout << frameCount / (finalTime - initialTime) << std::endl;
//            frameCount = 0;
//            initialTime = finalTime;
//        }
    }
    
    delete c;
    delete cc;
    glfwTerminate();
    return 0;
}

void draw (std::vector<std::vector<std::vector<Point>>>* fEllArr,
           std::vector<std::vector<std::vector<Point>>>* sEllArr,
           std::vector<std::vector<std::vector<Point>>>* linesArr,
           std::vector<std::vector<std::vector<std::vector<Point>>>>* normals)
{
    unsigned int start_time =  clock();
    if (reDraw)
    {
        c->recalc(n, k);
        textCoords = c->getTextCoords();
        cc->recalc(n, k);
        prepareAnimation();
        reDraw = false;
    }
    glColor3f(1, 0, 0);
    std::vector<std::vector<std::vector<Point>>>* aarr;
    for (int a = 0; a < 3; a++)
    {
        if (a == 0)
            aarr = fEllArr;
        else if (a == 1)
            aarr = sEllArr;
        else
            aarr = linesArr;
        for (int i = 0; i < aarr->size(); i++)
        {
            for (int j = 0; j < (*aarr)[i].size(); j++)
            {
                glBegin(GL_POLYGON);
                for (int m = 0; m < (*aarr)[i][j].size(); m++)
                {
                    glNormal3d((*normals)[a][i][j][m].x, (*normals)[a][i][j][m].y, (*normals)[a][i][j][m].z);
                    if (textured)
                        glTexCoord2d(textCoords[a][i][j][m].first, textCoords[a][i][j][m].second);
                    glVertex3d((*aarr)[i][j][m].x, (*aarr)[i][j][m].y, (*aarr)[i][j][m].z);
                }
                glEnd();
            }
        }
    }
    unsigned int end_time = clock();
    frames++;
    delta += end_time - start_time;
    if (frames == 1000)
    {
        std::cout << (float)frames / delta << std::endl;
        frames = 0;
        delta = 0;
    }
}

void prepareAnimation ()
{
    arr1.clear();
    arr1.push_back(c->getfEllArr());
    arr1.push_back(c->getsEllArr());
    arr1.push_back(c->getLinesArr());
    
    Cylinder cylinder = Cylinder(0.0, 0.0, 0.22, 0.22, 0.325, 0.0, 0.0);
    cylinder.recalc(n, k);
    arr2.clear();
    arr2.push_back(cylinder.getfEllArr());
    arr2.push_back(cylinder.getsEllArr());
    arr2.push_back(cylinder.getLinesArr());
    
    CutCylinder cutCylinder = CutCylinder(0.0, 0.0, 0.2, 0.2, 0.3, 0.3, 0.315);
    cutCylinder.recalc(n, k);
    arr3.clear();
    arr3.push_back(cutCylinder.getfEllArr());
    arr3.push_back(cutCylinder.getsEllArr());
    arr3.push_back(cutCylinder.getLinesArr());
    
    arr4.clear();
    arr4.push_back(cc->getfEllArr());
    arr4.push_back(cc->getsEllArr());
    arr4.push_back(cc->getLinesArr());
}

void tweeningAnimation ()
{
    if (t > 1 || t < 0)
    {
        animated = !animated;
        sign = -sign;
        std::cout << "Animation's end" << std::endl;
    }
    arr.clear();
    for (int a = 0; a < arr1.size(); a++)
    {
        std::vector<std::vector<std::vector<Point>>> ellArr;
        for (int i = 0; i < arr1[a].size(); i++)
        {
            std::vector<std::vector<Point>> sector;
            for (int j = 0; j < arr1[a][i].size(); j++)
            {
                std::vector<Point> block;
                for (int m = 0; m < arr1[a][i][j].size(); m++)
                {
                    block.push_back(cubeEq(arr1[a][i][j][m], arr2[a][i][j][m], arr3[a][i][j][m], arr4[a][i][j][m], t));
                }
                sector.push_back(block);
            }
            ellArr.push_back(sector);
        }
        arr.push_back(ellArr);
    }
    countNormals(&arr[0], &arr[1], &arr[2], &aNorm, 0, 0, arr[1][0][0][0].y - arr[0][0][0][0].y, arr[0][0][0][0].x, arr[0][0][0][0].z);
    t += 0.05 * sign;
}

Point cubeEq (Point p1, Point p2, Point p3, Point p4, float t)
{
    Point p;
    p.x = pow(1.f - t, 3) * p1.x + 3.f * t * pow(1.f - t, 2) * p2.x +
    3.f * pow(t, 2) * (1.f - t) * p3.x + pow(t, 3) * p4.x;
    p.y = pow(1.f - t, 3) * p1.y + 3.f * t * pow(1.f - t, 2) * p2.y +
    3.f * pow(t, 2) * (1.f - t) * p3.y + pow(t, 3) * p4.y;
    p.z = pow(1.f - t, 3) * p1.z + 3.f * t * pow(1.f - t, 2) * p2.z +
    3.f * pow(t, 2) * (1.f - t) * p3.z + pow(t, 3) * p4.z;
    return p;
}

void initTexture (char *filename)
{
    FILE *f = fopen(filename, "rb");
    unsigned char info[54];
    fread(info, sizeof(unsigned char), 54, f);
    int width = *(int*)&info[18];
    int height = *(int*)&info[22];
    int size = 3 * width * height;
    unsigned char *data = new unsigned char[size];
    fread(data, sizeof(unsigned char), size, f);
    fclose(f);
    
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height,GL_BGR, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
    
    delete[] data;
}

void saveFile ()
{
    clearFile();
    std::ofstream fout;
    std::string path = "/Users/ATOM/Documents/Bomonka/IV Semestr/Алгоритмы компьютерной графики/лабы/лаба 6/lab6/lab6/save.txt";
    fout.open(path, std::ofstream::app);
    if (fout.is_open())
    {
        stage.animated = animated;
        stage.animation = animation;
        stage.k = k;
        stage.lighted = lighted;
        stage.LightModel = LightModel;
        stage.n = n;
        stage.reDraw = reDraw;
        stage.rotationX = rotationX;
        stage.rotationY = rotationY;
        stage.rotationZ = rotationZ;
        stage.textured = textured;
        fout.write((char*)&stage, sizeof(t_stage));
    }
    fout.close();
}

void loadFile ()
{
    std::ifstream fin;
    std::string path = "/Users/ATOM/Documents/Bomonka/IV Semestr/Алгоритмы компьютерной графики/лабы/лаба 6/lab6/lab6/save.txt";
    fin.open(path);
    if (fin.is_open())
        fin.read((char*)&stage, sizeof(t_stage));
    animated = stage.animated;
    animation = stage.animation;
    k = stage.k;
    lighted = stage.lighted;
    LightModel = stage.LightModel;
    n = stage.n;
    reDraw = stage.reDraw;
    rotationX = stage.rotationX;
    rotationY = stage.rotationY;
    rotationZ = stage.rotationZ;
    textured = stage.textured;
    fin.close();
}

void clearFile ()
{
    std::ofstream fout;
    std::string path = "/Users/ATOM/Documents/Bomonka/IV Semestr/Алгоритмы компьютерной графики/лабы/лаба 6/lab6/lab6/save.txt";
    fout.open(path);
    fout.clear();
    fout.close();
}

void keyCallback (GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        const double rotationSpeed = 1.5;
        switch (key)
        {
            case GLFW_KEY_1:
                if (n > 3)
                {
                    n--;
                    reDraw = true;
                }
                break;
            case GLFW_KEY_2:
                if (n < 360)
                {
                    n++;
                    reDraw = true;
                }
                break;
            case GLFW_KEY_3:
                if (k > 1)
                {
                    k--;
                    reDraw = true;
                }
                break;
            case GLFW_KEY_4:
                if (k < 5)
                {
                    k++;
                    reDraw = true;
                }
                break;
            case GLFW_KEY_LEFT:
                rotationY -= rotationSpeed;
                break;
            case GLFW_KEY_RIGHT:
                rotationY += rotationSpeed;
                break;
            case GLFW_KEY_UP:
                rotationX += rotationSpeed;
                break;
            case GLFW_KEY_DOWN:
                rotationX -= rotationSpeed;
                break;
            case GLFW_KEY_O:
                rotationZ -= rotationSpeed;
                break;
            case GLFW_KEY_P:
                rotationZ += rotationSpeed;
                break;
            case GLFW_KEY_K:
                LightModel = !LightModel;
                break;
            case GLFW_KEY_L:
                lighted = !lighted;
                break;
            case GLFW_KEY_A:
                std::cout << "Animation's start" << std::endl;
                animation = !animation;
                break;
            case GLFW_KEY_T:
                textured = !textured;
                break;
            case GLFW_KEY_S:
                saveFile();
                break;
            case GLFW_KEY_Q:
                loadFile();
                break;
            case GLFW_KEY_5:
                ambV -= 0.1;
                break;
            case GLFW_KEY_6:
                ambV += 0.1;
                break;
            case GLFW_KEY_W:
                localViewer = !localViewer;
                break;
            case GLFW_KEY_E:
                twoSide = !twoSide;
                break;
            case GLFW_KEY_EQUAL:
                size += 0.25;
                break;
            case GLFW_KEY_MINUS:
                if (size > 0.25)
                    size -= 0.25;
                break;
        }
    }
}


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
void getArr (std::vector<std::vector<std::vector<Point>>>*,
             std::vector<std::vector<std::vector<Point>>>*,
             std::vector<std::vector<std::vector<Point>>>*,
             std::vector<std::vector<std::vector<std::vector<Point>>>>*,
             std::vector<std::vector<std::vector<std::vector<std::pair<double, double>>>>>*);
Point cubeEq (Point, Point, Point, Point, float);

Cylinder* c;
CutCylinder* cc;
t_stage stage;
bool reDraw = false, animation = false, animated = false, textured = false,
     localViewer = false, twoSide = false;
int n = 1500, k = 1, screenWidth = 1280, screenHeight = 720, sign = 1,
    initialTime = time(NULL), finalTime, frameCount;
float t = 0.f;
double rotationY = 0.0, rotationX = 0.0, rotationZ = 0.0, ambV = 0.2, size = 2.0;
std::vector<std::vector<std::vector<std::vector<Point>>>> arr1, arr2, arr3, arr4, arr, aNorm;
std::vector<std::vector<std::vector<std::vector<std::pair<double, double>>>>> textCoords, animationTextCoords;
std::vector<double> ellipse1, ellipse2, lines, norms1, norms2, norms3, texts1, texts2;
GLuint texture;
int frames = 0;
int delta = 0;

int main(int argc, const char * argv[])
{
    GLFWwindow *window;
    
    if (!glfwInit())
        return -1;
    
    window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Lab7", nullptr, nullptr);
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
    
    initTexture("/Users/ATOM/Documents/Bomonka/IV Semestr/Алгоритмы компьютерной графики/лабы/лаба 7/lab7/lab7/textures/texture.bmp");
    
    c = new Cylinder(0.0, 0.0, 0.25, 0.25, 0.4, 0.0, 0.0);
    cc = new CutCylinder(0.0, 0.0, 0.15, 0.15, 0.25, 0.25, 0.3);
    prepareAnimation();
    c->recalc(n, k);
    cc->recalc(n, k);
    textCoords = c->getTextCoords();
    arr.push_back(c->getfEllArr());
    arr.push_back(c->getsEllArr());
    arr.push_back(c->getLinesArr());
    aNorm = c->getNormals();
    getArr(&arr[0], &arr[1], &arr[2], &aNorm, &textCoords);
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
        
        glEnable(GL_NORMALIZE);
        glEnable(GL_LIGHTING);
        glEnable(GL_COLOR_MATERIAL);
        glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
        glEnable(GL_LIGHT0);
        glShadeModel(GL_SMOOTH);
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
        reDraw = false;
        if (!animation && !animated)
            getArr(&c->getfEllArr(), &c->getsEllArr(), &c->getLinesArr(), &c->getNormals(), &c->getTextCoords());
        else
            getArr(&cc->getfEllArr(), &cc->getsEllArr(), &cc->getLinesArr(), &cc->getNormals(), &c->getTextCoords());
    }
    
    GLfloat color[] = {1, 0, 0};
    glColor3fv(color);
    
    if ((!animation && animated) || (animation && !animated))
        getArr(fEllArr, sEllArr, linesArr, normals, &animationTextCoords);
    
    if (textured)
    {
        glEnableClientState(GL_NORMAL_ARRAY);
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        
        glVertexPointer(3, GL_DOUBLE, 0, ellipse1.data());
        glNormalPointer(GL_DOUBLE, 0, norms1.data());
        glTexCoordPointer(2, GL_DOUBLE, 0, texts1.data());
        glDrawArrays(GL_TRIANGLES, 0, ellipse1.size() / 3);
        
        glVertexPointer(3, GL_DOUBLE, 0, ellipse2.data());
        glNormalPointer(GL_DOUBLE, 0, norms2.data());
        glTexCoordPointer(2, GL_DOUBLE, 0, texts1.data());
        glDrawArrays(GL_TRIANGLES, 0, ellipse2.size() / 3);
        
        glVertexPointer(3, GL_DOUBLE, 0, lines.data());
        glNormalPointer(GL_DOUBLE, 0, norms3.data());
        glTexCoordPointer(2, GL_DOUBLE, 0, texts2.data());
        glDrawArrays(GL_QUADS, 0, lines.size() / 3);
        
        glDisableClientState(GL_NORMAL_ARRAY);
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }
    else
    {
        glEnableClientState(GL_NORMAL_ARRAY);
        glEnableClientState(GL_VERTEX_ARRAY);
        
        glVertexPointer(3, GL_DOUBLE, 0, ellipse1.data());
        glNormalPointer(GL_DOUBLE, 0, norms1.data());
        glDrawArrays(GL_TRIANGLES, 0, ellipse1.size() / 3);
        
        glVertexPointer(3, GL_DOUBLE, 0, ellipse2.data());
        glNormalPointer(GL_DOUBLE, 0, norms2.data());
        glDrawArrays(GL_TRIANGLES, 0, ellipse2.size() / 3);
        
        glVertexPointer(3, GL_DOUBLE, 0, lines.data());
        glNormalPointer(GL_DOUBLE, 0, norms3.data());
        glDrawArrays(GL_QUADS, 0, lines.size() / 3);
        
        glDisableClientState(GL_NORMAL_ARRAY);
        glDisableClientState(GL_VERTEX_ARRAY);
        
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
}

void getArr (std::vector<std::vector<std::vector<Point>>>* f,
             std::vector<std::vector<std::vector<Point>>>* s,
             std::vector<std::vector<std::vector<Point>>>* l,
             std::vector<std::vector<std::vector<std::vector<Point>>>>* nn,
             std::vector<std::vector<std::vector<std::vector<std::pair<double, double>>>>>* t)
{
    ellipse1.clear();
    ellipse2.clear();
    norms1.clear();
    norms2.clear();
    texts1.clear();
    for (int i = 0; i < (*f).size(); i++)
    {
        for (int j = 0; j < (*f)[i].size(); j++)
        {
            for (int m = 0; m < (*f)[i][j].size(); m++)
            {
                ellipse1.push_back((*f)[i][j][m].x);
                ellipse1.push_back((*f)[i][j][m].y);
                ellipse1.push_back((*f)[i][j][m].z);
                
                ellipse2.push_back((*s)[i][j][m].x);
                ellipse2.push_back((*s)[i][j][m].y);
                ellipse2.push_back((*s)[i][j][m].z);
                
                norms1.push_back((*nn)[0][i][j][m].x);
                norms1.push_back((*nn)[0][i][j][m].y);
                norms1.push_back((*nn)[0][i][j][m].z);
                norms2.push_back((*nn)[1][i][j][m].x);
                norms2.push_back((*nn)[1][i][j][m].y);
                norms2.push_back((*nn)[1][i][j][m].z);
                
                texts1.push_back((*t)[0][i][j][m].first);
                texts1.push_back((*t)[0][i][j][m].second);
            }
        }
    }
    
    lines.clear();
    norms3.clear();
    texts2.clear();
    for (int i = 0; i < (*l).size(); i++)
    {
        for (int j = 0; j < (*l)[i].size(); j++)
        {
            for (int m = 0; m < (*l)[i][j].size(); m++)
            {
                lines.push_back((*l)[i][j][m].x);
                lines.push_back((*l)[i][j][m].y);
                lines.push_back((*l)[i][j][m].z);
                
                norms3.push_back((*nn)[2][i][j][m].x);
                norms3.push_back((*nn)[2][i][j][m].y);
                norms3.push_back((*nn)[2][i][j][m].z);
                
                texts2.push_back((*t)[2][i][j][m].first);
                texts2.push_back((*t)[2][i][j][m].second);
            }
        }
    }
}

void prepareAnimation ()
{
    Cylinder cyl = Cylinder(0.0, 0.0, 0.25, 0.25, 0.4, 0.0, 0.0);
    cyl.recalc(36, 1);
    arr1.clear();
    arr1.push_back(cyl.getfEllArr());
    arr1.push_back(cyl.getsEllArr());
    arr1.push_back(cyl.getLinesArr());
    
    Cylinder cylinder = Cylinder(0.0, 0.0, 0.22, 0.22, 0.325, 0.0, 0.0);
    cylinder.recalc(36, 1);
    arr2.clear();
    arr2.push_back(cylinder.getfEllArr());
    arr2.push_back(cylinder.getsEllArr());
    arr2.push_back(cylinder.getLinesArr());
    
    CutCylinder cutCylinder = CutCylinder(0.0, 0.0, 0.2, 0.2, 0.3, 0.3, 0.315);
    cutCylinder.recalc(36, 1);
    arr3.clear();
    arr3.push_back(cutCylinder.getfEllArr());
    arr3.push_back(cutCylinder.getsEllArr());
    arr3.push_back(cutCylinder.getLinesArr());
    
    CutCylinder cutCyl = CutCylinder(0.0, 0.0, 0.15, 0.15, 0.25, 0.25, 0.3);
    cutCyl.recalc(36, 1);
    arr4.clear();
    arr4.push_back(cutCyl.getfEllArr());
    arr4.push_back(cutCyl.getsEllArr());
    arr4.push_back(cutCyl.getLinesArr());
    
    animationTextCoords = cyl.getTextCoords();
}

void tweeningAnimation ()
{
    if (t > 1 || t < 0)
    {
        animated = !animated;
        sign = -sign;
        reDraw = true;
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
    glTexSubImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
    
    delete[] data;
}

void saveFile ()
{
    clearFile();
    std::ofstream fout;
    std::string path = "/Users/ATOM/Documents/Bomonka/IV Semestr/Алгоритмы компьютерной графики/лабы/лаба 7/lab7/lab7/save.txt";
    fout.open(path, std::ofstream::app);
    if (fout.is_open())
    {
        stage.animated = animated;
        stage.animation = animation;
        stage.k = k;
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
    std::string path = "/Users/ATOM/Documents/Bomonka/IV Semestr/Алгоритмы компьютерной графики/лабы/лаба 7/lab7/lab7/save.txt";
    fin.open(path);
    if (fin.is_open())
        fin.read((char*)&stage, sizeof(t_stage));
    animated = stage.animated;
    animation = stage.animation;
    k = stage.k;
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
    std::string path = "/Users/ATOM/Documents/Bomonka/IV Semestr/Алгоритмы компьютерной графики/лабы/лаба 7/lab7/lab7/save.txt";
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
                if (k < 10)
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
            case GLFW_KEY_EQUAL:
                size += 0.25;
                break;
            case GLFW_KEY_MINUS:
                if (size > 0.25)
                    size -= 0.25;
                break;
            case GLFW_KEY_A:
                animation = !animation;
                std::cout << "Animation's start" << std::endl;
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
        }
    }
}

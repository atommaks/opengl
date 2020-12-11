#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "./cylinder.cpp"

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

void keyCallback (GLFWwindow*, int, int, int, int);
void drawCylinder (Cylinder* cylinder);

GLfloat translationX = 0.f, translationY = 0.f, translationZ = -0.4f;
GLfloat rotationX = 0.f, rotationY = 0.f, rotationZ = 0.f;
GLfloat size = 1.f;
bool isColored = false, reDraw = false;
int n = 36, k = 18, kr = 12;

int main (int argc, char** argv)
{
    GLFWwindow *window;
    
    if (!glfwInit())
        return -1;
    
    window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Lab3", nullptr, nullptr);
    
    int screenWidth, screenHeight;
    glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    glfwSetKeyCallback(window, keyCallback);
    glfwMakeContextCurrent(window);
    
    Cylinder* c1 = new Cylinder(0.25f, 0.25f, 0.25f, 0.15f, 0.6f, -0.15f, 0.f);
    while (!glfwWindowShouldClose(window))
    {
        glViewport(0.f, 0.f, screenWidth, screenHeight);
        glClear(GL_COLOR_BUFFER_BIT);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);
        
        glMatrixMode(GL_PROJECTION);
        float a = asin(tanf(M_PI / 6.f)), b = M_PI_4, ratio = (float) screenWidth / screenHeight;
        float m[16] =
        {
            cosf(b) / ratio, sinf(b) * sinf(a), 0.f, 0.f,
            0.f, cosf(a), 0.f, 0.f,
            sinf(b) / ratio, -sinf(a) * cosf(b), 0.f, 0.f,
            0.f, 0.f, 0.f, 1.f
        };
        glLoadMatrixf(m);
        
        glMatrixMode(GL_MODELVIEW);
        //dynamic cylinder
        glPushMatrix();
        glTranslatef(translationX, translationY, translationZ);
        glRotatef(rotationX, 1.f, 0.f, 0.f);
        glRotatef(rotationY, 0.f, 1.f, 0.f);
        glRotatef(rotationZ, 0.f, 0.f, 1.f);
        glScalef(size, size, size);
        glScalef(1.5f, 1.5f, 1.5f);
        drawCylinder(c1);
        glPopMatrix();
        
        //static cylinder
        glPushMatrix();
        glTranslatef(-0.75f, -0.5f, -0.5f);
        glScalef(0.75f, 0.75f, 0.75f);
        drawCylinder(c1);
        glPopMatrix();
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    delete c1;
    
    glfwTerminate();
    return 0;
}

void drawCylinder (Cylinder* cylinder)
{
    if (reDraw)
    {
        cylinder->recalc(n, k, kr);
        reDraw = false;
    }
    GLenum mode = GL_LINE_LOOP;
    if (isColored)
        mode = GL_POLYGON;

    
    bool changeColor = false;
    glColor3ub(255, 255, 255);
    std::vector<std::vector<std::vector<Point> > > arr;
    //first and second ellipses
    for (int a = 0; a < 2; a++)
    {
        if (a == 0)
            arr = cylinder->getfEllArr();
        else
            arr = cylinder->getsEllArr();
        for (int i = 0; i < kr; i++)
        {
            changeColor = !changeColor;
            for (int j = kr - 1; j >= 0; j--)
            {
                if (isColored && changeColor)
                {
                    glColor3ub(255, 0, 0);
                    changeColor = false;
                    if (a == 0)
                        glColor3ub(0, 0, 255);
                } else if (isColored && !changeColor)
                {
                    glColor3ub(0, 255, 0);
                    changeColor = true;
                    if (a == 0)
                        glColor3ub(255, 255, 0);
                }
                glBegin(mode);
                for (int k = 0; k < arr[j][i].size(); k++)
                    glVertex3f(arr[j][i][k].x, arr[j][i][k].y, arr[j][i][k].z);
                glEnd();
            }
        }
    }
    
    //lines
    changeColor = false;
    arr = cylinder->getLinesArr();
    for (int i = 0; i < arr.size(); i++)
    {
        changeColor = !changeColor;
        for (int j = 0; j < arr[i].size(); j++)
        {
            if (isColored && changeColor)
            {
                glColor3ub(255, 0, 255);
                changeColor = false;
            } else if (isColored && !changeColor)
            {
                glColor3ub(0, 255, 255);
                changeColor = true;
            }
            glBegin(mode);
            for (int k = 0; k < arr[i][j].size(); k++)
                glVertex3f(arr[i][j][k].x, arr[i][j][k].y, arr[i][j][k].z);
            glEnd();
        }
    }
}

void keyCallback (GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        const GLfloat translationSpeed = 0.1f, rotationSpeed = 2.f, sizeSpeed = 0.25f;
        switch (key)
        {
            case GLFW_KEY_W:
                translationX += translationSpeed;
                break;
            case GLFW_KEY_S:
                translationX -= translationSpeed;
                break;
            case GLFW_KEY_A:
                translationZ -= translationSpeed;
                break;
            case GLFW_KEY_D:
                translationZ += translationSpeed;
                break;
            case GLFW_KEY_E:
                translationY += translationSpeed;
                break;
            case GLFW_KEY_Q:
                translationY -= translationSpeed;
                break;
            case GLFW_KEY_LEFT:
                rotationX -= rotationSpeed;
                break;
            case GLFW_KEY_RIGHT:
                rotationX += rotationSpeed;
                break;
            case GLFW_KEY_UP:
                rotationZ += rotationSpeed;
                break;
            case GLFW_KEY_DOWN:
                rotationZ -= rotationSpeed;
                break;
            case GLFW_KEY_O:
                rotationY += rotationSpeed;
                break;
            case GLFW_KEY_P:
                rotationY -= rotationSpeed;
                break;
            case GLFW_KEY_EQUAL:
                size += sizeSpeed;
                break;
            case GLFW_KEY_MINUS:
                if (size != 0.25f)
                    size -= sizeSpeed;
                break;
            case GLFW_KEY_C:
                isColored = !isColored;
                break;
            case GLFW_KEY_1:
                if (n != 36)
                {
                    n-=2;
                    reDraw = true;
                }
                break;
            case GLFW_KEY_2:
                n+=2;
                reDraw = true;
                break;
            case GLFW_KEY_3:
                if (kr > 8)
                {
                    kr-=2;
                    reDraw = true;
                }
                break;
            case GLFW_KEY_4:
                kr+=2;
                reDraw = true;
                break;
            case GLFW_KEY_5:
                if (k > 2)
                {
                    k-=2;
                    reDraw = true;
                }
                break;
            case GLFW_KEY_6:
                k+=2;
                reDraw = true;
                break;
        }
    }
}

#include <iostream>
#include <fstream>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <OpenGL/gl.h>
#include "./cylinder.cpp"
#include "./shader.cpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <glm/gtc/matrix_transform.hpp>

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

void keyCallback (GLFWwindow*, int, int, int, int);
void draw (std::vector<std::vector<std::vector<Point>>>*,
           std::vector<std::vector<std::vector<Point>>>*,
           std::vector<std::vector<std::vector<Point>>>*,
           std::vector<std::vector<std::vector<std::vector<Point>>>>*);
void initArrays ();
void tweeningAnimation ();
void prepareAnimation ();
void initTexture (const char*);
void saveFile ();
void loadFile ();
void clearFile ();
void getArr (std::vector<std::vector<std::vector<Point>>>*,
             std::vector<std::vector<std::vector<Point>>>*,
             std::vector<std::vector<std::vector<Point>>>*,
             std::vector<std::vector<std::vector<std::vector<Point>>>>*,
             std::vector<std::vector<std::vector<std::vector<std::pair<float, float>>>>>*);
Point cubeEq (Point, Point, Point, Point, float);

Cylinder* c;
CutCylinder* cc;
t_stage stage;
bool reDraw = false, animation = false, animated = false, textured = false,
     localViewer = false, twoSide = false;
int n = 360, k = 5, screenWidth = 1280, screenHeight = 720, sign = 1;
float t = 0.f;
float rotationY = 0.0, rotationX = 0.0, rotationZ = 0.0, ambV = 0.2, size = 2.0;
std::string texturePath = "/Users/ATOM/Documents/Bomonka/IV Semestr/Алгоритмы компьютерной графики/лабы/лаба 8/lab8/lab8/textures/tex2.jpg",
            vsPath = "/Users/ATOM/Documents/Bomonka/IV Semestr/Алгоритмы компьютерной графики/лабы/лаба 8/lab8/lab8/vertexShader.glsl",
            fsPath = "/Users/ATOM/Documents/Bomonka/IV Semestr/Алгоритмы компьютерной графики/лабы/лаба 8/lab8/lab8/fragmentShader.glsl";
std::vector<std::vector<std::vector<std::vector<Point>>>> arr1, arr2, arr3, arr4, arr, aNorm;
std::vector<std::vector<std::vector<std::vector<std::pair<float, float>>>>> textCoords, animationTextCoords;
std::vector<float> vertices;
GLuint texture, VAO, VBO;

int main(int argc, const char * argv[])
{
    GLFWwindow *window;
    
    if (!glfwInit())
        return -1;
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_SAMPLES, 4);
    window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Lab8", nullptr, nullptr);
    glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    glfwSetKeyCallback(window, keyCallback);
    glfwMakeContextCurrent(window);
    
    glewExperimental = GL_TRUE;
    glewInit();
    
    float a = asin(tanf(M_PI / 6.f)), b = M_PI_4, ratio = (float) screenWidth / screenHeight;
    glm::mat4 projection = glm::mat4(1.0);
    projection[0] = glm::vec4(cosf(b) / ratio, sinf(b) * sinf(a), 0.0, 0.0);
    projection[1] = glm::vec4(0.0, cosf(a), 0.0, 0.0);
    projection[2] = glm::vec4(sinf(b) / ratio, -sinf(a) * cosf(b), 0.0, 0.0);
    projection[3] = glm::vec4(0.0, 0.0, 0.0, 1.0);

    glm::vec3 color = glm::vec3(1.0, 0.0, 0.0);
    glm::vec3 lightColor = glm::vec3(0.8, 0.8, 0.8);
    glm::vec3 lightPos = glm::vec3(1.5, -1.2, 15.0);
    glm::vec3 pos = glm::vec3(-1.0, 0.0, 2.0);
    
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

    Shader shader(vsPath.c_str(), fsPath.c_str());
    initArrays();
    initTexture(texturePath.c_str());
    while (!glfwWindowShouldClose(window))
    {
        glViewport(0.f, 0.f, screenWidth, screenHeight);
        glClear(GL_COLOR_BUFFER_BIT);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);
        
        glm::mat4 model = glm::mat4(1.0);
        model = glm::rotate(model, rotationX, glm::vec3(1, 0, 0));
        model = glm::rotate(model, rotationY, glm::vec3(0, 1, 0));
        model = glm::rotate(model, rotationZ, glm::vec3(0, 0, 1));
        
        if (!animation && !animated)
            draw(&c->getfEllArr(), &c->getsEllArr(), &c->getLinesArr(), &c->getNormals());
        else if ((!animation && animated) || (animation && !animated))
        {
            tweeningAnimation();
            draw(&arr[0], &arr[1], &arr[2], &aNorm);
        }
        else
            draw(&cc->getfEllArr(), &cc->getsEllArr(), &cc->getLinesArr(), &cc->getNormals());

        shader.use();
        glUniformMatrix4fv(glGetUniformLocation(shader.id, "projection"), 1, false, &projection[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(shader.id, "model"), 1, false, &model[0][0]);
        glUniform3fv(glGetUniformLocation(shader.id, "cylCol"), 1, &color[0]);
        glUniform3fv(glGetUniformLocation(shader.id, "lightCol"), 1, &lightColor[0]);
        glUniform3fv(glGetUniformLocation(shader.id, "lightPos"), 1, &lightPos[0]);
        glUniform3fv(glGetUniformLocation(shader.id, "viewPos"), 1, &pos[0]);
        glUniform1i(glGetUniformLocation(shader.id, "texture1"), 0);
        glBindTexture(GL_TEXTURE_2D, texture);
        
        if (textured)
            glUniform1i(glGetUniformLocation(shader.id, "isTextured"), true);
        else
            glUniform1i(glGetUniformLocation(shader.id, "isTextured"), false);
        
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 8);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    delete c;
    delete cc;
    glfwTerminate();
    return 0;
}

void initArrays ()
{
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    //вершинный атрибут
    glVertexAttribPointer(0, 3, GL_FLOAT, false, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //нормальный атрибут
    glVertexAttribPointer(1, 3, GL_FLOAT, false, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    //текстурный атрибут
    glVertexAttribPointer(2, 2, GL_FLOAT, false, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
}

void getArr (std::vector<std::vector<std::vector<Point>>>* f,
             std::vector<std::vector<std::vector<Point>>>* s,
             std::vector<std::vector<std::vector<Point>>>* l,
             std::vector<std::vector<std::vector<std::vector<Point>>>>* nn,
             std::vector<std::vector<std::vector<std::vector<std::pair<float, float>>>>>* t)
{
    vertices.clear();
    for (int i = 0; i < (*f).size(); i++)
    {
        for (int j = 0; j < (*f)[i].size(); j++)
        {
            for (int m = 0; m < (*f)[i][j].size(); m++)
            {
                vertices.push_back((*f)[i][j][m].x);
                vertices.push_back((*f)[i][j][m].y);
                vertices.push_back((*f)[i][j][m].z);
                
                vertices.push_back((*nn)[0][i][j][m].x);
                vertices.push_back((*nn)[0][i][j][m].y);
                vertices.push_back((*nn)[0][i][j][m].z);
                
                vertices.push_back((*t)[0][i][j][m].first);
                vertices.push_back((*t)[0][i][j][m].second);
            }
        }
    }
    
    for (int i = 0; i < (*s).size(); i++)
    {
        for (int j = 0; j < (*s)[i].size(); j++)
        {
            for (int m = 0; m < (*s)[i][j].size(); m++)
            {
                vertices.push_back((*s)[i][j][m].x);
                vertices.push_back((*s)[i][j][m].y);
                vertices.push_back((*s)[i][j][m].z);
                
                vertices.push_back((*nn)[1][i][j][m].x);
                vertices.push_back((*nn)[1][i][j][m].y);
                vertices.push_back((*nn)[1][i][j][m].z);
                
                vertices.push_back((*t)[0][i][j][m].first);
                vertices.push_back((*t)[0][i][j][m].second);
            }
        }
    }
    
    for (int i = 0; i < (*l).size(); i++)
    {
        for (int j = 0; j < (*l)[i].size(); j++)
        {
            for (int m = 0; m < (*l)[i][j].size(); m++)
            {
                vertices.push_back((*l)[i][j][m].x);
                vertices.push_back((*l)[i][j][m].y);
                vertices.push_back((*l)[i][j][m].z);
                
                vertices.push_back((*nn)[2][i][j][m].x);
                vertices.push_back((*nn)[2][i][j][m].y);
                vertices.push_back((*nn)[2][i][j][m].z);
                
                vertices.push_back((*t)[2][i][j][m].first);
                vertices.push_back((*t)[2][i][j][m].second);
            }
        }
    }
}

void draw (std::vector<std::vector<std::vector<Point>>>* fEllArr,
           std::vector<std::vector<std::vector<Point>>>* sEllArr,
           std::vector<std::vector<std::vector<Point>>>* linesArr,
           std::vector<std::vector<std::vector<std::vector<Point>>>>* normas)
{
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
        initArrays();
    }
    
    if ((!animation && animated) || (animation && !animated))
    {
        getArr(fEllArr, sEllArr, linesArr, normas, &animationTextCoords);
        initArrays();
    }
    
}

void prepareAnimation ()
{
    Cylinder cyl = Cylinder(0.0, 0.0, 0.25, 0.25, 0.4, 0.0, 0.0);
    cyl.recalc(200, 1);
    arr1.clear();
    arr1.push_back(cyl.getfEllArr());
    arr1.push_back(cyl.getsEllArr());
    arr1.push_back(cyl.getLinesArr());
    
    Cylinder cylinder = Cylinder(0.0, 0.0, 0.22, 0.22, 0.325, 0.0, 0.0);
    cylinder.recalc(200, 1);
    arr2.clear();
    arr2.push_back(cylinder.getfEllArr());
    arr2.push_back(cylinder.getsEllArr());
    arr2.push_back(cylinder.getLinesArr());
    
    CutCylinder cutCylinder = CutCylinder(0.0, 0.0, 0.2, 0.2, 0.3, 0.3, 0.315);
    cutCylinder.recalc(200, 1);
    arr3.clear();
    arr3.push_back(cutCylinder.getfEllArr());
    arr3.push_back(cutCylinder.getsEllArr());
    arr3.push_back(cutCylinder.getLinesArr());
    
    CutCylinder cutCyl = CutCylinder(0.0, 0.0, 0.15, 0.15, 0.25, 0.25, 0.3);
    cutCyl.recalc(200, 1);
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

void initTexture (const char *filename)
{
    int width, height, bpp;
    auto texBuf = stbi_load(filename, &width, &height, &bpp, 3);
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, texBuf);
    if (texBuf)
        stbi_image_free(texBuf);
}

void saveFile ()
{
    clearFile();
    std::ofstream fout;
    std::string path = "/Users/ATOM/Documents/Bomonka/IV Semestr/Алгоритмы компьютерной графики/лабы/лаба 8/lab8/lab8/save.txt";
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
    std::string path = "/Users/ATOM/Documents/Bomonka/IV Semestr/Алгоритмы компьютерной графики/лабы/лаба 8/lab8/lab8/save.txt";
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
    std::string path = "/Users/ATOM/Documents/Bomonka/IV Semestr/Алгоритмы компьютерной графики/лабы/лаба 8/lab8/lab8/save.txt";
    fout.open(path);
    fout.clear();
    fout.close();
}

void keyCallback (GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        const float rotationSpeed = 0.05;
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

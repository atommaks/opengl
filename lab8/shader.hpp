#include <stdio.h>
#include <glm/glm.hpp>

class Shader
{
public:
    unsigned int id;
    Shader (const char*, const char*);
    void use();
private:
    void checkCompileErrors(unsigned int, std::string);
};

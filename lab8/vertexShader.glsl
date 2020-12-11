#version 330 core

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vNormalCoords;
layout (location = 2) in vec2 vTexCoords;

out vec2 TexCoords;
out vec3 NormalCoords;
out vec3 gPos;

uniform mat4 projection;
uniform mat4 model;

void main()
{
    gl_Position = projection * model * vec4(vPos, 1.0);
    gPos = vec3(model * vec4(vPos, 1.0));
    TexCoords = vTexCoords;
    NormalCoords = vec3(model * vec4(vNormalCoords, 1.0));
}

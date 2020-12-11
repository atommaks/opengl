#version 330 core

out vec4 FragColor;

in vec3 gPos;
in vec3 NormalCoords;
in vec2 TexCoords;

uniform sampler2D texture1;
uniform vec3 cylCol;
uniform vec3 lightCol;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform bool isTextured;

void main()
{
    if (isTextured)
    {
        FragColor = texture(texture1, TexCoords);
        return;
    }
    vec3 tex = vec3(texture(texture1, TexCoords));
    vec3 n = normalize(NormalCoords);
    vec3 lightDir = normalize(lightPos - gPos);
    float diffEff = max(dot(n, lightDir), 0.0);
    vec3 diffuse = diffEff * lightCol;
    //Ambient
    float ambEff = 0.25;
    vec3 ambient = ambEff * lightCol;
    //Specular
    float specEff = 0.1;
    vec3 viewDir = normalize(viewPos - gPos);
    vec3 reflDir = reflect(-lightDir, n);
    float spec = pow(max(dot(viewDir, reflDir), 0.0), 32);
    vec3 specular = specEff * spec * lightCol;
    vec3 res = (ambient + diffuse + specular) * cylCol;
    FragColor = vec4(res, 1.0);
}

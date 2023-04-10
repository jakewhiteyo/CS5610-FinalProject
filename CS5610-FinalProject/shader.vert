#version 330 core

layout(location=0) in vec3 pos;
layout(location=1) in vec3 norm;
layout(location=2) in vec3 tex;

uniform mat4 mvp;
uniform mat3 mv;
uniform mat4 mv4;
uniform vec3 lightPos;
uniform vec3 color;
uniform vec3 white;
uniform vec3 ambient;

out vec3 normal;
out vec3 diffuse;
out vec3 specular;
out vec3 ambientVec;
out vec3 texColor;
out vec2 texCoord;

void main()
{
    normal = norm * mv;
    vec4 pos4 = vec4(pos, 1);
    vec4 view4 = pos4 * mv4;
    vec3 view = vec3(view4);
    diffuse = color * max(dot(normalize(normal), normalize(lightPos)), 0.0);
    specular = white * pow(max(dot(normalize(normal), normalize(normalize(lightPos) + normalize(-view))), 0.0), 2);
    gl_Position = mvp * vec4(pos * 0.09, 1);
    texColor = color;
    ambientVec = ambient;
    texCoord = vec2(tex.x, 1.0 - tex.y);
}
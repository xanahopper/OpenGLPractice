#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

struct Light
{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform Light light;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} vs_out;

void main() {
    gl_Position = projection * view * model * vec4(position, 1.0f);
    vs_out.Normal = normal;
    vs_out.TexCoords = texCoords;
    vs_out.FragPos = position;
    
//    light.ambient = vec3(0.05);
//    light.diffuse = vec3(0.15);
//    light.specular = vec3(0.5);
}

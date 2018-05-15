#version 330 core
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 lightPos;
uniform vec3 lightDir;
uniform vec3 lightColor;
uniform vec3 viewPos;

uniform sampler2D tex1;

out vec4 color;

void main() {
    color = texture(tex1, TexCoords);
}

#version 330 core

out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

struct Light
{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Light light;
uniform vec3 viewPos;

uniform sampler2D tex1;
uniform bool blinn;

void main() {
    float dist = length(viewPos - fs_in.FragPos);
    float attenuation = 1.0 / (dist);
    vec3 color = vec3(texture(tex1, fs_in.TexCoords));
    // ambient
    vec3 ambient = 0.05 * color;
    // diffuse
    vec3 norm = normalize(fs_in.Normal);
    vec3 lightDir = normalize(light.position - fs_in.FragPos);
    float diff = max(dot(lightDir, norm), 0.0);
    vec3 diffuse = light.diffuse * diff * color;
    // specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    float spec = 0.0;
    if (blinn) {
        vec3 halfwayDir = normalize(viewDir + lightDir);
        spec = pow(max(dot(norm, halfwayDir), 0.0), 2.0);
    } else {
        vec3 reflectDir = reflect(-lightDir, norm);
        spec = pow(max(dot(viewDir, reflectDir), 0.0), 1.0);
    }
    vec3 specular = light.specular * spec * color;
    FragColor = vec4((ambient + diffuse + specular) * attenuation, 1.0f);
    
    float gamma = 2.2;
    FragColor.rgb = pow(FragColor.rgb, vec3(1.0/gamma));
}

//
//  main.cpp
//  OpenGL_01_Advance
//
//  Created by Xana Hopper on 2017/5/14.
//  Copyright © 2017年 Xana Hopper. All rights reserved.
//

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#define GLEW_STATIC
#include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>
#include <OpenGL/OpenGL.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include <FreeImage.h>

#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "Camera.hpp"
#include "shader.h"

enum RenderType : int {
    DRAW_ARRAY,
    DRAW_ARRAY_STRIP,
    DRAW_ELEMENTS,
    DRAW_INSTANCE
};

enum UniformType : int {
    INT_1,
    FLOAT_1,
    FLOAT_3,
    FLOAT_VEC3,
    FLOAT_VEC4,
    MAT3,
    MAT4
};

struct RenderUniform {
    UniformType Type;
    GLvoid * Data;
};

struct RenderObject {
    GLuint VAO;
    std::map<GLuint, GLuint> Textures;
    std::map<std::string, RenderUniform> Uniforms;
    RenderType Type;
    GLuint uDrawStart;
    GLuint uDrawCount;
    GLuint* Indecies;
};

struct RenderScene {
    std::map<std::string, RenderUniform> Uniforms;
    std::vector<RenderObject> RenderObjects;
    GLuint uFramebuffer;
    GLenum uClearBit;
    Shader* pShader;
    GLuint uViewWidth, uViewHeight;
    GLboolean bDepthTest;
};

const int WINDOW_WIDTH = 1440;
const int WINDOW_HEIGHT = 900;
const std::string TITLE = "OpenGL Advance Demo";

// 前向声明
GLFWwindow* init_context(int width, int height, const std::string& title);
void setup_callbacks(GLFWwindow* pWindow);
void setup_data();
void do_movement(GLfloat delta_time);
void render(GLFWwindow* pWindow);

FIBITMAP* load_image(const std::string& filename);
GLuint load_texture(const std::string& filename, GLenum internal_format, GLenum file_format);

// 回调函数
void window_key_callback(GLFWwindow*, int key, int scancode, int action, int mode);
void window_size_callback(GLFWwindow*, int width, int height);
void window_pos_callback(GLFWwindow*, int xpos, int ypos);
void window_mouse_callback(GLFWwindow*, double xpos, double ypos);
void window_scroll_callback(GLFWwindow*, double xoffset, double yoffset);


GLfloat planeVertices[] = {
    // Positions          // Normals         // Texture Coords
    8.0f, -0.5f,  8.0f,  0.0f, 1.0f, 0.0f,  5.0f, 0.0f,
    -8.0f, -0.5f,  8.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
    -8.0f, -0.5f, -8.0f,  0.0f, 1.0f, 0.0f,  0.0f, 5.0f,
    
    8.0f, -0.5f,  8.0f,  0.0f, 1.0f, 0.0f,  5.0f, 0.0f,
    -8.0f, -0.5f, -8.0f,  0.0f, 1.0f, 0.0f,  0.0f, 5.0f,
    8.0f, -0.5f, -8.0f,  0.0f, 1.0f, 0.0f,  5.0f, 5.0f
};

GLfloat vertices[] = {
    // Back face
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // Bottom-left
    0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, // top-right
    0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, // bottom-right
    0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,  // top-right
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,  // bottom-left
    -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,// top-left
    // Front face
    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
    0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,  // bottom-right
    0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,  // top-right
    0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // top-right
    -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,  // top-left
    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,  // bottom-left
    // Left face
    -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
    -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-left
    -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,  // bottom-left
    -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
    -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,  // bottom-right
    -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
    // Right face
    0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-left
    0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-right
    0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-right
    0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,  // bottom-right
    0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,  // top-left
    0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom-left
    // Bottom face
    -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
    0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f, // top-left
    0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,// bottom-left
    0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, // bottom-left
    -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, // bottom-right
    -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
    // Top face
    -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,// top-left
    0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
    0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, // top-right
    0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
    -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,// top-left
    -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f // bottom-left
};

Camera camera(glm::vec3(0.0f, 1.0f, 3.0f));
glm::vec3 lightPos(-2.0f, 4.0f, -1.0f);
glm::mat4 *pProjection;

bool keys[1024];
GLboolean blinn = GL_FALSE;
GLboolean FramebufferSRGB = false;

RenderObject renderObject;
std::vector<RenderScene> scenes;

glm::mat4 lightSpaceMatrix;

GLuint frameWidth, frameHeight;

int main(int argc, const char * argv[]) {
    GLFWwindow* pWindow = init_context(WINDOW_WIDTH, WINDOW_HEIGHT, TITLE);
    if (!pWindow) {
        return -1;
    }
    
    setup_callbacks(pWindow);
    
    glViewport(0, 0, frameWidth, frameHeight);
    
    setup_data();
    
    GLfloat lastFrame = 0, currentFrame = 0;
    GLfloat delteTime = 0;
    
    glm::mat4 view = camera.view_matrix();
    glm::vec3 viewPos;
    glm::vec3 ambient(0.05f);
    glm::vec3 diffuse(0.15f);
    glm::vec3 specular(0.5f);
//    scenes[1].Uniforms["light.ambient"] = RenderUniform { FLOAT_VEC3, glm::value_ptr(ambient) };
//    scenes[1].Uniforms["light.diffuse"] = RenderUniform { FLOAT_VEC3, glm::value_ptr(diffuse) };
//    scenes[1].Uniforms["light.specular"] = RenderUniform { FLOAT_VEC3, glm::value_ptr(specular) };
    
    while (!glfwWindowShouldClose(pWindow)) {
        currentFrame = glfwGetTime();
        delteTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        glfwPollEvents();
        do_movement(delteTime);
        view = camera.view_matrix();
        viewPos = camera.position();
        
//        scenes[1].Uniforms["view"] = RenderUniform { MAT4, glm::value_ptr(view) };
//        scenes[1].Uniforms["viewPos"] = RenderUniform { FLOAT_VEC3, glm::value_ptr(viewPos) };
//        scenes[1].Uniforms["blinn"] = RenderUniform { INT_1, &blinn };
        
        render(pWindow);
        
    }
    
    if (pWindow)
        glfwTerminate();
    
    return 0;
}

GLFWwindow* init_context(int width, int height, const std::string& title) {
    if (!glfwInit()) {
        return nullptr;
    }
    
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    
    GLFWwindow* window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(window);
    
//    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    glEnable(GL_DEPTH_TEST);
    
#ifndef __APPLE__
    glewExperimental = GL_TRUE;
    glewInit();
#endif
    
    
    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
    frameWidth = w;
    frameHeight = h;
    
    return window;
}

void setup_callbacks(GLFWwindow* window) {
    glfwSetKeyCallback(window, window_key_callback);
    glfwSetWindowSizeCallback(window, window_size_callback);
    glfwSetWindowPosCallback(window, window_pos_callback);
    glfwSetCursorPosCallback(window, window_mouse_callback);
}

void setup_data() {
    GLuint tex_id = load_texture("wood.jpg", GL_SRGB, GL_BGR);
    
    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
    
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *) 0);
    
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));
    
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)(6 * sizeof(GLfloat)));
    
    glBindVertexArray(0);
    
    pProjection = new glm::mat4(glm::perspective(glm::radians(45.0f), (GLfloat) WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 100.0f));
    glm::mat4 *model = new glm::mat4();
    
    renderObject.VAO = VAO;
    renderObject.Type = DRAW_ARRAY;
    renderObject.Textures[0] = tex_id;
    renderObject.Uniforms["model"] = RenderUniform { MAT4, glm::value_ptr(*model) };
    renderObject.uDrawStart = 0;
    renderObject.uDrawCount = 6;
    
    GLuint cubeVAO, cubeVBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *) 0);
    
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));
    
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)(6 * sizeof(GLfloat)));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    RenderObject cubeObj;
    cubeObj.VAO = cubeVAO;
    cubeObj.Type = DRAW_ARRAY;
    cubeObj.Textures[0] = tex_id;
    cubeObj.uDrawStart = 0;
    cubeObj.uDrawCount = 36;
    
    RenderScene depthScene;
    RenderScene scene;
    
    depthScene.RenderObjects.push_back(renderObject);
    scene.RenderObjects.push_back(renderObject);
    // Cube 1
    model = new glm::mat4();
    *model = glm::translate(*model, glm::vec3(0.0, 1.5f, 0.0f));
    cubeObj.Uniforms["model"] = RenderUniform { MAT4, glm::value_ptr(*model) };
    
    depthScene.RenderObjects.push_back(cubeObj);
    scene.RenderObjects.push_back(cubeObj);
    // Cube 2
    model = new glm::mat4();
    *model = glm::translate(*model, glm::vec3(2.0f, 0.0f, 1.0f));
    cubeObj.Uniforms["model"] = RenderUniform { MAT4, glm::value_ptr(*model) };
    
    depthScene.RenderObjects.push_back(cubeObj);
    scene.RenderObjects.push_back(cubeObj);
    //Cube 3
    model = new glm::mat4();
    *model = glm::translate(*model, glm::vec3(-1.0f, 0.0f, 2.0f));
    *model = glm::rotate(*model, 60.0f, glm::normalize(glm::vec3(1.0f, 0.0f, 1.0f)));
    *model = glm::scale(*model, glm::vec3(0.5f));
    cubeObj.Uniforms["model"] = RenderUniform { MAT4, glm::value_ptr(*model) };
    
    depthScene.RenderObjects.push_back(cubeObj);
    scene.RenderObjects.push_back(cubeObj);
    
    // 平行光空间矩阵设置
    glm::mat4 lightProjection, lightView;
    GLfloat near_plane = 1.0f, far_plane = 7.5f;
    lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
    lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
    lightSpaceMatrix = lightProjection * lightView;
    
    std::cout << glm::to_string(lightSpaceMatrix) << std::endl;
    
    depthScene.Uniforms["lightSpaceMatrix"] = RenderUniform { MAT4, glm::value_ptr(lightSpaceMatrix) };
    
    // 设置阴影Framebuffer
    const int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;
    GLuint depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);
    
    // - Create depth texture
    GLuint depthMap;
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    depthScene.uFramebuffer = depthMapFBO;
    depthScene.uClearBit = GL_DEPTH_BUFFER_BIT;
    depthScene.uViewWidth = SHADOW_WIDTH;
    depthScene.uViewHeight = SHADOW_HEIGHT;
    depthScene.bDepthTest = GL_TRUE;
    depthScene.pShader = new Shader("shader/shadowDepth.vs", "shader/shadowDepth.fs");
    
    GLfloat quadVertices[] = {
        // Positions        // Texture Coords
        -1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
        1.0f,  1.0f, 0.0f,  1.0f, 1.0f,
        1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
    };
    GLuint quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid *) 0);
    
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    RenderScene debugDepthScene;
    RenderObject quadObj;
    quadObj.VAO = quadVAO;
    quadObj.Type = DRAW_ARRAY_STRIP;
    quadObj.Textures[0] = depthMap;
    quadObj.uDrawStart = 0;
    quadObj.uDrawCount = 4;
    
    debugDepthScene.uFramebuffer = 0;
    debugDepthScene.uClearBit = GL_COLOR_BUFFER_BIT;
    debugDepthScene.uViewWidth = frameWidth;
    debugDepthScene.uViewHeight = frameHeight;
    debugDepthScene.Uniforms["depthMap"] = RenderUniform { INT_1, new int[1] {0} };
    debugDepthScene.RenderObjects.push_back(quadObj);
    debugDepthScene.bDepthTest = GL_FALSE;
    debugDepthScene.pShader = new Shader("shader/debugDepth.vs", "shader/debugDepth.fs");
    
    scene.Uniforms["light.position"] = RenderUniform { FLOAT_VEC3, glm::value_ptr(lightPos) };
    scene.Uniforms["tex1"] = RenderUniform { INT_1, new int[1] { 0 } };
    scene.Uniforms["projection"] = RenderUniform { MAT4, glm::value_ptr(*pProjection) };
    scene.uFramebuffer = 0;
    scene.uClearBit = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;
    scene.uViewWidth = frameWidth;
    scene.uViewHeight = frameHeight;
    scene.bDepthTest = GL_TRUE;
    scene.pShader = new Shader("shader/shader.vs", "shader/shader.fs");
    
    scenes.push_back(depthScene);
    scenes.push_back(debugDepthScene);
//    scenes.push_back(scene);
    
}

void do_movement(GLfloat delta_time) {
    // Camera controls
    if(keys[GLFW_KEY_W])
        camera.on_keyboard(FORWARD, delta_time);
    if(keys[GLFW_KEY_S])
        camera.on_keyboard(BACKWARD, delta_time);
    if(keys[GLFW_KEY_A])
        camera.on_keyboard(LEFT, delta_time);
    if(keys[GLFW_KEY_D])
        camera.on_keyboard(RIGHT, delta_time);
    if(keys[GLFW_KEY_SPACE])
        camera.on_keyboard(UP, delta_time);
    if(keys[GLFW_KEY_LEFT_SHIFT] || keys[GLFW_KEY_RIGHT_SHIFT])
        camera.on_keyboard(DOWN, delta_time);

}

void set_uniforms(GLuint program, const std::map<std::string, RenderUniform>& uniforms) {
    for (auto &it : uniforms) {
//        std::cout << it.first << std::endl;
        GLint loc = glGetUniformLocation(program, it.first.c_str());
        if (loc < 0) continue;
        switch (it.second.Type) {
            case INT_1:
//                std::cout << it.first << ": " << * static_cast<int *>(it.second.Data) << std::endl;
                glUniform1i(loc, * static_cast<int *>(it.second.Data));
            case FLOAT_1:
                glUniform1f(loc, * static_cast<GLfloat*>(it.second.Data));
                break;
            case FLOAT_3:
            case FLOAT_VEC3:
                glUniform3fv(loc, 1, static_cast<GLfloat *>(it.second.Data));
                break;
            case FLOAT_VEC4:
                glUniform4fv(loc, 1, static_cast<GLfloat *>(it.second.Data));
                break;
            case MAT3:
                glUniformMatrix3fv(loc, 1, GL_FALSE, static_cast<GLfloat *>(it.second.Data));
                break;
            case MAT4:
//                std::cout << it.first << ": \n";
//                std::cout << glm::to_string(glm::make_mat4(static_cast<GLfloat *>(it.second.Data))) << std::endl;
                glUniformMatrix4fv(loc, 1, GL_FALSE, static_cast<GLfloat *>(it.second.Data));
                break;
        }
    }
}

void render(GLFWwindow* window) {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    for (auto &scene : scenes) {
        if (scene.pShader != nullptr) {
            scene.pShader->use();
            (scene.bDepthTest == GL_TRUE) ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
            glViewport(0, 0, scene.uViewWidth, scene.uViewHeight);
            glBindFramebuffer(GL_FRAMEBUFFER, scene.uFramebuffer);
            glClear(scene.uClearBit);
            set_uniforms(scene.pShader->program(), scene.Uniforms);
            for (auto &ro : scene.RenderObjects) {
                glBindVertexArray(ro.VAO);
                set_uniforms(scene.pShader->program(), ro.Uniforms);
                for (auto &it: ro.Textures) {
                    glActiveTexture(GL_TEXTURE0 + it.first);
                    glBindTexture(GL_TEXTURE_2D, it.second);
                }
                
                //            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                
                switch (ro.Type) {
                    case DRAW_ARRAY:
                        glDrawArrays(GL_TRIANGLES, ro.uDrawStart, ro.uDrawCount);
                        break;
                    case DRAW_ARRAY_STRIP:
                        glDrawArrays(GL_TRIANGLE_STRIP, ro.uDrawStart, ro.uDrawCount);
                        break;
                    case DRAW_ELEMENTS:
                        glDrawElements(GL_TRIANGLES, ro.uDrawCount, GL_UNSIGNED_INT, nullptr);
                        break;
                    default:
                        break;
                }
            }
        }
    }
    
    
    glfwSwapBuffers(window);
}

void window_key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    switch (key) {
        case GLFW_KEY_ESCAPE:
            if (action == GLFW_PRESS) {
                glfwSetWindowShouldClose(window, GL_TRUE);
            }
            break;
        case GLFW_KEY_L:
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            break;
        case GLFW_KEY_F:
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            break;
        case GLFW_KEY_B:
            if (action == GLFW_PRESS) {
                blinn = (blinn == GL_TRUE) ? GL_FALSE : GL_TRUE;
            }
            break;
        case GLFW_KEY_G:
            if (action == GLFW_PRESS) {
                FramebufferSRGB = !FramebufferSRGB;
                if (FramebufferSRGB) glEnable(GL_FRAMEBUFFER_SRGB);
                else glDisable(GL_FRAMEBUFFER_SRGB);
            }
        default:
            break;
    }
    
    if (key >= 0 && key < 1024)
    {
        if(action == GLFW_PRESS)
            keys[key] = true;
        else if(action == GLFW_RELEASE)
            keys[key] = false;
    }

}

void window_size_callback(GLFWwindow*, int width, int height) {
    
}

void window_pos_callback(GLFWwindow*, int xpos, int ypos) {
    
}

bool firstMouse = true;
double lastX = WINDOW_WIDTH / 2, lastY = WINDOW_HEIGHT / 2;

void window_mouse_callback(GLFWwindow*, double xpos, double ypos) {
    if(firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    
    GLfloat xoffset = xpos - lastX;
    GLfloat yoffset = lastY - ypos;  // Reversed since y-coordinates go from bottom to left
    
    lastX = xpos;
    lastY = ypos;
    
    camera.on_mouse(xoffset, yoffset);
}

void window_scroll_callback(GLFWwindow*, double xoffset, double yoffset) {
    
}

FIBITMAP* load_image(const std::string& filename) {
    FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
    FIBITMAP* pBitmap = nullptr;
    
    const char* f = filename.c_str();
    fif = FreeImage_GetFileType(f);
    if (fif == FIF_UNKNOWN) {
        fif = FreeImage_GetFIFFromFilename(f);
    }
    if (fif == FIF_UNKNOWN) {
        return nullptr;
    }
    
    if (FreeImage_FIFSupportsReading(fif)) {
        pBitmap = FreeImage_Load(fif, f);
    } else {
        return nullptr;
    }
    
    if (!pBitmap) {
        return nullptr;
    }
    
    FreeImage_FlipVertical(pBitmap);
    
    return pBitmap;
}

GLuint load_texture(const std::string& filename, GLenum internal_format, GLenum file_format) {
    FIBITMAP* pBitmap = load_image(filename);
    if (pBitmap == nullptr) {
        return 0;
    }
    unsigned int height = 0, width = 0;
    BYTE* pBits = nullptr;
    
    width = FreeImage_GetWidth(pBitmap);
    height = FreeImage_GetHeight(pBitmap);
    pBits = FreeImage_GetBits(pBitmap);
    
    GLuint texId;
    glGenTextures(1, &texId);
    glBindTexture(GL_TEXTURE_2D, texId);
    glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, file_format, GL_UNSIGNED_BYTE, pBits);
    glGenerateMipmap(GL_TEXTURE_2D);
    
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glBindTexture(GL_TEXTURE_2D, 0);

    FreeImage_Unload(pBitmap);
    
    return texId;

}

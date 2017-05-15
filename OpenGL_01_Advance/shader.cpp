//
//  shader.cpp
//  OpenGL_01_HelloWorld
//
//  Created by Xana Hopper on 2017/5/3.
//  Copyright © 2017年 Xana Hopper. All rights reserved.
//


#include <string>
#include <iostream>
#include <fstream>
#include <sstream>


#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#define GLEW_STATIC
#include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>

#include "shader.h"

Shader::Shader(const GLchar* vertexPath, const GLchar* fragmentPath)
{
    std::string vertexCode;
    std::string fragmentCode;
    
    std::fstream vShaderFile;
    std::ifstream fShaderFile;
    vShaderFile.exceptions(std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::badbit);
    
    try {
        
        vShaderFile.open (vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;
        
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        
        vShaderFile.close();
        fShaderFile.close();
        
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    } catch (std::ifstream::failure e) {
        std::cout << "ERROR:SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
        std::cout << e.what() << std::endl;
    }
    
    const GLchar* vShaderCode = vertexCode.c_str();
    const GLchar* fShaderCode = fragmentCode.c_str();
    
    GLuint vertex, fragment;
    vertex = compile_shader(vShaderCode, GL_VERTEX_SHADER);
    fragment = compile_shader(fShaderCode, GL_FRAGMENT_SHADER);
    
    this->m_program = link_program(vertex, fragment);
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

GLuint Shader::compile_shader(const GLchar* source, GLenum type) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, sizeof(infoLog), nullptr, infoLog);
        std::cerr << "ERROR:SHADER::PROGRAM::COMPILE_SHADER\n" << infoLog << std::endl;
        return -1;
    }
    return shader;
}

GLuint Shader::link_program(GLuint vertex, GLuint fragment) {
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertex);
    glAttachShader(shaderProgram, fragment);
    glLinkProgram(shaderProgram);
    
    GLint success;
    GLchar infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        return -1;
    }
    return shaderProgram;
}

GLuint Shader::program() const {
    return this->m_program;
}

void Shader::use() const {
    glUseProgram(this->m_program);
}


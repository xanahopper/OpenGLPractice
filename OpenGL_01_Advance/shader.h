//
//  shader.h
//  OpenGL_01_HelloWorld
//
//  Created by Xana Hopper on 2017/5/3.
//  Copyright © 2017年 Xana Hopper. All rights reserved.
//

#ifndef shader_h
#define shader_h

class Shader {
private:
    GLuint m_program;
    
    GLuint compile_shader(const GLchar* source, GLenum type);
    
    GLuint link_program(GLuint vertex, GLuint fragment);
    
public:
    GLuint program() const;
    
    Shader(const GLchar* vertexPath, const GLchar* fragmentPath);
    
    void use() const;
};


#endif /* shader_h */

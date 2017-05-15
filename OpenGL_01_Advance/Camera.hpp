//
//  Camera.hpp
//  OpenGL_01_HelloWorld
//
//  Created by Xana Hopper on 2017/5/5.
//  Copyright © 2017年 Xana Hopper. All rights reserved.
//

#ifndef Camera_hpp
#define Camera_hpp

extern const GLfloat YAW;
extern const GLfloat PITCH;
extern const GLfloat SPEED;
extern const GLfloat SENSITIVITY;
extern const GLfloat ZOOM;

enum camera_movement {
    FORWARD,
    BACKWARD,
    UP,
    DOWN,
    LEFT,
    RIGHT
};

class Camera {
private:
    glm::vec3 m_position;
    glm::vec3 m_front;
    glm::vec3 m_up;
    glm::vec3 m_world_up;
    glm::vec3 m_right;
    
    GLfloat m_yaw;
    GLfloat m_pitch;
    
    GLfloat m_movement_speed;
    GLfloat m_mouse_sensitivity;
    GLfloat m_zoom;
    
public:
    Camera(const glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f),
           const glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
           const GLfloat yaw = YAW,
           const GLfloat pitch = PITCH);
    
    glm::mat4 view_matrix();
    
    void on_keyboard(camera_movement direction, GLfloat delta_time);
    
    void on_mouse(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch = true);
    
    void on_scroll(GLfloat yoffset);
    
    glm::vec3 position();
    
    glm::vec3 front();
    
    GLfloat zoom() {
        return m_zoom;
    }
    
protected:
    virtual void update_camera();
};

#endif /* Camera_hpp */

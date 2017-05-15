//
//  Camera.cpp
//  OpenGL_01_HelloWorld
//
//  Created by Xana Hopper on 2017/5/5.
//  Copyright © 2017年 Xana Hopper. All rights reserved.
//


#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#define GLEW_STATIC
#include <GL/glew.h>
#endif

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Camera.hpp"

const GLfloat YAW         = -90.0F;
const GLfloat PITCH       = 0.0f;
const GLfloat SPEED       = 3.0f;
const GLfloat SENSITIVITY = 0.25f;
const GLfloat ZOOM        = 45.0f;

const GLfloat PITCH_LIMIT = 89.5f;

Camera::Camera(glm::vec3 position, glm::vec3 up, GLfloat yaw, GLfloat pitch)
:m_front(glm::vec3(0.0f, 0.0f, -1.0f)),
m_movement_speed(SPEED),
m_mouse_sensitivity(SENSITIVITY),
m_zoom(ZOOM)
{
    this->m_position = position;
    this->m_world_up = up;
    this->m_yaw = yaw;
    this->m_pitch = pitch;
    
    this->update_camera();
}

glm::mat4 Camera::view_matrix() {
    return glm::lookAt(this->m_position,
                       this->m_position + this->m_front,
                       this->m_up);
}

void Camera::on_keyboard(camera_movement direction, GLfloat delta_time) {
    GLfloat velocity = this->m_movement_speed * delta_time;
    glm::vec3 v = glm::normalize(glm::vec3(m_front.x, 0.0f, m_front.z));
    switch (direction) {
        case FORWARD:
            this->m_position += v * velocity;
            break;
        case BACKWARD:
            this->m_position -= v * velocity;
            break;
        case UP:
            this->m_position += this->m_world_up * velocity;
            break;
        case DOWN:
            this->m_position -= this->m_world_up * velocity;
            break;
        case LEFT:
            this->m_position -= this->m_right * velocity;
            break;
        case RIGHT:
            this->m_position += this->m_right * velocity;
            break;
        default:
            break;
    }
}

void Camera::on_mouse(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch) {
    xoffset *= m_mouse_sensitivity;
    yoffset *= m_mouse_sensitivity;
    
    this->m_yaw += xoffset;
    this->m_pitch += yoffset;
    
    if (constrainPitch) {
        if (this->m_pitch > PITCH_LIMIT)
            this->m_pitch = PITCH_LIMIT;
        else if (this->m_pitch < -PITCH_LIMIT)
            this->m_pitch = -PITCH_LIMIT;
    }
    
    this->update_camera();
}

void Camera::on_scroll(GLfloat yoffset) {
    if (this->m_zoom >= 1.0f && this->m_zoom <= 45.0f) {
        this->m_zoom += yoffset;
    }
    if (this->m_zoom > 45.0f) {
        this->m_zoom = 45.0f;
    } else if (this->m_zoom < 1.0f) {
        this->m_zoom = 1.0f;
    }
}

void Camera::update_camera() {
    glm::vec3 front;
    front.x = cos(glm::radians(this->m_yaw)) * cos(glm::radians(this->m_pitch));
    front.y = sin(glm::radians(this->m_pitch));
    front.z = sin(glm::radians(this->m_yaw)) * cos(glm::radians(this->m_pitch));
    
    this->m_front = glm::normalize(front);
    
    this->m_right = glm::normalize(glm::cross(this->m_front, this->m_world_up));
    this->m_up    = glm::normalize(glm::cross(this->m_right, this->m_front));
}

glm::vec3 Camera::position() {
    return m_position;
}

glm::vec3 Camera::front() {
    return m_front;
}


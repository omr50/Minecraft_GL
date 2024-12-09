#include "../../include/Camera.hpp"
#include <iostream>

Camera::Camera() {}

void Camera::update_camera_position(glm::vec3 direction)
{
    glm::mat4 camera_rotation_matrix = glm::yawPitchRoll(yaw, pitch, roll);
    float speed = 0.2;
    glm::vec3 translation_amount = glm::vec3(camera_rotation_matrix * glm::vec4(direction, 1.0)) * speed;
    position += translation_amount;
}

glm::mat4 Camera::create_view_matrix()
{
    glm::mat4 translation_matrix = glm::translate(glm::mat4(1.0f), -position);
    // inverse the camera rotation to be applied to objects
    glm::mat4 rotation_matrix = glm::transpose(glm::yawPitchRoll(yaw, pitch, roll));

    return rotation_matrix * translation_matrix;
}

glm::mat4 Camera::create_projection_matrix()
{
    return glm::perspective(fov, aspect_ratio, near_plane, far_plane);
}

glm::mat4 Camera::get_view_projection_matrix()
{
    return create_projection_matrix() * create_view_matrix();
}
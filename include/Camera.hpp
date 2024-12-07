#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/euler_angles.hpp>

#define WIDTH 800.0f
#define HEIGHT 600.0f

class Camera
{

public:
    glm::vec3 position = {0.0, 0.0, 0.0};
    float rotation_angle = 0.0f;
    float fov = glm::radians(45.0f);
    float yaw = 0;
    float pitch = 0;
    float roll = 0;
    float aspect_ratio = static_cast<float>(WIDTH) / static_cast<float>(HEIGHT);
    float near_plane = 0.1f;
    float far_plane = 100.0f;

    Camera();
    void update_camera_position(glm::vec3 direction);
    glm::mat4 create_view_matrix();
    glm::mat4 create_projection_matrix();
    glm::mat4 get_view_projection_matrix();
};

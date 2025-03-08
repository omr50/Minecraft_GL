#pragma once
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

#define WIDTH 800.0f
#define HEIGHT 600.0f

class Camera
{

public:
    glm::vec3 position = {0.0, 76.0, 0.0};
    float rotation_angle = 0.0f;
    float fov = glm::radians(60.0f);
    float yaw = 0;
    float pitch = 0;
    float roll = 0;
    float aspect_ratio = static_cast<float>(WIDTH) / static_cast<float>(HEIGHT);
    float near_plane = 0.1f;
    float far_plane = 3000.0f;
    bool moved = true;
    std::pair<int, int> prev_chunk;
    std::pair<int, int> curr_chunk;

    Camera();
    void update_camera_position(glm::vec3 direction);
    glm::mat4 create_view_matrix();
    glm::mat4 create_projection_matrix();
    glm::mat4 get_view_projection_matrix();
    std::pair<int, int> get_chunk();
    std::pair<int, int> get_direction();
};

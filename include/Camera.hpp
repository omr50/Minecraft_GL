#pragma once
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>
#include "./Cube.hpp"
#include "./Terrain.hpp"
#include "./Audio.hpp"
#include "./HUD.hpp"
#include <chrono>
using Clock = std::chrono::high_resolution_clock;

#define WIDTH 1600.0f
#define HEIGHT 1200.0f

class Terrain;
class HUD;
class Camera
{

public:
    HUD *hud;
    glm::vec3 position = {0.0, 76.0, 0.0};
    float rotation_angle = 0.0f;
    float fov = glm::radians(60.0f);
    float yaw = 0;
    float pitch = 0;
    float roll = 0;
    float aspect_ratio = static_cast<float>(WIDTH) / static_cast<float>(HEIGHT);
    float near_plane = 0.1f;
    float far_plane = 50000.0f;
    glm::vec3 velocity = {0.0, 0.0, 0.0};
    bool moved = true;
    bool keyboard_move;
    float move_speed = 7.0f;
    bool stop_set;
    std::pair<int, int> prev_chunk;
    std::pair<int, int> curr_chunk;
    std::chrono::_V2::system_clock::time_point stop_move_time = Clock::now();
    std::chrono::_V2::system_clock::time_point last_move_time = Clock::now();

    Camera();
    void update_camera_position(glm::vec3 direction);
    void camera_move();
    glm::mat4 create_view_matrix();
    glm::mat4 create_projection_matrix();
    glm::mat4 get_view_projection_matrix();
    std::pair<int, int> get_chunk();
    std::pair<int, int> get_direction();
    glm::vec3 get_look_direction();
    void raycast_block(Terrain *terrain, std::vector<std::pair<glm::vec3, glm::vec3>> *points, Audio *audioPlayer);
    std::pair<int, int> get_ray_chunk(int x, int z);
    void place_block(Terrain *terrain, std::vector<std::pair<glm::vec3, glm::vec3>> *points, Audio *audioPlayer);
    glm::vec3 get_ray_end(Terrain *terrain, int iterations);
    void draw_ray();
};

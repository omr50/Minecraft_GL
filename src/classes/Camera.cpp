#include "../../include/Camera.hpp"
#include <iostream>
#include "../../include/Chunk.hpp"

Camera::Camera()
{
    prev_chunk = get_chunk();
    curr_chunk = prev_chunk;
}

void Camera::update_camera_position(glm::vec3 direction)
{
    moved = true;
    glm::mat4 camera_rotation_matrix = glm::yawPitchRoll(yaw, pitch, roll);
    float speed = 2;

    glm::vec3 translation_amount = glm::vec3(camera_rotation_matrix * glm::vec4(direction, 1.0)) * speed;
    if (direction.x || direction.z)
    {

        position.x += translation_amount.x;
        position.z += translation_amount.z;
    }
    if (direction.y)
        position.y += (speed * direction.y);

    // update chunk to be able to find the direction move
    // to later realize which
    prev_chunk = curr_chunk;
    curr_chunk = get_chunk();
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

std::pair<int, int> Camera::get_chunk()
{
    // take x and z world positions and get chunk coords
    int x = position.x;
    int z = position.z;

    int cx = x / X;
    if (x < 0 && x % X != 0)
        cx--;
    int cz = z / Z;
    if (z < 0 && z % Z != 0)
        cz--;

    return std::make_pair(cx, cz);
}

std::pair<int, int> Camera::get_direction()
{

    auto direction = std::make_pair(curr_chunk.first - prev_chunk.first, curr_chunk.second - prev_chunk.second);
    if (direction.first != 0 && direction.second != 0)
        printf("WE GOT A EDGE CASE (%d, %d)!\n", direction.first, direction.second);
    return direction;
}
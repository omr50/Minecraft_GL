#include "../../include/Camera.hpp"
#include <iostream>
#include "../../include/Chunk.hpp"
#include "../../include/Cube.hpp"
#include <glm/gtc/type_ptr.hpp>

Camera::Camera()
{
    prev_chunk = get_chunk();
    curr_chunk = prev_chunk;
}

void Camera::update_camera_position(glm::vec3 direction)
{
    glm::mat4 camera_rotation_matrix = glm::yawPitchRoll(yaw, pitch, roll);
    float speed = 2;

    if (velocity.x == 0.0f && velocity.y == 0.0f && velocity.z == 0.0f)
    {
        printf("Start move!\n");
        stop_move_time = Clock::now();
    }

    last_move_time = Clock::now();
    velocity = glm::vec3(camera_rotation_matrix * glm::vec4(direction, 0.0)) * speed;
    keyboard_move = true;
    stop_set = false;
    // if (direction.x || direction.z)
    // {

    //     position.x += translation_amount.x;
    //     position.z += translation_amount.z;
    // }
    // if (direction.y)
    //     position.y += (speed * direction.y);

    // update chunk to be able to find the direction move
    // to later realize which
}

void Camera::camera_move()
{
    printf("Velocity: (%f, %f, %f)\n", velocity.x, velocity.y, velocity.z);

    if (!keyboard_move && !stop_set)
    {
        stop_move_time = Clock::now();
        stop_set = true;
    }

    std::chrono::duration<float, std::milli> difference_ms = Clock::now() - stop_move_time;
    if (difference_ms.count() > 1000)
    {
        printf("Set to 0\n");
        velocity = (glm::vec3){0.0f, 0.0f, 0.0f};
        stop_set = false;
    }

    if (velocity.x == 0.0f && velocity.y == 0.0f && velocity.z == 0.0f)
    {
        printf("cancel?\n");
        return;
    }
    moved = true;
    auto now = Clock::now();
    std::chrono::duration<float, std::milli> delta_ms = now - last_move_time;
    last_move_time = now;
    float delta_time_seconds = delta_ms.count() / 1000.0f;
    float speed = 5.0f;
    if (velocity.x)
        position.x += velocity.x * delta_time_seconds * speed;
    if (velocity.y)
        position.y += velocity.y * delta_time_seconds * speed;
    if (velocity.z)
        position.z += velocity.z * delta_time_seconds * speed;

    // std::chrono::duration<float, std::milli> since_last_time_ms = now - start_move_time;
    prev_chunk = curr_chunk;
    curr_chunk = get_chunk();
    keyboard_move = false;
}

glm::mat4 Camera::create_view_matrix()
{
    // SCALE APPLIED TO THE TRANSLATION TO ALLOW GENERATION TO WORK PROPERLY
    glm::mat4 translation_matrix = glm::translate(glm::mat4(1.0f), (-position * (float)SCALE));
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
inline int floorDiv(int a, int b)
{
    return (a >= 0) ? (a / b) : ((a - b + 1) / b);
}
std::pair<int, int> Camera::get_ray_chunk(int x, int z)
{
    int cx = floorDiv(x, X); // X is chunk width
    int cz = floorDiv(z, Z); // Z is chunk depth
    return {cx, cz};
}
std::pair<int, int> Camera::get_direction()
{
    // printf("got to this point 2.2.2\n");
    auto direction = std::make_pair(curr_chunk.first - prev_chunk.first, curr_chunk.second - prev_chunk.second);
    // printf("got to this point 2.2.3\n");
    return direction;
}

glm::vec3 Camera::get_look_direction()
{
    // Yaw: rotation around Y axis (left/right)
    // Pitch: rotation around X axis (up/down)
    // Both in radians

    // glm::vec3 direction;
    // direction.x = cos(pitch) * sin(yaw);
    // direction.y = -sin(pitch);
    // direction.z = cos(pitch) * cos(yaw);
    // return glm::normalize(direction);

    glm::mat4 R = glm::yawPitchRoll(yaw, pitch, roll);
    // w=0 => pure direction
    return glm::normalize(glm::vec3(R * glm::vec4(0, 0, -1, 0)));
}

inline int floor_div(int a, int b)
{
    int q = a / b;
    int r = a % b;
    return (r != 0 && ((r > 0) != (b > 0))) ? (q - 1) : q;
}

inline int half_round(float a)
{
}

void Camera::raycast_block(Terrain *terrain, std::vector<std::pair<glm::vec3, glm::vec3>> *points)
{
    auto look_direction = get_look_direction();
    int eye_height = 1.6;

    auto ray_pos = position;

    for (float i = 0; i < 100; i += 0.001)
    {
        auto ray_coord = ray_pos + look_direction * i;
        // (0.5 added because blocks are centered)
        int wx = (int)std::floor(ray_coord.x + 0.5);
        int wy = (int)std::floor(ray_coord.y + 0.5);
        int wz = (int)std::floor(ray_coord.z + 0.5);
        auto ray_chunk = get_ray_chunk(wx, wz);

        int chunk_x = floor_div(wx, X);
        int chunk_z = floor_div(wz, Z);
        int block_x = wx - chunk_x * X;
        int block_z = wz - chunk_z * Z;

        for (int j = 0; j < NUM_CHUNKS; j++)
        {
            if (terrain->chunks[j].chunk_coordinates == ray_chunk)
            {
                if (wy >= Y || wy < 0)
                {
                    continue;
                }
                int index = terrain->chunks[j].get_index(block_x, wy, block_z);
                if (terrain->chunks[j].blocks[index].block_type != "air")
                {
                    // printf("Ray found (%f, %f, %f)\n", ray_coord.x, ray_coord.y, ray_coord.z);
                    // printf("wx: %d, wy: %d, wz: %d)\n", wx, wy, wz);
                    // printf("Index: %d\n", index);
                    // get previous block (for now just testing on the current block)
                    points->push_back(std::make_pair(ray_pos, ray_coord));
                    terrain->chunks[j].blocks[index].block_type = "air";
                    terrain->chunks[j].needs_remesh();
                    // terrain->enqueue_update_task(&terrain->chunks[j]);
                    {
                        std::lock_guard<std::mutex> lock(terrain->chunks[j].chunk_mutex);
                        terrain->create_chunk_mesh(&terrain->chunks[j]);
                        // printf("after chunk lock!\n");
                    }
                    terrain->chunks[j].update_chunk();
                    moved = true;
                    return;
                }
            }
        }
    }
}

void Camera::place_block(Terrain *terrain, std::vector<std::pair<glm::vec3, glm::vec3>> *points)
{
    auto look_direction = get_look_direction();
    int eye_height = 1.6;

    auto ray_pos = position;

    for (float i = 0; i < 100; i += 0.001)
    {
        auto ray_coord = ray_pos + look_direction * i;
        // (0.5 added because blocks are centered)
        int wx = (int)std::floor(ray_coord.x + 0.5);
        int wy = (int)std::floor(ray_coord.y + 0.5);
        int wz = (int)std::floor(ray_coord.z + 0.5);
        auto ray_chunk = get_ray_chunk(wx, wz);

        int chunk_x = floor_div(wx, X);
        int chunk_z = floor_div(wz, Z);
        int block_x = wx - chunk_x * X;
        int block_z = wz - chunk_z * Z;

        for (int j = 0; j < NUM_CHUNKS; j++)
        {
            if (terrain->chunks[j].chunk_coordinates == ray_chunk)
            {
                if (wy >= Y || wy < 0)
                {
                    continue;
                }
                int index = terrain->chunks[j].get_index(block_x, wy, block_z);
                if (terrain->chunks[j].blocks[index].block_type != "air")
                {
                    auto prev_coord = ray_pos + look_direction * static_cast<float>(i - 0.001);

                    wx = (int)std::floor(prev_coord.x + 0.5);
                    wy = (int)std::floor(prev_coord.y + 0.5);
                    wz = (int)std::floor(prev_coord.z + 0.5);
                    ray_chunk = get_ray_chunk(wx, wz);

                    for (int k = 0; k < NUM_CHUNKS; k++)
                        if (terrain->chunks[k].chunk_coordinates == ray_chunk)
                        {
                            chunk_x = floor_div(wx, X);
                            chunk_z = floor_div(wz, Z);
                            block_x = wx - chunk_x * X;
                            block_z = wz - chunk_z * Z;

                            if (wy >= Y || wy < 0)
                            {
                                return;
                            }
                            // printf("Ray found (%f, %f, %f)\n", ray_coord.x, ray_coord.y, ray_coord.z);
                            // printf("wx: %d, wy: %d, wz: %d)\n", wx, wy, wz);
                            // printf("Prev Ray found (%f, %f, %f)\n", prev_coord.x, prev_coord.y, ray_coord.z);
                            // printf("pwx: %d, pwy: %d, pwz: %d)\n", wx, wy, wz);
                            index = terrain->chunks[k].get_index(block_x, wy, block_z);
                            // get previous block (for now just testing on the current block)
                            points->push_back(std::make_pair(ray_pos, ray_coord));
                            terrain->chunks[k].blocks[index].block_type = "wooden_plank";
                            terrain->chunks[k].needs_remesh();
                            // terrain->enqueue_update_task(&terrain->chunks[j]);
                            {
                                std::lock_guard<std::mutex> lock(terrain->chunks[k].chunk_mutex);
                                terrain->create_chunk_mesh(&terrain->chunks[k]);
                                // printf("after chunk lock!\n");
                            }
                            terrain->chunks[k].update_chunk();
                            moved = true;
                            return;
                        }
                }
            }
        }
    }
}

glm::vec3 Camera::get_ray_end(Terrain *terrain, int iterations)
{
    auto look_direction = get_look_direction();
    int eye_height = 1.6;
    // auto ray_pos = position + glm::vec3(0, 0.2, 0);
    auto ray_pos = position;

    for (float i = 0; i < 1000; i += 0.001)
    {
        auto ray_coord = ray_pos + look_direction * i;
        int wx = (int)std::floor(ray_coord.x);
        int wy = (int)std::floor(ray_coord.y);
        int wz = (int)std::floor(ray_coord.z);
        auto ray_chunk = get_ray_chunk(wx, wz);

        // world cell -> chunk coord + local index
        int chunk_x = floor_div(wx, X);
        int chunk_z = floor_div(wz, Z);
        int block_x = wx - chunk_x * X; // safe, 0..X-1 even for negatives
        int block_z = wz - chunk_z * Z;

        for (int j = 0; j < NUM_CHUNKS; j++)
        {
            if (terrain->chunks[j].chunk_coordinates == ray_chunk)
            {
                if (wy >= Y || wy < 0)
                {
                    continue;
                }
                int index = terrain->chunks[j].get_index(block_x, wy, block_z);
                if (terrain->chunks[j].blocks[index].block_type != "air")
                {
                    // get previous block (for now just testing on the current block)
                    if (terrain->chunks[j].blocks[index].block_type != "stone")
                    {
                        terrain->chunks[j].blocks[index].block_type = "stone";
                        terrain->chunks[j].needs_remesh();
                        // terrain->enqueue_update_task(&terrain->chunks[j]);
                        {
                            std::lock_guard<std::mutex> lock(terrain->chunks[j].chunk_mutex);
                            terrain->create_chunk_mesh(&terrain->chunks[j]);
                            // printf("after chunk lock!\n");
                        }
                        terrain->chunks[j].update_chunk();
                        moved = true;
                        return ray_coord;
                    }
                }
            }
        }
    }
}

void Camera::draw_ray()
{
    // prepare ray enpoints (start, end)
    // create and bind vao/vbo
    // write shaders (before this) (vertex / fragment)
    // per frame -> update VBO and draw

    // ray should show the line from the center of the
    // screen to maybe a couple 100 units away.

    auto start = position;
    auto end = position - 100.0f * get_look_direction();
}
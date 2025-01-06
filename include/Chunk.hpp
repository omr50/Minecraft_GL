#pragma once
#include "../include/Cube.hpp"
#include "../include/Renderable.hpp"
#include <glm/gtc/noise.hpp>
#include <vector>
#include <mutex>

#define X 16
#define Y 256
#define Z 16
#define MIN_BLOCK_HEIGHT 75

struct Vertex
{
    float x;
    float y;
    float z;
    float u;
    float v;
};

class Chunk
{
public:
    Cube *blocks;
    std::pair<int, int> chunk_coordinates;
    std::vector<Vertex> mesh_vertices;
    std::vector<glm::mat4> instance_vector;
    GLuint geometry_vbo;
    GLuint instance_vbo;
    GLuint chunk_vao;
    bool initialized = false;
    bool clean_terrain = false;
    bool clean_mesh = false;
    bool generated_vertices = false;
    bool sent_mesh = false;
    bool enqueued = false;
    std::mutex chunk_mutex;

    Chunk();
    Chunk(int x, int y);
    float generateHeight(float x, float z, float scale, float heightMultiplier);
    void initialize_vertex_buffers_and_array();
    void initialize_cubes();
    void generate_terrain();
    int get_index(int x, int y, int z);
    float get_cube_x(int x);
    float get_cube_z(int z);
    void get_mesh_vertices();
    void update_chunk();
    void buffer_data();
    void draw_chunk();
    void needs_remesh();
    void new_chunk_state();
    // void draw() override;
};
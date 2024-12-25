#pragma once
#include "../include/Cube.hpp"
#include "../include/Renderable.hpp"
#include <glm/gtc/noise.hpp>
#include <vector>

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
    bool clean_mesh = false;

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
    // void draw() override;
};
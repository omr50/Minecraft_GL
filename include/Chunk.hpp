#pragma once
#include "../include/Cube.hpp"
#include "../include/Renderable.hpp"
#include <glm/gtc/noise.hpp>
#include <vector>
#include <mutex>
#include <atomic>

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

enum CHUNK_ZONE
{
    DESERT = 0,
    FOREST = 1,
    PLAINS = 2,
    MOUNTAINS = 3
};

class Chunk
{
public:
    int chunk_num;
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
    bool enqueued_mesh_creation = false;
    bool rendered = false;
    std::mutex chunk_mutex;

    Chunk();
    Chunk(int x, int y);
    float generateHeight(float x, float z, float scale, float heightMultiplier);
    CHUNK_ZONE get_chunk_zone();
    int get_zone_bias();
    void generate_biome_terrain(int x, int z);
    void generate_desert(int x, int y, int z, float chunk_x, float chunk_z, int height);
    void generate_plains(int x, int y, int z, float chunk_x, float chunk_z, int height);
    void generate_forest(int x, int y, int z, float chunk_x, float chunk_z, int height);
    void generate_mountains(int x, int y, int z, float chunk_x, float chunk_z, int height);
    void initialize_vertex_buffers_and_array();
    void initialize_cubes();
    void generate_terrain();
    int get_index(int x, int y, int z);
    float get_cube_x(int x);
    float get_cube_z(int z);
    void get_mesh_vertices();
    void update_chunk();
    void buffer_data();
    void draw_chunk(bool rendered_chunks[]);
    void needs_remesh();
    void new_chunk_state();
    bool ready_to_buffer();
    bool is_renderable();
    bool in_chunk_coords(int x, int z);
    // void draw() override;
};
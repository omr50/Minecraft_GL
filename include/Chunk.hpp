#pragma once
#include "../include/Cube.hpp"
#include "../include/Renderable.hpp"
#include <glm/gtc/noise.hpp>
#include <vector>

#define X 16
#define Y 16
#define Z 16
#define MIN_BLOCK_HEIGHT 10

// holds uv information (2 flaots)
// and matrix = 4 rows of vec4.

class Chunk
{
public:
    Cube *blocks;
    std::pair<int, int> chunk_coordinates;
    std::vector<glm::vec3> mesh_vertices;
    std::vector<glm::mat4> instance_vector;

    Chunk();
    Chunk(int x, int y);
    float generateHeight(float x, float z, float scale, float heightMultiplier);
    void initialize_cubes();
    void generate_terrain();
    int get_index(int x, int y, int z);
    float get_cube_x(int x);
    float get_cube_z(int z);
    void get_mesh_vertices();
    // void draw() override;
};
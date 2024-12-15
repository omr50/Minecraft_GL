#pragma once
#include "../include/Cube.hpp"
#include "../include/Renderable.hpp"
#include <glm/gtc/noise.hpp>

#define X 16
#define Y 16
#define Z 16
#define MIN_BLOCK_HEIGHT 10

class Chunk
{
public:
    Cube *blocks;
    std::pair<int, int> chunk_coordinates;

    Chunk();
    Chunk(int x, int y);
    float generateHeight(float x, float z, float scale, float heightMultiplier);
    void initialize_cubes();
    void generate_terrain();
    int get_index(int x, int y, int z);
    float get_cube_x(int x);
    float get_cube_z(int z);
    void get_chunk_data();
    // void draw() override;
};
#pragma once
#include "../include/Cube.hpp"
#include <glm/gtc/noise.hpp>

#define X 16
#define Y 32
#define Z 16

class Chunk
{
public:
    Cube blocks[X][Y][Z];
    std::pair<int, int> chunk_coordinates;

    Chunk(int x, int y);
    float generateHeight(float x, float z, float scale, float heightMultiplier);
    void initialize_cubes();
    void generate_terrain();
};
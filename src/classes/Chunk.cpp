#include "../../include/Chunk.hpp"

Chunk::Chunk(int x, int y)
{
    chunk_coordinates.first = x;
    chunk_coordinates.second = y;
    initialize_cubes();
    generate_terrain();
}

float Chunk::generateHeight(float x, float z, float scale, float heightMultiplier)
{
    float noiseValue = glm::perlin(glm::vec2(x * scale, z * scale));
    // Normalize to [0, 1]
    noiseValue = (noiseValue + 1.0f) / 2.0f;
    return noiseValue * heightMultiplier;
}

// cubes use default constructor
// so just initializing params
void Chunk::initialize_cubes()
{
    for (int x = 0; x < X; x++)
    {
        for (int z = 0; z < Z; z++)
        {
            for (int y = 0; y < Y; y++)
            {
                blocks[x][y][z].x = chunk_coordinates.first * 16 + x;
                blocks[x][y][z].y = y;
                blocks[x][y][z].z = chunk_coordinates.second * 16 + z;
                blocks[x][y][z].block_type = "air";
                blocks[x][y][z].create_model_matrix();
            }
        }
    }
}

void Chunk::generate_terrain()
{
    // initialize all cubes
    for (int x = 0; x < X; x++)
    {
        for (int z = 0; z < Z; z++)
        {
            // get y value that will be used for
            // the height, if y < height, stone
            // if greater, air.
            float chunk_x = chunk_coordinates.first * 16 + x, chunk_z = chunk_coordinates.second * 16 + z;
            float height = generateHeight(chunk_x, chunk_z, 0.1, 30.0);
            printf("height is %f\n", height);
            for (int y = 0; y < Y; y++)
            {
                if (y < height)
                {
                    blocks[x][y][z].block_type = "stone";
                }
                else
                {
                    blocks[x][y][z].block_type = "air";
                }
            }
        }
    }
}
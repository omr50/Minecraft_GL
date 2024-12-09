#include "../../include/Chunk.hpp"

Chunk::Chunk()
{
    blocks = new Cube[X * Y * Z];
}

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
                blocks[get_index(x, y, z)].update_cube_state(x, y, z, "air");
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
            float chunk_x = get_cube_x(x), chunk_z = get_cube_z(z);
            float height = MIN_BLOCK_HEIGHT + generateHeight(chunk_x, chunk_z, 0.1, 5.0);
            // printf("height is %f\n", height);
            for (int y = 0; y < Y; y++)
            {
                if (y < height)
                {
                    blocks[get_index(x, y, z)].update_cube_state(chunk_x, y, chunk_z, "grass");
                }
                else
                {
                    blocks[get_index(x, y, z)].update_cube_state(chunk_x, y, chunk_z, "air");
                }
            }
        }
    }
}

int Chunk::get_index(int x, int y, int z)
{
    return x * Y * Z + y * Z + z;
}

float Chunk::get_cube_x(int x)
{
    return chunk_coordinates.first * X + x;
}

float Chunk::get_cube_z(int z)
{
    return chunk_coordinates.second * Z + z;
}
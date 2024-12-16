#include "../../include/Chunk.hpp"

Chunk::Chunk()
{
    blocks = new Cube[X * Y * Z];
    // initialize_cubes();
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
                float world_x = get_cube_x(x);
                float world_z = get_cube_z(z);
                blocks[get_index(x, y, z)].update_state(world_x, y, world_z, "air");
            }
        }
    }
}

// remember to customize this to add other blocks in the mix
// like dirt, gravel, etc.
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
            // printf("%f, %f\n", chunk_x, chunk_z);
            float height = MIN_BLOCK_HEIGHT + generateHeight(chunk_x, chunk_z, 0.1, 5.0);
            // printf("Block height %f\n", height);
            // printf("height is %f\n", height);
            for (int y = 0; y < Y; y++)
            {
                if (y < height)
                {
                    // create border around chunks for easier viewing
                    if (x == X - 1 || z == Z - 1 || x == 0 || z == 0)
                        blocks[get_index(x, y, z)].update_state(chunk_x, y, chunk_z, "stone");
                    else
                        blocks[get_index(x, y, z)].update_state(chunk_x, y, chunk_z, "grass");
                }
                else
                {
                    blocks[get_index(x, y, z)].update_state(chunk_x, y, chunk_z, "air");
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

void Chunk::get_mesh_vertices()
{
    mesh_vertices.clear();
    for (int x = 0; x < X; x++)
        for (int y = 0; y < X; y++)
            for (int z = 0; z < X; z++)
            {
                auto block = blocks[get_index(x, y, z)];
                if (block.block_type == "air")
                    return;

                for (int face = 0; face < 6; face++)
                {
                    // if non-renderable / not in mesh (continue)
                    if (!block.renderable_face[face])
                        continue;
                    std::array<FaceUV, 3> face_textures = Cube::texture_map[block.block_type];
                    // glActiveTexture(GL_TEXTURE0);
                    // Decide which texture to use (top, bottom, sides) based on face index:
                    auto offsets_to_use = (face == 5) ? face_textures[1] : // Bottom face
                                              (face == 4) ? face_textures[0]
                                                          :     // Top face
                                              face_textures[2]; // Other sides

                    Cube::faces[face][4] = Cube::faces[face][4] * (1.0f / 16.0f) + offsets_to_use.offset.x;
                    // glBindTexture(GL_TEXTURE_2D, texToUse);
                    // GLint textureLoc = glGetUniformLocation(Cube::shader_program, "ourTexture");
                    // glUniform1i(textureLoc, 0);

                    // glBindVertexArray(face_vaos[face]);
                    // glDrawArrays(GL_TRIANGLES, 0, 6);
                }
            }
}
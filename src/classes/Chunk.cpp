#include "../../include/Chunk.hpp"
#include <iostream>
#include <cmath>
#include <cstdlib>

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

BIOME Chunk::get_chunk_zone()
{
    // 0 = desert
    // 1 = forest
    // 2 = plains
    // 3 = mountains
    float global_x = chunk_coordinates.first * X;
    float global_z = chunk_coordinates.second * Z;
    // float biomeFrequency = 0.002f;
    // float noise = glm::perlin(glm::vec2(global_x, global_z) * biomeFrequency);
    // // Normalize from [-1, 1] to [0, 1]:
    // float biomeFactor = (noise + 1.0f) / 2.0f;

    // if (biomeFactor >= 0 && biomeFactor <= 0.25)
    // {
    //     return DESERT;
    // }
    // else if (biomeFactor > 0.25 && biomeFactor <= 0.5)
    // {
    //     return PLAINS;
    // }
    // else if (biomeFactor > 0.5 && biomeFactor <= 0.75)
    // {
    //     return FOREST;
    // }
    // else
    // {
    //     return MOUNTAINS;
    // }

    return Biome::get_biome((glm::vec2){global_x, global_z});
}
// Function to get a deterministic pseudo-random number from coordinates
int deterministic_biased_random(int x, int y, int min, int max, double power)
{
    // Combine coordinates into a unique hashable value
    size_t seed = std::hash<int>()(x) ^ (std::hash<int>()(y) << 1);

    // Convert hash to a pseudo-random number in [0, 1]
    double u = (double)(seed % 100000) / 100000.0;

    // Apply power-law transformation to bias towards lower numbers
    int result = min + (int)((max - min) * pow(u, power));

    return result;
}

void Chunk::generate_biome_terrain(int x, int z)
{
    float wx = get_cube_x(x);
    float wz = get_cube_z(z);
    glm::vec2 p(wx, wz);
    // printf("got chunk zone!\n");
    // get y value that will be used for
    // the height, if y < height, stone
    // if greater, air.
    float chunk_x = get_cube_x(x), chunk_z = get_cube_z(z);
    // printf("%f, %f\n", chunk_x, chunk_z);
    // min block height added so that total can be from 0 to 255.
    auto zone_bias = get_zone_bias();
    float height = Biome::get_height(p);
    // printf("Block height %f\n", height);
    // printf("height is %f\n", height);

    BIOME zone = Biome::get_biome(p);
    for (int y = 0; y < Y; y++)
    {
        if (zone == FOREST)
        {
            generate_forest(x, y, z, chunk_x, chunk_z, height);
            // printf("generated a forest!\n");
        }
        else if (zone == PLAINS)
        {
            generate_plains(x, y, z, chunk_x, chunk_z, height);
            // printf("generated a plains!\n");
        }
        if (zone == DESERT)
        {
            generate_desert(x, y, z, chunk_x, chunk_z, height);
            // printf("generated a desert!\n");
        }
        if (zone == MOUNTAINS)
        {
            generate_mountains(x, y, z, chunk_x, chunk_z, height);
            // printf("generated a mountain!\n");
        }
    }
}

void Chunk::generate_desert(int x, int y, int z, float chunk_x, float chunk_z, int height)
{
    // printf("desert generation!\n");
    if (y < height - 5)
    {
        blocks[get_index(x, y, z)].update_state(chunk_x, y, chunk_z, "stone");
    }
    else if (y > height - 5 && y < height)
    {

        blocks[get_index(x, y, z)].update_state(chunk_x, y, chunk_z, "sand");
    }
    else
    {

        blocks[get_index(x, y, z)].update_state(chunk_x, y, chunk_z, "air");
    }
}

void Chunk::generate_plains(int x, int y, int z, float chunk_x, float chunk_z, int height)
{
    // printf("plains generation!\n");
    if (y < height - 3)
    {
        blocks[get_index(x, y, z)].update_state(chunk_x, y, chunk_z, "stone");
    }
    else if (y > height - 3 && y < height)
    {

        blocks[get_index(x, y, z)].update_state(chunk_x, y, chunk_z, "dirt");
    }
    else
    {

        blocks[get_index(x, y, z)].update_state(chunk_x, y, chunk_z, "air");
    }
}

void Chunk::generate_forest(int x, int y, int z, float chunk_x, float chunk_z, int height)
{
    // printf("forest generation!\n");
    if (y < height - 3)
    {
        blocks[get_index(x, y, z)].update_state(chunk_x, y, chunk_z, "stone");
    }
    else if (y > height - 3 && y < height)
    {

        blocks[get_index(x, y, z)].update_state(chunk_x, y, chunk_z, "grass");
    }
    else
    {

        blocks[get_index(x, y, z)].update_state(chunk_x, y, chunk_z, "air");
    }
}

void Chunk::generate_mountains(int x, int y, int z, float chunk_x, float chunk_z, int height)
{
    // printf("mountain generation!\n");
    if (y < height - 3)
    {
        blocks[get_index(x, y, z)].update_state(chunk_x, y, chunk_z, "dirt");
    }
    else if (y > height - 3 && y < height)
    {

        blocks[get_index(x, y, z)].update_state(chunk_x, y, chunk_z, "stone");
    }
    else
    {

        blocks[get_index(x, y, z)].update_state(chunk_x, y, chunk_z, "air");
    }
}

int Chunk::get_zone_bias()
{
    BIOME zone = get_chunk_zone();
    int bias;
    if (zone == DESERT)
    {
        bias = 2;
    }
    else if (zone == PLAINS)
    {
        bias = 1;
    }
    else if (zone == FOREST)
    {
        bias = 3;
    }
    else if (zone == MOUNTAINS)
    {
        bias = 84;
    }
    return bias;
}

float Chunk::generateHeight(float x, float z, float scale, float heightMultiplier)
{
    BIOME zone = get_chunk_zone();
    float frequency = 1;
    float amplitude = 1;
    if (zone == MOUNTAINS)
    {
        frequency = 0.02f;
    }
    else
    {
        frequency = 0.05f;
    }

    float global_x = chunk_coordinates.first * X + x;
    float global_z = chunk_coordinates.second * Z + z;
    // float noiseValue = glm::perlin(glm::vec2((chunk_coordinates.first + x) * scale, (chunk_coordinates.second + z) * scale));
    float noiseValue = glm::perlin(glm::vec2(global_x, global_z) * frequency);
    // Normalize to [0, 1]
    // noiseValue = (noiseValue + 1.0f) / 2.0f;
    // int zone_bias = get_zone_bias();
    // return noiseValue * (heightMultiplier + zone_bias);
    return noiseValue * (heightMultiplier);
}

void Chunk::initialize_vertex_buffers_and_array()
{
    glGenVertexArrays(1, &chunk_vao);
    // glGenBuffers(1, &instance_vbo);
    glGenBuffers(1, &geometry_vbo);
    glBindVertexArray(chunk_vao);
    // glBindVertexArray(0);
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
    // printf("Chunk (%d, %d) initialized\n", chunk_coordinates.first, chunk_coordinates.second);
    initialized = true;
    // printf("initialized the cubes\n");
}

// remember to customize this to add other blocks in the mix
// like dirt, gravel, etc.
// should only be done once the chunk needs to be created, not each iteration
void Chunk::generate_terrain()
{
    // printf("Generating terrain!\n");
    BIOME zone = get_chunk_zone();
    // initialize all cubes
    for (int x = 0; x < X; x++)
    {
        for (int z = 0; z < Z; z++)
        {
            generate_biome_terrain(x, z);
        }
    }
    // printf("finished biome generation!\n");
    // printf("Chunk (%d, %d) terrain generated\n", chunk_coordinates.first, chunk_coordinates.second);
    clean_terrain = true;
    // clean = true;
}

// render chunks individually

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
    // printf("mesh vertices func working?\n");
    // printf("vertices size: %d\n", mesh_vertices.size());
    // printf("instance vector size: %d\n", instance_vector.size());
    if (generated_vertices)
        return;
    if (!clean_mesh || !clean_terrain || !initialized)
        return;
    mesh_vertices.clear();
    instance_vector.clear();
    for (int x = 0; x < X; x++)
        for (int y = 0; y < Y; y++)
            for (int z = 0; z < Z; z++)
            {
                auto block = blocks[get_index(x, y, z)];
                if (block.block_type == "air")
                    continue;

                std::array<FaceUV, 3> face_textures = Cube::texture_map[block.block_type];
                for (int face = 0; face < 6; face++)
                {
                    // if non-renderable / not in mesh (continue)
                    if (!block.renderable_face[face])
                        continue;
                    // one matrix per face, not per vertice
                    // instance_vector.push_back(block.model_matrix);

                    // glActiveTexture(GL_TEXTURE0);
                    // Decide which texture to use (top, bottom, sides) based on face index:
                    auto offsets_to_use = (face == 5) ? face_textures[0] : // top face
                                              (face == 4) ? face_textures[2]
                                                          :     // Top face
                                              face_textures[1]; // Other sides

                    // set up texture offsets
                    std::string side;
                    if (face == 5)
                        side = "bottom";
                    else if (face == 4)
                        side = "top";
                    else
                        side = "sides";
                    // printf("side is: %s\n", side.c_str());
                    Vertex vertex;
                    for (int i = 0; i < 6; i++)
                    {
                        // give world coordinates and
                        float localX = Cube::faces[face][0 + 5 * i]; // e.g. -0.5, +0.5, etc.
                        float localY = Cube::faces[face][1 + 5 * i];
                        float localZ = Cube::faces[face][2 + 5 * i];
                        float localU = Cube::faces[face][3 + 5 * i];
                        float localV = Cube::faces[face][4 + 5 * i];
                        vertex.x = block.x + localX;
                        vertex.y = block.y + localY;
                        vertex.z = block.z + localZ;
                        vertex.u = offsets_to_use.offset.x + localU * (1.0f / 16.0f);
                        vertex.v = offsets_to_use.offset.y + localV * (1.0f / 16.0f);
                        mesh_vertices.push_back(vertex);
                    }
                }
            }

    // printf("vertices size: %d\n", mesh_vertices.size());
    // printf("instance vector size: %d\n", instance_vector.size());
    // printf("Chunk (%d, %d) mesh created\n", chunk_coordinates.first, chunk_coordinates.second);
    generated_vertices = true;
}

void Chunk::update_chunk()
{

    // not for updating vao, vbos, because we want this function to be
    // thread safe. only will be called if camera moved or if block added/destroyed
    // so that we only update if necessary.
    std::lock_guard<std::mutex> lock(chunk_mutex);
    get_mesh_vertices();
}

void Chunk::buffer_data()
{
    // no need to buffer data if
    // mesh was already sent and clean.
    if (sent_mesh)
        return;

    // printf("buffering data\n");
    // DO SOMETHING HERE, SO IF CLEAN I DON"T NEED TO KEEP SENDING IT, BUT ONLY SEND
    // WHEN FIRST BECOMES CLEAN.
    // if (clean_mesh)
    // return;
    glBindVertexArray(chunk_vao);

    // Geometry VBO
    glBindBuffer(GL_ARRAY_BUFFER, geometry_vbo);
    glBufferData(GL_ARRAY_BUFFER, mesh_vertices.size() * sizeof(Vertex), mesh_vertices.data(), GL_DYNAMIC_DRAW);

    // Position attribute (location = 0)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);

    // UV attribute (location = 1)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(3 * sizeof(float)));

    // Instance VBO
    // glBindBuffer(GL_ARRAY_BUFFER, instance_vbo);
    // glBufferData(GL_ARRAY_BUFFER, instance_vector.size() * sizeof(glm::mat4), instance_vector.data(), GL_DYNAMIC_DRAW);

    // Model matrix (locations 2, 3, 4, 5)
    // std::size_t vec4Size = sizeof(glm::vec4);
    // for (int i = 0; i < 4; i++)
    // {
    //     glEnableVertexAttribArray(2 + i);
    //     glVertexAttribPointer(2 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void *)(i * vec4Size));
    //     glVertexAttribDivisor(2 + i, 1); // Advance once per instance
    // }

    // Unbind VAO and VBO
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    sent_mesh = true;
}

void Chunk::draw_chunk(bool rendered_chunks[])
{
    // try lock instead of holding up main thread
    // for some fucking reason, try lock doesn't unlock out of scope!!!!
    if (!chunk_mutex.try_lock())
        return;
    // std::lock_guard<std::mutex> chunk_lock(chunk_mutex);

    // if (is_renderable())
    // {
    //     chunk_mutex.unlock();
    //     return;
    // }
    // printf("ready to buffer!\n");
    buffer_data();

    // std::lock_guard<std::mutex> lock(chunk_mutex);
    if (!clean_mesh)
    {
        chunk_mutex.unlock();
        return;
    }
    // printf("drawing chunk\n");
    // printf("Chunk (%d, %d) DRAWING: size of mesh vertices = %d\n", chunk_coordinates.first, chunk_coordinates.second, mesh_vertices.size());
    glBindVertexArray(chunk_vao);
    // glDrawArraysInstanced(GL_TRIANGLES, 0, 6, terrain.chunks[i].instance_vector.size());

    // always draw, but only draw if
    // printf("vertice SIZE: %d\n", mesh_vertices.size());
    glDrawArrays(GL_TRIANGLES, 0, mesh_vertices.size());
    glBindVertexArray(0);
    chunk_mutex.unlock();
    // printf("Fully DRAWN THE CHUNK!!!!!!\n");
    rendered = true;
    rendered_chunks[chunk_num] = true;
}

void Chunk::needs_remesh()
{
    clean_mesh = false;
    sent_mesh = false;
    generated_vertices = false;
    rendered = false;
}

void Chunk::new_chunk_state()
{
    initialized = false;
    clean_terrain = false;
    clean_mesh = false;
    generated_vertices = false;
    sent_mesh = false;
    rendered = false;
}

bool Chunk::ready_to_buffer()
{
    // printf("is initialized = %d\nclean terrain = %d\nclean_mesh = %d\ngenerated vertices = %d\n", initialized, clean_terrain, clean_mesh, generated_vertices);
    return initialized && clean_terrain && clean_mesh && generated_vertices;
}

bool Chunk::is_renderable()
{
    return (initialized && clean_terrain && clean_mesh && generated_vertices && sent_mesh && enqueued && enqueued_mesh_creation);
}

// INCORRECT FUNCTION, REPLACE
bool Chunk::in_chunk_coords(int x, int z)
{
    return false;
    return (x / X == chunk_coordinates.first && z / Z == chunk_coordinates.second);
}
/*

is initialized = 1
clean terrain = 1
clean_mesh = 0
generated vertices = 0

*/
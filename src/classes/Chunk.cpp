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
    initialized = true;
    clean_terrain = true;
}

float Chunk::generateHeight(float x, float z, float scale, float heightMultiplier)
{
    float noiseValue = glm::perlin(glm::vec2(x * scale, z * scale));
    // Normalize to [0, 1]
    noiseValue = (noiseValue + 1.0f) / 2.0f;
    return noiseValue * heightMultiplier;
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
    initialized = true;
}

// remember to customize this to add other blocks in the mix
// like dirt, gravel, etc.
// should only be done once the chunk needs to be created, not each iteration
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
            // min block height added so that total can be from 0 to 255.
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
    if (clean_mesh)
        return;
    if (!clean_terrain || !initialized)
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
                                              (face == 4) ? face_textures[1]
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
    clean_mesh = true;
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
    {
        if (clean_mesh)
            return;
        glBindVertexArray(chunk_vao);

        // Geometry VBO
        glBindBuffer(GL_ARRAY_BUFFER, geometry_vbo);
        glBufferData(GL_ARRAY_BUFFER, mesh_vertices.size() * sizeof(Vertex), mesh_vertices.data(), GL_DYNAMIC_DRAW);
    }
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
}

void Chunk::draw_chunk()
{
    buffer_data();

    // std::lock_guard<std::mutex> lock(chunk_mutex);
    if (!clean_mesh)
        return;
    // printf("chunk renderable?\n");
    // printf("size of mesh vertices = %d\n", mesh_vertices.size());
    glBindVertexArray(chunk_vao);
    // glDrawArraysInstanced(GL_TRIANGLES, 0, 6, terrain.chunks[i].instance_vector.size());

    // always draw, but only draw if
    glDrawArrays(GL_TRIANGLES, 0, mesh_vertices.size());
    glBindVertexArray(0);
}

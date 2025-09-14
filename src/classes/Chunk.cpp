#include "../../include/Chunk.hpp"
#include "../../include/Terrain.hpp"
#include <iostream>
#include <cmath>
#include <algorithm>
#include <cstdlib>
#include <unordered_set>

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

inline float weight_of(const BIOME_DISTRIBUTION &w, BIOME b)
{
    switch (b)
    {
    case DESERT:
        return w.desert;
    case PLAINS:
        return w.plains;
    case FOREST:
        return w.forest;
    case MOUNTAINS:
        return w.mountains;
    }
    return 0.0f;
}

inline void top2_biomes(const BIOME_DISTRIBUTION &w, BIOME &b0, BIOME &b1)
{
    float vals[NUM_BIOMES] = {w.desert, w.plains, w.forest, w.mountains};
    int i0 = 0, i1 = 1;
    if (vals[i1] > vals[i0])
        std::swap(i0, i1);
    for (int i = 2; i < NUM_BIOMES; ++i)
    {
        if (vals[i] >= vals[i0])
        {
            i1 = i0;
            i0 = i;
        }
        else if (vals[i] > vals[i1])
        {
            i1 = i;
        }
    }
    b0 = static_cast<BIOME>(i0);
    b1 = static_cast<BIOME>(i1);
}

inline float hash01_int(int x, int z)
{
    uint32_t h = 2166136261u;
    h ^= static_cast<uint32_t>(x) + 0x9e3779b9u + (h << 6) + (h >> 2);
    h ^= static_cast<uint32_t>(z) + 0x85ebca6bu + (h << 6) + (h >> 2);
    // Final mixing (xorshift-ish)
    h ^= h >> 16;
    h *= 0x7feb352dU;
    h ^= h >> 15;
    h *= 0x846ca68bU;
    h ^= h >> 16;
    return (h) * (1.0f / 4294967296.0f); // [0,1)
}

inline int H_local(Chunk *chunk, int lx, int lz)
{
    // clamp to chunk to avoid out-of-bounds; good enough for slope
    lx = std::max(0, std::min(X - 1, lx));
    lz = std::max(0, std::min(Z - 1, lz));
    return chunk->heightMap[lx * Z + lz];
}

struct PairHash
{
    template <class T1, class T2>
    std::size_t operator()(const std::pair<T1, T2> &p) const
    {
        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T2>{}(p.second);
        // Combine hashes (e.g., using boost::hash_combine idea)
        return h1 ^ (h2 << 1);
    }
};

inline void create_leaves(int x, int y, int z, std::unordered_set<std::pair<int, int>, struct PairHash> &prev_visited, int iteration, Chunk *chunk, Chunk *original_chunk, bool start = true)
{
    Chunk *final_chunk = original_chunk;
    bool found_chunk = true;
    // printf("before (%d, %d, %d)\n", x, y, z);
    int cx = x / X;
    if (x < 0 && x % X != 0)
        cx--;
    int cz = z / Z;
    if (z < 0 && z % Z != 0)
        cz--;

    if (cx != original_chunk->chunk_coordinates.first || cz != original_chunk->chunk_coordinates.second)
    {
        found_chunk = false;

        for (int i = 0; i < NUM_CHUNKS; i++)
        {
            auto test_chunk = &chunk->all_chunks[i];
            if (test_chunk->chunk_coordinates.first == cx && test_chunk->chunk_coordinates.second == cz)
            {
                // printf("found chunk!\n");
                final_chunk = test_chunk;
                found_chunk = true;
                break;
            }
        }
    }

    if (!found_chunk)
    {
        return;
    }

    // printf("1\n");
    if (prev_visited.find({x, z}) != prev_visited.end())
    {
        // printf("Return?\n");
        return;
    }

    // printf("2\n");
    prev_visited.insert({x, z});

    // printf("3\n");
    float currMult = std::pow(1.15, iteration);
    auto chunk_x = original_chunk->chunk_coordinates.first;
    auto chunk_z = original_chunk->chunk_coordinates.second;
    auto hash = hash01_int(chunk_x, chunk_z);
    // printf("hash: %f\n", hash);
    // function calls should naturally die off as divisor gets bigger.
    // to give trees that natural look (larger leaves at the bottom, smaller ones on top)
    // printf("4\n");
    if ((2.05 - currMult - hash / 3) > 0.2)
    {

        if (!start)
        {
            // printf("(%d, %d %d)\n", x, y, z);
            // printf("4\n");
            int local_x = x - (cx * X);
            int local_z = z - (cz * Z);

            final_chunk->blocks[final_chunk->get_index(local_x, y, local_z)].update_state(x, y, z, "leaf");
            // printf("5\n");
        }
        // if (iteration < 4)
        iteration++;
        // printf("iteration: %d\n", iteration);
        create_leaves(x + 1, y, z, prev_visited, iteration, final_chunk, original_chunk, false);
        create_leaves(x - 1, y, z, prev_visited, iteration, final_chunk, original_chunk, false);
        create_leaves(x, y, z + 1, prev_visited, iteration, final_chunk, original_chunk, false);
        create_leaves(x, y, z - 1, prev_visited, iteration, final_chunk, original_chunk, false);
        // printf("6\n");
    }
}

void Chunk::generate_biome_terrain(int x, int z)
{
    float wx = get_cube_x(x);
    float wz = get_cube_z(z);
    glm::vec2 p(wx, wz);

    auto W = Biome::get_biome_distribution(p);
    BIOME B0, B1;
    top2_biomes(W, B0, B1);
    float edge = 1.0f - weight_of(W, B0);

    int Hc = Biome::get_height(p);
    int Hx1 = Biome::get_height(p + (glm::vec2)(1, 0));
    int Hx2 = Biome::get_height(p + (glm::vec2)(-1, 0));
    int Hz1 = Biome::get_height(p + (glm::vec2)(0, 1));
    int Hz2 = Biome::get_height(p + (glm::vec2)(0, -1));
    int max_h = Hc - Hx1;
    max_h = (max_h < abs(Hc - Hx1)) ? abs(Hc - Hx1) : max_h;
    max_h = (max_h < abs(Hc - Hx2)) ? abs(Hc - Hx2) : max_h;
    max_h = (max_h < abs(Hc - Hz1)) ? abs(Hc - Hz1) : max_h;
    max_h = (max_h < abs(Hc - Hz2)) ? abs(Hc - Hz2) : max_h;

    int slope = max_h;
    int Hmean = (Hx1 + Hx2 + Hz1 + Hz2) / 4;
    int resid = Hc - Hmean;
    std::string top;
    float chunk_x = get_cube_x(x), chunk_z = get_cube_z(z);

    // slope override
    if (slope > 2 || resid > 2)
        top = "stone";
    else
    {
        // edge borrow decision (deterministic)
        float h = hash01_int(wx, wz);
        float cluster = 0.5f * (glm::perlin(p / 45.0f) + 1.0f);
        float borrowP = edge * 0.65f;
        borrowP = 0.7f * borrowP + 0.3f * (borrowP * cluster);
        BIOME B = (h < borrowP ? B1 : B0);

        // per-biome surface with a dash of slope
        switch (B)
        {
        case PLAINS:
            top = (slope <= 1 ? "grass" : "stone");
            break;
        case FOREST:
            top = (slope <= 2 ? "grass" : "stone");
            break;
        case DESERT:
            top = (resid < 1 ? "sand" : (slope >= 2 ? "stone" : "sand"));
            break;
        case MOUNTAINS:
            top = (slope >= 2 || resid > 1 ? "stone" : "grass");
            break;
        }

        if (B == FOREST && h > 0.99)
        {
            int tree_height = 5 + (int)(h / 0.25);
            for (int i = 0; i < tree_height; i++)
            {
                blocks[get_index(x, Hc + 1 + i, z)].update_state(chunk_x, Hc + 1 + i, chunk_z, "wood");
            }

            float h2 = hash01_int(h, h);

            std::unordered_set<std::pair<int, int>, PairHash> prev_map;
            for (int i = tree_height - 1 - round(h2); i < tree_height + 3; i++)
            {
                int iteration = i - tree_height + 1 + round(h2);
                if (i > tree_height)
                    create_leaves(wx, Hc + i, wz, prev_map, iteration, this, this, false);
                else
                    create_leaves(wx, Hc + i, wz, prev_map, iteration, this, this);
                prev_map.clear();
            }
        }
    }

    if (Hc <= 54)
    {
        for (int y = Hc + 1; y <= Hc + 3; y++)
        {

            blocks[get_index(x, y, z)].update_state(chunk_x, y, chunk_z, "water");
        }
    }

    for (int y = 0; y < Hc - 1; y++)
    {

        blocks[get_index(x, y, z)].update_state(chunk_x, y, chunk_z, "stone");
    }
    blocks[get_index(x, Hc, z)].update_state(chunk_x, Hc, chunk_z, top);

    // std::vector<int> heightMap; // size X*Z

    // // printf("got chunk zone!\n");
    // // get y value that will be used for
    // // the height, if y < height, stone
    // // if greater, air.
    // float chunk_x = get_cube_x(x), chunk_z = get_cube_z(z);
    // // printf("%f, %f\n", chunk_x, chunk_z);
    // // min block height added so that total can be from 0 to 255.
    // auto zone_bias = get_zone_bias();
    // float height = Biome::get_height(p);
    // // printf("Block height %f\n", height);
    // // printf("height is %f\n", height);

    // BIOME zone = Biome::get_biome(p);
    // for (int y = 0; y < Y; y++)
    // {
    //     if (zone == FOREST)
    //     {
    //         generate_forest(x, y, z, chunk_x, chunk_z, height);
    //         // printf("generated a forest!\n");
    //     }
    //     else if (zone == PLAINS)
    //     {
    //         generate_plains(x, y, z, chunk_x, chunk_z, height);
    //         // printf("generated a plains!\n");
    //     }
    //     if (zone == DESERT)
    //     {
    //         generate_desert(x, y, z, chunk_x, chunk_z, height);
    //         // printf("generated a desert!\n");
    //     }
    //     if (zone == MOUNTAINS)
    //     {
    //         generate_mountains(x, y, z, chunk_x, chunk_z, height);
    //         // printf("generated a mountain!\n");
    //     }
    // }
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

        blocks[get_index(x, y, z)].update_state(chunk_x, y, chunk_z, "wooden_plank");
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
    // Fill before you write blocks:
    heightMap.resize(X * Z);
    for (int lx = 0; lx < X; ++lx)
    {
        for (int lz = 0; lz < Z; ++lz)
        {
            float wx = get_cube_x(lx);
            float wz = get_cube_z(lz);
            heightMap[lx * Z + lz] = Biome::get_height(glm::vec2(wx, wz));
        }
    }
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
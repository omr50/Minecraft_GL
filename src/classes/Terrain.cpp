#include "../../include/Terrain.hpp"

Terrain::Terrain(glm::vec3 *camera_position) : camera_position(camera_position)
{
    std::pair<int, int> positions[9];
    new_positions(positions);

    for (int i = 0; i < 9; i++)
    {
        chunks[i].chunk_coordinates.first = positions[i].first;
        chunks[i].chunk_coordinates.second = positions[i].second;
        chunks[i].initialize_cubes();
        chunks[i].generate_terrain();
    }
}
/*
- The 9 chunks are created in the beginning.
   - The center is given and we use that to initialize the instance in the constructor.
   - We then give the 8 other coordinates for the chunks around it easily by adding or sub 1 on each axis
   - basically double loop with -1 to 1 on each axis.
   - -1 -1 , -1 0, -1 1, 0 -1, 0 0, 0 1, 1 -1 1 0 1 1
   - For each of the chunks we have to initialize the cubes (should also be done in the constructor of Terrain since only base constructor of Chunk is called)

   - when the player moves to a new chunk, we do the shift chunks, within that function we compute all of the new chunks and basically check which chunks we should
     keep and which we shouldn't. Easy loop over all 9 existing ones, the ones where the coordinate is found, we keep, the one where it isn't we re-generate the cubes
     (use change_chunk_position function, give it the new position, and change position then re-render the terrain with the generate_terrain function for the chunk)
*/
void Terrain::shift_chunks()
{
    std::pair<int, int> positions[9];
    new_positions(positions);

    bool bound[9];
    bool new_vals[9];
    // bound: find which values are not in bounds any more
    // new_vals: find new values in position array
    for (int i = 0; i < 9; i++)
    {
        bound[i] = find_out_of_bound_chunk(chunks[i].chunk_coordinates.first, chunks[i].chunk_coordinates.second, positions);
        new_vals[i] = find_new_positions(positions[i]);
    }

    // pair up new vals with bounds
    // bounds = false means chunk not in bounds (needs to be replaced)
    // new_vals = true means that new value needs to be substituted into one of the chunks thats out of bounds since this is a new value
    int x, z;
    int p1 = 0, p2 = 0;
    while (p1 < 9 && p2 < 9)
    {
        while (p1 < 9 && bound[p1])
            p1++;
        while (p2 < 9 && !new_vals[p2])
            p2++;

        if (p1 < 9 && p2 < 9)
        {
            // Update chunk

            chunks[p1].chunk_coordinates.first = positions[p2].first;
            chunks[p1].chunk_coordinates.second = positions[p2].second;
            chunks[p1].initialize_cubes();
            chunks[p1].generate_terrain();
            p1++;
            p2++;
        }
    }
}

void Terrain::new_positions(std::pair<int, int> positions[])
{
    for (int i = -1; i < 2; i++)
    {
        for (int j = -1; j < 2; j++)
        {
            auto center = get_center_chunk_coordinates(camera_position->x, camera_position->z);
            positions[(i + 1) * 3 + (j + 1)].first = center.first + i;
            positions[(i + 1) * 3 + (j + 1)].second = center.second + j;
            // printf("Terrain: (%d, %d)\n", positions[(i + 1) * 3 + (j + 1)].first, positions[(i + 1) * 3 + (j + 1)].second);
        }
    }
    std::pair<int, int> center = get_center_chunk_coordinates(camera_position->x, camera_position->z);
    // printf("Center: (%d, %d)\n", center.first, center.second);
}

bool Terrain::find_out_of_bound_chunk(int x, int z, std::pair<int, int> positions[])
{
    for (int i = 0; i < 9; i++)
    {
        if (x == positions[i].first && z == positions[i].second)
            return true;
    }
    return false;
}

bool Terrain::find_new_positions(std::pair<int, int> val)
{
    for (int i = 0; i < 9; i++)
    {
        if (chunks[i].chunk_coordinates.first == val.first && chunks[i].chunk_coordinates.second == val.second)
            return false;
    }
    return true;
}

std::pair<int, int> Terrain::get_center_chunk_coordinates(float x, float z)
{
    return std::make_pair((int)x / X, (int)z / Z);
}

void Terrain::create_mesh()
{
    for (int i = 0; i < NUM_CHUNKS; i++)
    {
        for (int x = 0; x < X; x++)
            for (int y = 0; y < Y; y++)
                for (int z = 0; z < Z; z++)
                {
                    cube_face_renderability(&(chunks[i].blocks[chunks[i].get_index(x, y, z)]));
                }
    }
}

void Terrain::cube_face_renderability(Cube *cube)
{
    int coordinates[3] = {cube->x, cube->y, cube->z};
    int x = cube->x, y = cube->y, z = cube->z;

    // front
    cube->renderable_face[0] = determine_renderability(x, y, z + 1);
    // back
    cube->renderable_face[1] = determine_renderability(x, y, z - 1);
    // left face
    cube->renderable_face[2] = determine_renderability(x - 1, y, z);
    // right face
    cube->renderable_face[3] = determine_renderability(x + 1, y, z);
    // bottom
    cube->renderable_face[4] = determine_renderability(x, y - 1, z);
    // top
    cube->renderable_face[5] = determine_renderability(x, y + 1, z);
}

bool Terrain::determine_renderability(int x, int y, int z)
{
    // determine chunk of the cube
    std::pair<int, int> chunk_coords = std::make_pair(x / X, z / Z);
    int chunk_index;
    if ((chunk_index = get_chunk_index(chunk_coords)) == -1)
        return true;
    printf("chunk index is %d\n", chunk_index);
    Chunk *chunk = &(chunks[chunk_index]);
    printf("cube block type %s\n", chunk->blocks[chunk->get_index(x, y, z)].block_type.c_str());
    // find that block in the chunk
    int block_index = chunk->get_index(x, y, z);
    Cube *cube = &chunk->blocks[block_index];
    if (cube->block_type == "air")
        return true;
    return false;
}

int Terrain::get_chunk_index(std::pair<int, int> chunk_coords)
{
    for (int i = 0; i < NUM_CHUNKS; i++)
    {
        Chunk *chunk = &chunks[i];
        if (chunk->chunk_coordinates.first == chunk_coords.first && chunk->chunk_coordinates.second == chunk_coords.second)
            return i;
    }
    return -1;
}
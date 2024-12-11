#include "../../include/Terrain.hpp"

Terrain::Terrain(glm::vec3 *camera_position) : camera_position(camera_position)
{
    std::pair<int, int> positions[NUM_CHUNKS];
    new_positions(positions);

    for (int i = 0; i < NUM_CHUNKS; i++)
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
    std::pair<int, int> positions[NUM_CHUNKS];
    new_positions(positions);

    bool bound[NUM_CHUNKS];
    bool new_vals[NUM_CHUNKS];
    // bound: find which values are not in bounds any more
    // new_vals: find new values in position array
    for (int i = 0; i < NUM_CHUNKS; i++)
    {
        bound[i] = find_out_of_bound_chunk(chunks[i].chunk_coordinates.first, chunks[i].chunk_coordinates.second, positions);
        new_vals[i] = find_new_positions(positions[i]);
    }

    // pair up new vals with bounds
    // bounds = false means chunk not in bounds (needs to be replaced)
    // new_vals = true means that new value needs to be substituted into one of the chunks thats out of bounds since this is a new value
    int x, z;
    int p1 = 0, p2 = 0;
    while (p1 < NUM_CHUNKS && p2 < NUM_CHUNKS)
    {
        while (p1 < NUM_CHUNKS && bound[p1])
            p1++;
        while (p2 < NUM_CHUNKS && !new_vals[p2])
            p2++;

        if (p1 < NUM_CHUNKS && p2 < NUM_CHUNKS)
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
    int grid_width = sqrt(NUM_CHUNKS);
    int half_way = grid_width / 2;
    auto center = get_center_chunk_coordinates(camera_position->x, camera_position->z);
    auto first_coord = std::make_pair<int, int>(center.first - half_way, center.second - half_way);

    // for (int i = half_way - grid_width; i < half_way + is_odd; i++)
    // {
    //     for (int j = half_way - grid_width; j < half_way + is_odd; j++)
    //     {
    //         // printf("%d, %d)\n", i, j);
    //         auto center = get_center_chunk_coordinates(camera_position->x, camera_position->z);
    //         positions[(i + 1) * 3 + (j + 1)].first = center.first + i;
    //         positions[(i + 1) * 3 + (j + 1)].second = center.second + j;
    //         // printf("Terrain: (%d, %d)\n", positions[(i + 1) * 3 + (j + 1)].first, positions[(i + 1) * 3 + (j + 1)].second);
    //     }
    // }

    for (int i = 0; i < grid_width; i++)
        for (int j = 0; j < grid_width; j++)
        {

            positions[i * grid_width + j].first = first_coord.first + i;
            positions[i * grid_width + j].second = first_coord.second + j;
        }

    // printf("Center (%d, %d)", center.first, center.second);
    // for (int i = 0; i < NUM_CHUNKS; i++)
    // printf("coords (%d, %d)\n", positions[i].first, positions[i].second);
    // std::pair<int, int> center = get_center_chunk_coordinates(camera_position->x, camera_position->z);
    // printf("Center: (%d, %d)\n", center.first, center.second);
}

bool Terrain::find_out_of_bound_chunk(int x, int z, std::pair<int, int> positions[])
{
    for (int i = 0; i < NUM_CHUNKS; i++)
    {
        if (x == positions[i].first && z == positions[i].second)
            return true;
    }
    return false;
}

bool Terrain::find_new_positions(std::pair<int, int> val)
{
    for (int i = 0; i < NUM_CHUNKS; i++)
    {
        if (chunks[i].chunk_coordinates.first == val.first && chunks[i].chunk_coordinates.second == val.second)
            return false;
    }
    return true;
}

std::pair<int, int> Terrain::get_center_chunk_coordinates(float x, float z)
{
    // return std::make_pair((int)x / X, (int)z / Z);
    int chunk_x = std::floor(x / (float)X);
    int chunk_z = std::floor(z / (float)Z);
    return std::make_pair(chunk_x, chunk_z);
}

// later just do calculations for those that are in the
// frustum view range, otherwise don't waste time to calculate.
void Terrain::create_mesh()
{
    for (int i = 0; i < NUM_CHUNKS; i++)
    {
        for (int x = 0; x < X; x++)
            for (int y = 0; y < Y; y++)
                for (int z = 0; z < Z; z++)
                {
                    cube_face_renderability(&chunks[i], &(chunks[i].blocks[chunks[i].get_index(x, y, z)]));
                }
    }

    int total_faces = 0;
    int culled_faces = 0;

    for (int i = 0; i < NUM_CHUNKS; i++)
    {
        for (int x = 0; x < X; x++)
            for (int y = 0; y < Y; y++)
                for (int z = 0; z < Z; z++)
                {
                    Cube *cube = &chunks[i].blocks[chunks[i].get_index(x, y, z)];
                    if (cube->block_type != "air")
                    {
                        total_faces += 6; // 6 faces per solid cube
                        for (int f = 0; f < 6; f++)
                        {
                            if (!cube->renderable_face[f])
                            {
                                culled_faces++;
                            }
                        }
                    }
                }
    }
    printf("Total faces: %d, Culled faces: %d, Rendering: %d\n",
           total_faces, culled_faces, total_faces - culled_faces);
}

void Terrain::cube_face_renderability(Chunk *chunk, Cube *cube)
{
    // world coordinates for x, y, and z
    int x = chunk->get_cube_x(cube->x);
    int y = cube->y;
    int z = chunk->get_cube_z(cube->z);

    if (cube->block_type == "air")
        return;
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
    // printf("%d %d %d %d %d %d\n", cube->renderable_face[0], cube->renderable_face[1], cube->renderable_face[2], cube->renderable_face[3], cube->renderable_face[4], cube->renderable_face[5]);
}

bool Terrain::determine_renderability(int x, int y, int z)
{
    // determine chunk of the cube
    // a manual floor for negative value cases
    int cx = (x >= 0) ? x / X : (x / X) - 1; // a manual floor for negative values
    int cz = (z >= 0) ? z / Z : (z / Z) - 1;

    // Find our center chunk (the middle of our 3x3 grid)
    auto center = get_center_chunk_coordinates(camera_position->x, camera_position->z);

    // If the chunk we're trying to check is outside our 3x3 grid
    if (cx < center.first - 1 || cx > center.first + 1 ||
        cz < center.second - 1 || cz > center.second + 1)
    {
        // Treat blocks outside our loaded chunks as air
        return false;
    }

    static bool first_time = true;
    if (first_time)
    {
        // printf("Center chunk: (%d, %d)\n", center.first, center.second);
        // printf("Checking chunks from (%d, %d) to (%d, %d)\n",
        //    center.first - 1, center.second - 1,
        //    center.first + 1, center.second + 1);
        first_time = false;
    }

    std::pair<int, int> chunk_coords = std::make_pair(cx, cz);
    int chunk_index = get_chunk_index(chunk_coords);

    if (chunk_index == -1)
        return true; // This shouldn't happen anymore with above check

    Chunk *chunk = &chunks[chunk_index];

    // our x, y, z coordinates are world coordinates so we get the
    // local_x and local_z by using modulo because all coordinates
    // in the chunks cube array are from 0 to (chunk max x * chunk max y * chunk max z)
    int local_x = x % X;
    if (local_x < 0)
        local_x += X;
    int local_z = z % Z;
    if (local_z < 0)
        local_z += Z;
    int block_index = chunk->get_index(local_x, y, local_z);
    Cube *cube = &chunk->blocks[block_index];
    // printf("(local index (%d,%d) block index %d\n", local_x, local_z, block_index);
    return cube->block_type == "air";
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
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
    // find which values are not in bounds any more
    for (int i = 0; i < 9; i++)
    {
        bound[find_chunk(chunks[i].chunk_coordinates.first, chunks[i].chunk_coordinates.second, positions)];
    }

    // find new values in position array
    for (int i = 0; i < 9; i++)
    {
        new_vals[find_new_positions(positions[i])];
    }

    // pair up new vals with bounds
    // bounds = false means chunk not in bounds (needs to be replaced)
    // new_vals = true means that new value needs to be substituted into one of the chunks thats out of bounds since this is a new value
    int x, z;
    int p1 = 0, p2 = 0;
    while (p1 < 9 && p2 < 9)
    {
        while (bound[p1])
            p1++;
        while (!new_vals[p2])
            p2++;
        // chunk that is not bound needs to be replaced with new position value
        chunks[p1].chunk_coordinates.first = positions[p2].first;
        chunks[p1].chunk_coordinates.first = positions[p2].second;
        chunks[p1].generate_terrain();
        p1++;
        p2++;
    }
}

void Terrain::new_positions(std::pair<int, int> positions[])
{
    for (int i = -1; i < 2; i++)
    {
        for (int j = -1; j < 2; j++)
        {
            positions[(i + 1) * 3 + (j + 1)].first = ((int)camera_position->x / 16) + i;
            positions[(i + 1) * 3 + (j + 1)].second = ((int)camera_position->z / 16) + j;
            printf("Terrain: (%d, %d)\n", positions[(i + 1) * 3 + (j + 1)].first, positions[(i + 1) * 3 + (j + 1)].second);
        }
    }
}
bool Terrain::find_chunk(int x, int z, std::pair<int, int> positions[])
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
            return true;
    }
    return false;
}
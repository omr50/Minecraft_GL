#pragma once
#include "../include/Chunk.hpp"
#include <glm/glm.hpp>

#define NUM_CHUNKS 9
class Terrain
{
public:
    Chunk chunks[NUM_CHUNKS];
    glm::vec3 *camera_position;

    Terrain(glm::vec3 *camera_position);
    void shift_chunks();
    bool find_out_of_bound_chunk(int x, int z, std::pair<int, int> positions[]);
    bool find_new_positions(std::pair<int, int> val);
    void new_positions(std::pair<int, int> positions[]);
    std::pair<int, int> get_center_chunk_coordinates(float x, float z);
    void create_mesh();
    void cube_face_renderability(Cube *cube);
    bool determine_renderability(int x, int y, int z);
    int get_chunk_index(std::pair<int, int> chunk_coords);

    /*
   - can use threads to update each chunk potentially in parallel
   - Also threads can be used in determining faces that should be rendered.
   - Basically each face should only render if block next to it is air block.
   - How to determine when algo should stop (going over every cube seems like over kill)

    */
};
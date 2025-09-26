#pragma once
#include "../include/Chunk.hpp"
#include "../include/Renderable.hpp"
#include "../include/Camera.hpp"
#include "../include/ThreadPool.hpp"
#include <glm/glm.hpp>
// change
#define NUM_CHUNKS 361

// chunk will implement renderable
// we want to draw on a larger scal
// not on the cube level

class Camera;

class Terrain : public Renderable
{
public:
    Chunk chunks[NUM_CHUNKS];
    Camera *camera;
    ThreadPool *thread_pool;
    glm::vec3 prev_camera_position;
    // only access this integer through main thread, won't need any threads
    int num_chunks_rendered = 0;
    // only access this array through main thread, won't need any threads
    bool rendered_chunks[NUM_CHUNKS] = {0};
    bool rendered_water[NUM_CHUNKS] = {0};

    Terrain(Camera *camera);
    void shift_chunks();
    bool find_out_of_bound_chunk(int x, int z, std::pair<int, int> positions[]);
    bool find_new_positions(std::pair<int, int> val);
    void new_positions(std::pair<int, int> positions[]);
    std::pair<int, int> get_center_chunk_coordinates(float x, float z);
    void create_mesh();
    void cube_face_renderability(Chunk *chunk, Cube *cube);
    bool determine_renderability(int x, int y, int z, std::string prev_block = "");
    int get_chunk_index(std::pair<int, int> chunk_coords);
    void draw() override;
    bool camera_moved();
    void create_chunk_mesh(Chunk *chunk);
    void enqueue_update_task(Chunk *chunk);
    void enqueue_initial_task(Chunk *chunk);

    /*
   - can use threads to update each chunk potentially in parallel
   - Also threads can be used in determining faces that should be rendered.
   - Basically each face should only render if block next to it is air block.
   - How to determine when algo should stop (going over every cube seems like over kill)

    */
};
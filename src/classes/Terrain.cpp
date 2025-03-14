#include "../../include/Terrain.hpp"
#include <chrono>

Terrain::Terrain(Camera *camera) : camera(camera)
{
    thread_pool = new ThreadPool(7);
    std::pair<int, int> positions[NUM_CHUNKS];
    new_positions(positions);
    // set previous position to different value so that
    // first frame will cause a render.
    prev_camera_position.x = -1;
    prev_camera_position.y = -1;
    prev_camera_position.z = -1;

    for (int i = 0; i < NUM_CHUNKS; i++)
    {
        chunks[i].chunk_num = i;
        chunks[i].chunk_coordinates.first = positions[i].first;
        chunks[i].chunk_coordinates.second = positions[i].second;
        chunks[i].initialize_vertex_buffers_and_array();
        chunks[i].initialize_cubes();
        chunks[i].generate_terrain();
        // chunks[i].update_chunk();
        // enqueue_update_task(&chunks[i]);
    }

    for (int i = 0; i < NUM_CHUNKS; i++)
    {
        create_chunk_mesh(&chunks[i]);
        chunks[i].update_chunk();
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


     change this function later to instead just change state and not actually initialize the
     cubes or generate terrain. Because we want to do that in the threads. Basically the function
     would set the boolean state (one variable indicating cubes need re-initialization, and the other
     indicating that terrain needs re-genetration, or they could be one variable since needing initialization
     always requires generation. But in the case of edge chunks becoming non-edge chunks, this does not hold
     because we just need to re-generate the mesh, not actually initialize the cubes nor generate the terrain.
     And that would be un-necessary load. So yes just have different variables for them. and then place the chunk
     object on the queue (if it doesn't exist already before, if it does, then don't add it again). Then a thread will
     pick up the task.
*/

void Terrain::shift_chunks()
{
    if (!camera_moved())
        return;

    std::pair<int, int> positions[NUM_CHUNKS];
    new_positions(positions);

    bool bound[NUM_CHUNKS];
    bool new_vals[NUM_CHUNKS];
    // bound: find which values are not in bounds any more
    // new_vals: find new values in position array
    for (int i = 0; i < NUM_CHUNKS; i++)
    {
        // std::lock_guard<std::mutex> lock(chunks[i].chunk_mutex);
        if (!chunks[i].chunk_mutex.try_lock())
            return;
        bound[i] = find_out_of_bound_chunk(chunks[i].chunk_coordinates.first, chunks[i].chunk_coordinates.second, positions);
        new_vals[i] = find_new_positions(positions[i]);
        chunks[i].chunk_mutex.unlock();
    }
    // printf("got to point 2.1\n");

    // pair up new vals with bounds
    // bounds = false means chunk not in bounds (needs to be replaced)
    // new_vals = true means that new value needs to be substituted into one of the chunks thats out of bounds since this is a new value
    int x, z;
    int p1 = 0, p2 = 0;
    // get the direction shift
    // printf("got to this point 2.2\n");
    // printf("camera pointer %p\n", (void *)camera);
    auto direction_shift = camera->get_direction();
    // printf("got to this point 2.3\n");
    while (p1 < NUM_CHUNKS && p2 < NUM_CHUNKS)
    {
        while (p1 < NUM_CHUNKS && bound[p1])
            p1++;
        while (p2 < NUM_CHUNKS && !new_vals[p2])
            p2++;

        if (p1 < NUM_CHUNKS && p2 < NUM_CHUNKS)
        {
            // Update chunk
            auto chunk = &chunks[p1];
            // std::lock_guard<std::mutex> chunkLock(chunk->chunk_mutex);
            if (!chunk->chunk_mutex.try_lock())
            {
                continue;
            }
            chunks[p1].chunk_coordinates.first = positions[p2].first;
            chunks[p1].chunk_coordinates.second = positions[p2].second;

            chunks[p1].new_chunk_state();
            // printf("enqeueuing initial task!\n");
            enqueue_initial_task(chunk);

            int x_coord = chunks[p1].chunk_coordinates.first - direction_shift.first;
            int z_coord = chunks[p1].chunk_coordinates.second - direction_shift.second;
            for (auto &chunk : chunks)
            {
                bool is_adjacent = ((abs(chunk.chunk_coordinates.first - positions[p2].first) <= 1 &&
                                     abs(chunk.chunk_coordinates.second - positions[p2].second) <= 1));

                // if (x_coord == chunk.chunk_coordinates.first && z_coord == chunk.chunk_coordinates.second)
                if (is_adjacent)
                {
                    chunk.needs_remesh();
                    // Not sure if this is what causes
                    // the issue. But there is an issue with
                    // queue not properly clearing everything.
                    enqueue_update_task(&chunk);
                }
            }
            p1++;
            p2++;
            chunk->chunk_mutex.unlock();
        }
    }
}

void Terrain::new_positions(std::pair<int, int> positions[])
{
    int grid_width = sqrt(NUM_CHUNKS);
    int half_way = grid_width / 2;
    auto center = get_center_chunk_coordinates(camera->position.x, camera->position.z);
    auto first_coord = std::make_pair<int, int>(center.first - half_way, center.second - half_way);

    for (int i = 0; i < grid_width; i++)
        for (int j = 0; j < grid_width; j++)
        {

            positions[i * grid_width + j].first = first_coord.first + i;
            positions[i * grid_width + j].second = first_coord.second + j;
        }
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
    int chunk_x = std::floor(x / (float)X);
    int chunk_z = std::floor(z / (float)Z);
    return std::make_pair(chunk_x, chunk_z);
}

// later just do calculations for those that are in the
// frustum view range, otherwise don't waste time to calculate.
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! I feel like this function better belongs
// to the chunks themselves. Or the create mesh can call each chunks function.
void Terrain::create_mesh()
{
    // later try to use thread pool to run the some range of i per thread
    for (int i = 0; i < NUM_CHUNKS; i++)
    {
        std::lock_guard<std::mutex> lock(chunks[i].chunk_mutex);
        if (!chunks[i].clean_mesh && chunks[i].initialized && chunks[i].clean_terrain && !chunks[i].enqueued_mesh_creation)
        {
            // printf("enqueued mesh creation\n");
            // if (!chunks[i].enqueued)
            // {
            chunks[i].enqueued_mesh_creation = true;
            thread_pool->enqueue_task([this, i]
                                      { create_chunk_mesh(&chunks[i]); chunks[i].enqueued_mesh_creation = false; });
            // }
        }
        // clean will be set to false after iteration / frame
    }
}

void Terrain::create_chunk_mesh(Chunk *chunk)
{
    for (int x = 0; x < X; x++)
        for (int y = 0; y < Y; y++)
            for (int z = 0; z < Z; z++)
            {
                Cube *cube = nullptr;
                {
                    // std::lock_guard<std::mutex> lock(chunk->chunk_mutex);
                    cube = &(chunk->blocks[chunk->get_index(x, y, z)]);
                }
                cube_face_renderability(chunk, cube);
            }
    chunk->clean_mesh = true;
    chunk->sent_mesh = false;
    chunk->generated_vertices = false;
}

void Terrain::cube_face_renderability(Chunk *chunk, Cube *cube)
{

    // std::lock_guard<std::mutex> lock(chunk->chunk_mutex);

    // world coordinates for x, y, and z
    int x = cube->x;
    int y = cube->y;
    int z = cube->z;
    // printf("%d %d %d cube face renderability x y z\n", x, y, z);
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

    int cx = x / X;
    if (x < 0 && x % X != 0)
        cx--;
    int cz = z / Z;
    if (z < 0 && z % Z != 0)
        cz--;
    // int cx = (x >= 0) ? x / X : (x / X) - 1; // a manual floor for negative values
    // int cz = (z >= 0) ? z / Z : (z / Z) - 1;

    // printf("World coord (%d,%d,%d) -> Chunk (%d,%d)\n", x, y, z, cx, cz);
    // Find our center chunk (the middle of our 3x3 grid)
    auto center = get_center_chunk_coordinates(camera->position.x, camera->position.z);

    int half_width = ((int)sqrt(NUM_CHUNKS)) / 2;
    // If the chunk we're trying to check is outside our 3x3 grid
    if (cx < center.first - half_width || cx > center.first + half_width ||
        cz < center.second - half_width || cz > center.second + half_width)
    {
        // Treat blocks outside our loaded chunks as air

        // allow this to be true so that chunks that go from edge or boundary chunk
        // to a center or non-edge chunk will not have to have their mesh recompute
        return false;
    }
    // printf("%d %d %d\n", x, y, z);
    // Calculate the absolute grid boundaries
    // printf("center - half width = %d", center.first - half_width);
    int min_x = (center.first - half_width) * X;
    int max_x = (center.first + half_width) * X + (X - 1);
    int min_z = (center.second - half_width) * Z;
    int max_z = (center.second + half_width) * Z + (Z - 1);
    // printf("min x z : %d %d max x z: %d %d\n", min_x, min_z, max_x, max_z);
    // If we're checking a face that would be visible from outside the entire grid, don't render it
    if (x < min_x || x > max_x || z < min_z || z > max_z)
    {

        return false;
    }

    std::pair<int, int> chunk_coords = std::make_pair(cx, cz);
    int chunk_index = get_chunk_index(chunk_coords);

    if (chunk_index == -1)
        return true; // This shouldn't happen anymore with above check

    Chunk *chunk = &chunks[chunk_index];

    // our x, y, z coordinates are world coordinates so we get the
    // local_x and local_z by using modulo because all coordinates
    // in the chunks cube array are from 0 to (chunk max x * chunk max y * chunk max z)
    int local_x = x - (cx * X);
    int local_z = z - (cz * Z);

    // rules to stop sides that are out of bounds from rendering
    // printf("%d %d %d\n", local_x, y, local_z);
    if (local_x < 0 || y < 0 || local_z < 0 || local_x >= X || y >= 256 || local_z >= Z)
        return false;

    int block_index = chunk->get_index(local_x, y, local_z);
    Cube *cube = &chunk->blocks[block_index];
    // printf("(local index (%d,%d) block index %d\n", local_x, local_z, block_index);
    if (cube == nullptr)
    {
        // printf("WHY THE IS IT NULL\n");
        return true;
    }
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

void Terrain::draw()
{
    // for each chunk get the data and then combine it
    // send to GPU in batch rather than one at a time
}

bool Terrain::camera_moved()
{
    return camera->moved;
}

// make sure the thread pool only processes
// update tasks after all terrain tasks (initial)
// have been finished

// void Terrain::enqueue_update_task(Chunk *chunk)
// {
//     if (chunk->enqueued)
//         return;
//     thread_pool->enqueue_task([this, chunk]()
//                               {
//         // if (chunk->enqueued)
//             // return;
//         {
//             std::unique_lock<std::mutex> lock(thread_pool->num_task_mutex);
//             printf("Update task checking num_generation_tasks: %d\n", thread_pool->num_generation_tasks);

//             if (thread_pool->num_generation_tasks != 0) {
//                 chunk->enqueued = false;
//                 printf("Re-enqueueing update task. Queue size before: %d\n", thread_pool->task_queue.size());
//                 this->enqueue_update_task(chunk);
//                 return;
//             }
//         }
//             {
//                 std::lock_guard<std::mutex> lock(chunk->chunk_mutex);
//                 create_chunk_mesh(chunk);
//             }
//             chunk->update_chunk();
//             chunk->enqueued = false; });

//     chunk->enqueued = true;
// }

// void Terrain::enqueue_update_task(Chunk *chunk)
// {
//     if (chunk->enqueued)
//         return;
//     chunk->enqueued = true;
//     thread_pool->enqueue_task([this, chunk]()
//                               {
//         // Wait until there are no generation tasks.
//         while (true)
//         {
//             {
//                 std::unique_lock<std::mutex> lock(thread_pool->num_task_mutex);
//                 if (thread_pool->num_generation_tasks == 0)
//                     break;
//             }
//             // Sleep a little to avoid busy waiting.
//             std::this_thread::sleep_for(std::chrono::milliseconds(5));
//             printf("is sleeping?\n");
//         }

//         {
//             std::lock_guard<std::mutex> lock(chunk->chunk_mutex);
//             create_chunk_mesh(chunk);
//         }
//         chunk->update_chunk();
//         chunk->enqueued = false; printf("finished update task\n"); });
// }

// void Terrain::enqueue_update_task(Chunk *chunk)
// {
//     printf("update!\n");
//     if (chunk->enqueued)
//         return;
//     chunk->enqueued = true;
//     thread_pool->enqueue_task([this, chunk]()
//                               {
//         {
//             // Check if generation tasks are complete.
//             printf("before lock!\n");
//             std::unique_lock<std::mutex> lock(thread_pool->num_task_mutex);
//             printf("after lock!\n");
//             if (thread_pool->num_generation_tasks != 0)
//             {
//                 // Generation tasks are still running. Clear the flag and return.
//                 chunk->enqueued = false;
//                 // Optionally, you can schedule a delayed re-enqueue here.
//                 return;
//             }
//         }

//         {
//             printf("before chunk lock!\n");
//             std::lock_guard<std::mutex> lock(chunk->chunk_mutex);
//             printf("first after chunk lock!\n");
//             create_chunk_mesh(chunk);
//             printf("after chunk lock!\n");
//         }
//         chunk->update_chunk();
//         chunk->enqueued = false;
//         printf("finished update task\n"); });
// }

void Terrain::enqueue_update_task(Chunk *chunk)
{
    // printf("update!\n");
    if (chunk->enqueued)
        return;
    chunk->enqueued = true;
    thread_pool->enqueue_task([this, chunk]()
                              {
        {
            // Check if generation tasks are complete.
            // printf("before lock!\n");
            std::unique_lock<std::mutex> lock(thread_pool->num_task_mutex);
            // printf("after lock!\n");
            if (thread_pool->num_generation_tasks != 0)
            {
                // Generation tasks are still running. Clear the flag.
                chunk->enqueued = false;
                // Schedule a delayed re-enqueue without blocking this worker thread.
                std::thread([this, chunk]() {
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    enqueue_update_task(chunk);
                }).detach();
                return;
            }
        }
        
        {
            // printf("before chunk lock!\n");
            std::lock_guard<std::mutex> lock(chunk->chunk_mutex);
            // printf("first after chunk lock!\n");
            create_chunk_mesh(chunk);
            // printf("after chunk lock!\n");
        }
        chunk->update_chunk();
        chunk->enqueued = false; });
}

void Terrain::enqueue_initial_task(Chunk *chunk)
{
    {
        std::unique_lock<std::mutex> lock(thread_pool->num_task_mutex);
        thread_pool->num_generation_tasks++;
    }
    // issue with queueing too much
    thread_pool->enqueue_task([this, chunk]()
                              {
                                  chunk->initialize_cubes();
                                  chunk->generate_terrain();

                                  // after terrain creation, enqueue chunk update
                                //   chunk->enqueued = false;
                                  {
                                      std::unique_lock<std::mutex> lock(thread_pool->num_task_mutex);
                                      thread_pool->num_generation_tasks--;
                                  }
                                  enqueue_update_task(chunk); });
}
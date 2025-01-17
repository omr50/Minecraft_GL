#include "../../include/Renderer.hpp"
#include <iostream>
#include <filesystem>
#include <chrono>

Renderer::Renderer(Camera *camera) : camera(camera)
{
    terrain = new Terrain(camera);
}

void Renderer::add_block(Cube *blocks)
{
    all_blocks.push_back(blocks);
}

void Renderer::render_blocks()
{
    // // common shader program across all cubes
    // glUseProgram(Cube::shader_program);

    // for (auto &block : all_blocks)
    // {
    //     glm::mat4 view_projection_matrix = camera->get_view_projection_matrix();
    //     glm::mat4 MVP = view_projection_matrix * block->model_matrix;
    //     send_matrix_to_shader(&MVP);
    //     block->draw();
    // }
}

void Renderer::send_matrix_to_shader(glm::mat4 *matrix)
{
    GLuint vp_location = glGetUniformLocation(Cube::shader_program, "vp");
    if (vp_location == -1)
    {
        std::cerr << "Error: Uniform 'mvp' not found!" << std::endl;
        exit(EXIT_FAILURE);
    }
    glUniformMatrix4fv(vp_location, 1, GL_FALSE, glm::value_ptr(*matrix));
}

void Renderer::render_chunks(SDL_Window *window)
{
    // shifting and creating mesh may
    // be optimized with threads later

    // static int curr_iteration = 0;

    bool camera_moved = terrain->camera_moved();
    // don't render when camera doesn't move
    // keep screen as is.
    // printf("got to this point 2\n");
    if (camera_moved)
    {

        terrain->shift_chunks();
        // printf("got to this point 3\n");
        // terrain->create_mesh();
        // printf("got to this point 4\n");
        // view projection matrix is a uniform
        glm::mat4 view_projection_matrix = camera->get_view_projection_matrix();
        // view projection matrix is a uniform
        send_matrix_to_shader(&view_projection_matrix);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for (int i = 0; i < NUM_CHUNKS; i++)
        {
            {
                terrain->chunks[i].draw_chunk();
            }

            // if (in_camera_view(terrain->chunks[i]))
            // terrain->chunks[i].clean_mesh = true;

            // DRAW MUST BE IN MAIN. ALL OPENGL
            // CALLS STAY IN MAIN

            // for (int x = 0; x < X; x++)
            //     for (int y = 0; y < Y; y++)
            //         for (int z = 0; z < Z; z++)
            //         {
            //             glm::mat4 view_projection_matrix = camera->get_view_projection_matrix();
            //             glm::mat4 MVP = view_projection_matrix * terrain->chunks[i].blocks[terrain->chunks[i].get_index(x, y, z)].model_matrix;
            //             send_matrix_to_shader(&MVP);
            //             terrain->chunks[i].blocks[terrain->chunks->get_index(x, y, z)].draw();
        }
        terrain->camera->moved = false;
        SDL_GL_SwapWindow(window);
    }
    {
        std::unique_lock<std::mutex> lock(terrain->thread_pool->task_mutex);
        printf("Queue size %d\n", terrain->thread_pool->task_queue.size());
    }
    // printf("camera moved: %d\n", terrain->camera->moved);
    // printf("ms time for rendering %d\n", std::chrono::duration_cast<std::chrono::milliseconds>(time_end - time_start).count());
}
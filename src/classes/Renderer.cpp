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

    bool camera_moved = terrain->camera_moved();
    // don't render when camera doesn't move
    // keep screen as is.
    static time_t start = clock();
    static time_t end = clock();
    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
    if (camera_moved)
    {
        std::fill(std::begin(terrain->rendered_chunks), std::end(terrain->rendered_chunks), false);
    }

    int num_chunks_rendered = 0;
    for (int i = 0; i < NUM_CHUNKS; i++)
    {
        num_chunks_rendered += (terrain->rendered_chunks[i] ? 1 : 0);
    }
    if (camera_moved || num_chunks_rendered < NUM_CHUNKS)
    {
        start = end;
        terrain->shift_chunks();
        // terrain->create_mesh();
        // printf("got to this point 4\n");
        // view projection matrix is a uniform
        glm::mat4 view_projection_matrix = camera->get_view_projection_matrix() * Cube::create_model_matrix();
        // glm::mat4 view_projection_matrix = camera->get_view_projection_matrix();
        // view projection matrix is a uniform
        send_matrix_to_shader(&view_projection_matrix);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for (int i = 0; i < NUM_CHUNKS; i++)
        {
            {
                terrain->chunks[i].draw_chunk(terrain->rendered_chunks);
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
}
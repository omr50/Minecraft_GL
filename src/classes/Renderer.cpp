#include "../../include/Renderer.hpp"
#include <iostream>
#include <filesystem>
#include <chrono>

Renderer::Renderer(Camera *camera) : camera(camera)
{
    terrain = new Terrain(camera);
    crosshair = new Crosshair();
    lineRenderer = new LineRenderer();
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
    Renderer::set3DState();
    glUseProgram(Cube::shader_program);

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
        glm::mat4 view_projection_matrix = camera->get_view_projection_matrix();
        send_matrix_to_shader(&view_projection_matrix);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for (int i = 0; i < NUM_CHUNKS; i++)
        {
            {
                // glBindVertexArray(terrain->chunks[i].chunk_vao);
                terrain->chunks[i].draw_chunk(terrain->rendered_chunks);
            }
        }
        crosshair->draw_crosshair();
        SDL_GL_SwapWindow(window);
    }
    terrain->camera->moved = false;
}

void Renderer::set3DState()
{
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
}

void Renderer::setUIState()
{
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
}
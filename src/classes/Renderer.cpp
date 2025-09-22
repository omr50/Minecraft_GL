#include "../../include/Renderer.hpp"
#include <iostream>
#include <filesystem>
#include <chrono>
#include <stb/stb_image.h>

Renderer::Renderer(Camera *camera) : camera(camera)
{
    terrain = new Terrain(camera);
    crosshair = new Crosshair();
    lineRenderer = new LineRenderer();
    hud = new HUD();
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
    static clock_t start = clock();
    static clock_t end = clock();
    time_t water_timer = clock();
    static bool water_frame = false;
    double elapsed_ms = (double)(water_timer - start) * 1000.0 / CLOCKS_PER_SEC;
    if (elapsed_ms > 300.0f)
    {
        start = water_timer;
        for (int i = 0; i < NUM_CHUNKS; i++)
        {
            Chunk *chunk = &terrain->chunks[i];
            chunk->frame = !chunk->frame;
            terrain->rendered_chunks[chunk->chunk_num] = false;
            if (!chunk->contains_opaque)
                continue;
            water_frame = !water_frame;
            terrain->camera->moved = true;
            chunk->sent_mesh = false;
            chunk->sent_opaque_mesh = false;
            chunk->sent_non_opaque_mesh = false;
            chunk->rendered = false;
            camera->moved = true;
        }
    }
    if (camera_moved)
    {
        std::fill(std::begin(terrain->rendered_chunks), std::end(terrain->rendered_chunks), false);
        std::fill(std::begin(terrain->rendered_water), std::end(terrain->rendered_water), false);
    }

    int num_chunks_rendered = 0;
    int num_water_rendered = 0;
    for (int i = 0; i < NUM_CHUNKS; i++)
    {
        num_chunks_rendered += (terrain->rendered_chunks[i] ? 1 : 0);
        num_water_rendered += (terrain->rendered_water[i] ? 1 : 0);
    }
    if (camera_moved || num_chunks_rendered < NUM_CHUNKS || num_water_rendered < NUM_CHUNKS)
    {
        // start = end;
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
        for (int i = 0; i < NUM_CHUNKS; i++)
        {
            // glBindVertexArray(terrain->chunks[i].chunk_vao);
            terrain->chunks[i].draw_water(terrain->rendered_water);
        }
        crosshair->draw_crosshair();
        // draw the Hotbar
        hud->draw_hotbar();
        hud->draw_blocks();
        hud->update_selector();
        hud->draw_selector();
        // glDisable(GL_BLEND);
        SDL_GL_SwapWindow(window);
    }
    terrain->camera->moved = false;
}
/*
-------------------------------------------------------------------------------------------
Thinking of basically checking if all 4 around the chunk are rendered to then render water.
Find out how to swap window but the same time "keep" what's on the screen, not really erasing and re-doing?
(am i already doing that, or not, with the chunks being drawn because they aren't done on the same iteration)
-------------------------------------------------------------------------------------------

*/
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
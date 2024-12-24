#include "../../include/Renderer.hpp"
#include <iostream>
#include <filesystem>
#include <chrono>

Renderer::Renderer(Camera *camera) : camera(camera) {}

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
    glUseProgram(Cube::shader_program);
    // shifting and creating mesh may
    // be optimized with threads later

    static int curr_iteration = 0;

    bool camera_moved = terrain.camera_moved();
    // don't render when camera doesn't move
    // keep screen as is.
    if (camera_moved)
    {

        terrain.shift_chunks();
        terrain.create_mesh();
        glm::mat4 view_projection_matrix = camera->get_view_projection_matrix();
        // view projection matrix is a uniform
        send_matrix_to_shader(&view_projection_matrix);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for (int i = curr_iteration; i < NUM_CHUNKS; i++)
        {
            // printf("Does move???\n");
            terrain.chunks[i].update_chunk();
            terrain.chunks[i].buffer_data();
            glBindVertexArray(terrain.chunks[i].chunk_vao);
            // glDrawArraysInstanced(GL_TRIANGLES, 0, 6, terrain.chunks[i].instance_vector.size());

            // always draw, but only draw if
            glDrawArrays(GL_TRIANGLES, 0, terrain.chunks[i].mesh_vertices.size());
            glBindVertexArray(0);

            terrain.chunks[i].clean_mesh = true;

            // for (int x = 0; x < X; x++)
            //     for (int y = 0; y < Y; y++)
            //         for (int z = 0; z < Z; z++)
            //         {
            //             glm::mat4 view_projection_matrix = camera->get_view_projection_matrix();
            //             glm::mat4 MVP = view_projection_matrix * terrain.chunks[i].blocks[terrain.chunks[i].get_index(x, y, z)].model_matrix;
            //             send_matrix_to_shader(&MVP);
            //             terrain.chunks[i].blocks[terrain.chunks->get_index(x, y, z)].draw();
        }
        terrain.camera->moved = false;
        SDL_GL_SwapWindow(window);
    }
    // printf("camera moved: %d\n", terrain.camera->moved);
    // printf("ms time for rendering %d\n", std::chrono::duration_cast<std::chrono::milliseconds>(time_end - time_start).count());
}
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
    // common shader program across all cubes
    glUseProgram(Cube::shader_program);

    for (auto &block : all_blocks)
    {
        glm::mat4 view_projection_matrix = camera->get_view_projection_matrix();
        glm::mat4 MVP = view_projection_matrix * block->model_matrix;
        send_matrix_to_shader(&MVP);
        block->draw();
    }
}

void Renderer::send_matrix_to_shader(glm::mat4 *matrix)
{
    GLuint mvp_location = glGetUniformLocation(Cube::shader_program, "mvp");
    if (mvp_location == -1)
    {
        std::cerr << "Error: Uniform 'mvp' not found!" << std::endl;
        exit(EXIT_FAILURE);
    }
    glUniformMatrix4fv(mvp_location, 1, GL_FALSE, glm::value_ptr(*matrix));
}

void Renderer::render_chunks()
{
    glUseProgram(Cube::shader_program);
    // shifting and creating mesh may
    // be optimized with threads later

    static int curr_iteration = 0;

    auto time_start = std::chrono::high_resolution_clock::now();
    terrain.shift_chunks();
    auto time_end = std::chrono::high_resolution_clock::now();
    printf("ms time for shift %d\n", std::chrono::duration_cast<std::chrono::milliseconds>(time_end - time_start).count());
    time_start = std::chrono::high_resolution_clock::now();
    terrain.create_mesh();
    time_end = std::chrono::high_resolution_clock::now();
    printf("ms time for mesh %d\n", std::chrono::duration_cast<std::chrono::milliseconds>(time_end - time_start).count());
    time_start = std::chrono::high_resolution_clock::now();
    for (int i = curr_iteration; i < NUM_CHUNKS; i++)
    {
        for (int x = 0; x < X; x++)
            for (int y = 0; y < Y; y++)
                for (int z = 0; z < Z; z++)
                {
                    glm::mat4 view_projection_matrix = camera->get_view_projection_matrix();
                    glm::mat4 MVP = view_projection_matrix * terrain.chunks[i].blocks[terrain.chunks[i].get_index(x, y, z)].model_matrix;
                    send_matrix_to_shader(&MVP);
                    terrain.chunks[i].blocks[terrain.chunks->get_index(x, y, z)].draw();
                }
    }
    time_end = std::chrono::high_resolution_clock::now();
    printf("ms time for rendering %d\n", std::chrono::duration_cast<std::chrono::milliseconds>(time_end - time_start).count());
}
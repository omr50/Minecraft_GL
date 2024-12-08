#include "../../include/Renderer.hpp"
#include "../../include/Blocks/DirtBlock.hpp"
#include <iostream>
#include <filesystem>

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

void Renderer::create_test_dirt_blocks(int num)
{

    Cube::setup_vbo_vao_shaders();
    for (int i = 0; i < num; i++)
    {

        std::cout << "Current Working Directory: " << std::filesystem::current_path() << std::endl;
        all_blocks.push_back(new DirtBlock(0, i, "dirt_bottom.png", "dirt_side.png", "dirt_side.png"));
    }
}
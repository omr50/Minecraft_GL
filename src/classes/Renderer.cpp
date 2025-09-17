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
    hotbarShader = new Shader("../shaders/hotbarShader.vs", "../shaders/hotbarShader.fs");
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
    // double elapsed_ms = (double)(water_timer - start) * 1000.0 / CLOCKS_PER_SEC;
    // if (elapsed_ms > 300.0f)
    // {
    //     printf("time elapsed\n");
    //     start = water_timer;
    //     for (int i = 0; i < NUM_CHUNKS; i++)
    //     {
    //         Chunk *chunk = &terrain->chunks[i];
    //         if (!chunk->contains_water)
    //             continue;
    //         chunk->update_water_blocks(water_frame);
    //         chunk->needs_remesh();
    //         // terrain->enqueue_update_task(&terrain->chunks[j]);
    //         {
    //             std::lock_guard<std::mutex> lock(chunk->chunk_mutex);
    //             terrain->create_chunk_mesh(chunk);
    //             // printf("after chunk lock!\n");
    //         }
    //         chunk->update_chunk();
    //         water_frame = !water_frame;
    //         terrain->camera->moved = true;
    //     }
    // }
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
        crosshair->draw_crosshair();
        // draw the Hotbar
        draw_hotbar();
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

void Renderer::init_hotbar()
{
    // --- load texture ---
    int width, height, channels;
    unsigned char *image = stbi_load("../textures/hotbar1.png", &width, &height, &channels, 0);
    if (!image)
    {
        std::cerr << "Failed to load hotbar texture\n";
        return;
    }

    glGenTextures(1, &hotbar_texture);
    glBindTexture(GL_TEXTURE_2D, hotbar_texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(image);

    // --- setup quad geometry ---
    float hotbarVertices[] = {
        // pos       // tex
        -0.5f, -0.9f, 0.0f, 1.0f, // bottom left
        0.5f, -0.9f, 1.0f, 1.0f,  // bottom right
        0.5f, -0.7f, 1.0f, 0.0f,  // top right
        -0.5f, -0.7f, 0.0f, 0.0f  // top left
    };

    unsigned int indices[] = {0, 1, 2, 2, 3, 0};

    glGenVertexArrays(1, &hotbarVAO);
    glGenBuffers(1, &hotbarVBO);
    GLuint EBO;
    glGenBuffers(1, &EBO);

    glBindVertexArray(hotbarVAO);

    glBindBuffer(GL_ARRAY_BUFFER, hotbarVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(hotbarVertices), hotbarVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // layout (location=0) -> vec2 position
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // layout (location=1) -> vec2 texcoords
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void Renderer::draw_hotbar()
{
    glUseProgram(hotbarShader->shader_program);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hotbar_texture);
    glUniform1i(glGetUniformLocation(hotbarShader->shader_program, "uTexture"), 0);

    glBindVertexArray(hotbarVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

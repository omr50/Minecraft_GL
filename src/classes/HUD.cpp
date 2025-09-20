#include "../../include/HUD.hpp"
#include <iostream>
#include <filesystem>
#include <chrono>
#include <stb/stb_image.h>

HUD::HUD()
{
    hotbarShader = new Shader("../shaders/hotbarShader.vs", "../shaders/hotbarShader.fs");
    blockShader = new Shader("../shaders/hotbarShader.vs", "../shaders/hotbarShader.fs");

    add_block(0, "../textures/grass_block.png");
    add_block(1, "../textures/grass_block.png");
}

void HUD::add_block(int index, std::string block_img_path)
{
    // --- load texture ---
    int width, height, channels;
    unsigned char *image = stbi_load(block_img_path.c_str(), &width, &height, &channels, 0);
    if (!image)
    {
        std::cerr << "Failed to load block texture\n";
        return;
    }
    auto &block = block_img[index];
    glGenTextures(1, &block.block_texture);
    glBindTexture(GL_TEXTURE_2D, block.block_texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(image);

    Quad q = quad_helper(index);

    float verts[] = {
        // pos         // uv
        q.x0, q.y0, 0.0f, 1.0f, // bottom-left
        q.x1, q.y0, 1.0f, 1.0f, // bottom-right
        q.x1, q.y1, 1.0f, 0.0f, // top-right
        q.x0, q.y1, 0.0f, 0.0f  // top-left
    };
    unsigned int idx[] = {0, 1, 2, 2, 3, 0};

    Block &b = block_img[index];

    glGenVertexArrays(1, &b.blockVAO);
    glGenBuffers(1, &b.blockVBO);
    GLuint ebo;
    glGenBuffers(1, &ebo); // or store in struct

    glBindVertexArray(b.blockVAO);

    glBindBuffer(GL_ARRAY_BUFFER, b.blockVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(idx), idx, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}
void HUD::init_hotbar()
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
        0.5f, -0.72f, 1.0f, 0.0f, // top right
        -0.5f, -0.72f, 0.0f, 0.0f // top left
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

void HUD::draw_hotbar()
{
    glUseProgram(hotbarShader->shader_program);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hotbar_texture);
    glUniform1i(glGetUniformLocation(hotbarShader->shader_program, "uTexture"), 0);

    glBindVertexArray(hotbarVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void HUD::draw_blocks()
{
    glUseProgram(blockShader->shader_program);
    for (int i = 0; i < 2; i++)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, block_img[i].block_texture);
        glUniform1i(glGetUniformLocation(blockShader->shader_program, "uTexture"), 0);

        glBindVertexArray(block_img[i].blockVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
}

Quad HUD::quad_helper(int k)
{
    const float left = -0.5f;
    const float right = 0.5f;
    const float bottom = -0.9f;
    const float top = -0.7f;

    const float width = right - left;
    const float slotW = width / 9.0f;
    const float padX = 0.005f;
    const float padY = 0.008f;

    const float x0 = left + k * slotW + padX;
    const float x1 = left + (k + 1) * slotW - padX;
    const float y0 = bottom + padY;
    const float y1 = top - padY;

    return {x0, y0, x1, y1};
}

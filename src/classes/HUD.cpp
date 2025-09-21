#include "../../include/HUD.hpp"
#include <iostream>
#include <filesystem>
#include <chrono>
#include <stb/stb_image.h>

HUD::HUD()
{
    hotbarShader = new Shader("../shaders/hotbarShader.vs", "../shaders/hotbarShader.fs");
    blockShader = new Shader("../shaders/hotbarShader.vs", "../shaders/hotbarShader.fs");
    selectorShader = new Shader("../shaders/selectorShader.vs", "../shaders/selectorShader.fs");
    selector_slot_index = 0;

    add_block(0, "../textures/grass_block.png");
    add_block(1, "../textures/stone_block.png");
    add_block(2, "../textures/dirt_block.png");
    add_block(3, "../textures/cobblestone_block.png");
    add_block(4, "../textures/woodenplank_block.png");
    add_block(5, "../textures/log_block.png");
    add_block(6, "../textures/brick_block.png");
    add_block(7, "../textures/glass_block.png");
    add_block(8, "../textures/obsidian_block.png");

    // initialize the selector
    glGenVertexArrays(1, &selectorVAO);
    glGenBuffers(1, &selectorVBO);
    glBindVertexArray(selectorVAO);
    glBindBuffer(GL_ARRAY_BUFFER, selectorVBO);
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), nullptr, GL_DYNAMIC_DRAW); // 4 vec2 corners
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}

void HUD::add_block(int index, std::string block_img_path)
{
    // --- load texture ---
    int width, height, channels;
    unsigned char *image = stbi_load(block_img_path.c_str(), &width, &height, &channels, STBI_rgb_alpha);
    if (!image)
    {
        std::cerr << "Failed to load block texture\n";
        return;
    }
    auto &block = block_img[index];
    glGenTextures(1, &block.block_texture);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glBindTexture(GL_TEXTURE_2D, block.block_texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // safe for any row stride
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, image);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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
        0.5f, -0.74f, 1.0f, 0.0f, // top right
        -0.5f, -0.74f, 0.0f, 0.0f // top left
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

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

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
    // glDisable(GL_DEPTH_TEST);
    // glEnable(GL_BLEND);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glUseProgram(blockShader->shader_program);
    for (int i = 0; i < 9; i++)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, block_img[i].block_texture);
        glUniform1i(glGetUniformLocation(blockShader->shader_program, "uTexture"), 0);

        glBindVertexArray(block_img[i].blockVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
    // glEnable(GL_DEPTH_TEST);
    // glDisable(GL_BLEND);
}

Quad HUD::quad_helper(int k)
{
    const float left = -0.5f;
    const float right = 0.5f;
    const float bottom = -0.9f;
    const float top = -0.74f;

    const float width = right - left;
    const float slotW = width / 9.0f;
    const float padX = 0.012f;
    const float padY = 0.022f;

    const float x0 = left + k * slotW + padX;
    const float x1 = left + (k + 1) * slotW - padX;
    const float y0 = bottom + padY;
    const float y1 = top - padY;

    return {x0, y0, x1, y1};
}

Quad HUD::selector_slot_quad(int k)
{
    const float left = -0.5f, right = 0.5f, bottom = -0.9f, top = -0.74f;
    const float slotW = (right - left) / 9.0f;

    const float padX = 0.012f;
    const float padY = 0.022f;

    float x0 = left + k * slotW + padX;
    float x1 = left + (k + 1) * slotW - padX;
    float y0 = bottom + padY;
    float y1 = top - padY;
    return {x0, y0, x1, y1};
}

void HUD::update_selector()
{
    float padding_y = 0.05;
    Quad q = selector_slot_quad(selector_slot_index);
    float verts[8] = {
        q.x0, q.y0, q.x1, q.y0, q.x1, q.y1, q.x0, q.y1};
    glBindBuffer(GL_ARRAY_BUFFER, selectorVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verts), verts);
}

void HUD::draw_selector()
{
    // glDisable(GL_DEPTH_TEST);
    // glEnable(GL_BLEND);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUseProgram(selectorShader->shader_program);
    GLint colorLoc = glGetUniformLocation(selectorShader->shader_program, "uColor");
    glUniform4f(colorLoc, 1.0f, 1.0f, 1.0f, 1.0f);

    glLineWidth(4.0f);
    glBindVertexArray(selectorVAO);
    glDrawArrays(GL_LINE_LOOP, 0, 4);
    glBindVertexArray(0);

    // glDisable(GL_BLEND);
    // glEnable(GL_DEPTH_TEST);
}

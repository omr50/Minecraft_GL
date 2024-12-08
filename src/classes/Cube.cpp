#include "../../include/Cube.hpp"
#include "../../include/Shader.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <iostream>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

GLuint Cube::face_vaos[6] = {0}, Cube::face_vbos[6] = {0};
GLuint Cube::cube_vbo = 0, Cube::cube_vao = 0, Cube::texture_id = 0;
GLuint Cube::shader_program;

float Cube::front_face_vertices[30] = { // Front face
    -0.5f, -0.5f, +0.5f, 0.0f, 0.0f,
    +0.5f, -0.5f, +0.5f, 1.0f, 0.0f,
    +0.5f, +0.5f, +0.5f, 1.0f, 1.0f,
    +0.5f, +0.5f, +0.5f, 1.0f, 1.0f,
    -0.5f, +0.5f, +0.5f, 0.0f, 1.0f,
    -0.5f, -0.5f, +0.5f, 0.0f, 0.0f};

float Cube::back_face_vertices[30] = {
    // Back face
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
    -0.5f, +0.5f, -0.5f, 0.0f, 1.0f,
    +0.5f, +0.5f, -0.5f, 1.0f, 1.0f,
    +0.5f, +0.5f, -0.5f, 1.0f, 1.0f,
    +0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f};

float Cube::left_face_vertices[30] = {
    // Left face
    -0.5f, +0.5f, +0.5f, 1.0f, 0.0f,
    -0.5f, +0.5f, -0.5f, 1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
    -0.5f, -0.5f, +0.5f, 0.0f, 0.0f,
    -0.5f, +0.5f, +0.5f, 1.0f, 0.0f};

float Cube::right_face_vertices[30] = {
    // Right face
    +0.5f, +0.5f, +0.5f, 1.0f, 0.0f,
    +0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
    +0.5f, +0.5f, -0.5f, 1.0f, 1.0f,
    +0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
    +0.5f, +0.5f, +0.5f, 1.0f, 0.0f,
    +0.5f, -0.5f, +0.5f, 0.0f, 0.0f};

float Cube::bottom_face_vertices[30] = {
    // Bottom face
    -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
    +0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
    +0.5f, -0.5f, +0.5f, 1.0f, 0.0f,
    +0.5f, -0.5f, +0.5f, 1.0f, 0.0f,
    -0.5f, -0.5f, +0.5f, 0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, 1.0f};

float Cube::top_face_vertices[30] = {
    // Top face
    -0.5f, +0.5f, -0.5f, 0.0f, 0.0f,
    -0.5f, +0.5f, +0.5f, 0.0f, 1.0f,
    +0.5f, +0.5f, +0.5f, 1.0f, 1.0f,
    +0.5f, +0.5f, +0.5f, 1.0f, 1.0f,
    +0.5f, +0.5f, -0.5f, 1.0f, 0.0f,
    -0.5f, +0.5f, -0.5f, 0.0f, 0.0f};

float *Cube::faces[6] = {
    Cube::front_face_vertices,
    Cube::back_face_vertices,
    Cube::left_face_vertices,
    Cube::right_face_vertices,
    Cube::bottom_face_vertices,
    Cube::top_face_vertices};

void Cube::setup_vbo_vao_shaders()
{
    Shader shader = Shader("../shaders/basicShader.vs", "../shaders/basicShader.fs");
    Cube::shader_program = shader.shader_program;
    glGenVertexArrays(6, face_vaos);
    glGenBuffers(6, face_vbos);
    for (int i = 0; i < 6; i++)
    {

        glBindVertexArray(face_vaos[i]);
        glBindBuffer(GL_ARRAY_BUFFER, face_vbos[i]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 30, faces[i], GL_STATIC_DRAW);

        // Position attribute
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);

        // Texture attribute
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));

        glBindVertexArray(0);
    }
}

void Cube::add_textures(std::string top_filename, std::string bottom_filename, std::string sides_filename)
{
    unsigned char *image;
    int width, height, channels;
    const char *files[] = {top_filename.c_str(), bottom_filename.c_str(), sides_filename.c_str()};

    for (int i = 0; i < 3; i++)
    {
        image = stbi_load(files[i], &width, &height, &channels, 0);
        if (!image)
        {
            std::cerr << "Failed to load texture " << files[i] << std::endl;
        }

        glGenTextures(1, &face_textures[i]);
        glBindTexture(GL_TEXTURE_2D, face_textures[i]);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        GLenum format = channels == 4 ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, image);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(image);
    }
}

void Cube::create_model_matrix()
{
    model_matrix = glm::translate(glm::mat4(1.0f), glm::vec3((float)x, (float)y, -1.0f));
}
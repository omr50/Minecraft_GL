#include "../../include/Cube.hpp"
#include "../../include/Shader.hpp"

float Cube::front_face_vertices[30] = { // Front face
    -0.5f,
    -0.5f, +0.5f, 0.0f, 0.0f,
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
#pragma once
#include "../include/Renderable.hpp"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>

class Cube : public Renderable
{
public:
    static GLuint face_vaos[6], face_vbos[6];
    static GLuint cube_vbo, cube_vao, texture_id;
    static GLuint shader_program;
    static float front_face_vertices[30];
    static float back_face_vertices[30];
    static float left_face_vertices[30];
    static float right_face_vertices[30];
    static float bottom_face_vertices[30];
    static float top_face_vertices[30];
    static float *faces[6];
    int x;
    int y;

    static void setup_vbo_vao_shaders();

    // non static members
    glm::mat4 model_matrix;
    GLuint face_textures[3];
    // all 4 sides use same texture

    void add_textures(std::string top_filename, std::string bottom_filename, std::string sides_filename);
    void create_model_matrix();
    // even better than the images have the loaded textures instead (using stb_image)
    // const char* texture_files[6] = {
    // "front.png", "back.png", "left.png", "right.png", "bottom.png", "top.png"
    // };
};
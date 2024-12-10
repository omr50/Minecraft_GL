#pragma once
#include "../include/Renderable.hpp"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <unordered_map>
#include <array>

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
    // maps the block type to the
    static std::unordered_map<std::string, std::array<GLuint, 3>> texture_map;
    int x;
    int y;
    int z;

    static void setup_vbo_vao_shaders();
    static void add_textures(std::string block_type, std::string top_filename, std::string bottom_filename, std::string sides_filename);
    static void add_all_block_textures();

    // non static members
    glm::mat4 model_matrix;
    std::string block_type;
    bool renderable_face[6];
    // GLuint face_textures[3];

    // all 4 sides use same texture

    Cube();
    Cube(int x, int y, int z, std::string block_type);
    void create_model_matrix();
    void draw() override;
    void update_state() override;
    void update_cube_state(int x, int y, int z, std::string block_type);
    // even better than the images have the loaded textures instead (using stb_image)
    // const char* texture_files[6] = {
    // "front.png", "back.png", "left.png", "right.png", "bottom.png", "top.png"
    // };
};
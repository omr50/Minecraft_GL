#pragma once
#include "../include/Updatable.hpp"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <unordered_map>
#include <array>

#define GRID_WIDTH 16
#define SCALE 1

struct FaceUV
{
    glm::vec2 offset;
};

class Cube : public Updatable
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
    static GLuint texture_atlas;
    // maps the block type to the
    static std::unordered_map<std::string, std::array<FaceUV, 3>> texture_map;
    float x;
    float y;
    float z;

    // static void add_textures(std::string block_type, std::string top_filename, std::string bottom_filename, std::string sides_filename);
    static void setup_cube_shaders();
    static void add_texture(std::string texture_filename);
    static void initialize_texture_map(std::string texture_atlas_filename);
    static void add_block_to_map(std::string block_type, FaceUV offset_top, FaceUV offset_side, FaceUV offset_bottom);
    // static void add_all_block_textures();

    // non static members
    glm::mat4 model_matrix;
    std::string block_type;
    bool renderable_face[6];
    // GLuint face_textures[3];

    // all 4 sides use same texture

    Cube();
    Cube(int x, int y, int z, std::string block_type);
    static glm::mat4 create_model_matrix();
    void update_state(float x, float y, float z, std::string block_type) override;
    // void update_cube_state(int x, int y, int z, std::string block_type);
    // even better than the images have the loaded textures instead (using stb_image)
    // const char* texture_files[6] = {
    // "front.png", "back.png", "left.png", "right.png", "bottom.png", "top.png"
    // };
};
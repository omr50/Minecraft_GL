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
GLuint Cube::texture_atlas;
std::unordered_map<std::string, std::array<FaceUV, 3>> Cube::texture_map;

float Cube::front_face_vertices[30] = { // Front face
    -0.5f, -0.5f, +0.5f, 0.0f, 1.0f,
    +0.5f, -0.5f, +0.5f, 1.0f, 1.0f,
    +0.5f, +0.5f, +0.5f, 1.0f, 0.0f,
    +0.5f, +0.5f, +0.5f, 1.0f, 0.0f,
    -0.5f, +0.5f, +0.5f, 0.0f, 0.0f,
    -0.5f, -0.5f, +0.5f, 0.0f, 1.0f};

float Cube::back_face_vertices[30] = {
    // Back face
    -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, // Bottom-left
    +0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // Bottom-right
    +0.5f, +0.5f, -0.5f, 0.0f, 0.0f, // Top-right
    +0.5f, +0.5f, -0.5f, 0.0f, 0.0f, // Top-right (reused)
    -0.5f, +0.5f, -0.5f, 1.0f, 0.0f, // Top-left
    -0.5f, -0.5f, -0.5f, 1.0f, 1.0f  // Bottom-left (reused)

};

float Cube::left_face_vertices[30] = {
    // Left face
    -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, // Bottom-left
    -0.5f, -0.5f, +0.5f, 0.0f, 1.0f, // Bottom-right
    -0.5f, +0.5f, +0.5f, 0.0f, 0.0f, // Top-right
    -0.5f, +0.5f, +0.5f, 0.0f, 0.0f, // Top-right (reused)
    -0.5f, +0.5f, -0.5f, 1.0f, 0.0f, // Top-left
    -0.5f, -0.5f, -0.5f, 1.0f, 1.0f  // Bottom-left (reused)
};

float Cube::right_face_vertices[30] = {
    // Right face
    +0.5f, -0.5f, +0.5f, 1.0f, 1.0f, // Bottom-left
    +0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // Bottom-right
    +0.5f, +0.5f, -0.5f, 0.0f, 0.0f, // Top-right
    +0.5f, +0.5f, -0.5f, 0.0f, 0.0f, // Top-right (reused)
    +0.5f, +0.5f, +0.5f, 1.0f, 0.0f, // Top-left
    +0.5f, -0.5f, +0.5f, 1.0f, 1.0f  // Bottom-left (reused)
};

float Cube::bottom_face_vertices[30] = {
    // Bottom face
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, // Bottom-left
    +0.5f, -0.5f, -0.5f, 1.0f, 0.0f, // Bottom-right
    +0.5f, -0.5f, +0.5f, 1.0f, 1.0f, // Top-right
    +0.5f, -0.5f, +0.5f, 1.0f, 1.0f, // Top-right (reused)
    -0.5f, -0.5f, +0.5f, 0.0f, 1.0f, // Top-left
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f  // Bottom-left (reused)
};

float Cube::top_face_vertices[30] = {
    // Top face
    -0.5f, +0.5f, -0.5f, 0.0f, 1.0f, // Bottom-left
    +0.5f, +0.5f, -0.5f, 1.0f, 1.0f, // Bottom-right
    +0.5f, +0.5f, +0.5f, 1.0f, 0.0f, // Top-right
    +0.5f, +0.5f, +0.5f, 1.0f, 0.0f, // Top-right (reused)
    -0.5f, +0.5f, +0.5f, 0.0f, 0.0f, // Top-left
    -0.5f, +0.5f, -0.5f, 0.0f, 1.0f  // Bottom-left (reused)
};

float *Cube::faces[6] = {
    Cube::front_face_vertices,
    Cube::back_face_vertices,
    Cube::left_face_vertices,
    Cube::right_face_vertices,
    Cube::bottom_face_vertices,
    Cube::top_face_vertices};

void Cube::setup_cube_shaders()
{
    Shader shader = Shader("../shaders/basicShader.vs", "../shaders/basicShader.fs");
    Cube::shader_program = shader.shader_program;
}

Cube::Cube() {}

Cube::Cube(int x, int y, int z, std::string block_type) : x((float)x), y((float)y), z((float)z), block_type(block_type)
{
    create_model_matrix();
}
void Cube::add_texture(std::string texture_filename)
{
    unsigned char *image;
    int width, height, channels;
    // stbi_set_flip_vertically_on_load(true);
    image = stbi_load(texture_filename.c_str(), &width, &height, &channels, 0);
    if (!image)
    {
        std::cerr << "Failed to load texture " << texture_filename << std::endl;
    }

    glGenTextures(1, &Cube::texture_atlas);
    glBindTexture(GL_TEXTURE_2D, Cube::texture_atlas);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLenum format = channels == 4 ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(image);
}
void Cube::initialize_texture_map(std::string texture_atlas_filename)
{
    add_texture(texture_atlas_filename);
    // bunch of add_block_to_map calls for different blocks that will be done only once
    float texture_square_size = 1.0f / (float)GRID_WIDTH;

    // cubes with the same texture for all faces
    FaceUV stone_offset = {{1.0f * texture_square_size, 0.0f}};
    FaceUV dirt_offset = {{2.0f * texture_square_size, 0.0f}};
    FaceUV sand_offset = {{2.0f * texture_square_size, 1.0f * texture_square_size}};
    FaceUV cobble_stone_offset = {{0.0f, 1.0f * texture_square_size}};
    FaceUV wooden_plank_offset = {{4.0f * texture_square_size, 0.0f}};

    FaceUV grass_top_offset = {{0.0f, 0.0f * texture_square_size}};
    FaceUV grass_bottom_offset = dirt_offset;
    FaceUV grass_side_offset = {{3.0f * texture_square_size, 0.0f}};

    add_block_to_map("stone", stone_offset, stone_offset, stone_offset);
    add_block_to_map("dirt", dirt_offset, dirt_offset, dirt_offset);
    add_block_to_map("sand", sand_offset, sand_offset, sand_offset);
    add_block_to_map("cobble_stone", cobble_stone_offset, cobble_stone_offset, cobble_stone_offset);
    add_block_to_map("wooden_plank", wooden_plank_offset, wooden_plank_offset, wooden_plank_offset);

    add_block_to_map("grass", grass_top_offset, grass_side_offset, grass_bottom_offset);
}
void Cube::add_block_to_map(std::string block_type, FaceUV offset_top, FaceUV offset_side, FaceUV offset_bottom)
{
    std::array<FaceUV, 3> face_texture_offsets = std::array<FaceUV, 3>({offset_top, offset_side, offset_bottom});
    Cube::texture_map[block_type] = face_texture_offsets;
}

glm::mat4 Cube::create_model_matrix()
{
    // float scale_factor = 3.0f;
    glm::mat4 model_matrix = glm::scale(glm::mat4(1.0f), glm::vec3((float)SCALE));
    return model_matrix;
}

void Cube::update_state(float x, float y, float z, std::string block_type)
{
    this->x = x;
    this->y = y;
    this->z = z;
    this->block_type = block_type;
    create_model_matrix();
    for (int i = 0; i < 6; i++)
    {
        this->renderable_face[i] = true;
    }
}
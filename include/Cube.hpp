#include "../include/Renderable.hpp"
#include <GL/glew.h>
#include <glm/glm.hpp>

class Cube
{
public:
    static float cube_vertices[180];
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

    static void setup_vbo_vao_shaders();

    // non static members
    glm::mat4 model_matrix;
    GLuint face_textures[6];
    // even better than the images have the loaded textures instead (using stb_image)
    // const char* texture_files[6] = {
    // "front.png", "back.png", "left.png", "right.png", "bottom.png", "top.png"
    // };
};
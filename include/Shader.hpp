#pragma once
#include <string>
#include <GL/glew.h>

class Shader
{

public:
    std::string vertex_shader_filename;
    std::string fragment_shader_filename;
    std::string vertex_shader_src_string;
    std::string fragment_shader_src_string;

    GLuint vertex_shader;
    GLuint fragment_shader;
    GLuint shader_program;

    Shader(std::string vertex_shader_filename, std::string fragment_shader_filename);

    void load_shader_src(GLenum shader_type);
    void compile_shader(GLenum shader_type);
    void attach_and_link_shaders();
    void create_shader(GLenum shader_type);
    void create_program();
    void shader_error_detection(GLenum shader_type, GLenum err_type, bool is_program);
    void set_src_string(GLenum shader_type, std::string value);
    void set_shader(GLenum shader_type, GLuint value);
    std::string get_shader_src_string(GLenum shader_type);
    std::string get_shader_filename(GLenum shader_type);
    GLuint get_shader(GLenum shader_type);
};
#include "../../include/Shader.hpp"
#include <fstream>
#include <iostream>

Shader::Shader(std::string vrtx_src_str, std::string frag_src_str) : vertex_shader_filename(vrtx_src_str), fragment_shader_filename(frag_src_str)
{
    create_program();
}
void Shader::load_shader_src(GLenum shader_type)
{
    std::string shader_filename = get_shader_filename(shader_type);

    std::ifstream shader_file = std::ifstream(shader_filename);

    std::string src_string, line;

    if (!shader_file.is_open())
    {
        std::cerr << "Shader file " << shader_filename << " does not exist!" << std::endl;
    }

    while (!shader_file.eof())
    {
        std::getline(shader_file, line);
        src_string += line + '\n';
    }
    set_src_string(shader_type, src_string);
}

void Shader::create_shader(GLenum shader_type)
{
    printf("shader before: %d\n", vertex_shader);
    set_shader(shader_type, glCreateShader(shader_type));
    printf("shader after: %d\n", vertex_shader);
    load_shader_src(shader_type);
    std::string src_str = get_shader_src_string(shader_type);
    const GLchar *shader_src_str = src_str.c_str();
    GLint shader_length = src_str.length();
    GLuint shader = get_shader(shader_type);
    glShaderSource(shader, 1, &shader_src_str, &shader_length);
    printf("shader src\n");
    compile_shader(shader_type);
}

void Shader::compile_shader(GLenum shader_type)
{
    printf("Compiling?\n");
    GLuint shader = get_shader(shader_type);
    glCompileShader(shader);
    shader_error_detection(shader_type, GL_COMPILE_STATUS, false);
}

void Shader::attach_and_link_shaders()
{
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glBindAttribLocation(shader_program, 0, "position");
    glBindAttribLocation(shader_program, 1, "aTexCoord");
    printf("linking here?\n");
    glLinkProgram(shader_program);
    shader_error_detection(0, GL_LINK_STATUS, true);
}

void Shader::create_program()
{
    shader_program = glCreateProgram();
    create_shader(GL_VERTEX_SHADER);
    create_shader(GL_FRAGMENT_SHADER);
    attach_and_link_shaders();
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
}

void Shader::shader_error_detection(GLenum shader_type, GLenum err_type, bool is_program)
{
    GLint success;

    if (is_program)
    {
        glGetProgramiv(shader_program, err_type, &success);
        if (!success)
        {
            GLchar infoLog[512];
            glGetProgramInfoLog(shader_program, 512, NULL, infoLog);
            std::cerr << "Program error: " << infoLog << std::endl;
        }
        else
        {
            std::cout << "Shader program created successfully!" << std::endl;
        }
    }
    else
    {
        glGetShaderiv(get_shader(shader_type), err_type, &success);
        if (!success)
        {
            GLchar infoLog[512];
            glGetShaderInfoLog(get_shader(shader_type), 512, NULL, infoLog);
            std::cerr << "Shader error: " << infoLog << std::endl;
        }
        else
        {
            std::cout << "Shader program linked successfully!" << std::endl;
        }
    }
}

void Shader::set_src_string(GLenum shader_type, std::string value)
{
    if (shader_type == GL_VERTEX_SHADER)
        vertex_shader_src_string = value;
    else if (shader_type == GL_FRAGMENT_SHADER)
        fragment_shader_src_string = value;
}
void Shader::set_shader(GLenum shader_type, GLuint value)
{
    if (shader_type == GL_VERTEX_SHADER)
        vertex_shader = value;
    else if (shader_type == GL_FRAGMENT_SHADER)
        fragment_shader = value;
}
std::string Shader::get_shader_src_string(GLenum shader_type)
{
    if (shader_type == GL_VERTEX_SHADER)
        return vertex_shader_src_string;

    else if (shader_type == GL_FRAGMENT_SHADER)
        return fragment_shader_src_string;
}
GLuint Shader::get_shader(GLenum shader_type)
{
    if (shader_type == GL_VERTEX_SHADER)
        return vertex_shader;
    else if (shader_type == GL_FRAGMENT_SHADER)
        return fragment_shader;
    printf("unexpected shader type!\n");
    return 0;
}

std::string Shader::get_shader_filename(GLenum shader_type)
{
    if (shader_type == GL_VERTEX_SHADER)
        return vertex_shader_filename;
    else if (shader_type == GL_FRAGMENT_SHADER)
        return fragment_shader_filename;
}
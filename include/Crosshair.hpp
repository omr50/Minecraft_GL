#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>
#include "./Shader.hpp"

class Crosshair
{

public:
    const float L = 0.04f;
    float verts[8] = {
        -L, 0.0f, L, 0.0f, // horizontal
        0.0f, -L, 0.0f, L  // vertical
    };
    GLuint crosshairVAO, crosshairVBO;

    Shader shader = Shader("../shaders/crosshairShader.vs", "../shaders/crosshairShader.fs");

    Crosshair();
    void setup_crosshair();
    void draw_crosshair();
};
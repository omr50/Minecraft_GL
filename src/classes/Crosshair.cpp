#include "../../include/Crosshair.hpp"
#include "../../include/Renderer.hpp"

Crosshair::Crosshair()
{
    setup_crosshair();
}

void Crosshair::setup_crosshair()
{
    glGenVertexArrays(1, &crosshairVAO);
    glGenBuffers(1, &crosshairVBO);

    glBindVertexArray(crosshairVAO);
    glBindBuffer(GL_ARRAY_BUFFER, crosshairVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);

    glBindVertexArray(0);
}

void Crosshair::draw_crosshair()
{
    Renderer::setUIState();
    glUseProgram(shader.shader_program);
    glUniform4f(glGetUniformLocation(shader.shader_program, "uColor"), 1.0f, 1.0f, 1.0f, 0.9f);
    glBindVertexArray(crosshairVAO);
    glLineWidth(1.0f); // >1.0 is poorly supported; for thicker, use quads
    glDrawArrays(GL_LINES, 0, 4);
    glBindVertexArray(0);

    // glDisable(GL_BLEND);
}
#include "../../include/LineRenderer.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
std::vector<std::pair<glm::vec3, glm::vec3>> LineRenderer::points{};

LineRenderer::LineRenderer()
{
    shader = new Shader("../shaders/lineRenderer.vs", "../shaders/lineRenderer.fs");
    gLineProg = shader->shader_program;
    initLineRenderer();
}

void LineRenderer::initLineRenderer()
{
    // VAO/VBO
    glGenVertexArrays(1, &gLineVAO);
    glGenBuffers(1, &gLineVBO);
    glBindVertexArray(gLineVAO);
    glBindBuffer(GL_ARRAY_BUFFER, gLineVBO);
    glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(glm::vec3), nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)0);
    glBindVertexArray(0);

    gVPu = glGetUniformLocation(gLineProg, "uVP");
    gColoru = glGetUniformLocation(gLineProg, "uColor");
}

void LineRenderer::drawRayLine(glm::vec3 vert1, glm::vec3 vert2, glm::mat4 *VP)
{
    glm::vec3 verts[2] = {vert1, vert2};
    glBindBuffer(GL_ARRAY_BUFFER, gLineVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verts), verts);

    // Draw
    glUseProgram(gLineProg);
    glUniformMatrix4fv(gVPu, 1, GL_FALSE, glm::value_ptr(*VP));
    glUniform4f(gColoru, 1.0f, 0.1f, 0.1f, 1.0f); // red-ish
    glBindVertexArray(gLineVAO);
    glLineWidth(1.0f); // >1.0 may not work cross-platform
    glDrawArrays(GL_LINES, 0, 2);
    glBindVertexArray(0);
}

void LineRenderer::drawAllRays(glm::mat4 *VP)
{
    for (int i = 0; i < points.size(); i++)
    {
        drawRayLine(points[i].first, points[i].second, VP);
    }
}
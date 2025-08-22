#pragma once
#include <GL/glew.h>
#include "./Shader.hpp"
#include <glm/glm.hpp>
#include <vector>

class LineRenderer
{
public:
    GLuint gLineVAO = 0, gLineVBO = 0, gLineProg = 0;
    GLint gVPu = -1, gColoru = -1;
    Shader *shader;
    static std::vector<std::pair<glm::vec3, glm::vec3>> points;

    LineRenderer();
    void initLineRenderer();
    void drawRayLine(glm::vec3 v1, glm::vec3 v2, glm::mat4 *vp);
    void drawAllRays(glm::mat4 *vp);
    // void drawGeneralLine();
};
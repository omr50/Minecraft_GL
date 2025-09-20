#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include "./Cube.hpp"
#include "./Camera.hpp"
#include "./Chunk.hpp"
#include "./Terrain.hpp"
#include "./Crosshair.hpp"
#include "./LineRenderer.hpp"
#include "./Shader.hpp"
#include <SDL2/SDL.h>

class HUD
{

public:
    //  may need to change to another data structure
    // or later on have another class made for doing the
    // intense calculations then passing final renderables
    // to this rendererer class.
    GLuint hotbar_texture;
    GLuint block_texture;
    GLuint hotbarVAO, hotbarVBO;
    GLuint blockVAO, blockVBO;
    Shader *hotbarShader;
    Shader *blockShader;

    HUD();

    void init_hotbar();
    void draw_hotbar();
};
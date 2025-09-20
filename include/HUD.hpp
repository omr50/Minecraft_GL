#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include "./Cube.hpp"
#include "./Chunk.hpp"
#include "./Terrain.hpp"
#include "./Crosshair.hpp"
#include "./LineRenderer.hpp"
#include "./Shader.hpp"
#include <SDL2/SDL.h>

struct Block
{
    GLuint blockVAO;
    GLuint blockVBO;
    GLuint block_texture;
};

struct Quad
{
    float x0, y0, x1, y1; // bottom-left (x0,y0), top-right (x1,y1)
};

class HUD
{

public:
    //  may need to change to another data structure
    // or later on have another class made for doing the
    // intense calculations then passing final renderables
    // to this rendererer class.
    GLuint hotbar_texture;
    GLuint hotbarVAO, hotbarVBO;
    GLuint selectorVAO, selectorVBO;
    int selector_slot_index;
    Shader *hotbarShader;
    Shader *blockShader;
    Shader *selectorShader;
    Block block_img[9];

    Quad hotbar_slot_quad(int k);
    HUD();

    void init_hotbar();
    void draw_hotbar();
    void add_block(int index, std::string block_img_path);
    void draw_blocks();
    Quad quad_helper(int index);
    Quad selector_slot_quad(int index);
    void update_selector();
    void draw_selector();
};
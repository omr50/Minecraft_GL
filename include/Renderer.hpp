#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include "./Cube.hpp"
#include "./Camera.hpp"
#include "./Chunk.hpp"
#include "./Terrain.hpp"
#include <SDL2/SDL.h>

class Renderer
{

public:
    //  may need to change to another data structure
    // or later on have another class made for doing the
    // intense calculations then passing final renderables
    // to this rendererer class.
    std::vector<Cube *> all_blocks;
    Camera *camera;
    Terrain *terrain;

    Renderer(Camera *camera);
    void add_block(Cube *cube);
    void render_blocks();
    void send_matrix_to_shader(glm::mat4 *matrix);
    void render_chunks(SDL_Window *window);
};
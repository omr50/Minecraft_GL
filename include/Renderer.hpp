#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include "./Cube.hpp"
#include "./Camera.hpp"

class Renderer
{

public:
    //  may need to change to another data structure
    // or later on have another class made for doing the
    // intense calculations then passing final renderables
    // to this rendererer class.
    std::vector<Cube *> all_blocks;
    Camera *camera;

    Renderer(Camera *camera);
    void add_block(Cube *cube);
    void render_blocks();
    void send_matrix_to_shader(glm::mat4 *matrix);
    void create_test_dirt_blocks(int num);
};
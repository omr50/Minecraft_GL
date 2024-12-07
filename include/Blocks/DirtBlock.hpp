#pragma once
#include "../Cube.hpp"
#include "../Renderable.hpp"
#include <string>

class DirtBlock : public Cube
{
public:
    DirtBlock(int x, int y, std::string top_texture, std::string bottom_texture, std::string side_texture);
    void draw() override;
    void update_state() override;
};
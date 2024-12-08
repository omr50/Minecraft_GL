#pragma once
#include "../Cube.hpp"
#include "../Renderable.hpp"
#include <string>

class GrassBlock : public Cube
{
public:
    GrassBlock(int x, int y, int z, std::string top_texture, std::string side_texture, std::string bottom_texture);
    void update_state() override;
};
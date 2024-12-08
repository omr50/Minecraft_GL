#pragma once
#include "../Cube.hpp"
#include "../Renderable.hpp"
#include <string>

class StoneBlock : public Cube
{
public:
    StoneBlock(int x, int y, int z, std::string texture);
    void update_state() override;
};
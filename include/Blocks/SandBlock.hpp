#pragma once
#include "../Cube.hpp"
#include "../Renderable.hpp"
#include <string>

class SandBlock : public Cube
{
public:
    SandBlock(int x, int y, int z, std::string texture);
    void update_state() override;
};
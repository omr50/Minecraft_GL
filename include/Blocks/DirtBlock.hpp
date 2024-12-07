#pragma once
#include "../Cube.hpp"
#include "../Renderable.hpp"
#include <string>

class DirtBlock : public Cube
{
public:
    DirtBlock(int x, int y, int z, std::string texture);
    void update_state() override;
};
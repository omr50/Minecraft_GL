#pragma once
#include "../Cube.hpp"
#include "../Renderable.hpp"

class DirtBlock : public Cube
{
    void draw() override;
    void update_state() override;
};
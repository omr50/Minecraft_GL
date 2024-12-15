#pragma once
#include <glm/glm.hpp>
#include <string>

class Updatable
{
    virtual void update_state(float x, float y, float z, std::string block_type) = 0;
};
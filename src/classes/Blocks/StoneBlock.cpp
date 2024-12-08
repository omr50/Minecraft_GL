#include "../../../include/Blocks/StoneBlock.hpp"

StoneBlock::StoneBlock(int x, int y, int z, std::string texture)
{
    this->x = x;
    this->y = y;
    this->z = z;
    this->add_textures(texture, texture, texture);
    create_model_matrix();
}

void StoneBlock::update_state()
{
}
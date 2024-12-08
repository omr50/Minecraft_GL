#include "../../../include/Blocks/DirtBlock.hpp"

DirtBlock::DirtBlock(int x, int y, int z, std::string texture)
{
    this->x = x;
    this->y = y;
    this->z = z;
    this->add_textures(texture, texture, texture);
    create_model_matrix();
}

void DirtBlock::update_state()
{
}

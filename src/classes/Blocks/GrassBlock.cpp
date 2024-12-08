#include "../../../include/Blocks/GrassBlock.hpp"

GrassBlock::GrassBlock(int x, int y, int z, std::string top_texture, std::string bottom_texture, std::string side_texture)
{
    this->x = x;
    this->y = y;
    this->z = z;
    this->add_textures(top_texture, bottom_texture, side_texture);
    create_model_matrix();
}

void GrassBlock::update_state()
{
}

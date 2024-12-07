#include "../../../include/Blocks/DirtBlock.hpp"

DirtBlock::DirtBlock(int x, int y, std::string top_texture, std::string bottom_texture, std::string side_texture)
{
    this->x = x;
    this->y = y;
    this->add_textures(top_texture, bottom_texture, side_texture);
}

void DirtBlock::update_state()
{
}

void DirtBlock::draw()
{
    // cubes will be drawn in a loop so glUseProgram
    // only needs to be called once in that case.
    // glUseProgram(Cube::shader_program);

    for (int face = 0; face < 6; face++)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, face_textures[face]);
        GLint textureLoc = glGetUniformLocation(Cube::shader_program, "ourTexture");
        glUniform1i(textureLoc, 0);

        glBindVertexArray(face_vaos[face]);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
}

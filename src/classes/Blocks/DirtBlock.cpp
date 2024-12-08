#include "../../../include/Blocks/DirtBlock.hpp"

DirtBlock::DirtBlock(int x, int y, std::string top_texture, std::string bottom_texture, std::string side_texture)
{
    this->x = x;
    this->y = y;
    this->add_textures(top_texture, bottom_texture, side_texture);
    create_model_matrix();
}

void DirtBlock::update_state()
{
}

void DirtBlock::draw()
{
    for (int face = 0; face < 6; face++)
    {
        glActiveTexture(GL_TEXTURE0);
        // Decide which texture to use (top, bottom, sides) based on face index:
        GLuint texToUse = (face == 5) ? face_textures[0] : // Top face
                              (face == 4) ? face_textures[1]
                                          :     // Bottom face
                              face_textures[2]; // Other sides

        glBindTexture(GL_TEXTURE_2D, texToUse);
        GLint textureLoc = glGetUniformLocation(Cube::shader_program, "ourTexture");
        glUniform1i(textureLoc, 0);

        glBindVertexArray(face_vaos[face]);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
}
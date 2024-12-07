#include "../../../include/Blocks/DirtBlock.hpp"

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

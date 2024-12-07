#include "../Cube.hpp"
#include "../Renderable.hpp"

class DirtBlock : public Cube, public Renderable
{
    void draw() override;
    void update_state() override;
};
// interface for objects that
// are renderable.
#pragma once
class Renderable
{
public:
    virtual void draw();
    virtual void update_state();
};
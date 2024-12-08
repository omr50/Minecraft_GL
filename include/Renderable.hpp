// interface for objects that
// are renderable.
#pragma once
class Renderable
{
public:
    virtual void draw() = 0;
    virtual void update_state() = 0;
};
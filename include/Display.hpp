#define SDL_MAIN_HANDLED
#include <iostream>
#include <gl/glew.h>
#include <SDL2/SDL.h>
#include "./EventHandler.hpp"

class Display
{
private:
    SDL_Window *window = nullptr;
    bool running = true;
    EventHandler event_handler = EventHandler(&this->running);
    // Renderer renderer;
    bool centered = true;

public:
    Display();
    void init_window();
    void init_context_and_gl_properties();
    void init_gl_sdl();
    void main_loop();
};
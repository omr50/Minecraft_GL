#pragma once
#define SDL_MAIN_HANDLED
#include <iostream>
#include <gl/glew.h>
#include <SDL2/SDL.h>
#include "./EventHandler.hpp"
#include "./Camera.hpp"
#include "./Renderer.hpp"

class Display
{
private:
    SDL_Window *window = nullptr;
    bool running = true;
    // Camera camera = Camera();
    // EventHandler event_handler = EventHandler(&camera, window, &this->running);
    // Renderer renderer = Renderer(&camera);
    Camera *camera;
    EventHandler *event_handler;
    Renderer *renderer;
    bool centered = true;

public:
    Display();
    void init_window();
    void init_context_and_gl_properties();
    void init_gl_sdl();
    void main_loop();
};
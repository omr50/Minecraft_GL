#pragma once
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include "./Camera.hpp"
#include "./Renderer.hpp"
#include <chrono>
using Clock = std::chrono::high_resolution_clock;

class EventHandler
{

private:
    SDL_Event e;
    bool *running = nullptr;
    bool moved = false;
    bool centered = true;
    bool software_mouse_move_event;
    Camera *camera;
    SDL_Window *window;
    Renderer *renderer = nullptr;
    std::chrono::_V2::system_clock::time_point last_click = Clock::now();

public:
    EventHandler(Camera *camera, SDL_Window *window, bool *running, Renderer *renderer);
    void event_handler();
    void keyboard_handler();
    void mouse_movement_handler();
    void mouse_click_handler(bool place);
    void window_event_handler();
};
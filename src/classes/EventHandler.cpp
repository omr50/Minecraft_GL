#include "../../include/EventHandler.hpp"
#include <iostream>
#include <fcntl.h>
#define WIDTH 800.0
#define HEIGHT 600.0

EventHandler::EventHandler(Camera *camera, SDL_Window *window, bool *running, Renderer *renderer) : running(running), window(window), camera(camera), renderer(renderer) {}

void EventHandler::event_handler()
{
    // bool moved = false;
    while (SDL_PollEvent(&e))
    {
        if (e.type == SDL_KEYDOWN)
        {
            moved = true;
            keyboard_handler();
        }
        else if (e.type == SDL_MOUSEMOTION)
        {
            mouse_movement_handler();
        }
        else if (e.type == SDL_MOUSEBUTTONDOWN)
        {
            mouse_click_handler();
        }
        // later these events may be split into more distinct functions
        else if (e.type == SDL_WINDOWEVENT)
        {
            window_event_handler();
        }
    }

    if (!moved)
    {
        camera->update_camera_position({0.0, 0.0, 0.0});
    }
}

void EventHandler::keyboard_handler()
{
    glm::vec3 direction_vector = {0.0, 0.0, 0.0};
    auto key = e.key.keysym.sym;
    if (key == SDLK_w)
    {
        direction_vector = {0.0, 0.0, -1.0};
    }
    else if (key == SDLK_s)
    {
        direction_vector = {0.0, 0.0, 1.0};
    }
    else if (key == SDLK_a)
    {
        direction_vector = {-1.0, 0.0, 0.0};
    }
    else if (key == SDLK_d)
    {
        direction_vector = {1.0, 0.0, 0.0};
    }
    else if (key == SDLK_SPACE)
    {
        direction_vector = {0.0, 1.0, 0.0};
    }
    else if (key == SDLK_LSHIFT)
    {
        direction_vector = {0.0, -1.0, 0.0};
    }
    else if (key == SDLK_ESCAPE)
    {
        centered = false;
    }
    else if (key == SDLK_x)
    {
        centered = true;
    }
    camera->update_camera_position(direction_vector);
}

void EventHandler::mouse_click_handler()
{
    if (e.button.button == SDL_BUTTON_LEFT)
    {
        // roll += (0.01);
    }
    else if (e.button.button == SDL_BUTTON_RIGHT)
    {
        // roll -= (0.01);
    }
}

void EventHandler::mouse_movement_handler()
{
    if (software_mouse_move_event)
    {
        software_mouse_move_event = false;
        return;
    }
    int x, y;
    SDL_GetMouseState(&x, &y);

    float width_center = (float)WIDTH / 2.0f;
    float height_center = (float)HEIGHT / 2.0f;
    float mouse_displacement_x = x - width_center;
    float mouse_displacement_y = y - height_center;
    camera->yaw -= (mouse_displacement_x * 0.003);
    camera->pitch -= (mouse_displacement_y * 0.003);
    camera->yaw = glm::clamp(camera->yaw, -90.0f, 90.0f);
    camera->moved = true;
    if (centered)
    {
        SDL_WarpMouseInWindow(window, width_center, height_center);
        software_mouse_move_event = true;
    }
}

void EventHandler::window_event_handler()
{
    if (e.window.event == SDL_WINDOWEVENT_CLOSE)
    {
        // first log the different chunks and their state
        int stdout_copy = dup(STDOUT_FILENO);
        int fd = open("log.txt", O_WRONLY | O_CREAT, 0644);
        dup2(fd, 1);
        for (int i = 0; i < NUM_CHUNKS; i++)
            printf("chunk %d: is initialized = %d\nclean terrain = %d\nclean_mesh = %d\ngenerated vertices = %d\nis enqueued = %d\n\n\n\n\n", i + 1, renderer->terrain->chunks[i].initialized, renderer->terrain->chunks[i].clean_terrain, renderer->terrain->chunks[i].clean_mesh, renderer->terrain->chunks[i].generated_vertices, renderer->terrain->chunks[i].enqueued);
        fflush(stdout);
        dup2(stdout_copy, STDOUT_FILENO); // Restore original stdout
        close(fd);
        std::cout << "Exiting screen!" << std::endl;
        *running = false;
    }
}

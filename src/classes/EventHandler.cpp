#include "../../include/EventHandler.hpp"
#include "../../include/LineRenderer.hpp"
#include <iostream>
#include <fcntl.h>
#define WIDTH 800.0
#define HEIGHT 600.0

EventHandler::EventHandler(Camera *camera, SDL_Window *window, bool *running, Renderer *renderer, Audio *audioHandler) : running(running), window(window), camera(camera), renderer(renderer), audioPlayer(audioHandler) {}

void EventHandler::event_handler()
{
    // bool moved = false;
    while (SDL_PollEvent(&e))
    {
        if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP)
        {
            moved = true;
            keyboard_handler();
        }
        else if (e.type == SDL_MOUSEMOTION)
        {
            mouse_movement_handler();
        }
        // else if (e.type == SDL_MOUSEBUTTONDOWN)
        // {
        //     mouse_click_handler();
        // }
        // later these events may be split into more distinct functions
        else if (e.type == SDL_WINDOWEVENT)
        {
            window_event_handler();
        }

        if (e.type == SDL_MOUSEWHEEL)
        {
            // Compute sign: +1 for up/right, -1 for down/left, 0 if no scroll on that axis
            int step = 0;

            auto &selected = renderer->hud->selector_slot_index;
            if (e.wheel.y != 0)
            {
                step = (e.wheel.y > 0) ? +1 : -1; // vertical wheel
            }
            else if (e.wheel.x != 0)
            {
                step = (e.wheel.x > 0) ? +1 : -1; // horizontal wheels/trackpads
            }

            // Respect “natural”/flipped scrolling
            // if (e.wheel.direction == SDL_MOUSEWHEEL_FLIPPED)
            // {
            //     step = -step;
            // }

            // If you only want to react when there *is* a scroll:
            if (step != 0)
            {
                selected = (selected + step + 9) % 9; // wrap 0..8
            }
            else
            {
                selected = selected + step;
            }
        }

        const Uint8 *ks = SDL_GetKeyboardState(nullptr);
        glm::vec3 dir(0.0f);
        if (ks[SDL_SCANCODE_W])
            dir.z -= 1.0f;
        if (ks[SDL_SCANCODE_S])
            dir.z += 1.0f;
        if (ks[SDL_SCANCODE_A])
            dir.x -= 1.0f;
        if (ks[SDL_SCANCODE_D])
            dir.x += 1.0f;
        if (ks[SDL_SCANCODE_SPACE])
            dir.y += 1.0f;
        if (ks[SDL_SCANCODE_LSHIFT])
            dir.y -= 1.0f;

        int mx, my;
        if (SDL_GetMouseState(&mx, &my) & SDL_BUTTON(SDL_BUTTON_LEFT))
        {
            mouse_click_handler(false);
        }

        else if (SDL_GetMouseState(&mx, &my) & SDL_BUTTON(SDL_BUTTON_RIGHT))
        {
            mouse_click_handler(true);
        }

        camera->update_camera_position(dir);

        if (!moved)
        {
            camera->update_camera_position({0.0, 0.0, 0.0});
        }
    }
}

void EventHandler::keyboard_handler()
{
    // if (key == SDLK_w)
    // {
    //     direction_vector = {0.0, 0.0, -1.0};
    // }
    // else if (key == SDLK_s)
    // {
    //     direction_vector = {0.0, 0.0, 1.0};
    // }
    // else if (key == SDLK_a)
    // {
    //     direction_vector = {-1.0, 0.0, 0.0};
    // }
    // else if (key == SDLK_d)
    // {
    //     direction_vector = {1.0, 0.0, 0.0};
    // }
    // else if (key == SDLK_SPACE)
    // {
    //     direction_vector = {0.0, 1.0, 0.0};
    // }
    // else if (key == SDLK_LSHIFT)
    // {
    //     direction_vector = {0.0, -1.0, 0.0};
    // }

    auto key = e.key.keysym.sym;

    if (key == SDLK_ESCAPE)
    {
        centered = false;
    }
    if (key == SDLK_x)
    {
        centered = true;
    }
    if (key == SDLK_LCTRL)
    {
        if (e.type == SDL_KEYDOWN)
            camera->move_speed = 25.0f;
        else
            camera->move_speed = 8.0f;
    }
}

void EventHandler::mouse_click_handler(bool place)
{
    auto now = Clock::now();
    auto difference = (place) ? 200 : 300;
    std::chrono::duration<float, std::milli> difference_ms = now - last_click;

    if (difference_ms.count() > difference)
    {
        last_click = now;
        if (!place)
        {
            camera->raycast_block(renderer->terrain, &LineRenderer::points, audioPlayer);
        }

        else
        {
            camera->place_block(renderer->terrain, &LineRenderer::points, audioPlayer);
        }
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

    // camera->update_camera_position((glm::vec3){0, 0, 0});
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

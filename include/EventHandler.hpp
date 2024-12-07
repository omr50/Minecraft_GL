#include <GL/glew.h>
#include <SDL2/SDL.h>

class EventHandler
{

private:
    SDL_Event e;
    bool *running = nullptr;

public:
    EventHandler(bool *running);
    void event_handler();
    void keyboard_handler();
    void mouse_movement_handler();
    void mouse_click_handler();
    void window_event_handler();
};
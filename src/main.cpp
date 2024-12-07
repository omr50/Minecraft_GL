#include "../include/Display.hpp"

int main()
{

    Display display = Display();
    display.init_gl_sdl();
    display.main_loop();
}
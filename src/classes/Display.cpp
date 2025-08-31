#include "../../include/Display.hpp"
#include "../../include/Shader.hpp"
#include <chrono>

#define WIDTH 800.0
#define HEIGHT 600.0

Display::Display()
{
    // initialize gl and sdl before creating
    // variables which will use opengl functions.
    init_gl_sdl();

    camera = new Camera();
    renderer = new Renderer(camera);
    event_handler = new EventHandler(camera, window, &running, renderer);
    SDL_ShowCursor(SDL_DISABLE);
    SDL_SetRelativeMouseMode(SDL_TRUE);
    main_loop();
}
void Display::init_window()
{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    window = SDL_CreateWindow("MinecraftGL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_OPENGL);
}

void Display::init_context_and_gl_properties()
{
    glViewport(0, 0, WIDTH, HEIGHT);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    // SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);

    SDL_GLContext gl_context = SDL_GL_CreateContext(window);

    if (!gl_context)
    {
        std::cerr << "Failed to create OpenGL context: " << SDL_GetError() << std::endl;
        SDL_Quit();
        exit(1);
    }

    glewExperimental = GL_TRUE;
    GLenum status = glewInit();
    if (status != GLEW_OK)
    {
        std::cerr << "GLEW Initialization Error: " << glewGetErrorString(status) << std::endl;
        SDL_Quit();
        exit(1);
    }

    glEnable(GL_DEPTH_TEST);
}

void Display::init_gl_sdl()
{
    init_window();
    init_context_and_gl_properties();
    printf("Opengl initialized!\n");
}

void Display::main_loop()
{
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    glEnable(GL_DEPTH_TEST);
    // glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    // Cube::add_all_block_textures();
    Cube::initialize_texture_map("../textures/texture_atlas_2.png");
    Cube::setup_cube_shaders();
    glClearColor(0.527f, 0.805f, 0.918f, 1.0f);
    glUseProgram(Cube::shader_program);

    // set up for the chunks after the texture map has loaded.
    for (int i = 0; i < NUM_CHUNKS; i++)
    {
        renderer->terrain->create_chunk_mesh(&renderer->terrain->chunks[i]);
        renderer->terrain->chunks[i].update_chunk();
    }

    auto time_start = std::chrono::high_resolution_clock::now();
    int fps = 0;
    while (running)
    {
        // printf("got to this point?\n");
        this->event_handler->event_handler();
        camera->camera_move();
        // renderer.render_blocks();
        // printf("WORKING?!!!!!!!!!!!!!!!!!!!!!\n");

        // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // renderer->render_chunks(window);
        // glDisable(GL_DEPTH_TEST);
        // renderer->draw_crosshair(window);
        // glEnable(GL_DEPTH_TEST);
        // SDL_GL_SwapWindow(window);

        renderer->render_chunks(window); // 3D world
        // render the ray here to see what the offset issue is.

        // SDL_Delay(5);

        auto time_end = std::chrono::high_resolution_clock::now();
        fps++;
        if (std::chrono::duration_cast<std::chrono::milliseconds>(time_end - time_start).count() > 1000)
        {
            // printf("FPS: %d\n", fps);
            // find a way to control fps limits
            fps = 0;
            time_start = std::chrono::high_resolution_clock::now();
        }
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    exit(0);
}

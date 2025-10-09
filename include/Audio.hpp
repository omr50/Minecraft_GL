#pragma once
#include <string>
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <iostream>

class Audio
{

public:
    Mix_Music *main_background_music;
    Mix_Chunk *block_break_SFX[9];
    Mix_Chunk *block_place_SFX[9];

    Audio();
    void init_sdl_audio();
    void init_sdl_mixer();
    void open_device(int sampling_rate, int buffer_samples);
    void load_background_music(std::string path);
    void load_place_SFX(std::string path, int index);
    void load_break_SFX(std::string path, int index);
    void start_background_music();
    void play_break_SFX(int index);
    void play_place_SFX(int index);
    void audio_shutdown();
};
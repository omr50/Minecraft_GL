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
    Mix_Chunk *block_break_SFX;
    Mix_Chunk *block_place_SFX;

    Audio();
    void init_sdl_audio();
    void init_sdl_mixer();
    void open_device(int sampling_rate, int buffer_samples);
    void load_background_music(std::string path);
    void load_place_SFX(std::string path);
    void load_break_SFX(std::string path);
    void start_background_music();
    void play_break_SFX();
    void play_place_SFX();
    void audio_shutdown();
};
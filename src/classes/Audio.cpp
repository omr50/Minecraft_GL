#include "../../include/Audio.hpp"

// Mix_Music *main_background_music;
// Mix_Chunk *block_break_sound;
// Mix_Chunk *block_place_sound;
Audio::Audio()
{
    init_sdl_audio();
    init_sdl_mixer();
    load_background_music("../../background_music.mp3");
    load_place_sound("../../place_block.mp3");
}
void Audio::init_sdl_audio()
{
    if (SDL_Init(SDL_INIT_AUDIO) != 0)
    {
        std::cerr << "SDL init audio failed: " << SDL_GetError() << "\n";
    }
}

void Audio::init_sdl_mixer()
{
    int mixFlags = MIX_INIT_MP3 | MIX_INIT_OGG | MIX_INIT_FLAC | MIX_INIT_OPUS;
    if ((Mix_Init(mixFlags) & mixFlags) == 0)
    {
        std::cerr << "Mix_Init failed: " << Mix_GetError() << "\n";
    }
}
void Audio::open_device(int sampling_rate, int buffer_samples)
{
    if (Mix_OpenAudio(sampling_rate, AUDIO_F32SYS, 2, buffer_samples) < 0)
    {
        std::cerr << "Mix_OpenAudio failed: " << Mix_GetError() << "\n";
    }
}

void load_background_music(std::string path);
void load_place_sound(std::string path);
void load_break_sound(std::string path);
void Audio::load_background_music(std::string path)
{
    // "../../Audio_files/background_music.mp3"
    main_background_music = Mix_LoadMUS(path.c_str());
    if (!main_background_music)
    {
        std::cerr << "Load music failed: " << Mix_GetError() << "\n";
    }
}

void Audio::load_place_SFX(std::string path)
{
    block_place_SFX = Mix_LoadWAV(path.c_str());
    if (!block_place_SFX)
    {
        std::cerr << "Load place sound failed: " << Mix_GetError() << "\n";
    }
}

void Audio::load_break_SFX(std::string path)
{
    block_break_SFX = Mix_LoadWAV(path.c_str());
    if (!block_break_SFX)
    {
        std::cerr << "Load break sound failed: " << Mix_GetError() << "\n";
    }
}

void Audio::start_background_music()
{
    if (Mix_PlayMusic(main_background_music, -1) == -1)
    {
        std::cerr << "Play music failed:: " << Mix_GetError() << "\n";
    }
}

void Audio::play_break_SFX()
{
    int ch = Mix_PlayChannel(-1, block_break_SFX, 0);
    if (ch == -1)
    {
        std::cerr << "Play break SFX failed: " << Mix_GetError() << "\n";
    }
}

void Audio::play_place_SFX()
{
    int ch = Mix_PlayChannel(-1, block_place_SFX, 0);
    if (ch == -1)
    {
        std::cerr << "Play place SFX failed: " << Mix_GetError() << "\n";
    }
}
void Audio::audio_shutdown()
{
    Mix_HaltChannel(-1);
    Mix_HaltMusic();
    Mix_FreeChunk(block_place_SFX);
    Mix_FreeChunk(block_break_SFX);
    Mix_FreeMusic(main_background_music);
    Mix_CloseAudio();
    Mix_Quit();
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
}
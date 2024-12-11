#include "./../include/sound.h"

Music::Music(const std::string &file)
    : music(Mix_LoadMUS(file.c_str()), Mix_FreeMusic)
{
    if (!music)
    {
        throw std::runtime_error("Failed to load music: " + file + " - " + Mix_GetError());
    }
}

Music::~Music()
{
    // Mix_FreeMusic is called automatically by unique_ptr
}

void Music::play(int loops)
{
    if (Mix_PlayMusic(music.get(), loops) == -1)
    {
        throw std::runtime_error("Failed to play music: " + std::string(Mix_GetError()));
    }
}

void Music::pause()
{
    Mix_PauseMusic();
}

void Music::resume()
{
    Mix_ResumeMusic();
}

void Music::stop()
{
    Mix_HaltMusic();
}

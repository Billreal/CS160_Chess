#include "./../include/music.h"

Music::Music(const std::string &file)
    : music(Mix_LoadMUS(file.c_str()), Mix_FreeMusic)
{
    if (!music)
    {
        throw std::runtime_error("Failed to load music: " + file + " - " + Mix_GetError());
    }
    Mix_VolumeMusic(currentVolume);
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

void Music::increaseVolume()
{
    currentVolume += volumeIncrement;
    if(currentVolume > MIX_MAX_VOLUME)
    {
        currentVolume = MIX_MAX_VOLUME;
    }

    Mix_VolumeMusic(currentVolume);
}

void Music::decreaseVolume()
{
    currentVolume -= volumeIncrement;
    if(currentVolume < 0)
    {
        currentVolume = 0;
    }

    Mix_VolumeMusic(currentVolume);
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

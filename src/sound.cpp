#include "./../include/sound.h"
#include <iostream>
Sound::Sound()
{
    for (int i = 0; i < 8; i++)
        chunk[i] = nullptr;
}

Sound::~Sound()
{
    for (int i = 0; i < 8; i++)
        Mix_FreeChunk(chunk[i]);
}

void Sound::loadSoundEffect(SoundEffect soundEffect, const std::string musicDirectory)
{
    chunk[int(soundEffect)] = Mix_LoadWAV(musicDirectory.c_str());
}

void Sound::playSound(SoundEffect soundEffect, int times)
{
    Mix_PlayChannel(-1, chunk[(int)soundEffect], times);
}

void Sound::setVolume(int volume)
{
    volume = volume * 128 / 16;
    std::cerr << "Setting volume\n";
    currentVolume = volume;
    for (int i = 0; i < 8 && chunk[i]; i++)
    {
        Mix_VolumeChunk(chunk[i], currentVolume);
        std::cerr << "Setting " << i << "-th chunk done to :" << currentVolume << std::endl;
    }
}

void Sound::mute()
{
    for (int i = 0; i < 8 && chunk[i]; i++)
        Mix_VolumeChunk(chunk[i], 0);
}

void Sound::unmute()
{
    for (int i = 0; i < 8 && chunk[i]; i++)
        Mix_VolumeChunk(chunk[i], currentVolume);

}
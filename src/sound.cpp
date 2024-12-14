#include "./../include/sound.h"

Sound::Sound()
{

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
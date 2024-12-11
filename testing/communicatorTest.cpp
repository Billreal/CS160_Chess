
#include <SDL.h>
#include <SDL_mixer.h>

#include <string>
#include <iostream>
#include <memory>
#include <stdexcept>

class Sound
{
public:
    Sound(const std::string &file);
    ~Sound();

    void play(int loops = 0);
    void pause();
    void resume();
    void stop();

private:
    std::unique_ptr<Mix_Chunk, void (*)(Mix_Chunk *)> chunk;
};

class Music
{
public:
    Music(const std::string &file);
    ~Music();

    void play(int loops = -1);
    void pause();
    void resume();
    void stop();

private:
    std::unique_ptr<Mix_Music, void (*)(Mix_Music *)> music;
};

class AudioManager
{
public:
    AudioManager(int frequency = MIX_DEFAULT_FREQUENCY, Uint16 format = MIX_DEFAULT_FORMAT, int channels = 2, int chunksize = 4096);
    ~AudioManager();

    void setVolume(int volume);
    int getVolume() const;

private:
    int volume;
};


Sound::Sound(const std::string &file)
    : chunk(Mix_LoadWAV(file.c_str()), Mix_FreeChunk)
{
    if (!chunk)
    {
        throw std::runtime_error("Failed to load sound: " + file + " - " + Mix_GetError());
    }
}

Sound::~Sound()
{
    // Mix_FreeChunk is called automatically by unique_ptr
}

void Sound::play(int loops)
{
    if (Mix_PlayChannel(-1, chunk.get(), loops) == -1)
    {
        throw std::runtime_error("Failed to play sound: " + std::string(Mix_GetError()));
    }
}

void Sound::pause()
{
    Mix_Pause(-1);
}

void Sound::resume()
{
    Mix_Resume(-1);
}

void Sound::stop()
{
    Mix_HaltChannel(-1);
}

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

AudioManager::AudioManager(int frequency, Uint16 format, int channels, int chunksize)
    : volume(MIX_MAX_VOLUME)
{
    if (Mix_OpenAudio(frequency, format, channels, chunksize) == -1)
    {
        throw std::runtime_error("Failed to initialize SDL_mixer: " + std::string(Mix_GetError()));
    }
}

AudioManager::~AudioManager()
{
    Mix_CloseAudio();
}

void AudioManager::setVolume(int volume)
{
    this->volume = volume;
    Mix_Volume(-1, volume);
    Mix_VolumeMusic(volume);
}

int AudioManager::getVolume() const
{
    return volume;
}

int main(int argc, char **argv)
{
    std::cerr << "communicator initialized\n";
    if (SDL_Init(SDL_INIT_AUDIO) < 0)
    {
        SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
        return -1;
    }

    try
    {
        AudioManager audioManager;
        Sound sound("path/to/sound.wav");
        Music music("path/to/music.mp3");

        sound.play();
        music.play();

        SDL_Delay(5000); // Wait for 5 seconds to hear the sound
    }
    catch (const std::exception &e)
    {
        SDL_Log("Error: %s", e.what());
    }

    SDL_Quit();
    return 0;
}
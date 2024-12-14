#include <iostream>
#include <SDL2/SDL.h>
#include <SDL_mixer.h>
#include <memory>
#include <stdexcept>


class Music
{
public:
    Music(const std::string &file);
    ~Music();

    bool isPaused() const { return Mix_PausedMusic(); }
    int getVolume() const { 
        // std::cerr << "Current volume: " << currentVolume << std::endl;
        return currentVolume / volumeIncrement; 
    }
    void play(int loops = -1);
    void pause();
    void resume();
    void stop();
    void increaseVolume();
    void decreaseVolume();

private:
    std::unique_ptr<Mix_Music, void (*)(Mix_Music *)> music;
    int currentVolume = MIX_MAX_VOLUME / 4;
    int volumeIncrement = MIX_MAX_VOLUME / 16;
};

#include <SDL_mixer.h>
#include <string>

enum SoundEffect
{
    PICKUP = 0,
    MOVE = 1,
    CAPTURE = 2,
    GAMEOVER = 3,
    PROMOTION = 4,
    ILLEGAL = 5,
    CHECK = 6,
};

class Sound
{
private: 
    Mix_Chunk* chunk[8];
public:
    Sound();
    ~Sound();

    void loadSoundEffect(SoundEffect channel, const std::string musicDirectory);
    void playSound(SoundEffect channel, int times = 0);
};
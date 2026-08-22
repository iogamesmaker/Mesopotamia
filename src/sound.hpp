// sound.hpp
// originally based on deep space starships equivelant but i rewrote it
// https://lazyfoo.net/tutorials/SDL/21_sound_effects_and_music/index.php
#pragma once
#ifndef sound_hpp
#define sound_hpp

#include "headers.hpp"
#include "global.hpp"

class soundManager
{
public:
    soundManager();
    ~soundManager();
    void init();
    void loadAudio(const std::string& audioPath, const std::string& audioName); // pretty much only for internal use in the soundManager class
    void loadMusic(const std::string& musicPath, const std::string& musicName); //
    void playMusic(const std::string& musicName, bool loop);
    void playAudio(const std::string& audioName);

    void toggleMusic(); // toggling is just setting volume to 0 or to 128 (max. vol)
    void toggleSound();

    Mix_Chunk* returnAudio(const std::string& audioName);
    Mix_Music* returnMusic(const std::string& musicName);

    bool musicOn = true;
    bool soundOn = true;
private:
    std::unordered_map<std::string, Mix_Chunk*> audio; // Sound effects
    std::unordered_map<std::string, Mix_Music*> music; // Music
    // The difference between Mix_Chunk and Mix_Music is that Mix_Chunk is stored in RAM while Mix_Music apparently gets streamed from the disk or something.
};
#endif

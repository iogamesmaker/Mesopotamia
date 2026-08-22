// sound.cpp
// most of this is adopted from Lazy Foo's SDL2 Tutorial
#include "global.hpp"

soundManager::soundManager() {
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {std::cout << "SDL_mixer blew its speakers up: " << Mix_GetError() << std::endl;} // 2 = 2 channels for stereo sound, 2048 = each chunk is 2048 bytes. not sure what that does but yeah "change it around if the audio is laggy" - random article
    // if (!(Mix_Init(MIX_INIT_FLAC) & MIX_INIT_FLAC)) {std::cout << "FLAC failed (no support?? noob): " << Mix_GetError() << std::endl;} // i like flac files - might remove later if im gonna use emscripten for web compilation (which I'm probably not). I vividly remember trying to get FLAC audio to work via WASM for hours and hours and then giving up and commenting this line out in Deep Space Starships.

    // okay i gave up again fuck emscripten fuck flac files (i hope i get emscripten working)

    std::ifstream audioFile(getpath("assets/audio/audio.txt")); // file loading copied from texture.cpp - i don't know it's kind of shitty to have 2 seoerate files but oh well

    if (!audioFile.is_open()) {
        SDL_Log("Failed to open audio assets file: audio/assets.txt");
        return;
    }

    std::string name, arrow, location;
    while (audioFile >> name >> arrow >> location) {
        if(name == "#" || name == "") {
            continue;
        }
        std::string fullPath = getpath("assets/audio/" + location);
        loadAudio(fullPath, name);
    }
    std::ifstream musicFile(getpath("assets/audio/music/music.txt"));

    if (!musicFile.is_open()) {
        SDL_Log("Failed to open music assets file: audio/music.txt");
        return;
    }

    while (musicFile >> name >> arrow >> location) {
        if(name == "#" || name == "") {
            continue;
        }
        std::string fullPath = getpath("assets/audio/music/") + location;
        loadMusic(fullPath, name);
    }
}

void soundManager::toggleMusic() {
    if(musicOn) {
        setStatus("Music is OFF");
        Mix_VolumeMusic(0);
    } else {
        setStatus("Music is ON!");
        Mix_VolumeMusic(128);
    }
    musicOn = !musicOn;
}

void soundManager::toggleSound() {
    if(soundOn) {
        setStatus("Sound is OFF");
        Mix_Volume(-1, 0); // -1 is the channel. -1 means all channels
    } else {
        setStatus("Sound is ON!");
        Mix_Volume(-1, 128);
    }
    soundOn = !soundOn;
}

soundManager::~soundManager() {
    for (auto& [name, chunk] : audio) {
        Mix_FreeChunk(chunk);
    }
    audio.clear();

    for (auto& [name, track] : music) {
        Mix_FreeMusic(track);
    }
    music.clear();

    Mix_Quit();
    Mix_CloseAudio();
}


void soundManager::loadAudio(const std::string& audioPath, const std::string& audioName) {
    Mix_Chunk* chunk = Mix_LoadWAV((audioPath).c_str());
    if (!chunk) {
        printf("failed to load audio %s: %s\n", audioPath.c_str(), Mix_GetError());
        return;
    }
    audio[audioName] = chunk;
}

void soundManager::loadMusic(const std::string& musicPath, const std::string& musicName) {
    Mix_Music* track = Mix_LoadMUS((musicPath).c_str()); // SDL made this pretty easy :) thanks SDL
    if (!track) {
        printf("failed to load music %s: %s\n", musicPath.c_str(), Mix_GetError());
        return;
    }
    music[musicName] = track;
}

void soundManager::playAudio(const std::string& audioName) {
    // std::cout << audioName << std::endl;
    if(audioName == "" || audioName == "null") return;
    Mix_PlayChannel(-1, returnAudio(audioName), 0); // -1 = nearest available channel | 0 = repeat 0x
}

void soundManager::playMusic(const std::string& musicName, bool loop) { // loop function is chat gpt
    if(musicName == "" || musicName == "null") return;

    if (Mix_PlayingMusic()) { // checks if there's already music playing, and fade it out
        Mix_FadeOutMusic(3000); // fade out

        while (Mix_FadingMusic() == MIX_FADING_OUT) { // hell yeah SDL makes a lot of sense
            SDL_Delay(10);
        } // wait until the music fades out
    }

    Mix_Music* newTrack = returnMusic(musicName); // get the super cool new piece of art
    if (newTrack) {
        Mix_FadeInMusic(newTrack, loop ? -1 : 0, 3000); // 3000 = fade-in milliseconds
    }
}


Mix_Chunk* soundManager::returnAudio(const std::string& audioName) { // copied from texture.cpp lol
    auto iter = audio.find(audioName);
    return iter != audio.end() ? iter->second : audio.at("NULL");
}

Mix_Music* soundManager::returnMusic(const std::string& musicName) { // copied from texture.cpp
    auto iter = music.find(musicName);
    return iter != music.end() ? iter->second : music.at("NULL");
}

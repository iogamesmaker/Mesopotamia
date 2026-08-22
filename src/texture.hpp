// texture.hpp
// load texture in nice array
#pragma once
#ifndef texture_hpp
#define texture_hpp

#include "headers.hpp"
#include "global.hpp"

class textureManager {
    public:
        textureManager();
        ~textureManager();

        void loadTexture(const std::string& texturePath, const std::string& textureName);
        void put(SDL_Texture* texture, const SDL_Rect* src = NULL, const SDL_Rect* dst = NULL);
        void put(SDL_Texture* texture, const SDL_Rect src, const SDL_Rect dst);

        // helpers
        SDL_Texture* get(const std::string& textureName);

        SDL_Surface* getSurface(const std::string& textureName); // inefficient (but working) garbage
        SDL_Rect toRect(SDL_Texture* texture, int x = 0, int y = 0);
    private:
        std::unordered_map<std::string, SDL_Texture*> textures;
        std::map<std::string, std::string> nameToPath;
};
#endif

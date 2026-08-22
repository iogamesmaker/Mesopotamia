// images.hpp
#pragma once
#ifndef images_hpp
#define images_hpp
#include "headers.hpp"
#include "global.hpp"

class Image {
    public:
        Image(SDL_Texture* texture, SDL_Rect src, SDL_Rect dst, const std::string& ID);

        void update();
        void render();

        std::string id;
        bool hovered = false;
        bool active  = false;
        SDL_Rect source;
        SDL_Texture* image;
    private:
        SDL_Rect bounds;

};
#endif

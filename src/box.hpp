// box.hpp
// to render fancy bakcgrounds of box elements
// box code based on button code
#pragma once
#ifndef box_hpp
#define box_hpp

#include "headers.hpp"
#include "global.hpp"

class Box {
    public:
        Box(SDL_Rect rect, const std::string& ID);

        void update();
        void render();

        std::string id;
        bool hovered = false;
        bool active  = false;
    private:
        SDL_Rect bounds;

        SDL_Texture* boxTexture;
};
#endif

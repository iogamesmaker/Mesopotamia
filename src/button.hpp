// button.hpp
#pragma once
#ifndef button_hpp
#define button_hpp

#include "headers.hpp"
#include "global.hpp"

class Button {
    public:
        Button(SDL_Rect rect, const std::string& one, const std::string& two, const std::string& three, std::function<void()> callback, const std::string& ID, const std::string& audioName = "");

        void update();
        void render();
        void both();

        void disable();
        void enable();

        std::string id;
        bool hovered = false;
        bool clicked = false;
        bool active  = false;
        std::string normal;
        std::string hover;
        std::string click;
        std::function<void()> func;
    private:
        SDL_Rect bounds;

        bool prevActive = false;

        std::string audio = "null";
};
#endif

// input.hpp
#pragma once
#ifndef input_hpp
#define input_hpp

#include "headers.hpp"
#include "global.hpp"

class inputManager {
    public:
        void processEvent(const SDL_Event& e);
        void update();

        bool keyDown(SDL_Scancode key) const;
        bool keyPressed(SDL_Scancode key) const;
        bool keyReleased(SDL_Scancode key) const;
        void releaseKey(SDL_Scancode key);

        bool isHovered(SDL_Rect rect);

        // public accessible variables

        bool quit = false;

        bool lmb, rmb, mmb;
        bool firstPressLMB;
        SDL_Point mousePos;
        SDL_Point mouse;
        SDL_Point tile;

        float scrollX;
        float scrollY;
    private:
        std::unordered_map<SDL_Scancode, bool> keysDown;
        std::unordered_map<SDL_Scancode, bool> keysPressed;
        std::unordered_map<SDL_Scancode, bool> keysReleased;
};

#endif

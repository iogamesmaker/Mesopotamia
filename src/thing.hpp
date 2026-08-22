// thing.hpp
// Long running tradition of mine. I always have one "thing.hpp" in a project, where I put all of my trash that isn't supposed to go in main.cpp nor in anything else really.
// Better name would be "wrapper.hpp" or "game.hpp" but fuck that.
#pragma once
#ifndef thing_hpp
#define thing_hpp

#include "headers.hpp"
#include "global.hpp"

class gameClass {
    public:
        gameClass();
        ~gameClass();

        SDL_Texture* lowres; // screen gets rendered to this at 320x180 resolution
        SDL_Rect dest = {0, 0, SCREEN_W, SCREEN_H}; // for SDL_Texture* lowres

        void render();
        void mainLoop();

        void startGame();

        void openCapitalMenu();

        void pauseGame();
        void resumeGame();

        void nextTurn();

        void quitToMenu();
        void saveGame();
        void doFrame();

        void openInventory();

        void tick();
        void initElements();

        std::string gameState = "mainmenu";
    private:
        tileManager* menuWorld;
        SDL_Rect createDstRect();
        bool running;
        float accum = 0;
        Uint32 lastTick;

        const int aimFPS = 60;
        const float FIXED_TIMESTEP = 1 / static_cast<float>(aimFPS);
        const Uint32 sleep = 1000 / aimFPS;
};

#endif

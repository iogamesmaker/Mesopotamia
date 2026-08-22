// input.cpp
// Mainly based on what ChatGPT told me. Looks good though.
// Do want to re-do this 100% by myself at some point but it works really well for now.
#include "global.hpp"

void inputManager::processEvent(const SDL_Event& event) { // might want to switch to a switch soon next time you add more shit here
    firstPressLMB = false;

    if (event.type == SDL_QUIT) {
        quit = true;
    } else if (event.type == SDL_KEYDOWN && !event.key.repeat) {
        keysDown[event.key.keysym.scancode] = true;
        keysPressed[event.key.keysym.scancode] = true;
    } else if (event.type == SDL_KEYUP) {
        keysDown[event.key.keysym.scancode] = false;
        keysReleased[event.key.keysym.scancode] = true;
    } else if (event.type == SDL_MOUSEMOTION) {
        SDL_GetMouseState(&mousePos.x, &mousePos.y);
    } else if (event.type == SDL_MOUSEBUTTONDOWN) {
        if (event.button.button == SDL_BUTTON_LEFT) {
            if(!lmb) {
                firstPressLMB = true;
                lmb = true;
            } else {
                firstPressLMB = false;
            }
        }
        if (event.button.button == SDL_BUTTON_RIGHT) rmb = true;
        if (event.button.button == SDL_BUTTON_MIDDLE) mmb = true;
    } else if (event.type == SDL_MOUSEBUTTONUP) {
        if (event.button.button == SDL_BUTTON_LEFT) {
            lmb = false;
            firstPressLMB = false;
        }
        if (event.button.button == SDL_BUTTON_RIGHT) rmb = false;
        if (event.button.button == SDL_BUTTON_MIDDLE) mmb = false;
    } else if (event.type == SDL_MOUSEWHEEL) {
        scrollX = event.wheel.x;
        scrollY = event.wheel.y;
    }
}

bool inputManager::isHovered(SDL_Rect rect) {
    SDL_Point temp = scalePoint(mousePos);
    return SDL_PointInRect(&temp, &rect);
}

void inputManager::update() {
    firstPressLMB = false;
    mouse = scalePoint(mousePos, true);

    keysPressed.clear();
    keysReleased.clear();
}

bool inputManager::keyDown(SDL_Scancode key) const {
    auto iterator = keysDown.find(key);
    return iterator != keysDown.end() && iterator->second;
}

bool inputManager::keyPressed(SDL_Scancode key) const {
    auto iterator = keysPressed.find(key);
    return iterator != keysPressed.end() && iterator->second;
}

bool inputManager::keyReleased(SDL_Scancode key) const {
    auto iterator = keysReleased.find(key);
    return iterator != keysReleased.end() && iterator->second;
}

void inputManager::releaseKey(SDL_Scancode key) {
    auto iterator = keysPressed.find(key);
    if (iterator != keysPressed.end()) {
        keysPressed[key] = false;
        keysReleased[key] = false;
        keysPressed[key] = false;
    }
}

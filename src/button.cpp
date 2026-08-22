// button.cpp
#include "global.hpp"

Button::Button(SDL_Rect rect, const std::string& one, const std::string& two, const std::string& three, std::function<void()> callback, const std::string& ID, const std::string& audioName) {
    bounds = rect;
    normal = one;
    hover = two;
    click = three;
    func = callback;
    audio = audioName;
    id = ID;
}

void Button::update() {
    if(!active) {
        clicked = false;
        hovered = false;
        return;
    }
    if(clicked && !(IN->lmb)) {
        clicked = false;
        if(hovered && func) {
            SM->playAudio(audio);
            func();
        }
    }

    if(IN->isHovered(bounds)) {
        hovered = true;
        if(!clicked && IN->firstPressLMB && hovered && prevActive) {
            clicked = true;
        }
    } else {
        hovered = false;
        clicked = false;
    }
}

void Button::render() {
    prevActive = active;
    if(!active) return;
    std::string texture = normal;

    if(clicked) {
        texture = click;
    } else if(hovered) {
        texture = hover;
    }

    SDL_Texture* buttTexture /* amazing */ = TM->get(texture); // maybe store it locally in the button class?

    TM->put(buttTexture, NULL, &bounds);
    // if(boxRect.w != 0) GUI->renderBox(boxRect);
}

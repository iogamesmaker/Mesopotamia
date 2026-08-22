// images.cpp
// class for putting images as elements on the screen
#include "global.hpp"
Image::Image(SDL_Texture* texture, SDL_Rect src, SDL_Rect dst, const std::string& ID) {
    bounds = dst;
    source = src;
    image = texture;
    hovered = false;
    id = ID;
}

void Image::update() {
    if(!active) {
        hovered = false;
        return;
    }
    if(IN->isHovered(bounds)) {
        hovered = true;
    } else {
        hovered = false;
    }
}

void Image::render() {
    if(active) TM->put(image, &source, &bounds);
}

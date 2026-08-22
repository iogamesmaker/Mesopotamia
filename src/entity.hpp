// entity.hpp
#pragma once
#ifndef entity_hpp
#define entity_hpp

#include "headers.hpp"
#include "global.hpp"

class Entity {
public:
    Entity(std::string textureName, int frames = 1);
    void render();
    void setCoords(float nx, float ny);
    void update(tileManager* world);
    float x;
    float y;
    int frames;
    int frame;
    int tileX;
    int tileY;
    int size;
private:
    SDL_Texture* texture;
    SDL_Rect source;
    SDL_Rect dest;
};

#endif

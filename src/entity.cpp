// entity.cpp
#include "headers.hpp"
#include "global.hpp"
#include "entity.hpp"

Entity::Entity(std::string textureName, int framesN) {
    texture = TM->get(textureName);
    frames = framesN;
    source = TM->toRect(texture);
    source.h /= frames;
    dest = source;
}

void Entity::setCoords(float nx, float ny) {
    x = nx;
    y = ny;
    tileX = std::floor(nx);
    tileY = std::floor(ny);
    camX = x;
    camY = y;
}

void Entity::update(tileManager* world) {
    tileManager::Point tile = world->getTile(tileX, tileY);
    if(tile.surface == tileManager::WATER_VAR1 || tile.surface == tileManager::WATER_VAR2) {
        Entity::setCoords(x, y + 1);
    }

    std::cout << tile.surface << std::endl;

    if(x > world->worldSize * 0.5 ||
       y > world->worldSize * 0.5 ||
       x < -world->worldSize * 0.5 ||
       y < -world->worldSize * 0.5) {
        setCoords(0, 0);
    }
}

void Entity::render() {
    frame = int(SDL_GetTicks() / 200) % frames;
    source.y = source.h * frame;
    dest = source;
    dest.y = 0;
    dest.w *= std::floor(16.0f * camZoom) / 16.0f;
    dest.h *= std::floor(16.0f * camZoom) / 16.0f;

    float destx = (std::floor(16.0f * camZoom) * (camX + x));
    float desty = (std::floor(16.0f * camZoom) * (camY + y));
    destx += -dest.w * 0.5f + SCREEN_W * 0.5f;
    desty += -dest.h * 0.5f + SCREEN_H * 0.5f;

    dest.x = destx;
    dest.y = desty;

    std::cout << "SCREEN_W: " << SCREEN_W << ", DEST.W: " << dest.w << ", camZoom: " << camZoom << ", thing: " << std::floor(16.0f * camZoom) << std::endl;
    std::cout << camZoom << ", X: " << dest.x << ", Y: " << dest.y << ", camX: " << camX << ", camY: " << camY << "x" << x << "y" << y << std::endl;
    TM->put(texture, &source, &dest);
}

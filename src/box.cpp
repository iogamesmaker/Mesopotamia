// box.cpp
#include "global.hpp"

Box::Box(SDL_Rect rect, const std::string& ID) {
    bounds = rect;
    hovered = false;
    id = ID;
    boxTexture = TM->get("boxtexture");
}

void Box::update() {
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

void Box::render() {
    if(!active) return;
    SDL_Rect bg = SDL_Rect{0, 0, 1, 1}; // linksboven pixel is de achtergrondkleur
    SDL_Rect corner = SDL_Rect{0, 0, 12, 12};
    SDL_Rect edge = SDL_Rect{12, 0, 12, 12};

    SDL_Rect dst = bounds;
    SDL_RenderCopy(renderer, boxTexture, &bg, &dst); // bg

    // sides
    SDL_Rect sideDst = bounds;
    sideDst.h = 12;
    SDL_RenderCopyEx(renderer, boxTexture, &edge, &sideDst, 0.0, NULL, SDL_FLIP_NONE);
    sideDst.y = bounds.y + bounds.h - 12;
    SDL_RenderCopyEx(renderer, boxTexture, &edge, &sideDst, 180.0, NULL, SDL_FLIP_NONE);

    sideDst = bounds;
    sideDst.w = std::ceil(sideDst.h / 12) * 12; // ik heb 4 uur verspilt aan de boxes fixen
    sideDst.x = 6 + sideDst.x - sideDst.w / 2;   // ik was telkens 1 stap van het fixen ):
    sideDst.h = 12;                              // als je ze niet afrond per 12 pixels dan is hij misaligned voor een of andere reden????
    sideDst.y = sideDst.y + sideDst.w / 2 - 6;       // supergaar did heb je als je zonder game engines werkt lol

    SDL_RenderCopyEx(renderer, boxTexture, &edge, &sideDst, 270.0, NULL, SDL_FLIP_NONE);
    sideDst.x += bounds.w - 12;
    SDL_RenderCopyEx(renderer, boxTexture, &edge, &sideDst, 90.0, NULL, SDL_FLIP_NONE);

    // corners
    SDL_Rect cornerDst;
    cornerDst.w = 12; cornerDst.h = 12;
    cornerDst.x = bounds.x; cornerDst.y = bounds.y;

    SDL_RenderCopyEx(renderer, boxTexture, &corner, &cornerDst, 0.0, NULL, SDL_FLIP_NONE);
    cornerDst.x = bounds.x + bounds.w - 12;
    SDL_RenderCopyEx(renderer, boxTexture, &corner, &cornerDst, 0.0, NULL, SDL_FLIP_HORIZONTAL);
    cornerDst.y = bounds.y + bounds.h - 12;
    SDL_RenderCopyEx(renderer, boxTexture, &corner, &cornerDst, 0.0, NULL, SDL_RendererFlip(SDL_FLIP_HORIZONTAL | SDL_FLIP_VERTICAL));
    cornerDst.x = bounds.x;
    SDL_RenderCopyEx(renderer, boxTexture, &corner, &cornerDst, 0.0, NULL, SDL_FLIP_VERTICAL);
}

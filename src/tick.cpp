// tick.hpp
#include "headers.hpp"
#include "global.hpp"
#include "thing.hpp"
// dump for code stuff just dumping all the ugly game logic into here so that thing.cpp doesnt get polluted with all my garbage
void gameClass::tick() {
    if(tileSetting != -1) {
        if(!GUI->buttonIsOn("cancel") && !IN->lmb) GUI->enableButton("cancel");
    } else {
        GUI->disableButton("cancel");
    }
    if(gameState == "ingame") {
        if(!GUI->imageIsOn("black")) {
            int speed = IN->keyDown(SDL_SCANCODE_LCTRL) ? 5 : 1;

            if (IN->keyDown(SDL_SCANCODE_W) || IN->keyDown(SDL_SCANCODE_UP   )) camY += movespeed * speed;
            if (IN->keyDown(SDL_SCANCODE_S) || IN->keyDown(SDL_SCANCODE_DOWN )) camY -= movespeed * speed;
            if (IN->keyDown(SDL_SCANCODE_A) || IN->keyDown(SDL_SCANCODE_LEFT )) camX += movespeed * speed;
            if (IN->keyDown(SDL_SCANCODE_D) || IN->keyDown(SDL_SCANCODE_RIGHT)) camX -= movespeed * speed;
            if (IN->keyDown(SDL_SCANCODE_MINUS)) camZoom -= 0.02;
            if (IN->keyDown(SDL_SCANCODE_EQUALS)) camZoom += 0.02;
            if (IN->keyDown(SDL_SCANCODE_0)) camZoom = 1.0;

            if (IN->firstPressLMB && GUI->top && tileSetting == -1) {
                if(gameWorld->capitalLevel != 0
                    && (gameWorld->capital.x) - (IN->tile.x + worldSize * 0.5) <= 0
                    && (gameWorld->capital.y) - (IN->tile.y + worldSize * 0.5) <= 0
                    && (gameWorld->capital.x) - (IN->tile.x + worldSize * 0.5) >= -1
                    && (gameWorld->capital.y) - (IN->tile.y + worldSize * 0.5) >= -1) {
                    openCapitalMenu();
                } else {
                    gameWorld->openMenu(IN->tile.x,IN->tile.y);
                }
            }
        }

        camZoom = std::clamp(camZoom, 0.25f, 2.0f);

    } else {
        tileSetting = -1;
    }

    // events
    if(cache[1] != -1) {
        if(cache[0] < SDL_GetTicks() && cache[0] != 0) {
            GUI->disableText("welcometext");
            GUI->disableBox("welcomebox");
            cache[1] = -1;
        }
    }

    if(debug) {
        std::string debugText;
        debugText = "FPS: " + std::to_string(int(fps)) + "\nX: " + std::to_string(int(IN->mouse.x)) + ", Y: " + std::to_string(int(IN->mouse.y)) + "\ntile X: " + std::to_string(int(IN->tile.x)) + ", Y: " + std::to_string(int(IN->tile.y));
        if(gameState == "ingame") debugText += "\n:) " + std::to_string(gameWorld->happiness);
		GUI->changeText("debug", debugText);
    }
}

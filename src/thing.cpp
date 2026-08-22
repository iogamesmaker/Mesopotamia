// thing.cpp
#include "thing.hpp"

gameClass::gameClass() {
    // Init function. Get the window going, do some other shit.
    running = true;

    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow(
        "Settlers of Mesopotamia",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        1280, 720,
        SDL_WINDOW_SHOWN
    );

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0"); // 0 nearest, 1 linear, 2 anisotropic(?)
    SDL_SetWindowResizable(window, SDL_TRUE);
    SDL_SetWindowMinimumSize(window, SCREEN_W, SCREEN_H);
    SDL_ShowCursor(SDL_DISABLE);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED/* | SDL_RENDERER_PRESENTVSYNC*/); // vsync because my laptop likes to fuck itself up without vsync.
    // TODO: make vsync and fps cap an option for sweaty gamers that need 240fps in notepad (TODO: make a settings menu first!!)
    // also make render scale quality an option

    lowres = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGB565,
        SDL_TEXTUREACCESS_TARGET,
        SCREEN_W,SCREEN_H
    );

    IN = new inputManager();
    SM = new soundManager();
    TM = new textureManager();
    GUI = new guiManager();
    // lol123super = new Entity("convoy", 4);

    worldSize = 64;

    initElements(); // do NOT look at els.cpp lmao

    quitToMenu();

    mainLoop();
}

gameClass::~gameClass() {
    std::cout << "cya" << std::endl;
    delete IN;
    delete SM;
    delete TM;
    delete GUI;
    delete gameWorld;

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyTexture(lowres);
}

void gameClass::quitToMenu() {
    gameState = "mainmenu";
    camX = -40;
    camY = -40;
    camZoom = 1.0;

    setStatus("", 0);

    GUI->disableAll();

    GUI->enableImage("black");
    GUI->enableImage("menubackground");
    GUI->enableButton("Menu Start");
    GUI->enableButton("Menu Settings");
    GUI->enableButton("Menu Exit");
    menuWorld = new tileManager(200, 0);
}

SDL_Rect gameClass::createDstRect() { // (the 320x180 screen)
    SDL_Rect dstRect;

    float scale = getWindowScale();

    int w,h;
    SDL_GetWindowSize(window, &w, &h);

    int scaledW = static_cast<int>(SCREEN_W * scale);
    int scaledH = static_cast<int>(SCREEN_H * scale);

    dstRect.w = scaledW;
    dstRect.h = scaledH;
    dstRect.x = (w - scaledW) / 2;
    dstRect.y = (h - scaledH) / 2;

    return dstRect;
}

void gameClass::render() {
    GUI->update();
    if(gameState == "mainmenu") {
        int cameraState = SDL_GetTicks() % 200000;
        if(cameraState < 50000) {
            camX += 0.01;
            camY += 0.01;
        } else if(cameraState < 100000) {
            camX -= 0.01;
            camY += 0.01;
        } else if(cameraState < 150000) {
            camX -= 0.01;
            camY -= 0.01;
        } else {
            camX += 0.01;
            camY -= 0.01;
        }
        menuWorld->renderWorld();
    } else if(gameState ==  "ingame" || gameState == "paused") {
        gameWorld->renderWorld();
        if(gameWorld->capitalLevel == 0) {
            if(cache[1] == -1) {
                GUI->renderBox({0, 0, 148, 40});
                GUI->renderText(SDL_Point{13,8}, "Place your capitol", 0);
                GUI->renderText(SDL_Point{13,19}, "hint: place it near fertile ground,\nbut not too far from the desert.", 1);
            }

            if(!GUI->imageIsOn("black")) tileSetting = tileManager::PAL1_TL;
        }
    }

    GUI->renderAll();
    if(GUI->getBox("hotbar")->active) {
        SDL_Point point = GUI->renderText(SDL_Point{1,152}, std::to_string(gameWorld->lapis) + "{LA}", 0);

        int x = point.x;

        point = GUI->renderText(SDL_Point{1,166}, std::to_string(gameWorld->bricks) + "{BR}", 0);

        x = std::max(point.x, x) + 1;

        int x2 = GUI->renderText(SDL_Point{x + 1,152}, std::to_string(gameWorld->wood) + "{WO}", 0).x;
        point = GUI->renderText(SDL_Point{x + 1,166}, std::to_string(gameWorld->bread) + "{BD}",0);

        x = std::max(point.x, x2) + 1;

        x2 = GUI->renderText(SDL_Point{x + 1,152}, std::to_string(gameWorld->grapes) + "{GR}",0).x;
        point = GUI->renderText(SDL_Point{x + 1,166}, std::to_string(gameWorld->grain) + "{WH}", 0);

        x = std::max(point.x, x2) + 1;

        x2 = GUI->renderText({x + 1, 152}, std::to_string(gameWorld->figs) + "{FI}", 0).x;
        point = GUI->renderText({x + 1, 166}, std::to_string(gameWorld->workers) + "{PL}", 0);

        x = std::max(point.x, x2) - 2;

        // --------
        // building selection
        if(!GUI->imageIsOn("black")) {
            // get src rect of a tile: gameWorld->getOverlayTile(gameWorld->PALACE).src
            SDL_Rect dst;
            SDL_Rect boxToRender = {0,0,0,0};
            int tileIdx;
            std::string line1;
            std::string line2;

            // QUARRY
            tileIdx = static_cast<int>(gameWorld->QUARRYTL);
            dst = {x,149, 32,32};
            SDL_Rect src = gameWorld->getOverlayTile(gameWorld->QUARRYTL).src;
            src.w = 32;
            src.h = 32;

            TM->put(TM->get("overlayatlas"), src, dst);
            if(IN->isHovered(dst)) {
                cache[100 + tileIdx] += 1;
                if(cache[100 + tileIdx] > 24) {
                    boxToRender = {x - 20, 120, 128, 28};
                    line1 = "QUARRY - produces bricks";
                    line2 = "25 wood, 2 bricks";
                }
            } else {
                cache[100 + tileIdx] = 0;
            }

            if(cache[100 + tileIdx] != 0 && IN->firstPressLMB && tileSetting == -1 && gameWorld->farmPlaced && gameWorld->farmlandPlaced >= 1) {
                tileSetting = tileIdx;
            }

            x += 32;

            // HOUSE
            dst = {x,145,16,16};
            tileIdx  = static_cast<int>(gameWorld->HOUSE1);
            TM->put(TM->get("overlayatlas"), gameWorld->getOverlayTile(gameWorld->HOUSE1).src, dst);
            if(IN->isHovered({dst.x,dst.y,dst.w,dst.h})) {
                cache[100 + tileIdx] += 1;
                if(cache[100 + tileIdx] > 24) {
                    boxToRender = {x - 20, 119, 125, 28};
                    line1 = "HOUSING - houses 5 people.";
                    line2 = "5 wood";
                }
                if(IN->firstPressLMB && tileSetting == -1 && gameWorld->farmPlaced && gameWorld->farmlandPlaced >= 2) {
                    tileSetting = tileIdx;
                }
            } else {
                cache[100 + tileIdx] = 0;
            }
            // x+= 16;

            // LUMBERJACK
            dst = {x,163,16,16};
            tileIdx  = static_cast<int>(gameWorld->LUMBERJACK);
            TM->put(TM->get("overlayatlas"), gameWorld->getOverlayTile(gameWorld->LUMBERJACK).src, dst);
            if(IN->isHovered({dst.x,dst.y,dst.w,dst.h})) {
                cache[100 + tileIdx] += 1;
                if(cache[100 + tileIdx] > 24) {
                    boxToRender = {x - 20, 119, 125, 28};
                    line1 = "LUMBERJACK - produce wood.";
                    line2 = "5 wood";
                }
                if(IN->firstPressLMB && tileSetting == -1 && gameWorld->farmPlaced && gameWorld->farmlandPlaced >= 2) {
                    tileSetting = tileIdx;
                }
            } else {
                cache[100 + tileIdx] = 0;
            }

            x+= 17;

            // FARM
            dst = {x,145,16,16};
            tileIdx  = static_cast<int>(gameWorld->FARM);
            TM->put(TM->get("overlayatlas"), gameWorld->getOverlayTile(gameWorld->FARM).src, dst);
            if(IN->isHovered({dst.x,dst.y,dst.w,dst.h})) {
                cache[100 + tileIdx] += 1;
                if(cache[100 + tileIdx] > 24) {
                    boxToRender = {x - 20, 119, 125, 28};
                    line1 = "FARM - Produce food / grain.";
                    line2 = "5 wood, 2 bricks";
                }
                if(IN->firstPressLMB && tileSetting == -1) {
                    tileSetting = tileIdx;
                }
            } else {
                cache[100 + tileIdx] = 0;
            }

            // BAKERY
            dst = {x,163,16,16};
            tileIdx  = static_cast<int>(gameWorld->BAKERY);
            TM->put(TM->get("overlayatlas"), gameWorld->getOverlayTile(gameWorld->BAKERY).src, dst);
            if(IN->isHovered({dst.x,dst.y,dst.w,dst.h})) {
                cache[100 + tileIdx] += 1;
                if(cache[100 + tileIdx] > 24) {
                    boxToRender = {x - 20, 115, 132, 28 + (gameWorld->breadUnlocked ? 0 : 6)}; // bit hacky
                    line1 = "BAKERY - Turn grain into bread.";
                    if(gameWorld->breadUnlocked) {line2 = "5 wood, 15 bricks";}
                    else {line2 = "Bread has to be unlocked\nvia the trader.";}
                }
                if(IN->firstPressLMB && tileSetting == -1 && gameWorld->breadUnlocked) {
                    tileSetting = tileIdx;
                }
            } else {
                cache[100 + tileIdx] = 0;
            }
            x += 17;

            // FARMLAND

            dst = {x,164,16,16};
            tileIdx  = static_cast<int>(gameWorld->FARMLAND);
            TM->put(TM->get("farmland"),{0,0,16,16}, dst);
            if(IN->isHovered({dst.x,dst.y,dst.w,dst.h})) {
                cache[100 + tileIdx] += 1;
                if(cache[100 + tileIdx] > 24) {
                    boxToRender = {x - 50, 128, 132, 28};
                    line1 = "FARMLAND - Used to plant crops.";
                    line2 = "2 workers.";
                }
                if(IN->firstPressLMB && tileSetting == -1 && gameWorld->farmPlaced) {
                    tileSetting = tileIdx;
                }
            } else {
                cache[100 + tileIdx] = 0;
            }

            // PLAINS

            dst = {x,150,16,16};
            tileIdx  = static_cast<int>(gameWorld->PLAINS);
            TM->put(TM->get("plains"),{0,0,16,16}, dst);
            if(IN->isHovered({dst.x,dst.y,dst.w,dst.h})) {
                cache[100 + tileIdx] += 1;
                if(cache[100 + tileIdx] > 24) {
                    boxToRender = {x - 50, 119, 132, 28};
                    line1 = "PLAINS  - Can be built upon.";
                    line2 = "4 workers.";
                }
                if(IN->firstPressLMB && tileSetting == -1 && gameWorld->farmPlaced && gameWorld->farmlandPlaced >= 2) {
                    tileSetting = tileIdx;
                }
            } else {
                cache[100 + tileIdx] = 0;
            }
            x += 16;

            // WATER

            dst = {x,150,16,16};
            tileIdx  = static_cast<int>(gameWorld->WATER);
            TM->put(TM->get("water"),{0,0,16,16}, dst);
            if(IN->isHovered({dst.x,dst.y,dst.w,dst.h})) {
                cache[100 + tileIdx] += 1;
                if(cache[100 + tileIdx] > 24) {
                    boxToRender = {x - 50, 119, 132, 28};
                    line1 = "WATER - Dig irrigation canals.";
                    line2 = "2 workers.";
                }
                if(IN->firstPressLMB && tileSetting == -1 && gameWorld->farmPlaced && gameWorld->farmlandPlaced >= 2) {
                    tileSetting = tileIdx;
                }
            } else {
                cache[100 + tileIdx] = 0;
            }

            if(boxToRender.w != 0) {
                GUI->renderBox(boxToRender);
                point = GUI->renderText(SDL_Point{boxToRender.x + 10,boxToRender.y + 8}, line1, 1);
                point = GUI->renderText(SDL_Point{boxToRender.x + 10,boxToRender.y + 15}, line2,1);
            }
        } else {
            TM->put(TM->get("blacktransparent"), {0,0,1,1}, {0,145,320,35});
        }
    }
    // GUI->renderText(SDL_Point{5,5}, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz | The quick brown fox jumps over the lazy dog The Quick Brown Fox Jumps Over The Lazy Dog THE QUICK BROWN FOX JUMPS OVER THE LAZY DOG", 1); // dogshit font testing
    if(SDL_GetTicks() < statusTimer && gameState == "ingame") {
        GUI->renderBox(statusbox);
        GUI->renderText(SDL_Point{1,1}, status, 1);
    }

    TM->put(TM->get("cursor"), {0,0,16,16}, {IN->mouse.x, IN->mouse.y, 16,16});

    if(gameWorld) {if(gameWorld->capitalLevel == 0 && !GUI->imageIsOn("black")) { tileSetting = tileManager::PAL1_TL; }}
}

void gameClass::startGame() {
    memset(cache, 0, sizeof(cache)); // cursed C function
    GUI->disableAll();

    resumeGame();

    GUI->enableBox("welcomebox");
    GUI->enableText("welcometext");

    cache[0] = SDL_GetTicks() + 3500; // when does the welomce stuff disappear?

    delete menuWorld;
    gameWorld = new tileManager(worldSize, genHash(cache[0]));
    camX = 0;
    camY = 0;
    // SDL_Point spawnPoint = gameWorld->getSpawn();
    // lol123super->setCoords(spawnPoint.x, spawnPoint.y);
}

void gameClass::openInventory() {
    GUI->disableAll();
    GUI->enableBox("inventory");
    GUI->enableText("inventory");
    GUI->enableText("inventoryState");
    GUI->enableImage("black");

    GUI->enableButton("inventory exit");
}

void gameClass::openCapitalMenu() { // made this function 3 days before writing a more general approach to opening building upgrade menus.
    GUI->disableAll(); // too lazy to port it lol. keeping it here.
    GUI->enableImage("black");
    GUI->enableButton("return");
    GUI->enableBox("capital");
    GUI->enableText("capital");

    std::string happiness;
    switch(gameWorld->happy) {
        case 0:
            happiness = "angry";
            break;
        case 1:
            happiness = "unhappy";
            break;
        case 2:
            happiness = "fine";
            break;
        case 3:
            happiness = "happy";
            break;
        case 4:
            happiness = "very happy";
            break;
        case 5:
            happiness = "extremely happy";
            break;
        default :
            happiness = "happier than possible"; // real
            break;
    }
    SDL_Rect src;
    std::string mats;
    switch(gameWorld->capitalLevel) {
        case 1:
            src = {0,64,32,32};
            mats = "\nRequired for upgrade:\n    20 wood\n    15 bricks\n+5 population cap\n0.5x fig production,2x grain production";
            break;
        case 2:
            src = {32,64,32,32};
            mats = "\nRequired for upgrade:\n    15 lapis\n    5 wood\n    10 bricks\n+5 population cap";
            break;
        case 3:
            src = {0,96,32,32};
            mats = "\nRequired for upgrade:\n    30 lapis\n    25 wood\n    25 bricks\n+10 population cap";
            break;
        case 4:
            src = {32,96,32,32};
            mats = "\nRequired for upgrade:\n    75 lapis\n    50 wood\n    125 bricks";
            break;
        case 5:
            src = {32,16,32,32};
            break;
        default:
            src = {0,0,32,32};
            break;
    }
    GUI->imageSetSrc("capital", src);

    GUI->changeText("capitalstatus", "CAPITOL LEVEL " + std::to_string(gameWorld->capitalLevel) + "\nPopulation: " + std::to_string(gameWorld->population) + "/" + std::to_string(gameWorld->cap) + "\nNo. of employed people: " + std::to_string(gameWorld->population - gameWorld->unemployed) + "\nYour people are " + happiness + ".\n" + mats);


    GUI->enableText("capitalstatus");
    if(gameWorld->capitalLevel < 5) GUI->enableButton("upgradecapital");
    GUI->enableImage("capital");
}

void gameClass::nextTurn() {
    gameWorld->handleTurn();
    if(gameWorld->lose) {
        GUI->disableAll();
        gameState = "lose"; // should've used enums for the gamestate. whatever
        GUI->enableBox("build");
        GUI->changeText("build", "You lose! Turn " + std::to_string(gameWorld->turn) + ".");
        GUI->changeText("buildstatus", gameWorld->losestring);
        GUI->enableButton("Pause Main Menu");
        GUI->enableText("build");
        GUI->enableText("buildstatus");
        GUI->enableImage("black");
    }
}

void gameClass::pauseGame() {
    GUI->disableAll();

    // setStatus("", 0);

    GUI->enableBox("pausebox");

    GUI->enableText("pausedtext0");

    GUI->enableButton("Pause Resume");
    GUI->enableButton("Pause Save");
    GUI->enableButton("Pause Main Menu");
    GUI->enableImage("black");
    gameState = "paused";
}

void gameClass::resumeGame() {
    GUI->disableAll(); // donkey ass UI management system tbh, still better than immediate mode lol

    // setStatus("", 0);

    GUI->enableBox("hotbar");
    GUI->enableText("hotbar");
    GUI->enableButton("showstats");
    GUI->enableButton("next");
    GUI->enableButton("pause");
    GUI->enableButton("music");
    GUI->enableButton("sound");
    gameState = "ingame";
}

void gameClass::doFrame() {
    Uint32 currentTick = SDL_GetTicks();
    float deltaTime = std::clamp((currentTick - lastTick) / 1000.0f, 0.0f, 0.25f);
    lastTick = currentTick;

    IN->update();

    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        IN->processEvent(event);
    }

    if (IN->quit) {
        running = false;
    }

    if (IN->keyPressed(SDL_SCANCODE_F11)) {
        Uint32 flags = SDL_GetWindowFlags(window);
        bool isFullscreen = flags & SDL_WINDOW_FULLSCREEN_DESKTOP;

        SDL_SetWindowFullscreen(window, isFullscreen ? 0 : SDL_WINDOW_FULLSCREEN_DESKTOP);

        IN->releaseKey(SDL_SCANCODE_F11);
    }

    if (IN->keyPressed(SDL_SCANCODE_F3)) {
        debug = !debug;
        if(debug) {
            GUI->enableText("debug");
        } else {
            GUI->disableText("debug");
        }
        IN->releaseKey(SDL_SCANCODE_F3);
    }

    if ((IN->keyPressed(SDL_SCANCODE_ESCAPE) || IN->keyPressed(SDL_SCANCODE_E)) && gameState != "mainmenu" && gameState != "lose") {
        if(gameState != "paused" && (tileSetting != -1 && tileSetting != tileManager::PAL1_TL)) {tileSetting = -1;}
        else if(gameState != "paused" && !GUI->imageIsOn("black")) {
            pauseGame();
        } else { resumeGame(); }
        IN->releaseKey(SDL_SCANCODE_ESCAPE);
        IN->releaseKey(SDL_SCANCODE_E);
    }

    accum += deltaTime;
    while (accum >= FIXED_TIMESTEP) {
        tick(); // run tick until you're all catched up
        // lol dont have tick exceed 1/60 seconds per frame or it'll freeze the pc from 1999, TODO: fix if we ever release to people
        accum -= FIXED_TIMESTEP;
    }

    SDL_SetRenderTarget(renderer, lowres); // set renderer to plot to the 320x180 game texture
    SDL_RenderClear(renderer); // clear the texture

    render();

    SDL_SetRenderTarget(renderer, NULL); // "hey render it to the real screen now!!!"
    SDL_RenderClear(renderer); // clear screen

    SDL_Rect dst = createDstRect();
    SDL_RenderCopy(renderer, lowres, NULL, &dst); // plot the lowres texture on the screen

    SDL_RenderPresent(renderer); // swap buffer

    Uint32 frameTime = SDL_GetTicks() - currentTick;

    if(frameTime == 0 && debug) {
        fps = aimFPS;
    } else if(debug) {
        fps = 1000 / frameTime;
    }

    if (frameTime < sleep) {
        SDL_Delay(sleep - frameTime);
    }
}

#ifdef __EMSCRIPTEN__
void run_main_loop(void* arg) {
    static_cast<gameClass*>(arg)->doFrame();
}
#endif

void gameClass::mainLoop() { // AI helped with the implementation of aimFPS and FIXED_TIMESTEP, thank you chat gpt
    SM->playMusic("bob", true); // just wanted a little soundtrack lol
    // this is just snail bob 3's music ripped from the SWF
    // if we ever continue development we're either gonna make some of our own shitty music with a solid music engine with events and context and all of that good stuff or we just rip some of Eitan Epstein's music.

    lastTick = SDL_GetTicks();
    movespeed = 5.0f * FIXED_TIMESTEP;

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop_arg(run_main_loop, this, 0, 1);
#else
    while(running) {
        doFrame(); // stuffed all the frame logic into a function for the sake of emscripten
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
#endif
}


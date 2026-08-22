// els.cpp
#include "headers.hpp"
#include "global.hpp"
#include "thing.hpp"
// GUI elements graveyard lol
void gameClass::initElements() {
    GUI->newButton(
        centeredRect(TM->toRect( TM->get("button_menu_start_normal"), 160, 50 )),
        "button_menu_start_normal",
        "button_menu_start_hover" ,
        "button_menu_start_click" ,
        [this](){startGame();},
        "Menu Start",
        "click"
    );

    GUI->newButton(
        centeredRect(TM->toRect( TM->get("button_menu_settings_normal"), 160, 67 )),
        "button_menu_settings_normal",
        "button_menu_settings_hover" ,
        "button_menu_settings_click" ,
        std::bind(explode, "settings yes"),
        "Menu Settings",
        "click"
    );

    GUI->newButton(
        centeredRect(TM->toRect( TM->get("button_menu_exit_normal"), 160, 84 )),
        "button_menu_exit_normal",
        "button_menu_exit_hover" ,
        "button_menu_exit_click" ,
        [this](){running = false;},
        "Menu Exit",
        "click"
    );

    // Pause stuff

    GUI->newButton(
        centeredRect(TM->toRect( TM->get("button_resume"), 160, 63 )),
        "button_resume",
        "button_resume_hover" ,
        "button_resume_click" ,
        [this](){resumeGame();},
        "Pause Resume",
        "click"
    );

    GUI->newButton(
        centeredRect(TM->toRect( TM->get("button_save"), 160, 80 )),
        "button_save",
        "button_save_hover" ,
        "button_save_click" ,
        std::bind(explode, "save yes"),
        "Pause Save",
        "click"
    );

    GUI->newButton(
        centeredRect(TM->toRect( TM->get("button_mainmenu"), 160, 97 )),
        "button_mainmenu",
        "button_mainmenu_hover" ,
        "button_mainmenu_click" ,
        [this](){quitToMenu();},
        "Pause Main Menu",
        "click",
        99999
    );

    GUI->newText(
        {83, 80},
        "name",
        1,
        "tradername"
    );
    GUI->newText(
        {137, 53},
        "a",
        1,
        "traderdialog"
    );
    GUI->newButton(
        {87,98,28,28},
        "empty","empty","empty",
        [this](){},
        "traderslot1",
        "click",99990
    );
    GUI->newButton(
        {87+39,98,28,28},
        "empty","empty","empty",
        [this](){},
        "traderslot2",
        "click",99990
    );
    GUI->newButton(
        {87+39*2,98,28,28},
        "empty","empty","empty",
        [this](){},
        "traderslot3",
        "click",99990
    );
    GUI->newButton(
        {87+39*3,98,28,28},
        "empty","empty","empty",
        [this](){},
        "traderslot4",
        "click",99990
    );

    GUI->newImage(
        TM->get("items"),
        {0,0,16,16},
        {92,98,16,16},
        "traderslot1",80000
    );
    GUI->newImage(
        TM->get("items"),
        {0,0,16,16},
        {92+39,98,16,16},
        "traderslot2",80000
    );
    GUI->newImage(
        TM->get("items"),
        {0,0,16,16},
        {92+39*2,98,16,16},
        "traderslot3",80000
    );
    GUI->newImage(
        TM->get("items"),
        {0,0,16,16},
        {92+39*3,98,16,16},
        "traderslot4",80000
    );

    GUI->newText({90, 114},"a{LA}",0,"traderslot1");
    GUI->newText({129, 114},"a{WO}",0,"traderslot2");
    GUI->newText({168, 114},"a{BR}",0,"traderslot3");
    GUI->newText({207, 114},"a{WH}",0,"traderslot4");

    GUI->newText({90 , 92},"a{LA}",1,"toptraderslot1");
    GUI->newText({129, 92},"a{WO}",1,"toptraderslot2");
    GUI->newText({168, 92},"a{BR}",1,"toptraderslot3");
    GUI->newText({207, 92},"a{WH}",1,"toptraderslot4");
    GUI->newImage(TM->get("merchantui"), {0,0,320,180}, {0,-19,SCREEN_W,SCREEN_H}, "tradermenu", 24000);
    // ---

    GUI->newImage(TM->get("blacktransparent"), {0,0,1,1}, {0, 0, SCREEN_W, SCREEN_H}, "black", 100);
    GUI->newImage(TM->get("background"), {0, 0, 320, 180}, {0, 0, SCREEN_W, SCREEN_H}, "menubackground", 1000);

    GUI->newText({64, 20}, "Welcome to MESOPOTAMIA!", 0, "welcometext", {0,0,320,200}, 81); // welcome
    GUI->newBox(SDL_Rect{53, 11, 214, 29}, "welcomebox", 80);

    GUI->newBox(SDL_Rect{90, 35, 140, 85}, "pausebox", 20000); // pause
    GUI->newText({130,41}, "PAUSED", 0, "pausedtext0");
    GUI->newBox({-16, 145, 352, 60}, "hotbar", 150); // hotbar

    GUI->newBox(SDL_Rect{80, 25, 160, 110}, "capital", 20020); // capital menu
    GUI->newText({120,31}, "The capitol.", 0, "capital");
    GUI->newText({267,156}, "    Stats:\n\nNext Turn:", 1, "hotbar");
    GUI->newText({88, 43}, "", 1, "capitalstatus");
    GUI->newImage(TM->get("overlayatlas"), {0,64,32,32}, {170,42,64,64}, "capital");

    GUI->newBox(SDL_Rect{80, 25, 170, 105}, "build", 20020); // build menu - kinda just the generic box used by a bunch of things to show statistics, or menus or whatever.
    GUI->newText({91,31}, "Build", 0, "build");
    GUI->newText({91,61}, "123", 0, "sacrifice");
    GUI->newText({91, 43}, "", 1, "buildstatus");

    GUI->newText({205,38}, "Active: ", 1, "active");
	
    // GUI->newBox({0, 0, 148, 40}, "tutorial");
    // GUI->newText({8,8})
    GUI->newButton(
        TM->toRect(TM->get("trash"), 232, 49),
        "trash",
        "trash_hover",
        "trash_click",
        [](){}, // function gets set in the tileManager::openMenu function
        "trash",
        "demolish"
    );

    GUI->newButton(
        TM->toRect(TM->get("checkon"), 232, 36),
        "empty",
        "empty",
        "empty",
        [](){}, // function gets set in the tileManager::openMenu function
        "active",
        "demolish"
    );

    GUI->newButton( // statistics + what do you have to sacrifice next?
        TM->toRect(TM->get("stats"), 307, 152),
        "stats",
        "stats_hover",
        "stats_click",
        [this](){
            gameWorld->openStats();
        },
        "showstats",
        "click"
    );

    GUI->newButton(
        TM->toRect(TM->get("next"), 90, 69),
        "next",
        "next_hover",
        "next_click",
        [](){},
        "farmchangecrop",
        "farmland"
    );

    GUI->newButton(
        TM->toRect( TM->get("accept"), 93, 112 ),
        "accept",
        "accept_hover",
        "accept_click" ,
        [this](){
            gameWorld->openTraderMenu();
        },
        "traderpromptaccept",
        "click"
    );
    GUI->newButton(
        TM->toRect( TM->get("decline"), 111, 112 ),
        "decline",
        "decline_hover",
        "decline_click" ,
        [this](){resumeGame();},
        "traderpromptdecline",
        "click"
    );
    GUI->newButton(
        TM->toRect( TM->get("return"), 307, 1 ),
        "return",
        "return_hover",
        "return_click" ,
        [this](){resumeGame();},
        "return",
        "click"
    );

    GUI->newButton(
        TM->toRect( TM->get("next"), 307, 166 ),
        "next",
        "next_hover",
        "next_click" ,
        [this](){
            if(gameWorld->capitalLevel != 0) {
                nextTurn();
            } else {
                status = "Place your capital first.";
                statusTimer = SDL_GetTicks() + 2000;
            }
        },
        "next",
        "click",
        199
    );

    GUI->newButton(
        TM->toRect( TM->get("pause"), 307, 1 ),
        "pause",
        "pause_hover",
        "pause_click",
        [this](){pauseGame();},
        "pause",
        "click"
    );

    GUI->newButton(
        {0, 145, 320, 35},
        "empty",
        "empty",
        "empty",
        [this](){tileSetting = -1;},
        "cancel",
        "click"
    );

    GUI->newButton(
        {281, 1, 12, 12},
        "sound",
        "sound_hover",
        "sound_click",
        [this](){
            SM->toggleSound();
            if(SM->soundOn) {
                GUI->buttonChangeTexture("sound", "sound_hover", "sound_click", "sound");
            } else {
                GUI->buttonChangeTexture("sound_off", "sound_off_hover", "sound_off_click", "sound");
            }
        },
        "sound",
        "click"
    );

    GUI->newButton(
        {294, 1, 12, 12},
        "music",
        "music_hover",
        "music_click",
        [this](){
            SM->toggleMusic();
            if(SM->musicOn) {
                GUI->buttonChangeTexture("music", "music_hover", "music_click", "music");
            } else {
                GUI->buttonChangeTexture("music_off", "music_off_hover", "music_off_click", "music");
            }},
        "music",
        "click"
    );

    GUI->newButton(
        centeredRect(TM->toRect( TM->get("upgrade"), 160, 115 )),
        "upgrade",
        "upgrade_hover",
        "upgrade_click",
        [this](){
            gameWorld->upgradeCapital();
            resumeGame();
            openCapitalMenu();
        },
        "upgradecapital",
        "click"
    );

    GUI->newButton(
        centeredRect(TM->toRect( TM->get("upgrade"), 160, 110 )),
        "upgrade",
        "upgrade_hover",
        "upgrade_click",
        [this](){},
        "upgrade",
        "click"
    );
    GUI->newText({5,5}, "", 1, "debug");
}

// global.hpp
// includes all dependencies in a single line, and has some nice global functions and variables.
#pragma once

#define SCREEN_W 320
#define SCREEN_H 180

#ifdef __EMSCRIPTEN__ // web compilation!!! YAY!
#include <emscripten.h>
#endif

#include "headers.hpp"

#include "input.hpp"
#include "sound.hpp"
#include "texture.hpp"
#include "button.hpp"
#include "box.hpp"
#include "text.hpp"
#include "image.hpp"
#include "elements.hpp"
#include "world.hpp"
#include "entity.hpp"

extern inputManager* IN;
extern soundManager* SM;
extern textureManager* TM;
extern guiManager* GUI;
extern tileManager* gameWorld;

extern Entity* lol123super;

extern SDL_Window* window;
extern SDL_Renderer* renderer;

extern siv::PerlinNoise* gperlin;

extern float camX, camY;
extern float camZoom;
extern float seed;
extern float movespeed;
extern int worldSize;
extern float fps;
extern int tileSetting;
extern bool debug;
extern std::string status;
extern SDL_Rect statusbox;
extern int statusTimer;

extern float cache[1024]; // just fill with random shit instead of making a thousand global variables

// global functions / tools / debug things
float getWindowScale(); // min(winwidth / 320, winheight / 180)
SDL_Point scalePoint(SDL_Point point, bool borders = true); // scale a point on the window to the lowres thing
SDL_Rect centeredRect(int x, int y, int w, int h); // make x, y centered instead of top left
SDL_Rect centeredRect(SDL_Rect rect);
SDL_Rect rectCentered(int x, int y, int w, int h); //make x,y top left instead of ecntered
SDL_Rect rectCentered(SDL_Rect rect); // great naming
void explode(std::string string); // beautiful debug for buttons
int genHash(int x, int y =0 ); // 2 ints to 1 int hash
void setStatus(std::string statusM, int time = 2000, SDL_Rect box = {-10, -10, -1, -1});
std::string getpath(const std::string& subDir = "");
float fsqrt(float x);

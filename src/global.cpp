// global.cpp

#include "global.hpp"

SDL_Window* window;
SDL_Renderer* renderer;
Entity* lol123super;
siv::PerlinNoise* gperlin = new siv::PerlinNoise(1234);

float camZoom = 1;
float camX = 0; float camY = 0;
float seed = 1234;
bool debug = false;
float movespeed;
float fps = 60;
int worldSize;
int tileSetting = -1;
float cache[1024]{};
int statusTimer;
std::string status = "";
SDL_Rect statusbox;

float getWindowScale() {
    int w, h;
    SDL_GetWindowSize(window, &w, &h);

    float scaleWidth = w / static_cast<float>(SCREEN_W);
    float scaleHeight = h / static_cast<float>(SCREEN_H);
    return std::min(scaleWidth, scaleHeight);
}

SDL_Point scalePoint(SDL_Point point, bool borders) { // scale a set of coordinates to fit the 320x180 window
    int w, h;                                         // mainly just for the mouse but might be handy if we ever add higher resolution stuff
    SDL_GetWindowSize(window, &w, &h);

    float scaleWidth = w / static_cast<float>(SCREEN_W);
    float scaleHeight = h / static_cast<float>(SCREEN_H);

    float scale = std::min(scaleWidth, scaleHeight);

    point.x /= scale;
    point.y /= scale;

    // std::cout << scale << point.x << point.y << std::endl;
    if(borders) { // borders boolean is wether or not we should take the borders into account.
        if(scaleWidth > scaleHeight) {
            point.x -= (scaleWidth - scaleHeight) * (SCREEN_W * 0.5 / scale);
        } else {
            point.y -= (scaleHeight - scaleWidth) * (SCREEN_H * 0.5 / scale);
        } // sexy
        return SDL_Point{std::clamp(point.x, -1, SCREEN_W + 1), std::clamp(point.y, -1, SCREEN_H + 1)};
    } else {
        return point;
    }
}

SDL_Rect centeredRect(int x, int y, int w, int h) { // center rect so i can make stuff look good without trying 20000 combinations or doing m*th
    return SDL_Rect{
        x - w / 2,
        y - h / 2,
        w,
        h
    };
}

SDL_Rect centeredRect(SDL_Rect rect) { // same thing but different input xd
    return SDL_Rect{
        rect.x - rect.w / 2,
        rect.y - rect.h / 2,
        rect.w,
        rect.h
    };
}

SDL_Rect rectCentered(SDL_Rect rect) {
    return SDL_Rect{
        rect.x + rect.w / 2,
        rect.y + rect.h / 2,
        rect.w,
        rect.h
    };
}

SDL_Rect rectCentered(int x, int y, int w, int h) {
    return SDL_Rect{
        x + w / 2,
        y + h / 2,
        w,
        h
    };
}

std::string getpath(const std::string& subDir) {
    char* base = SDL_GetBasePath();
    std::string path;

    if (base) {
        path = base;
        SDL_free(base); // fucking hate C++ memory management
    } else {
        std::cerr << "SDL_GetBasePath: " << SDL_GetError() << std::endl;
        return "./";
    }

    return path + subDir;
}

void explode(std::string string) {
    std::cout << string << std::endl;
}

int genHash(int x, int y) {
    x = abs(x * 25) % 10000;
    y = abs(y * 25) % 10000;
    int temp = x * (983 + seed * 4.2) + y * (1558 + seed * 6.9);

    temp = (temp ^ (temp >> 13)) * 23495;
    temp = temp ^ (temp >> 16); // magic bitwise operator shit - it's fine to copy weird hash functions from the internet I guess
    return temp;
}

void setStatus(std::string statusM, int time, SDL_Rect box) {
    status = statusM;
    if(box.w == -1 && box.h == -1) {box.w = GUI->getTextLen(status, 1) + 28;box.h = 22;}
    statusbox = box;
    if(time != -1) {
        statusTimer = time + SDL_GetTicks();
    }
    else {
        statusTimer = 1569325055; // the compiler output gave this to me as the maximum integer limit
    }
}

float fsqrt(float x) { // legendary quake 3 stuff - its probably not necessary to optimize fsqrt like this
    float xhalf = 0.5f * x;
    int i = *(int*)&x;        // evil bit hack (for real what the fuck)
    i = 0x5f3759df - (i >> 1);
    float y = *(float*)&i;
    y = y * (1.5f - xhalf * y * y); // 1 Newton iteration
    return x * y; // sqrt(x) ≈ x * (1/sqrt(x))
}

// text.hpp
// little text manager and renderer
#pragma once
#ifndef text_hpp
#define text_hpp

#include "headers.hpp"
#include "global.hpp"

class Text {
    public:
        Text(SDL_Point& pos, const std::string& string, SDL_Texture* tex, int fw, int fh, std::map<std::string, SDL_Rect>& charMap, SDL_Rect bound, const std::string& ID);

        SDL_Point render();

        std::string id;
        std::string content;
        bool active = false;
    private:
        SDL_Texture* texture;
        int w, h;
        SDL_Rect box;
        std::map<std::string, SDL_Rect> characterSrc;

        SDL_Point position;

        void renderCharacter(std::string& character, int x, int y);
};
#endif

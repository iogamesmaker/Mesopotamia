#include <headers.hpp>
#include <global.hpp>
#include "text.hpp"

Text::Text(SDL_Point& pos, const std::string& string, SDL_Texture* tex, int fw, int fh, std::map<std::string, SDL_Rect>& charMap, SDL_Rect bound, const std::string& ID) {
    id = ID;
    w = fw;
    h = fh;
    position = pos;
    content = string;
    texture = tex;
    characterSrc = charMap;
    box = bound;
}

SDL_Point Text::render() {
    if(!active) return {-1, -1};
    int drawerX = position.x;
    int drawerY = position.y;

    for(int i = 0; i < content.size(); i++) {
        std::string character = {content[i]};
        if(content[i] == '{' && content[i + 3] == '}') {
            std::string code = {content[i + 1], content[i + 2]};
            i+= 3;
            if(code == "WO") character = "Þ"; // wood
            if(code == "BR") character = "¢"; // bricks
            if(code == "BD") character = "¤"; // bread
            if(code == "FI") character = "⊰"; // figs
            if(code == "GR") character = "⊱"; // grape
            if(code == "WH") character = "⤚"; // wheat/grain
            if(code == "LA") character = "⤙"; // lapis
            if(code == "PL") character = "¡"; // people (people work on something for a turn, can't be employed elsewhere for that turn)
            if(code == "SL") character = "»"; // slaves (people count goes down, people get sold)
        }
        if((drawerX + characterSrc[character].w + 1 > box.x + box.w) || character == "\n") {
            drawerX = position.x - 1;
            drawerY += h + 1;
            if(drawerY + h > box.y + box.h) return {drawerX, drawerY};;
        }
        renderCharacter(character, drawerX, drawerY);
        drawerX += characterSrc[character].w + 1;
    }

    return {drawerX, drawerY};
}

void Text::renderCharacter(std::string& character, int x, int y) {

    SDL_Rect src = characterSrc[character];
    SDL_Rect dst = {x, y, src.w, src.h};

    TM->put(texture, &src, &dst);
}

// really happy with how compact this class is lol

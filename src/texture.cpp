// texture.cpp
#include "global.hpp"
// TODO: add support for animated textures. don't know how but yeah.
//  - i'd rather have each animated texture run its own "clock", it needs to have consistent timing and shit but it'd be lame if each texture went to its next frame on the exact same game tick like twice a second
//      - to do that

textureManager::textureManager() { // chat gpt partially
    std::ifstream file(getpath("assets/assets.txt"));

    if (!file.is_open()) {
        SDL_Log("failed to open assets file: assets/assets.txt");
        return;
    }

    std::string name, arrow, location;
    while (file >> name >> arrow >> location) { // null.png > NULL
        if(name == "#" || name == "") {
            continue;
        }

        std::string fullPath = getpath("assets/" + location);
        SDL_Texture* tex = IMG_LoadTexture(renderer, fullPath.c_str()); // has to be a pointer because of SDL weirdness. the texture isn't actually stored in the variable - it's in VRAM. This is just a way to access it.
        if (!tex) {
            SDL_Log("failed to load texture '%s' from '%s': %s",
                    name.c_str(), fullPath.c_str(), IMG_GetError()); // error handling 🤮🤮🤮🤮
            continue;
        }
        textures[name] = tex; // add the texture to array
        nameToPath[name] = location;
    }
}

textureManager::~textureManager() { // chat gpt
    for (auto& [name, tex] : textures) {
        SDL_DestroyTexture(tex);
    }
}

void textureManager::put(SDL_Texture* texture, const SDL_Rect* src, const SDL_Rect* dst) {
    // if(dst == NULL) { // no dst rect selected? just render it to the top left and call it a day. can't imagine how this was useful
    //     SDL_Rect notapointer = toRect(texture);
    //     dst = &notapointer; // duct tape kinda solution but a solution nonetheless!!!
    // }
    SDL_RenderCopy(renderer, texture, src, dst);
}
void textureManager::put(SDL_Texture* texture, const SDL_Rect src, const SDL_Rect dst) {
    SDL_RenderCopy(renderer, texture, &src, &dst);
}
void textureManager::loadTexture(const std::string& texturePath, const std::string& textureName) { // adapted from tutorial Lazy Foo' Productions
    SDL_Texture* texture;
    SDL_Surface* surface = IMG_Load((getpath("assets/" + texturePath)).c_str()); // get the raw image into RAM

    texture = SDL_CreateTextureFromSurface(renderer, surface); // create a texture out of it and stuff it into the vram
    SDL_FreeSurface(surface); // remove the image from RAM

    if(!texture) {
        std::cout << "failed to load texture " << textureName << std::endl;
        return;
    }
    textures[textureName] = texture;
}

SDL_Rect textureManager::toRect(SDL_Texture* texture, int x, int y) {
    int w, h;
    SDL_QueryTexture(texture, nullptr, nullptr, &w, &h);
    return SDL_Rect{x, y, w, h};
}

SDL_Texture* textureManager::get(const std::string& textureName) { // chat gpt helped improve this to reduce lookups - free 0.01% performance boost in the end product!!!!
    auto iter = textures.find(textureName);
    return iter != textures.end() ? iter->second : textures.at("NULL");
}

SDL_Surface* textureManager::getSurface(const std::string& textureName) { // rarely used. dont use it often it's slow as shit
    SDL_Surface* surface = IMG_Load(getpath(("assets/" + nameToPath[textureName])).c_str());

    if(surface == nullptr) std::cout << "surface of " << nameToPath[textureName] << " returned null" << std::endl;
    return surface;
}

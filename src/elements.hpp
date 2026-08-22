// elements.hpp
// handle all buttons / gui shits that will get added, delete 'em all at once and that kind of stuff.
#pragma once
#ifndef elements_hpp
#define elements_hpp

#include "headers.hpp"
#include "global.hpp"

class guiManager {
    public:
        guiManager();
        ~guiManager() = default;

        // -- buttons

        void newButton(SDL_Rect rect, const std::string& one, const std::string& two, const std::string& three, std::function<void()> callback, const std::string& id, const std::string& sound = "null", int z = 50000);
        // When a button got removed while a function using that button was coming up BEFORE updateButtons was called, it would crash out and give me a segfault.
        // Solution: don't remove buttons at all :) Bad for memory, maybe, but it's only a couple megabytes at most.
        // old comments but yeah I kept that and I'm just gonna call it object pooling instead of not being a lazy fuck and fixing it!!!

        bool buttonIsOn(const std::string& id);

        void enableButton(const std::string& id);
        void disableButton(const std::string& id);

        void disableAllButtons();

        void buttonChangeFunction(const std::string& id, std::function<void()> callback);

        void buttonChangeTexture(const std::string& normal, const std::string& hover, const std::string& click, const std::string& id);

        // -- boxes

        void newBox(SDL_Rect rect, const std::string& id, int z = 50000);
        void enableBox(const std::string& id);
        void disableBox(const std::string& id);
        void renderBox(SDL_Rect rect);
        Box* getBox(const std::string& id);

        void disableAllBoxes();

        // -- text shits

        void newText(SDL_Point position, const std::string& content, int fontidx, const std::string& id, SDL_Rect bound = {0, 0, SCREEN_W, SCREEN_H}, int z = 50000);

        void enableText(const std::string& id);
        void disableText(const std::string& id);


        void disableAllText();

        int getTextLen(const std::string& string, int font);

        SDL_Point renderText(SDL_Point position, const std::string& content, int fontidx, SDL_Rect bounds = {0,0,SCREEN_W,SCREEN_H});
        void changeText(const std::string& id, const std::string& content);
        // -- images

        void newImage(SDL_Texture* texture, SDL_Rect src, SDL_Rect dst, const std::string& id, int z = 50000);

        void enableImage(const std::string& id);
        void disableImage(const std::string& id);

        void changeImage(const std::string& id, SDL_Texture* texture);

        void imageSetSrc(const std::string& id, SDL_Rect src);
        Image* getImage(const std::string& id);
        bool imageIsOn(const std::string& id);

        void disableAllImages();

        // -- general
        void update();
        void sort();

        void renderAll();
        void disableAll();

        bool top;

    private:
        bool shouldSort = true; // should we resort the elemnets this frame
        struct Element { // it's like programming in HTML!!!!! incredible
            int z;
            int index;
            int type;
        };


        std::vector<Element> order;

        void loadFont(std::string textureName, int w, int h, std::vector<std::string> longasslist);
        struct Font {
            SDL_Texture* texture;
            int w, h;
            std::map<std::string, SDL_Rect> characterMap;

        };

        std::vector<Font> fonts;

        std::vector<std::unique_ptr<Text>> texts;
        std::vector<std::unique_ptr<Button>> buttons; // hell yeah unique ptr
        std::vector<std::unique_ptr<Box>> boxes;
        std::vector<std::unique_ptr<Image>> images;

        std::map<int, int> elements; // index, z value
};
#endif

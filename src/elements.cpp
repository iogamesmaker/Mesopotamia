// elements.cpp
#include "global.hpp"

guiManager::guiManager() {
    // load fonts
    loadFont("font910", 9, 10, {"A", "B", "C", "D", "E", "F", "G", "H", "I", "ä", "ë", "ï", "ö", "ü", "ç", "█", "█", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "â", "ê", "î", "ô", "û", "█", "█", "█", "S", "T", "U", "V", "W", "X", "Y", "Z", "█", "á", "é", "í", "ó", "ú", "█", "█", "█", "a", "b", "c", "d", "e", "f", "g", "h", "i", "à", "è", "ì", "ò", "ù", "█", "█", "█", "j", "k", "l", "m", "n", "o", "p", "q", "r", "◂", "◃", "ĩ", "█", "█", "ñ", "█", "█", "s", "t", "u", "v", "w", "x", "y", "z", "█", "<", ">", "█", "█", "█", "█", "█", "█", "?", "!", ".", ",", "#", "'", "\"", "^", "0", "[", "]", "(", ")", " ", "☺", "☹", "█", "1", "2", "3", "4", "5", "6", "7", "8", "9", "{", "}", ":", ";", "&", "⌂", "»", "█", "ß", "←", "→", "↑", "↓", "+", "-", "%", "/", "\\", "@", "$", "€", "*", "=", "~", "`", "Þ", "⤙", "⤚", "⊱", "⊰", "¤", "¢", "¡", "█", "█", "█", "█", "█", "█", "█", "█", "█"}); // font #0
    loadFont("font35", 3, 5, {"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "!", "?", "@", "#", "$", "(", ")", "[", "]", "<", ">", "=", "{", "}", "+", "*", "%", "/", "\\", "&", "-", "_", "'", "\"", ":", ";", ",", ".", "`", "~", "^", " "});
}


void guiManager::loadFont(std::string textureName, int w, int h, std::vector<std::string> longasslist) {
    int tw, th;
    std::map<std::string, SDL_Rect> characterMap;
    SDL_QueryTexture(TM->get(textureName), NULL, NULL, &tw, &th);
    SDL_Surface* textureSurface = TM->getSurface(textureName);
    SDL_Surface *fontSurface =  SDL_ConvertSurfaceFormat(textureSurface, SDL_PIXELFORMAT_RGBA8888, 0);
    SDL_FreeSurface(textureSurface);

    Uint32 *pixels = (Uint32 *)fontSurface->pixels;
    int pitch = fontSurface->pitch / 4;

    Uint32 pixel;
    Uint8 r,g,b, alpha;

    SDL_Rect rect = {0, 0, w, h};

    for(int i = 0; i < longasslist.size(); i++) {
        std::string character = longasslist[i]; // the character is a string instead of a character because of fancy unicode symbols that aren't supported with regular old char, and char32 is just a pain.
        SDL_Rect finalRect = rect; // no longer monospaced !!!
        if((w <= 5 || h <= 5) && character == " ") {
        } else {
            bool leftTransparent = true;
            bool leftTransparent2 = true;
            bool rightTransparent = true;
            bool rightTransparent2 = true;
            for(int y = rect.y; y < rect.y + h; y++) { // messy but OK
                pixel = pixels[y * pitch + rect.x];
                alpha = pixel & 0xFF;
                if(alpha) leftTransparent = false;

                pixel = pixels[y * pitch + rect.x + 1];
                alpha = pixel & 0xFF;
                if(alpha) leftTransparent2 = false;

                pixel = pixels[y * pitch + rect.x + w - 1];
                alpha = pixel & 0xFF; // got a segfault when trying to access a pixels alpha data via an SDL function but it works like this lol
                if(alpha) rightTransparent = false;

                pixel = pixels[y * pitch + rect.x + w - 2];
                alpha = pixel & 0xFF;
                if(alpha) rightTransparent2 = false;
            }

            if(leftTransparent) {
                finalRect.x += leftTransparent2 ? 2 : 1;
                finalRect.w -= leftTransparent2 ? 2 : 1;
            }
            if(rightTransparent) {
                finalRect.w -= rightTransparent2 ? 2 : 1;
            }
        }

        characterMap[character] = finalRect;
        rect.x += w;
        if(rect.x >= tw) {
            rect.x = 0;
            rect.y += h;
            if (rect.y + h > th) {
                break;
            }
        }

    }
    fonts.push_back({TM->get(textureName), w, h, characterMap});
    SDL_FreeSurface(fontSurface);
}

// --

void guiManager::newButton(SDL_Rect rect,
                           const std::string& one,
                           const std::string& two,
                           const std::string& three,
                           std::function<void()> callback,
                           const std::string& id,
                           const std::string& sound,
                           int z)
{
    if(buttons.size() > 9000) {
        std::cout << "buttons are about to go crazy" << std::endl;
    }
    Button* butt = new Button(rect, one, two, three, callback, id, sound);
    buttons.push_back(std::unique_ptr<Button>(butt));
    elements[10000 + buttons.size()] = z; // may not have more than 9999 buttons, careful with procedural buttons in the future to not have them leak. + memory management is gonna be a pain

    shouldSort = true;
}

void guiManager::buttonChangeTexture(const std::string& normal, const std::string& hover, const std::string& click, const std::string& id) {
    for (int i = 0; i < buttons.size(); i++) {
        if (buttons[i]->id == id) {
            buttons[i]->normal= normal;
            buttons[i]->hover = hover;
            buttons[i]->click = click;
            break;
        }
    }
}

void guiManager::buttonChangeFunction(const std::string& id, std::function<void()> callback) {
    for (int i = 0; i < buttons.size(); i++) {
        if (buttons[i]->id == id) {
            buttons[i]->func = callback;
            break;
        }
    }
}

void guiManager::disableButton(const std::string& id) {
    for (int i = 0; i < buttons.size(); i++) {
        if (buttons[i]->id == id) {
            buttons[i]->active = false;
            break;
        }
    }
}

void guiManager::enableButton(const std::string& id) {
    for (int i = 0; i < buttons.size(); i++) {
        if (buttons[i]->id == id) {
            buttons[i]->active = true;
            break;
        }
    }
}

void guiManager::disableAllButtons() {
    for (int i = 0; i < buttons.size(); i++) {
        buttons[i]->active = false;
    }
}

bool guiManager::buttonIsOn(const std::string& id) {
    for (int i = 0; i < buttons.size(); i++) {
        if (buttons[i]->id == id) {
            return buttons[i]->active;
        }
    }
    return false;
}

// -- boxes

void guiManager::newBox(SDL_Rect rect, const std::string& id, int z)  {
    if(boxes.size() > 9000) {
        std::cout << "boxes are about to go crazy" << std::endl;
    }
    Box* newBox = new Box(rect, id);
    boxes.push_back(std::unique_ptr<Box>(newBox));
    elements[20000 + boxes.size()] = z; // !

    shouldSort = true;
}

void guiManager::enableBox(const std::string& id) {
    for (int i = 0; i < boxes.size(); i++) {
        if (boxes[i]->id == id) {
            boxes[i]->active = true;
            break;
        }
    }
}

void guiManager::disableBox(const std::string& id) {
    for (int i = 0; i < boxes.size(); i++) {
        if (boxes[i]->id == id) {
            boxes[i]->active = false;
            break;
        }
    }
}

void guiManager::renderBox(SDL_Rect rect) { // just a little hack for rendering box w/o setting up a whole seperate element
    Box* box = new Box(rect, "");
    box->active = true;
    box->update();
    box->render();
    delete box;
}

void guiManager::disableAllBoxes() {
    for (int i = 0; i < boxes.size(); i++) {
        boxes[i]->active = false;
    }
}


Box* guiManager::getBox(const std::string& id) {
    for (int i = 0; i < boxes.size(); i++) {
        if (boxes[i]->id == id) {
            return boxes[i].get();
            break;
        }
    }
    return nullptr;
}

// text

void guiManager::newText(SDL_Point position, const std::string& content, int fontidx, const std::string& id, SDL_Rect bounds, int z) {
    if(texts.size() > 9000) {
        std::cout << "text is about to go crazy" << std::endl;
    }
    Text* text = new Text(position, content, fonts[fontidx].texture, fonts[fontidx].w, fonts[fontidx].h, fonts[fontidx].characterMap, bounds, id);

    texts.push_back(std::unique_ptr<Text>(text));
    elements[30000 + texts.size()] = z;

    shouldSort = true;
}

void guiManager::enableText(const std::string& id) {
    for (int i = 0; i < texts.size(); i++) {
        if (texts[i]->id == id) {
            texts[i]->active = true;
            break;
        }
    }
}

void guiManager::disableText(const std::string& id) {
    for (int i = 0; i < texts.size(); i++) {
        if (texts[i]->id == id) {
            texts[i]->active = false;
            break;
        }
    }

    if(debug) GUI->enableText("debug");
}

void guiManager::disableAllText() {
    for (int i = 0; i < texts.size(); i++) {
        texts[i]->active = false;
    }
    if(debug) GUI->enableText("debug");
}

void guiManager::changeText(const std::string& id, const std::string& content) {
    for (int i = 0; i < texts.size(); i++) {
        if (texts[i]->id == id) {
            texts[i]->content = content;
            break;
        }
    }
}

int guiManager::getTextLen(const std::string& string, int font) {
    int len = 0;

    for(int i = 0; i < string.size(); i++) {
        std::string character(1, string[i]);
        len += fonts[font].characterMap[character].w + 1;
    }

    return len;
}

SDL_Point guiManager::renderText(SDL_Point position, const std::string& content, int fontidx, SDL_Rect bounds) { // just a little hack for rendering text w/o setting up a whole seperate element
    Text* text = new Text(position, content, fonts[fontidx].texture, fonts[fontidx].w, fonts[fontidx].h, fonts[fontidx].characterMap, bounds, "");
    text->active = true;
    SDL_Point returnP = text->render();
    delete text;
    return returnP; // return the cooridnates - it's useful yes
}
// images

void guiManager::newImage(SDL_Texture* texture, SDL_Rect src, SDL_Rect dst, const std::string& id, int z) {
    if(images.size() > 9000) {
        std::cout << "text is about to go crazy" << std::endl;
    }
    Image* image = new Image(texture, src, dst, id);

    images.push_back(std::unique_ptr<Image>(image));
    elements[40000 + images.size()] = z;

    shouldSort = true;
}

void guiManager::changeImage(const std::string& id, SDL_Texture* texture) {
    for (int i = 0; i < images.size(); i++) {
        if (images[i]->id == id) {
            images[i]->image = texture;
            break;
        }
    }
}

void guiManager::enableImage(const std::string& id) {
    for (int i = 0; i < images.size(); i++) {
        if (images[i]->id == id) {
            images[i]->active = true;
            break;
        }
    }
}

void guiManager::disableImage(const std::string& id) {
    for (int i = 0; i < images.size(); i++) {
        if (images[i]->id == id) {
            images[i]->active = false;
            break;
        }
    }
}
bool guiManager::imageIsOn(const std::string& id) {
    for (int i = 0; i < images.size(); i++) {
        if (images[i]->id == id) {
            return images[i]->active;
        }
    }
    return false;
}

void guiManager::imageSetSrc(const std::string& id, SDL_Rect src) {
    for (int i = 0; i < images.size(); i++) {
        if (images[i]->id == id) {
            images[i]->source = src;
        }
    }
}

Image* guiManager::getImage(const std::string& id) {
    for (int i = 0; i < images.size(); i++) {
        if (images[i]->id == id) {
            return images[i].get();
            break;
        }
    }
    return nullptr;
}

void guiManager::disableAllImages() {
    for (int i = 0; i < images.size(); i++) {
        images[i]->active = false;
    }
}


// stuff
void guiManager::sort() {
    for(int i = 0; i < buttons.size(); ++i) { // button
        int z = elements[10001 + i];
        order.push_back({z,i,1}); // z, index, type
    }
    for(int i = 0; i < boxes.size(); ++i) { // box
        int z = elements[20001 + i];
        order.push_back({z,i,2});
    }
    for(int i = 0; i < texts.size(); ++i) { // text
        int z = elements[30001 + i];
        order.push_back({z,i,3});
    }
    for(int i = 0; i < images.size(); ++i) { // images
        int z = elements[40001 + i];
        order.push_back({z,i,4});
    }
    std::sort(order.begin(), order.end(), // chat gpt fkjgkfjgkdjfdkj - i might rewrite this but it looks more efficient than what i could come up with so yeahhh
        [](const Element& a, const Element& b) {
            return a.z < b.z;
    });
}


void guiManager::update() { // goal of this function is to have layered buttons / boxes, and make it so that you can't click a button under a box or another button or whatever.
    top = true;
    for (int i = order.size() - 1; i >= 0; i--) {
        auto thing = order[i];
        if (thing.type == 1) {
            if(top) {
                buttons[thing.index]->update();
                if(buttons[thing.index]->hovered) top = false;
            } else {
                buttons[thing.index]->hovered = false;
                buttons[thing.index]->clicked = false;
            }
        } else if (thing.type == 2) { // should maybe change the numbers to enum later lol
            if(top) {
                boxes[thing.index]->update();
                if(boxes[thing.index]->hovered) top = false;
            } else {
                boxes[thing.index]->hovered = false;
            }
        } else if (thing.type == 3) { // do nothing text can just sit there i dont care if an object is selected through a text boo hoo

        } else if (thing.type == 4) { // image
            if(top) {
                images[thing.index]->update();
                if(images[thing.index]->hovered) top = false;
            } else {
                images[thing.index]->hovered = false;
            }
        }
    }
    if(shouldSort) {
        sort();
        shouldSort = false;
    }
}

void guiManager::renderAll() {
    for (int i = 0; i < order.size(); i++) { // hell yeah
        auto thing = order[i];
        if (thing.type == 1) {
            buttons[thing.index]->render();
        } else if (thing.type == 2) {
            boxes[thing.index]->render();
        } else if (thing.type == 3) {
            texts[thing.index]->render();
        } else if (thing.type == 4) {
            images[thing.index]->render();
        }
    }
}

void guiManager::disableAll() {
    disableAllBoxes();
    disableAllButtons();
    disableAllText();
    disableAllImages();
}

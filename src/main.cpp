// main.cpp
// Started development on 2025-08-16, 20:36
// First segfault 8(!) days later.
// PIRATES: SILVER
// nope; Settlers of Mesopotamia
// nope; Kings of Gwynedd
// mars terraformer idea for a grand total of 2 hours, made the world gen and a simple underground tile system (which I kept and modified for the overlay tiles for buildings and stuff) :)
// nope; (Settlers of) Mesopotamia
// https://lazyfoo.net/tutorials/SDL/index.php is a great source
#include "global.hpp"
#include "thing.hpp"

inputManager* IN = nullptr; // these are global because i'm only gonna make one of each, and if they're global i can call them everywhere without having to juggle pointers between classes.
soundManager* SM = nullptr; // they used to be private inside of gameClass. then i had to pass pointers of classes everywhere and that wasn't too handy.
textureManager* TM = nullptr;
guiManager* GUI = nullptr;
tileManager* gameWorld = nullptr;

int main(int argc, char* argv[]) {
    gameClass* game = new gameClass(); // Pointer. Ran into an issue with my older DSS project that was solved by making pointers out of all the classes. It's fine.
    delete game;
    return 0;                          // Doesn't hurt to have all of it inside a class :)
}
// I should really keep commenting like this lol

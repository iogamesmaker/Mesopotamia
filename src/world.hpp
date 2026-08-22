// tiles.hpp
// https://www.youtube.com/watch?v=DNu8yUsxOnE used as a starting point
#pragma once
#ifndef tiles_hpp
#define tiles_hpp

#include "headers.hpp"
#include "global.hpp"
class tileManager {
    public:
        enum ItemType : uint8_t {
            ITEM_LAPIS,
            ITEM_GRAIN,
            ITEM_BRICKS,
            ITEM_FIGS,
            ITEM_BREAD,
            ITEM_GRAPES,
            ITEM_WOOD,
            ITEM_SLAVES, //
            ITEM_PEOPLE, //
            UNLOCK_BREAD,
            UNLOCK_GRAPES,
            SHOP_RELOAD
        };

        enum overlayAlias : uint8_t {
            PALACE_TOWERS,
            HOUSE1,
            HOUSE2,
            HOUSE3,
            PALACE,
            LUMBERJACK,
            FARM,
            FIGSEED,
            GRAINSEED,
            GRAPESEED,
            FIG,
            GRAIN,
            GRAPE,
            NONE,

            PAL1_TL,
            PAL1_TR,
            PAL1_BL,
            PAL1_BR,
            PAL2_TL,
            PAL2_TR,
            PAL2_BL,
            PAL2_BR,
            PAL3_TL,
            PAL3_TR,
            PAL3_BL,
            PAL3_BR,
            PAL4_TL,
            PAL4_TR,
            PAL4_BL,
            PAL4_BR,
            ZIG_TL,
            ZIG_TR,
            ZIG_BL,
            ZIG_BR,

            QUARRYTL,
            QUARRYTR,
            QUARRYBL,
            QUARRYBR,

            BAKERY,

            FARMLAND,
            WATER,
            PLAINS
        };

        enum tileAlias : uint8_t {
            // PLAINS
            PLAINS_TREE_PINE, // 0
            PLAINS_TREE_OAK,
            PLAINS_ROCKS,
            PLAINS_SUNFLOWER_FIELDS,
            PLAINS_VAR1,
            PLAINS_VAR2,
            PLAINS_SIDE,
            PLAINS_CORNER,
            PLAINS_INNER_CORNER,
            PLAINS_ENCLOSED,
            PLAINS_FILLED,

            // WATER
            WATER_VAR1, // 11
            WATER_VAR2,
            WATER_SIDE_EW,
            WATER_SIDE_NS,
            WATER_CORNER,
            WATER_INNER_CORNER,
            WATER_ENCLOSED1,
            WATER_ENCLOSED2,
            WATER_FILLED,

            // DESERT
            DESERT_HOLE, // 20
            DESERT_DEAD_SHRUB,
            DESERT_FOSSIL,
            DESERT_CACTI,
            DESERT_EMPTY,
            DESERT_SIDE_E,
            DESERT_SIDE_W,
            DESERT_SIDE_N,
            DESERT_SIDE_S,
            DESERT_CORNER,
            DESERT_INNER_CORNER_NW,
            DESERT_INNER_CORNER_NE,
            DESERT_INNER_CORNER_SW,
            DESERT_INNER_CORNER_SE,
            DESERT_ENCLOSED1,
            DESERT_ENCLOSED2,
            DESERT_ENCLOSED3,
            DESERT_ENCLOSED4,
            DESERT_FILLED,
            // FARM
            FARM_SAT,
            FARM_STARVE,

            // CLOUD
            CLOUD,
            CLOUD_SIDE,
            CLOUD_INNER_CORNER,

            // OTHER
            EMPTY,
            NOT_FOUND,
            SELECTOR
        };

        struct Tile {
            SDL_Rect src; // converts the tiles.txt coordinates to a src rect to use with the atlas.
            int height;
            bool rot;
            bool flipHori;
            bool flipVert;

            enum tileAlias id;
        };

        struct overlayTile {
            SDL_Rect src; // converts the tiles.txt coordinates to a src rect to use with the atlas.

            enum overlayAlias id;
        };

        struct Point {
            tileAlias surface;

            overlayAlias overlay;
            int data = 0;
            uint8_t workers;
            uint8_t height;
            bool active = true;
        };

        enum Biome {
            PLAINS_BIOME,
            WATER_BIOME,
            DESERT_BIOME,
            CLOUD_BIOME,
            FARM_BIOME,
            NULL_BIOME
        };

        Biome getTileBiome(tileAlias tile) {
            switch(tile) {
                // PLAINS
                case PLAINS_TREE_PINE:
                case PLAINS_TREE_OAK:
                case PLAINS_ROCKS:
                case PLAINS_SUNFLOWER_FIELDS:
                case PLAINS_VAR1:
                case PLAINS_VAR2:
                case PLAINS_SIDE:
                case PLAINS_CORNER:
                case PLAINS_INNER_CORNER:
                case PLAINS_ENCLOSED:
                case PLAINS_FILLED:
                    return PLAINS_BIOME;

                // WATER
                case WATER_VAR1:
                case WATER_VAR2:
                case WATER_SIDE_EW:
                case WATER_SIDE_NS:
                case WATER_CORNER:
                case WATER_INNER_CORNER:
                case WATER_ENCLOSED1:
                case WATER_ENCLOSED2:
                case WATER_FILLED:
                    return WATER_BIOME;

                // DESERT
                case DESERT_HOLE:
                case DESERT_DEAD_SHRUB:
                case DESERT_FOSSIL:
                case DESERT_CACTI:
                case DESERT_EMPTY:
                case DESERT_SIDE_E:
                case DESERT_SIDE_W:
                case DESERT_SIDE_N:
                case DESERT_SIDE_S:
                case DESERT_CORNER:
                case DESERT_INNER_CORNER_NW:
                case DESERT_INNER_CORNER_NE:
                case DESERT_INNER_CORNER_SW:
                case DESERT_INNER_CORNER_SE:
                case DESERT_ENCLOSED1:
                case DESERT_ENCLOSED2:
                case DESERT_ENCLOSED3:
                case DESERT_ENCLOSED4:
                case DESERT_FILLED:
                    return DESERT_BIOME;

                // CLOUD
                case CLOUD:
                case CLOUD_SIDE:
                case CLOUD_INNER_CORNER:
                    return CLOUD_BIOME;

                case FARM_SAT:
                case FARM_STARVE:
                    return FARM_BIOME;

                default:
                    return NULL_BIOME;
            }
        }

        tileManager(int worldBigness, int seed); // world bigness lmao
        ~tileManager();

        void renderTile(enum tileAlias tileName, const SDL_Rect* dst, double angle = 0.0, SDL_RendererFlip flipVal = SDL_FLIP_NONE);
        void renderTile(int tile, int x, int y);
        void renderOverlayTile(int tile, int x, int y);
        void renderOverlayTile(enum overlayAlias tileName, SDL_Rect* dst, double angle = 0.0, SDL_RendererFlip flipVal = SDL_FLIP_NONE);

        void renderWorld();
        void generateWorld();
        void pass2();
        void pass2xy(int x, int y);

        bool allowPlacement(overlayAlias tile, int tileX, int tileY, bool place = false);

        SDL_Point getSpawn();

        Biome getBiome(int x, int y); // x0 y0 is center of the world

        void handleTurn();

        void renderDecorationTile(int x, int y, SDL_Rect dst);
        void setTile(int x, int y, int tile);
        void setWorldTile(int x, int y, int tile);
        void updateStats();
        void openTraderMenu(int rngThing = 0);
        void openStats();

        void openMenu(int x, int y);

        void upgradeCapital();

        bool doCost(int index);

        std::string itemToCode(ItemType type);

        enum tileAlias tileToEnum(Tile tile);

        overlayTile getOverlayTile(const int tileIndex);
        Point getTile(int x, int y, bool half = true);
        Tile getTile(const int tileIndex);
        Tile getTile(enum tileAlias tileName);

        int worldSize;
        int worldSeed;

        SDL_Point capital;

        int capitalLevel = 0;
        int population = 0;
        int happy = 0;
        int happiness = 65;
        int turn = 1;
        int cap = 0;
        int unemployed = 0;
        int workers = 0;

        // inventory
        int lapis = 0;
        int figs = 10;
        int grain = 0;
        int bricks = 2;
        int bread = 0;
        int grapes = 0;
        int wood = 5;
        int skips = 1;

        // sacrifice
        int nextSacrifice = 25;
        int lapisCost = 0;
        int figsCost = 0;
        int grainCost = 0;
        int bricksCost = 0;
        int breadCost = 0;
        int grapesCost = 0;
        int woodCost = 0;
        int peopleCost = 3;


        bool breadUnlocked = false;
        bool grapesUnlocked = false;
        bool lose = false;
        std::string losestring = "";

        bool farmPlaced = false;
        int farmlandPlaced = 0;
    private:
        void newUpgradeButton(std::function<void()> callback);

        siv::PerlinNoise* perlin;
        SDL_Texture* atlas; // TM->get("tileatlas") works too instead of using this texture but that's an extra hash lookup and i'm gonna be using this atlas a lot.
        SDL_Texture* overlayAtlas; // put overlays and buildings and shit here

        std::unordered_map<int, Tile> tiles;
        std::unordered_map<int, overlayTile> overlayTiles;

        std::vector<std::vector<Point>> worldMap;

        Point outOfBoundsPoint = {CLOUD, NONE, 0, 0, 128, true};
};
#endif

// world.cpp
// TODO: make world size configurable
// should've broke this file into several smaller chunks but fuck that it's too late now!!!
// oh well
#include "global.hpp"
#include "world.hpp"

tileManager::tileManager(int worldBigness, int seed) {

    worldSize = worldBigness; // 10/10 1337 h4xx07

    worldMap.resize(worldSize, std::vector<Point>(worldSize)); // 🙻🙻🙻🙻🙻🙻🙹
    perlin = new siv::PerlinNoise(seed); // external library. fuck learning perlin noise from scratch
    worldSeed = seed;

    generateWorld();

    atlas = TM->get("tileatlas");
    overlayAtlas = TM->get("overlayatlas");

    // load tiles.txt

    std::ifstream file(getpath("assets/tiles.txt"));
    if(!file.is_open()) {
        std::cerr << "couldn't open assets/tiles.txt" << std::endl;
        return;
    }

    std::string line;
    int i = 0;
    while(std::getline(file, line)) { // chatGPT helped with this
        if(line.empty() || line[0] == '#') continue;

        std::istringstream thing(line);  // converts the line into seperate values (seperated by spaces)
        int x, y;
        std::string rotate, fliph, flipv;
        int height;
        bool rot = false;
        bool flipHori = false;
        bool flipVert = false;

        if(!(thing >> x >> y >> height >> rotate >> fliph >> flipv)) {
            std::cerr << "line " << line << " is mentally challenged" << std::endl;
        }

        if(rotate == "true") rot = true; // wow
        if(fliph == "true") flipHori = true;
        if(flipv == "true") flipVert = true;

        SDL_Rect src = {
            (x - 1) * 16,
            (y - 1) * 16,
            16,
            16
        };

        tiles[i] = {src, height, rot, flipHori, flipVert, static_cast<tileAlias>(i)};
        i++;
    }

    // load overlaytiles.txt

    std::ifstream overlayFile(getpath("assets/overlay.txt"));
    if(!overlayFile.is_open()) {
        std::cerr << "couldn't open assets/overlay.txt" << std::endl;
        return;
    }

    i = 0;

    while(std::getline(overlayFile, line)) { // chatGPT helped with this
        if(line.empty() || line[0] == '#') continue;

        std::istringstream thing(line);  // converts the line into seperate values (seperated by spaces)
        int x, y;
        std::string rotate, fliph, flipv;
        bool rot = false;
        bool flipHori = false;
        bool flipVert = false;

        if(!(thing >> x >> y)) {
            std::cerr << "line " << line << " is mentally challenged" << std::endl;
        }

        SDL_Rect src = {
            (x - 1) * 16, // (x-1) is ugly but i wrote tiles.txt before this function so fuck that
            (y - 1) * 16,
            16,
            16
        };

        overlayTiles[i] = {src, static_cast<overlayAlias>(i)};
        i++;
    }
}

tileManager::~tileManager() {
    delete perlin;
}

SDL_Point tileManager::getSpawn() {
    SDL_Point spawn;
    spawn.x = genHash(SDL_GetTicks()) % worldSize;
    spawn.y = genHash(SDL_GetTicks() - 20120) % worldSize;

    while(getTile(spawn.x, spawn.y).surface == WATER_VAR1 || getTile(spawn.x, spawn.y).surface == WATER_VAR2) {
        spawn.x = genHash(SDL_GetTicks()) % worldSize;
        spawn.y = genHash(SDL_GetTicks() - 20120) % worldSize;
    }
    return spawn;
}

void tileManager::pass2() {
    for(int x = 0; x < worldSize; x++) {
        for(int y = 0; y < worldSize; y++) {
            pass2xy(x,y);
        }
    }
    updateStats();
}

void tileManager::openStats() {
    GUI->disableAll();
    GUI->enableImage("black");
    GUI->enableBox("hotbar");
    GUI->enableButton("return");
    GUI->enableBox("build");

    GUI->changeText("build", "Info: Turn " + std::to_string(gameWorld->turn) + ".");
    GUI->enableText("build");

    std::string newStatus = "Population: " + std::to_string(gameWorld->population) + "/" + std::to_string(gameWorld->cap) + "\n";
    newStatus += "Next sacrifice in turn " + std::to_string(gameWorld->nextSacrifice) + ".\n";
    newStatus += "To be sacrificed:";
    newStatus += "\n\n\nRANDOM HINT:\n";
    std::vector hints = {
        "All eight tiles adjacent to water\ncan be turned into farmland,\nwhen there's an adjacent farm too.",
        "You can get important resources from\nthe trader when starting out.",
        "Every turn your workers rest and\ntheir workpoints can be spent again.",
        "Only population who aren't allocated\nto a building produce workpoints.",
		"The more unemployed people you have,\nthe happier your population gets.",
        "Bread is a way better food source\nfor CAPITAL LEVEL 2 compared to figs.",
		"1 GRAIN can be turned into 2 BREAD.\nOne bakery can process 4 GRAIN in a\nturn.",
        "If you have more jobs than population\nyour people get overworked and\nunhappy.",
        "If your people get too unhappy,\nthey'll get fed up and leave your\nsettlement.",
        "You can disable buildings\ntemporarily to free up some\nworkers.", // important hint
        "You can disable buildings\ntemporarily to free up some\nworkers."
    };
    std::string hint = hints[genHash(turn + seed + SDL_GetTicks()) % hints.size()];

    newStatus += hint + "\n\nClick your capital for more.";

    GUI->changeText("buildstatus", newStatus);
    GUI->enableText("buildstatus");
    std::string sacrificeString;
    if(lapisCost != 0) sacrificeString += std::to_string(lapisCost) + "{LA} ";
    if (woodCost != 0) sacrificeString += std::to_string(woodCost)  + "{WO} ";
    if(bricksCost!= 0) sacrificeString += std::to_string(bricksCost)+ "{BR} ";
    if(breadCost != 0) sacrificeString += std::to_string(breadCost) + "{BD} ";
    if(figsCost  != 0) sacrificeString += std::to_string(figsCost ) + "{FI} ";
    if(grapesCost!= 0) sacrificeString += std::to_string(grapesCost)+ "{GR} ";
    if(grainCost != 0) sacrificeString += std::to_string(grainCost) + "{WH} ";
    if(peopleCost!= 0) sacrificeString += std::to_string(peopleCost)+ "{SL} ";

    GUI->changeText("sacrifice", sacrificeString); // seperate string for the icons. can only have 1 font type per string and the small font does not have icons (yet?)
    GUI->enableText("sacrifice");

    // if(gameWorld->lapisCost > 0) newStatus += "";
}

void tileManager::openTraderMenu(int rngThing) {
    rngThing += skips;

    GUI->disableAll();
    GUI->enableButton("return");
    GUI->enableImage("black");
    GUI->enableImage("tradermenu");
    GUI->enableBox("hotbar");

    std::vector<std::string> names = {
        "Ea-nasir", // Ea-nāṣir - historical name. one of the oldest surviving pieces of writing was about someone complaining that a person named Ea-Nasir was selling poor quality copper.
        "Abdi-arah", // random names
        "Adda-kalla",
        "Aham-nishi",
        "Iddin-enlil",
        "Ishme-adad",
        "Puzur-ishtar",
        "Namzu",
        "Lugal-urudu",
        "Naram-sin",
        "Kal-hazir",
        "Warad-iliya",
        "Zimri-dagan"
    };

    std::string name = names[genHash(seed + turn) % names.size()];
    GUI->changeText("tradername", name);
    GUI->enableText("tradername");
    std::vector dialogs = {
        "I bring the finest lapis\nfrom the SAR-I SANG\nMINES!",
        "Every 3 turns I appear\nwith fresh goods from the\neastern plains.",
        "I offer a different set\nof items to differently\nsized villages!",
        "I sell the secrets to\ngrain and grapes."
    };
    std::string dialog = dialogs[genHash(turn + seed + rngThing + SDL_GetTicks()) % dialogs.size()];
    GUI->changeText("traderdialog", dialog);
    GUI->enableText("traderdialog");

    for (int i = 1; i <= 4; i++) {
        rngThing++;
        std::string slot = "traderslot" + std::to_string(i);

        int cost = 1;
        ItemType costtype = ITEM_WOOD;
        int amount = 1;
        ItemType type = ITEM_WOOD;

        // logic for deciding price / item

        if(i == 4 && (!grapesUnlocked || !breadUnlocked)) {
            if(!breadUnlocked) {
                cost = 15;
                costtype = ITEM_BRICKS;
                amount = -1;
                type = UNLOCK_BREAD;
            } else {
                cost = 10;
                costtype = ITEM_LAPIS;
                amount = -1;
                type = UNLOCK_GRAPES;
            }
        } else if (i == 4) continue;
        if(i == 2 || i == 3) {
            rngThing++;
            while(type == costtype) {
                amount = 1 + genHash(SDL_GetTicks(), seed + rngThing + skips + turn) % 9;

                if(capitalLevel == 1) {
                    rngThing++;
                    int hash = genHash(SDL_GetTicks(), seed + rngThing + skips + turn) % 4;
                    if(hash == 0) type = ITEM_WOOD;
                    if(hash == 1) type = ITEM_BRICKS;
                    if(hash == 2) type = ITEM_PEOPLE;
                    if(hash == 3) type = ITEM_WOOD;

                    rngThing++;
                    int hash2 = genHash(SDL_GetTicks(), seed + rngThing + skips + turn) % 6;
                    if(hash2 == 0) costtype = ITEM_FIGS;
                    if(hash2 == 1) costtype = ITEM_FIGS;
                    if(hash2 == 2) costtype = ITEM_SLAVES;
                    if(hash2 == 3) costtype = ITEM_SLAVES;
                    if(hash2 == 4) costtype = ITEM_WOOD;
                    if(hash2 == 5) costtype = ITEM_BRICKS;
                } else {
                    int hash = genHash(SDL_GetTicks(), seed + rngThing + skips + turn) % 4;
                    if(hash == 0) type = ITEM_PEOPLE;
                    if(hash == 1) type = ITEM_LAPIS;
                    if(hash == 2) type = ITEM_BRICKS;
                    if(hash == 3) type = ITEM_WOOD;

                    rngThing++;

                    if(breadUnlocked) {
                        if(grapesUnlocked) {
                            costtype = (genHash(SDL_GetTicks(), seed + rngThing + skips + turn - 2) % 3 == 0) ? ITEM_FIGS : ((genHash(SDL_GetTicks() + seed + rngThing + skips + turn - 2) % 3 == 1) ? ITEM_BREAD : ITEM_GRAPES);
                        } else {
                            costtype = genHash(SDL_GetTicks(), seed + rngThing + skips + turn - 2) % 2 == 0 ? ITEM_FIGS : ITEM_BREAD;
                        }
                    } else if(grapesUnlocked) {
                        costtype = genHash(SDL_GetTicks(), seed + rngThing + skips + turn - 2) % 2 == 0 ? ITEM_FIGS : ITEM_GRAPES;
                    } else {costtype = ITEM_FIGS;cost *= 2;}
                }
                cost = 0.2 * amount * (1 + genHash(SDL_GetTicks() + seed + rngThing + skips + turn - 1) % 3);
                cost = std::floor(std::max(cost, 1));
            }
        }
        if(i == 1) {
            amount = 0;
            cost = skips;
            int hash = genHash(SDL_GetTicks() + seed + rngThing + skips + turn) % 5;
            if(hash == 0) costtype = ITEM_LAPIS;
            if(hash == 1) costtype = ITEM_PEOPLE; // WORKERS!! you don't lose the people themselves, you just lose 1 worker for a turn
            if(hash == 2) costtype = ITEM_WOOD;
            if(hash == 3) costtype = ITEM_BRICKS;
            if(hash == 4) costtype = ITEM_SLAVES; // they basically just get removed from the population in exchange for a refresh... not too ethical. stupid naming too
            type = SHOP_RELOAD;
        }

        GUI->buttonChangeFunction(slot, [this, cost, costtype, amount, type, rngThing](){
            // if(!IN->firstPressLMB) return;
            bool okay = true;

            auto gain = [this, amount, type]() {
                if(type == ITEM_GRAIN)      grain       += amount;
                if(type == ITEM_GRAPES)     grapes      += amount;
                if(type == ITEM_LAPIS)      lapis       += amount;
                if(type == ITEM_BREAD)      bread       += amount;
                if(type == ITEM_FIGS)       figs        += amount;
                if(type == ITEM_BRICKS)     bricks      += amount;
                if(type == ITEM_WOOD)       wood        += amount;
                if(type == ITEM_PEOPLE) 	workers 	+= amount;
                if(type == ITEM_SLAVES) {
					int growth = cap - std::min(cap, population + amount);
					workers    += growth;
					population = std::min(cap, population + amount);
				}
                if(type == UNLOCK_BREAD)    breadUnlocked= true;
                if(type == UNLOCK_GRAPES)   grapesUnlocked=true;
                if(type == SHOP_RELOAD)     skips++;
            };

            if(type == ITEM_SLAVES) {
                if(population >= cap) {
                    population = cap;
                    setStatus("Poplulation cap reached");
                    okay = false;
                }
            }

            if(costtype == ITEM_GRAIN && okay) {
                if(grain >= cost) {
                    grain -= cost;
                    gain();
                    openTraderMenu(rngThing);
                } else {setStatus("Too expensive.");}
            } else
            if(costtype == ITEM_GRAPES && okay) {
                if(grapes >= cost) {
                    grapes -= cost;
                    gain();
                    openTraderMenu(rngThing);
                } else {setStatus("Too expensive.");}
            } else
            if(costtype == ITEM_LAPIS && okay) {
                if(lapis >= cost) {
                    lapis -= cost;
                    gain();
                    openTraderMenu(rngThing);
                } else {setStatus("Too expensive.");}
            } else
            if(costtype == ITEM_BREAD && okay) {
                if(bread >= cost) {
                    bread -= cost;
                    gain();
                    openTraderMenu(rngThing);
                } else {setStatus("Too expensive.");}
            } else
            if(costtype == ITEM_FIGS && okay) {
                if(figs >= cost) {
                    figs -= cost;
                    gain();
                    openTraderMenu(rngThing);
                } else {setStatus("Too expensive.");}
            } else
            if(costtype == ITEM_BRICKS && okay) {
                if(bricks >= cost) {
                    bricks -= cost;
                    gain();
                    openTraderMenu(rngThing);
                } else {setStatus("Too expensive.");}
            } else
            if(costtype == ITEM_WOOD && okay) {
                if(wood >= cost) {
                    wood -= cost;
                    gain();
                    openTraderMenu(rngThing);
                } else {setStatus("Too expensive.");}
            } else
            if(costtype == ITEM_PEOPLE && okay) {
                if(workers >= cost) {
                    workers -= cost;
                    gain();
                    openTraderMenu(rngThing);
                } else {setStatus("Too expensive.");}
            } else
            if(costtype == ITEM_SLAVES && okay) {
                if(population - 2 >= cost) {
                    population -= cost;
                    gain();
                    openTraderMenu(rngThing);
                } else {setStatus("Not enough people.");}
            }
        });
        std::string costString = std::to_string(cost) + itemToCode(costtype);

        SDL_Rect src;
        if(type == ITEM_GRAIN || type == UNLOCK_BREAD) src = {0,0,16,16};
        if(type == ITEM_GRAPES || type == UNLOCK_GRAPES) src = {16,0,16,16};
        if(type == ITEM_FIGS) src = {32,0,16,16};
        if(type == ITEM_LAPIS) src = {48,0,16,16};
        if(type == ITEM_BRICKS) src = {0,16,16,16};
        if(type == ITEM_BREAD) src = {16,16,16,16};
        if(type == ITEM_WOOD) src = {32,16,16,16};
        if(type == ITEM_SLAVES) src = {48,16,16,16};
        if(type == ITEM_PEOPLE) src = {0,32,16,16};
        if(type == SHOP_RELOAD) src = {48,48,16,16};
        GUI->imageSetSrc(slot, src);
        GUI->changeText(slot, costString);

        if(amount > 0) {GUI->changeText("top" + slot, std::to_string(amount) + "x");}
        else if (amount != 0) {GUI->changeText("top" + slot, "Unlock");} else {
            GUI->changeText("top" + slot, "Refresh");
        }

        GUI->enableText("top" + slot);
        GUI->enableImage(slot);
        GUI->enableText(slot);
        GUI->enableButton(slot);
    }
}

void tileManager::handleTurn() {
    if(turn == 1 && !farmPlaced || farmlandPlaced == 0) {setStatus("First place a farm and some farmland"); return;}
    updateStats();
    int oldHappiness = happiness;
    setStatus("", 0);
    turn++;

    skips = 1;
    int peopleDied = 0; // dead people
    int toAdd = 0; // newly born population
    float tooLittleFood = 0; // people that dont have anything to eat

    // Groetjes Thijmen :)
    // ^ friend that wanted to be mentioned in the source code lol
    for(int x = 0; x < worldSize; x++) {
        for(int y = 0; y < worldSize; y++) {
            Point todo = worldMap[x][y];
            if(!todo.active) continue;
            if(todo.overlay == QUARRYTL) {
                bricks += 3;
            }
            if(todo.overlay == LUMBERJACK) {
                wood += 3;
            }
            if(todo.overlay == FARM) {
                for(int i = -1; i <= 1; i++) {
                    for(int j = -1; j <= 1; j++) {
                        if(i == 0 && j == 0) continue;
                        Point farmCheck = getTile(x + i, y + j, false);
                        if(farmCheck.surface == FARM_SAT) {
                            if(farmCheck.overlay == FIG)   {worldMap[x + i][y + j].overlay = NONE; figs+= capitalLevel == 1 ? 2 : 1;} // buff figs at the start of the game.
                            // for balance. + a little challenge at the start of capital level 2, move to grain and bread
                            if(farmCheck.overlay == GRAIN) {worldMap[x + i][y + j].overlay = NONE; grain += capitalLevel == 1 ? 1 : 2;} // debuff grain at the start of the game
                            if(farmCheck.overlay == GRAPE) {worldMap[x + i][y + j].overlay = NONE; grapes++;} // grapes are for wine
                            if(farmCheck.overlay >= FIGSEED && farmCheck.overlay <= GRAPESEED) worldMap[x + i][y + j].overlay = static_cast<overlayAlias>(farmCheck.overlay + 3);
                            if(farmCheck.overlay == NONE) {worldMap[x + i][y + j].overlay = static_cast<overlayAlias>(FIGSEED + todo.data);}
                        }
                    }
                }
            }
        }
    }

    int prevFig = figs; // DEBUG

    for(int x = 0; x < worldSize; x++) {
        for(int y = 0; y < worldSize; y++) {
            Point todo = worldMap[x][y];
            if(!todo.active) continue;
            if(todo.overlay == BAKERY && grain >= 1) {
                for(int i = 0; grain >= 1 && i <= 4; i++) {
                    bread += 2;
                    grain -= 1;
                }
            }
        }
    }

    if(turn == nextSacrifice) {
        nextSacrifice += 25;
        if(lapis >= lapisCost) {
            lapis -= lapisCost;
        } else {
            losestring = "Sacrifice not met.";
            lose = true;
        }
        if(wood >= woodCost) {
            wood -= woodCost;
        } else {
            losestring = "Sacrifice not met.";
            lose = true;
        }
        if(bread >= breadCost) {
            bread -= breadCost;
        } else {
            losestring = "Sacrifice not met.";
            lose = true;
        }
        if(bricks >= bricksCost) {
            bricks -= bricksCost;
        } else {
            losestring = "Sacrifice not met.";
            lose = true;
        }
        if(figs >= figsCost) {
            figs -= figsCost;
        } else {
            losestring = "Sacrifice not met.";
            lose = true;
        }
        if(grain >= grainCost) {
            grain -= grainCost;
        } else {
            losestring = "Sacrifice not met.";
            lose = true;
        }
        if(grapes >= grapesCost) {
            grapes -= grapesCost;
        } else {
            losestring = "Sacrifice not met.";
            lose = true;
        }
        if(population >= peopleCost + 2) {
            population -= peopleCost;
        } else {
            losestring = "Sacrifice not met.";
            lose = true;
        }

        // -----

        float mult = (25 + turn) / 45;
        peopleCost = std::ceil(peopleCost+(nextSacrifice * 0.1));
        ItemType item = static_cast<ItemType>(genHash(seed + turn) % 8);
        int amount = std::floor(mult * (1 + genHash(seed + turn) % 2));

        if(nextSacrifice == 75) item = ITEM_BREAD;
		
		if(item == ITEM_FIGS && grapesUnlocked) {
			item = ITEM_GRAPES;
		} else if (item == ITEM_FIGS && breadUnlocked) {
			item = ITEM_BREAD;
		}
		
		if(item == ITEM_BREAD && grapesUnlocked) item = ITEM_GRAPES;
		
		if(figsCost != 0 && grapesUnlocked) {
			grapesCost += figsCost;
			figsCost = 0;
		}
		else if(figsCost != 0 && breadUnlocked) {
			breadCost += std::ceil(figsCost * 1.5);
			figsCost = 0;
		}
		
		if(breadCost != 0 && grapesUnlocked) {
			grapesCost += breadCost;
			breadCost = 0;
		}

        switch(item) {
            case ITEM_WOOD:
                woodCost += amount * 2;
                break;
            case ITEM_BRICKS:
                bricksCost += amount;
                break;
            case ITEM_LAPIS:
                lapisCost += std::floor(amount * 0.5);
                break;
            case ITEM_BREAD:
                if(nextSacrifice == 50) {
                    peopleCost += amount * 2;
                    break;
                }
                breadCost += amount;
                break;
            case ITEM_FIGS:
                figsCost += amount * 3;
                break;
            case ITEM_GRAPES:
                if(grapesUnlocked) {
                    grapesCost += amount;
                } else {
                    peopleCost += amount * 2;
                }
                break;
            default:
                peopleCost += amount;
                break;
        }

    }
    for(int i = 0; i < population; i++) {
		
		if(bread + figs + grapes <= 0) {
			tooLittleFood++;
		} else {
			bool ate = false;
			int j = 0;
			while(!ate) {
				if(j > 100) {
					std::cout << "warning: eat loop1" <<  std::endl;
					tooLittleFood++;
					ate = true;
				}
				ItemType foodType = static_cast<ItemType>(ITEM_FIGS + genHash(turn + j + seed + population + 9 + 10 + 21) % 3);
				switch(foodType) {
					case ITEM_FIGS:
						if(figs >= 1) {
							figs--;
							ate = true;
						}
						break;
					case ITEM_BREAD:
						if(bread >= 1) {
							bread--;
							ate = true;
						}
						break;
					case ITEM_GRAPES:
						if(grapes >= 1) {
							grapes--;
							ate = true;
						}
						break;
					default:
						ate = false;
						break;
				}
				j++;
			}
		}
		
        if(i % 2 == 1) { // every 2 people check if they can make a baby
            if(bread + figs + grapes > population + toAdd && cap > population + toAdd) {
                bool ate = false;
				
                int j;
                while(!ate) {
					if(j > 100) {
						std::cout << "warning: eat loop " <<  std::endl;
						tooLittleFood++;
						ate = true;
					}
                    ItemType foodType = static_cast<ItemType>(ITEM_FIGS + genHash(turn + j + seed + population) % 3);
                    switch(foodType) {
                        case ITEM_FIGS:
                            if(figs >= 1) {
                                figs--;
                                ate = true;
                            }
                            break;
                        case ITEM_BREAD:
                            if(bread >= 1) {
                                bread--;
                                ate = true;
                            }
                            break;
                        case ITEM_GRAPES:
                            if(grapes >= 1) {
                                grapes--;
                                ate = true;
                            }
                            break;
                        default:
                            ate = false;
                            break;
                    }
                    j++;
                }
                toAdd++;
            }
        }
    }
    if(tooLittleFood != 0) {
        float factor = tooLittleFood / population;
        peopleDied = population - std::ceil(population * (1 - factor * 0.5));
        population -= peopleDied;
        happiness -= 5 * peopleDied;
    }
    population += toAdd;
    unemployed += toAdd;
    population = std::min(cap, population);
    unemployed = std::max(population, unemployed);
    updateStats();

    happy = std::floor(happiness / 20);


    int peopleLeft = 0;
    if(happy == 0) {
        peopleLeft = std::round(population * 0.20);
    }
    if(happy == 1) {
        peopleLeft = std::round(population * 0.05);
    }
    std::string newStatus = "";
    if(peopleDied != 0) newStatus += "\nPeople died: " + std::to_string(peopleDied);
    if(toAdd != 0) newStatus += "\nPeople born: " + std::to_string(toAdd);
    if(peopleLeft != 0) newStatus += "\nPeople left: " + std::to_string(peopleLeft);

    if(newStatus != "") newStatus = "Events: " + newStatus;

    if(unemployed <= 0) {
        if(population == cap) {newStatus += "\nShortage of people.\nBuild more houses!";}
        else  {newStatus += "\nShortage of people.\nIncrease food production!";}
    }
	//std::cout << "unemployed: " << std::to_string(unemployed) << std::endl;
	//std::cout << "population: " << std::to_string(population) << std::endl;
	float ratio = (float)population / unemployed;
	if(unemployed == 0) ratio = 0;
	//std::cout << "ratio: " << std::to_string(ratio) << std::endl;
	if(unemployed > 0) {happiness += std::round(ratio * 1.5);}
	else {happiness += std::round(4.0 * ratio);}	

    happiness = std::clamp(happiness, 0, 100);

    if(happiness != oldHappiness) {
		//std::cout << std::to_string(happiness - oldHappiness) << std::endl;
        if(happiness > oldHappiness) {
            newStatus += "\nYour people got happier.";
        } else {
            if(happy == 1) {newStatus += "\nYour people hate you!!";}
            else {newStatus += "\nYour people grow angry";}
        }
    }
    if(turn == nextSacrifice - 25) { newStatus+= "\nSucessfully sacrificed.\nThe Gods are pleased."; }
	else {
		newStatus = "Days till sacrifice: " + std::to_string(nextSacrifice - turn) + "\n" + newStatus;
	}

    GUI->disableAll();
    if(turn % 3 == 0) {
        newStatus += "\n\nA BADAKSHANI trader has arrived.\nHe brings valuable resources like\nLAPIS LAZULI from the mines of\nSAR-I SANG. Would you like to trade?";

        GUI->enableButton("traderpromptaccept");
        GUI->enableButton("traderpromptdecline");
    }
    workers = unemployed; // most intuitive statement ever
	// unemployed are like reserve ants in a colony, they don't do anything unless it's required
    if(workers < 0 && cache[999] != 1) {
        cache[999] = 1;
        setStatus("Your population is working overtime.\nThey will leave if they become too unhappy.",-1, {-10,-10,190,30});
    }

    GUI->enableBox("hotbar");
    GUI->enableBox("build");
    GUI->changeText("build", "TURN " + std::to_string(turn));
    GUI->changeText("buildstatus", newStatus);
    GUI->enableText("build");
    GUI->enableText("buildstatus");
    GUI->enableButton("return");
    GUI->enableImage("black");

    // std::cout << "pop: " << std::to_string(population) << ", change figs" << std::to_string(prevFig-figs) << std::endl;

    if(population <= 1) {lose = true;losestring = "Population went extinct.";}
}

void tileManager::pass2xy(int x, int y) { // still calling it pass2, should rename to something like "updateTile" or whatever
    if(x < 0 || y < 0 || x > worldSize -1  || y > worldSize - 1) return;
    if(worldMap[x][y].surface == WATER_VAR1 || worldMap[x][y].surface == WATER_VAR2) return;

    bool nearRiver = false;
    bool isRiver = false;

    float humidity = 0.0;

    float dist;

    if(worldMap[x][y].surface != FARM_SAT) {
        if(worldMap[x][y].overlay >= FIGSEED && worldMap[x][y].overlay <= GRAPE) {
            worldMap[x][y].overlay = NONE;
        }
    }

    for(int i = x - 5; i <= x + 5; i++) {
        for(int j = y - 5; j <= y + 5; j++) {
            int dx = i - x;
            int dy = j - y;
            dist = fsqrt(static_cast<float>(dx*dx + dy*dy));
            if (dist > 5) continue;

            float riverVal = 294;

            if (i >= 0 && i < worldSize &&
                j >= 0 && j < worldSize) {
                if (worldMap[i][j].surface == WATER_VAR1 ||
                    worldMap[i][j].surface == WATER_VAR2) {
                    riverVal = 0.0;
                }
            } else {
                riverVal = std::clamp(
                static_cast<float>(pow(
                    fabs( // fast abs
                        perlin->noise2D(i * 0.01f + 349.0f, j * 0.01f - 2483.0f)
                    ), 1.6f
                )), 0.0f, 1.0f); // super spghetti
            }

            if(riverVal < 0.001 && i == x && j == y) {isRiver = true; break;}
            if(riverVal < 0.001) { humidity += 0.8 / dist; nearRiver = true;}
        }
    }

    int hash = genHash(x, y);

    if(nearRiver && (humidity > 0.55) && !isRiver) {
        if(getTileBiome(worldMap[x][y].surface) != FARM_BIOME) {
            worldMap[x][y].surface = static_cast<tileAlias>(hash % 6); // set plains
			if(worldMap[x][y].overlay == QUARRYTL) {
				worldMap[x][y].overlay = FIG;
				worldMap[x+1][y].overlay = FIG;
				worldMap[x][y+1].overlay = FIG;
				worldMap[x+1][y+1].overlay = FIG;
				updateStats();
			}
			if(worldMap[x][y].overlay == QUARRYTR) {
				worldMap[x][y].overlay = GRAIN;
				worldMap[x-1][y].overlay = GRAIN;
				worldMap[x][y+1].overlay = GRAIN;
				worldMap[x-1][y+1].overlay = GRAIN;
				updateStats();
			}
			if(worldMap[x][y].overlay == QUARRYBL) {
				worldMap[x][y].overlay = GRAPE;
				worldMap[x+1][y].overlay = GRAPE;
				worldMap[x][y-1].overlay = GRAPE;
				worldMap[x+1][y-1].overlay = GRAPE;
				updateStats();
			}
			if(worldMap[x][y].overlay == QUARRYBR) {
				worldMap[x][y].overlay = GRAPESEED;
				worldMap[x-1][y].overlay = GRAPESEED;
				worldMap[x][y-1].overlay = GRAPESEED;
				worldMap[x-1][y-1].overlay = GRAPESEED;
				updateStats();
			}
        } else {
            worldMap[x][y].surface = FARM_STARVE;
        }
    } else {
        if(getTileBiome(worldMap[x][y].surface) != FARM_BIOME) {
            int var = hash % 100;
            worldMap[x][y].surface = DESERT_EMPTY;
            if(var > 75) worldMap[x][y].surface = DESERT_CACTI;
            if(var > 85) worldMap[x][y].surface = DESERT_FOSSIL;
            if(var > 86) worldMap[x][y].surface = DESERT_DEAD_SHRUB;
            if(var > 94) worldMap[x][y].surface = DESERT_HOLE;
			if(worldMap[x][y].overlay == LUMBERJACK) {
				worldMap[x][y].overlay = NONE; // lumberjacks can't operate in the desert lol
				updateStats();
			}
        } else {
            worldMap[x][y].surface = FARM_STARVE;
        }
    }

    if(worldMap[x][y].surface == FARM_STARVE) {
        for(int i = -1; i <= 1; i++) {
            for(int j = -1; j <= 1; j++) {
                Point farmCheck = getTile(x + i, y + j, false);
                if(getTileBiome(farmCheck.surface) == WATER_BIOME) {
                    worldMap[x][y].surface = FARM_SAT;
                }
            }
        }
    }
    if(worldMap[x][y].surface != FARM_SAT && (worldMap[x][y].overlay >= FIGSEED && worldMap[x][y].overlay <= GRAPE)) {
       worldMap[x][y].overlay = NONE;
    }
}

void tileManager::generateWorld() {
    for(int x = 0; x < worldSize; x++) {
        for(int y = 0; y < worldSize; y++) {
            worldMap[x][y].overlay = NONE;
            int hash = genHash(x, y);

            enum Biome biome;

            float riverVal = std::clamp(
                static_cast<float>(pow(
                    fabs( // fast abs
                        perlin->noise2D(x * 0.01f + 349.0f, y * 0.01f - 2483.0f)
                    ), 1.6f
                )
                - 0.01 * perlin->noise2D(x * 2.0f + 439.0f, y * 2.0f - 56889.0f)), 0.0f, 1.0f); // super spghetti

            float temp =
                perlin->noise2D_01(x * 0.1 - 234583, y * 0.1 - 35843) * 0.60f +
                perlin->noise2D_01(x * 0.5 - 38549, y * 0.5 - 985432) * 0.25f +
                perlin->noise2D_01(x * 2.0 - 1234, y * 2.0 - 3243) * 0.15f;

            float humidity =
                perlin->noise2D_01((x + 23324.0f) * 0.2f, (y + 456645.0f) * 0.2f) * 0.60f +
                perlin->noise2D_01((x + 13984.0f) * 0.4f, (y + 4532.0f) * 0.4f) * 0.25f +
                perlin->noise2D_01((x + 9123.0f) * 0.9f, (y + 5342.0f) * 0.9f) * 0.15f;

            float height =
                perlin->noise2D_01((x + 56234.0f) * 0.05f, (y + 1234.0f) * 0.05f) * 0.75f +
                perlin->noise2D_01((x + 4321.0f)  * 0.15f, (y + 8765.0f) * 0.15f) * 0.25f;


            height -= riverVal * 0.9;

            humidity += (1.0f - riverVal) * 0.25; // river
            height   -= (1.0f - riverVal) * 0.25;

            humidity -= height * 0.35 + 0.1;
            temp     -= height * 0.35;

            temp      = std::clamp(temp, 0.0f, 1.0f);
            humidity  = std::clamp(humidity, 0.0f, 1.0f);
            height    = std::clamp(height, 0.0f, 1.0f); // temperature and humidity were only really used in the Kings of Gwynned version, kept em in

            worldMap[x][y].height = height * 255;

            biome = DESERT_BIOME;

            if(humidity > 0.87) riverVal = 0.0f;

            enum tileAlias currentTile;

            if(riverVal < 0.0001|| x == worldSize * 0.5 && y == worldSize * 0.5) { // all of this gets overwritten by pass 2 don't worry about it too much
                currentTile = genHash(x,y) % 2 == 0 ? WATER_VAR1 : WATER_VAR2;
            } else {
                if(biome == DESERT_BIOME) {
                    int var = hash % 100;
                    currentTile = DESERT_EMPTY;
                    if(var > 75) currentTile = DESERT_CACTI;
                    if(var > 85) currentTile = DESERT_FOSSIL;
                    if(var > 86) currentTile = DESERT_DEAD_SHRUB;
                    if(var > 94) currentTile = DESERT_HOLE;
                } else { // plains
                    int var = hash % 6;
                    currentTile = static_cast<tileAlias>(var); // compact
                }
            }
            worldMap[x][y].surface = currentTile;
        }
    }

    // pass 2: humidify all the tiles surrounding water
    pass2();

    for(int x = 0; x < worldSize; x++) { // make all the plains tiles that got generated by pass2 water
        for(int y = 0; y < worldSize; y++) { // enlarges the river
            if(getTileBiome(worldMap[x][y].surface) == PLAINS_BIOME) {
                worldMap[x][y].surface = (genHash(x, y) % 2 == 0) ? WATER_VAR1 : WATER_VAR2;
            } else if(getTileBiome(worldMap[x][y].surface) == WATER_BIOME) {

            }
        }
    }
    pass2(); // part 2 lol
}

void tileManager::upgradeCapital() {
    int requiredLapis = 0;
    int requiredWood = 0;
    int requiredBricks = 0;
    int capIncrease = 0;
    switch(capitalLevel) {
        case 1:
            requiredLapis = 0;
            requiredWood = 20;
            requiredBricks = 15;
            capIncrease = 5;
            break;
        case 2:
            requiredLapis = 15;
            requiredWood = 5;
            requiredBricks = 10;
            capIncrease = 5;
            break;
        case 3:
            requiredLapis = 30;
            requiredWood = 25;
            requiredBricks = 25;
            capIncrease = 10;
            break;
        case 4:
            requiredLapis = 75;
            requiredWood = 50;
            requiredBricks = 125;
            capIncrease = 0;
            break;
        default:
            return;
    }
    if(lapis >= requiredLapis && bricks >= requiredBricks && wood >= requiredWood) {
        lapis -= requiredLapis;
        bricks -= requiredBricks;
        wood -= requiredWood;
        cap += capIncrease;
        capitalLevel += 1;
        worldMap[capital.x][capital.y].overlay   = static_cast<overlayAlias>(static_cast<int>(PAL1_TL) + (capitalLevel - 1) * 4);
        worldMap[capital.x+1][capital.y].overlay = static_cast<overlayAlias>(static_cast<int>(PAL1_TR) + (capitalLevel - 1) * 4);
        worldMap[capital.x][capital.y+1].overlay = static_cast<overlayAlias>(static_cast<int>(PAL1_BL) + (capitalLevel - 1) * 4);
        worldMap[capital.x+1][capital.y+1].overlay=static_cast<overlayAlias>(static_cast<int>(PAL1_BR) + (capitalLevel - 1) * 4);
        SM->playAudio("build");
    } else {
        setStatus("Too poor.");
    }
}

bool tileManager::allowPlacement(overlayAlias tile, int tileX, int tileY, bool place) {
    if(tileX < 0 || tileY < 0 || tileX > worldSize - (tile == PAL1_BL ? 3 : 2) || tileY > worldSize - (tile == PAL1_BL ? 3 : 2) || tileSetting == -1) return false;
    if(turn == 1 && (tile != FARMLAND && tile != FARM && tile != PAL1_TL)) return false; // turn 1 don't allow placing anything except farms, farmland and the capital.
    if(!GUI->top) place = false;
    Point pt = getTile(tileX - worldSize * 0.5, tileY - worldSize * 0.5);
    int range;
    switch(capitalLevel) {
        case 0:
            range = 999; // capital doesnt exist yet -- can build anywhere xd
            break;
        case 1:
            range = 7;
            break;
        case 2:
            range = 10;
            break;
        case 3:
            range = 15;
            break;
        case 4:
            range = 20;
            break;
        default:
            range = 999;
            break;
    }
    if(tile == PAL1_TL) {
        if(getTileBiome(pt.surface) == WATER_BIOME || pt.overlay != NONE) return false;
        pt = getTile(tileX + 1 - worldSize * 0.5, tileY - worldSize * 0.5);
        if(getTileBiome(pt.surface) == WATER_BIOME || pt.overlay != NONE) return false;
        pt = getTile(tileX + 1 - worldSize * 0.5, tileY + 1 - worldSize * 0.5);
        if(getTileBiome(pt.surface) == WATER_BIOME || pt.overlay != NONE) return false;
        pt = getTile(tileX - worldSize * 0.5, tileY + 1 - worldSize * 0.5);
        if(getTileBiome(pt.surface) == WATER_BIOME || pt.overlay != NONE) return false;
        if(pt.overlay != NONE) return false;
        if(place) {
            capitalLevel = 1;
            worldMap[tileX + 1][tileY].overlay = static_cast<overlayAlias>(tileSetting + 1); // the capital is a 2x2 building
            worldMap[tileX][tileY + 1].overlay = static_cast<overlayAlias>(tileSetting + 2); // these couple of lines place the rest of the building
            worldMap[tileX+1][tileY+1].overlay = static_cast<overlayAlias>(tileSetting + 3);
            capital = {tileX,tileY};
            population = 5;
            cap = 10;
            happy = 2;
        }
        return true;
    } else if (std::hypot(tileX - capital.x, tileY - capital.y) > range && std::hypot(tileX - capital.x - 1, tileY - capital.y - 1) > range && std::hypot(tileX - capital.x, tileY - capital.y - 1) > range && std::hypot(tileX - capital.x - 1, tileY - capital.y) > range) {
        return false;
    }
    if(tile == QUARRYTL) {
        if(getTileBiome(pt.surface) != DESERT_BIOME || pt.overlay != NONE) return false;
        pt = getTile(tileX + 1 - worldSize * 0.5, tileY - worldSize * 0.5);
        if(getTileBiome(pt.surface) != DESERT_BIOME || pt.overlay != NONE) return false;
        pt = getTile(tileX + 1 - worldSize * 0.5, tileY + 1 - worldSize * 0.5);
        if(getTileBiome(pt.surface) != DESERT_BIOME || pt.overlay != NONE) return false;
        pt = getTile(tileX - worldSize * 0.5, tileY + 1 - worldSize * 0.5);
        if(getTileBiome(pt.surface) != DESERT_BIOME || pt.overlay != NONE) return false;
        if(pt.overlay != NONE) return false;
        if(place) {
            if(!doCost(tile)) return false;
            worldMap[tileX + 1][tileY].overlay = static_cast<overlayAlias>(tileSetting + 1);
            worldMap[tileX][tileY + 1].overlay = static_cast<overlayAlias>(tileSetting + 2);
            worldMap[tileX+1][tileY+1].overlay = static_cast<overlayAlias>(tileSetting + 3);
        }
        return true;
    }
    if(tile == HOUSE1 || tile == BAKERY) {
        if(getTileBiome(pt.surface) == WATER_BIOME || getTileBiome(pt.surface) == FARM_BIOME || pt.overlay != NONE) return false;
        if(place) {
            if(!doCost(tile)) return false;
            if(tile == HOUSE1) cap += 5;
        }
        return true;
    }
    if(tile == LUMBERJACK) {
        if(pt.overlay != NONE || getTileBiome(pt.surface) != PLAINS_BIOME) return false;
        if(place) {
            if(!doCost(tile)) return false;
        }
        return true;
    }
    if(tile == FARM) {
        if((pt.overlay != NONE && !(pt.overlay >= FIGSEED && pt.overlay <= GRAPE)) || getTileBiome(pt.surface) != PLAINS_BIOME) return false;
        for(int i = -2; i <= 2; i++) {
            for(int j = -2; j <= 2; j++) {
                if(j == 0 && i == 0) continue;
                Point farmCheck = getTile(tileX + i, tileY + j, false);
                if(getTileBiome(farmCheck.surface) == WATER_BIOME) {
                    if(place) {if(!doCost(tile)) return false;}
                    return true;
                }
            }
        }
        if(place) {
            if(!doCost(tile)) return false;
        }
        return false;
    }
    if(tile == FARMLAND) {
        // if (getTileBiome(getTile(tileX - worldSize * 0.5, tileY - worldSize * 0.5).surface) != PLAINS_BIOME || pt.overlay != NONE) return false; // for some reason this line breaks checking neighbours. can't be bothered to fix it. works without this line too :)

        if(pt.overlay == NONE && getTileBiome(pt.surface) == PLAINS_BIOME) {
            bool hasFarm = false;
            bool hasWater = false;
            for(int i = -1; i <= 1; i++) {
                for(int j = -1; j <= 1; j++) {
                    Point farmCheck = getTile(tileX + i, tileY + j, false);
                    if(farmCheck.overlay == FARM) hasFarm = true;
                    if(getTileBiome(farmCheck.surface) == WATER_BIOME) {
                        hasWater = true;
                    }
                }
            }

            if(hasFarm && hasWater) {
                if(place) {
                    if(doCost(tile)) {
                        return true;
                    }
                } else {
                    return true;
                }
            }
            return false;
        }
    }
    if(tile == WATER) {
        // if (getTileBiome(getTile(tileX - worldSize * 0.5, tileY - worldSize * 0.5).surface) != PLAINS_BIOME || pt.overlay != NONE) return false;
        if((pt.overlay == NONE || (pt.overlay >= FIGSEED && pt.overlay <= GRAPE)) && getTileBiome(pt.surface) != WATER_BIOME) {
            for(int i = -1; i <= 1; i++) {
                for(int j = -1; j <= 1; j++) {
                    Point waterCheck = getTile(tileX + i, tileY + j, false);
                    if(getTileBiome(waterCheck.surface) == WATER_BIOME) {
                        if(place) {
                            if(!doCost(tile)) return false;
                            pass2();
                        }
                        return true;
                    }
                }
            }
        }
    }
    if(tile == PLAINS) {
        if (getTileBiome(pt.surface) == DESERT_BIOME || getTileBiome(pt.surface) == PLAINS_BIOME || (pt.overlay != NONE && pt.overlay < FIGSEED && pt.overlay > GRAPE)) return false;
        if(place) {
            if(doCost(tile)) return true;
            return false;
        }
        return true;
    }
    return false;
}

bool tileManager::doCost(int index) {
    // return true;
    overlayAlias building = static_cast<overlayAlias>(index);

    if(building == PAL1_TL) {
        return true;

    } // palace level 1 is free
    if(building == HOUSE1 || building == LUMBERJACK) {
        if(wood - 5 >= 0) {
            wood -= 5;
            return true;
        } else {
            setStatus("Too poor.");
            tileSetting = -1;
            return false;
        }
    }
    if(building == FARM) {
        if(wood - 5 >= 0 && bricks - 2 >= 0) {
            wood -= 5;
            bricks -= 2;
            if(turn == 1) {
                farmPlaced = true;
                setStatus("Now place some FARMLAND\nADJACENT to your farm.", -1, {-10,-10,122,30});
            }
            return true;
        } else {
            setStatus("Too poor.");
            tileSetting = -1;
            return false;
        }
    }
    if(building == QUARRYTL) {
        if(wood - 25 >= 0 && bricks >= 2) {
            wood -= 25;
            bricks -= 2;
            return true;
        } else {
            setStatus("Too poor.");
            tileSetting = -1;
            return false;
        }
    }
    if(building == BAKERY) {
        if(wood - 5 >= 0 && bricks - 15 >= 0) {
            wood -= 5;
            bricks -= 15;
            return true;
        } else {
            setStatus("Too poor.");
            tileSetting = -1;
            return false;
        }
    }
    if(building == PLAINS) {
        if(workers >= 4) {
            workers -= 4;
            return true;
        } else {
            setStatus("Not enough workers.", 2000);
            tileSetting = -1;
            return false;
        }

        pass2();
    }
    if(building == FARMLAND || building == WATER) {
        if(workers >= 2) {
            workers -= 2;
            farmlandPlaced++;

            if(turn == 1 && building == FARMLAND && workers >= 2) {
                setStatus("Try to build a lot of farmland ADJACENT\nto this farm in the first couple of turns.", -1, {-10,-10,180,30});
            }
            return true;
        } else {
            if(turn == 1) {
                setStatus("Not enough workers. Press \">\" to advance turns, bottom right.", 7500);
            } else {
                setStatus("Not enough workers.", 2000);
            }
            tileSetting = -1;
            return false;
        }

        pass2();
    }
    setStatus("Building not found??? that's crazy");
    return false;
}

void tileManager::openMenu(int x, int y) {
    Point toOpen = getTile(x,y);
    if(toOpen.overlay == NONE || toOpen.overlay >= FIGSEED && toOpen.overlay <= GRAPE) {
        return;
    }
    if(toOpen.overlay == QUARRYBR) {
        x -= 1;
        y -= 1;
    } else
    if(toOpen.overlay == QUARRYBL) {
        y -= 1;
    } else
    if(toOpen.overlay == QUARRYTR) {
        x -= 1;
    }
    toOpen = getTile(x,y);

    GUI->disableAll();
    GUI->enableButton("return");
    GUI->enableImage("black");
    GUI->enableButton("trash");
    GUI->enableBox("hotbar");
    if(toOpen.overlay != HOUSE1 && toOpen.overlay != HOUSE2 && toOpen.overlay != HOUSE3) {
        GUI->enableButton("active");
        GUI->enableText("active");
    }
    if(toOpen.active) {
        GUI->buttonChangeTexture("checkon","checkon_hover","checkon_click","active");
        GUI->buttonChangeFunction("active", [this,x,y](){
            worldMap[x + worldSize * 0.5][y + worldSize * 0.5].active = false;
            openMenu(x,y);
        });
    } else {
        GUI->buttonChangeTexture("checkoff","checkoff_hover","checkoff_click","active");
        GUI->buttonChangeFunction("active", [this,x,y](){
            worldMap[x + worldSize * 0.5][y + worldSize * 0.5].active = true;
            openMenu(x,y);
        });
    }
    GUI->buttonChangeFunction("trash", [this, x, y](){
        if(getTile(x,y).overlay == QUARRYTL) {
            setTile(x + 1, y, static_cast<int>(NONE));
            setTile(x, y + 1, static_cast<int>(NONE));
            setTile(x + 1, y + 1, static_cast<int>(NONE));
        }
        else if(getTile(x,y).overlay == QUARRYTR) {
            setTile(x - 1, y, static_cast<int>(NONE));
            setTile(x - 1, y + 1, static_cast<int>(NONE));
            setTile(x, y + 1, static_cast<int>(NONE));
        }
        else if(getTile(x,y).overlay == QUARRYBL) {
            setTile(x, y - 1, static_cast<int>(NONE));
            setTile(x + 1, y - 1, static_cast<int>(NONE));
            setTile(x + 1, y, static_cast<int>(NONE));
        }
        else if(getTile(x,y).overlay == QUARRYBR) {
            setTile(x - 1, y - 1, static_cast<int>(NONE));
            setTile(x, y - 1,     static_cast<int>(NONE));
            setTile(x - 1, y,     static_cast<int>(NONE));
        }
        setTile(x,y,static_cast<int>(NONE));
        if(getTile(x,y).overlay == FARM) {
            for(int i = -1; i < 1; i++) {
                for(int j = -1; j < 1; j++) {
                    if(getTile(x + i, y + j).overlay >= FIGSEED && getTile(x + i, y + j).overlay <= GRAPE) setTile(x + i, y + j, NONE);
                }
            }
        }
        GUI->disableAll(); // shitty copy of resumeGame(); in world.cpp
        GUI->enableBox("hotbar"); // I'm sure there's a better way to copy it but fuck that
        GUI->enableButton("showstats");
        GUI->enableButton("next");
        GUI->enableButton("pause");
        GUI->enableButton("music");
        GUI->enableButton("sound");
    });

    if (toOpen.overlay >= HOUSE1 && toOpen.overlay <= HOUSE3) {
        GUI->changeText("build", ((toOpen.overlay == HOUSE3) ? "House - MAX LEVEL" : ((toOpen.overlay == HOUSE2) ? "House - LEVEL 2" : "House - LEVEL 1")));
        std::string newStatus;
        switch(toOpen.overlay) {
            case HOUSE1:
                newStatus = "House 5 people. Upgrading costs:\n2 bricks, 1 wood\nRequires Capitol level 2.";
                GUI->enableButton("upgrade");
                break;
            case HOUSE2:
                newStatus = "House 10 people. Upgrading costs:\n5 bricks, 2 wood\nRequires Capitol level 3.";
                GUI->enableButton("upgrade");
                break;
            case HOUSE3:
                newStatus = "House 15 people. Max level.";
                break;
            default:
                newStatus = "house is a weird level lol" + std::to_string(toOpen.overlay);
        }

        GUI->changeText("buildstatus", newStatus);
        GUI->buttonChangeFunction("upgrade", [this, x, y, toOpen](){ // the shit in the brackets is what the function has access to
                                                                     // "this" is just the current class apparently
            if(toOpen.overlay == HOUSE1 && capitalLevel != 1) {
                if(bricks >= 2 && wood >= 1) {
                    wood -= 1;
                    bricks -= 2;
                    worldMap[x + worldSize * 0.5][y + worldSize * 0.5].overlay = HOUSE2;
                    cap += 5;
                    SM->playAudio("build");
                } else {
                    setStatus("Too poor.");
                }
            } else if (toOpen.overlay == HOUSE2 && capitalLevel != 2) {
                if(bricks >= 5 && wood >= 2) {
                    wood -= 2;
                    bricks -= 5;
                    worldMap[x + worldSize * 0.5][y + worldSize * 0.5].overlay = HOUSE3;
                    cap += 5;
                    SM->playAudio("build");
                } else {
                    setStatus("Too poor.");
                }
            } else {
                setStatus("Capital level too low.", 2500);
            }
            openMenu(x,y);
        });
    } else if (toOpen.overlay == LUMBERJACK) {
        GUI->changeText("build", "Lumberjack");
        GUI->changeText("buildstatus", "Produces 3 wood a turn for you.\nWorkers: " + std::to_string(toOpen.workers));
    } else if (toOpen.overlay == BAKERY) {
        GUI->changeText("build", "Bakery");
        GUI->changeText("buildstatus", "Turns 1 grain into 2 bread.\nMax 8 bread per turn per bakery.\nWorkers: " + std::to_string(toOpen.workers));
    } else if (toOpen.overlay == FARM) { // holy spaghetti
        int Nfarmland = 0;

        for(int i = -1; i <= 1; i++) {
            for(int j = -1; j <= 1; j++) {
                if(i == 0 && j == 0) continue;
                Point farmCheck = getTile(x + i, y + j, true);
                if(farmCheck.surface == FARM_SAT) Nfarmland++;
            }
        }
        std::string newStatus = "Surrounding farmland: " + std::to_string(Nfarmland) + ".\nYou need to build farmland tiles\nadjacent to water and a farm.\nSelect crop:\n\n\n\nCurrently producing " + ((toOpen.data == 0) ? "figs\nTIP: Multiple farms can\nuse the same farm tile." : (toOpen.data == 1) ? "grain\nTIP: You need bakeries to\nproduce food from grain." : "grapes") + ".";

        if(!breadUnlocked && !grapesUnlocked) newStatus += "\nUnlock new crops by trading.\nOnly figs are unlocked.";

        GUI->changeText("buildstatus", newStatus + "\nWorkers: " + std::to_string(toOpen.workers));
        GUI->changeText("build", ((toOpen.data == 0) ? "Farm - figs {FI}" : (toOpen.data == 1 ? "Farm - grain {WH}" : "Farm - grapes {GR}")));
        GUI->buttonChangeFunction("farmchangecrop", [this,x,y,toOpen](){
            worldMap[x + worldSize * 0.5][y + worldSize * 0.5].data++;
            if(worldMap[x + worldSize * 0.5][y + worldSize * 0.5].data == 1 && !breadUnlocked) worldMap[x + worldSize * 0.5][y + worldSize * 0.5].data++;
            if(worldMap[x + worldSize * 0.5][y + worldSize * 0.5].data == 2 && !grapesUnlocked) worldMap[x + worldSize * 0.5][y + worldSize * 0.5].data++;
            if(worldMap[x + worldSize * 0.5][y + worldSize * 0.5].data == 3) worldMap[x + worldSize * 0.5][y + worldSize * 0.5].data = 0;
            gameWorld->openMenu(x,y);
        });
        GUI->enableButton("farmchangecrop");
    } else if (toOpen.overlay >= QUARRYTL && toOpen.overlay <= QUARRYBR) {
        GUI->changeText("build", "Quarry");
        GUI->changeText("buildstatus", "Mines 3 bricks in a turn.\nWorkers: " + std::to_string(toOpen.workers));
    }
    GUI->enableText("buildstatus");
    GUI->enableBox("build");
    GUI->enableText("build");
}

void tileManager::updateStats() {
    cap = 0;

    unemployed = population;
    for(int x = 0; x < worldSize; x++) {
        for(int y = 0; y < worldSize; y++) {
            overlayAlias check = worldMap[x][y].overlay;
            int toEmploy = 0;
            switch(check) {
                case HOUSE1:
                    cap += 5;
                    break;
                case HOUSE2:
                    cap += 10;
                    break;
                case HOUSE3:
                    cap += 15;
                    break;
                case PAL1_TL:
                    cap += 5;
                    break;
                case PAL2_TL:
                    cap += 10;
                    break;
                case PAL3_TL:
                    cap += 15;
                    break;
                case PAL4_TL:
                    cap += 25;
                    break;
                case ZIG_TL:
                    cap += 25;
                    break;
                case LUMBERJACK:
                    if(worldMap[x][y].active) toEmploy = 2;
                    break;
                case FARM:
                    if(worldMap[x][y].active) toEmploy = 1;
                    break;
                case BAKERY:
                    if(worldMap[x][y].active) toEmploy = 2;
                    break;
                case QUARRYTL:
                    if(worldMap[x][y].active) toEmploy = 5;
                    break;
                default:
                    toEmploy = 0;
                    break;
            }

            worldMap[x][y].workers = toEmploy;
            unemployed -= toEmploy;
        }
    }
    if(population > cap) {
        population = cap;
    }
}

std::string tileManager::itemToCode(ItemType type) {
    switch(type) {
        case ITEM_WOOD:
            return "{WO}";
        case ITEM_LAPIS:
            return "{LA}";
        case ITEM_BREAD:
            return "{BD}";
        case ITEM_GRAPES:
            return "{GR}";
        case ITEM_GRAIN:
            return "{WH}";
        case ITEM_PEOPLE:
            return "{PL}";
        case ITEM_SLAVES:
            return "{SL}";
        case ITEM_FIGS:
            return "{FI}";
        case ITEM_BRICKS:
            return "{BR}";
        default:
            return "UNKNOWN ITEM (tileManager::itemToCode)";
    }
}

void tileManager::renderWorld() {
    float tileSizeZoomed = std::floor(16 * camZoom);
    float screenTileWidth = std::ceil(SCREEN_W / tileSizeZoomed);
    float screenTileHeight = std::ceil(SCREEN_H / tileSizeZoomed);

    float screenTileWidth05 = std::ceil(screenTileWidth * 0.5);
    float screenTileHeight05 = std::ceil(screenTileHeight * 0.5); // wow
    SDL_Rect selectorRect;
    bool setTile = false;
    // main layer
    for(int x = -screenTileWidth05 - 2 - camX; x < screenTileWidth05 - camX + 2; x++) {
        for(int y = -screenTileHeight05 - 2 - camY; y < screenTileHeight05 - camY + 2; y++) {
            SDL_Rect dst;

            dst.x = x * tileSizeZoomed + std::floor(camX * tileSizeZoomed);
            dst.y = y * tileSizeZoomed + std::floor(camY * tileSizeZoomed);
            dst.w = tileSizeZoomed;
            dst.h = tileSizeZoomed;

            dst.x += SCREEN_W / 2;
            dst.y += SCREEN_H / 2;

            if( dst.x + dst.w < 0 ||
                dst.y + dst.h < 0 ||
                dst.x > SCREEN_W  ||
                dst.y > SCREEN_H) {continue;} // tile is offscreen; don't render it.

            else if(IN->isHovered(dst)) {
                // std::cout << std::to_string(worldMap[x + worldSize * 0.5][y + worldSize * 0.5].active) << std::endl;
                selectorRect = dst;
                IN->tile.x = x;
                IN->tile.y = y;
                if(IN->lmb && GUI->top) {
                    if(tileSetting != -1 && allowPlacement(static_cast<overlayAlias>(tileSetting), x + worldSize * 0.5, y + worldSize * 0.5, true)) {
                        if(static_cast<overlayAlias>(tileSetting) >= FARMLAND && static_cast<overlayAlias>(tileSetting) <= PLAINS) {

                            if(static_cast<overlayAlias>(tileSetting) == FARMLAND) {
                                worldMap[x + worldSize * 0.5][y + worldSize * 0.5].surface = FARM_SAT;
                                SM->playAudio("farmland");
                            }
                            if(static_cast<overlayAlias>(tileSetting) == PLAINS) {
                                worldMap[x + worldSize * 0.5][y + worldSize * 0.5].surface = PLAINS_VAR1;
                                SM->playAudio("farmland");
                                pass2();
                            }
                            if(static_cast<overlayAlias>(tileSetting) == WATER) {
                                worldMap[x + worldSize * 0.5][y + worldSize * 0.5].surface = genHash(x,y) % 2 == 0 ? WATER_VAR1 : WATER_VAR2;
                                worldMap[x + worldSize * 0.5][y + worldSize * 0.5].overlay = NONE;
                                SM->playAudio("water");
                                pass2();
                            }
                        }
                        else {
                            worldMap[x + worldSize * 0.5][y + worldSize * 0.5].overlay = static_cast<overlayAlias>(tileSetting);
                            SM->playAudio("build");
                        }

                        setTile = true;
                        updateStats();
                    }
                }
            }

            Point toRender = getTile(x,y); // lol I should rename one of these

            if(tileSetting == -1) {
                int bri = 150 + toRender.height;
                bri = 127 + (bri / 2);
                bri = std::clamp(bri, 0, 255);
                SDL_SetTextureColorMod(atlas, bri, bri, bri);
            } else {
                if(allowPlacement(static_cast<overlayAlias>(tileSetting), x + worldSize * 0.5, y + worldSize * 0.5)) {
                    int bri = 150 + toRender.height;
                    bri = 127 + (bri / 2);
                    bri = std::clamp(bri, 0, 255);
                    SDL_SetTextureColorMod(atlas, bri, bri, bri);
                } else {
                    SDL_SetTextureColorMod(atlas, 255, 180, 180);
                }
            }

            int var = genHash(x, y); // variation hash

            double angle = 0;
            if(getTile(toRender.surface).rot) { // random rotation
                int rotIndex = var & 3; // will return 0, 1, 2, or 3. & is more performant than doing a modulus, but more limited too.
                angle = rotIndex * 90.0;
            }

            SDL_RendererFlip flipVal = SDL_FLIP_NONE;

            if (getTile(toRender.surface).flipHori && getTile(toRender.surface).flipVert) {
                switch (var % 4) {
                    case 1: flipVal = SDL_FLIP_HORIZONTAL; break;
                    case 2: flipVal = SDL_FLIP_VERTICAL; break;
                    case 3: flipVal = (SDL_RendererFlip)(SDL_FLIP_HORIZONTAL | SDL_FLIP_VERTICAL); break;
                }
            } else if (getTile(toRender.surface).flipHori) {
                if (var & 1) flipVal = SDL_FLIP_HORIZONTAL; // & takes the first bit, sees if it's 0 or 1.
            } else if (getTile(toRender.surface).flipVert) {
                if (var & 1) flipVal = SDL_FLIP_VERTICAL;
            }

            renderTile(getTile(toRender.surface).id, &dst, angle, flipVal);
            if(toRender.overlay >= FIGSEED && toRender.overlay <= GRAPE) renderOverlayTile(toRender.overlay, &dst);
            if(camZoom > 0.5) {
                renderDecorationTile(x, y, dst);
            }
        }
    }
    // decoration
    for(int x = -screenTileWidth05 - 2 - camX; x < screenTileWidth05 - camX + 2; x++) {
        for(int y = -screenTileHeight05 - 2 - camY; y < screenTileHeight05 - camY + 2; y++) {
            SDL_Rect dst;

            dst.x = x * tileSizeZoomed + std::floor(camX * tileSizeZoomed);
            dst.y = y * tileSizeZoomed + std::floor(camY * tileSizeZoomed);
            dst.w = tileSizeZoomed;
            dst.h = tileSizeZoomed;

            dst.x += SCREEN_W / 2;
            dst.y += SCREEN_H / 2;

            if( dst.x + dst.w < 0 ||
                dst.y + dst.h < 0 ||
                dst.x > SCREEN_W ||
                dst.y > SCREEN_H) {continue;}

            Point toRender = getTile(x,y);
            if(tileSetting == -1) {
                int bri = 150 + toRender.height;
                bri = 127 + (bri / 2);
                bri = std::clamp(bri, 0, 255);
                SDL_SetTextureColorMod(overlayAtlas, bri, bri, bri);
            } else {
                if(allowPlacement(static_cast<overlayAlias>(tileSetting), x + worldSize * 0.5, y + worldSize * 0.5)) { // huge spaghetti. if you're placing a 2x2 building we gotta render the full 2x2 building
                    SDL_SetTextureColorMod(overlayAtlas, 200, 255, 200);
                    if(IN->tile.x == x && IN->tile.y == y) {
                        renderOverlayTile(static_cast<overlayAlias>(tileSetting), &dst);
                    }
                    if(static_cast<overlayAlias>(tileSetting) == PAL1_TL && allowPlacement(PAL1_TL, IN->tile.x + worldSize * 0.5, IN->tile.y + worldSize * 0.5)) {
                        if(IN->tile.x == x - 1 && IN->tile.y == y) {
                            renderOverlayTile(static_cast<overlayAlias>(tileSetting + 1), &dst);
                        }
                        if(IN->tile.x == x && IN->tile.y == y - 1) {
                            renderOverlayTile(static_cast<overlayAlias>(tileSetting + 2), &dst);
                        }
                        if(IN->tile.x == x - 1 && IN->tile.y == y - 1) {
                            renderOverlayTile(static_cast<overlayAlias>(tileSetting + 3), &dst);
                        }
                    }
                    if(static_cast<overlayAlias>(tileSetting) == QUARRYTL && allowPlacement(QUARRYTL, IN->tile.x + worldSize * 0.5, IN->tile.y + worldSize * 0.5)) {
                        if(IN->tile.x == x - 1 && IN->tile.y == y) {
                            renderOverlayTile(static_cast<overlayAlias>(tileSetting + 1), &dst);
                        }
                        if(IN->tile.x == x && IN->tile.y == y - 1) {
                            renderOverlayTile(static_cast<overlayAlias>(tileSetting + 2), &dst);
                        }
                        if(IN->tile.x == x - 1 && IN->tile.y == y - 1) {
                            renderOverlayTile(static_cast<overlayAlias>(tileSetting + 3), &dst);
                        }
                    }
                } else if(static_cast<overlayAlias>(tileSetting) == PAL1_TL && allowPlacement(PAL1_TL, IN->tile.x + worldSize * 0.5, IN->tile.y + worldSize * 0.5)) {
                    if(IN->tile.x == x - 1 && IN->tile.y == y) {
                        SDL_SetTextureColorMod(overlayAtlas, 200, 255, 200);
                        renderOverlayTile(static_cast<overlayAlias>(tileSetting + 1), &dst);
                    }
                    if(IN->tile.x == x && IN->tile.y == y - 1) {
                        SDL_SetTextureColorMod(overlayAtlas, 200, 255, 200);
                        renderOverlayTile(static_cast<overlayAlias>(tileSetting + 2), &dst);
                    }
                    if(IN->tile.x == x - 1 && IN->tile.y == y - 1) {
                        SDL_SetTextureColorMod(overlayAtlas, 200, 255, 200);
                        renderOverlayTile(static_cast<overlayAlias>(tileSetting + 3), &dst);
                    }
                } else if(static_cast<overlayAlias>(tileSetting) == QUARRYTL && allowPlacement(QUARRYTL, IN->tile.x + worldSize * 0.5, IN->tile.y + worldSize * 0.5)) {
                    if(IN->tile.x == x - 1 && IN->tile.y == y) {
                        SDL_SetTextureColorMod(overlayAtlas, 200, 255, 200);
                        renderOverlayTile(static_cast<overlayAlias>(tileSetting + 1), &dst);
                    }
                    if(IN->tile.x == x && IN->tile.y == y - 1) {
                        SDL_SetTextureColorMod(overlayAtlas, 200, 255, 200);
                        renderOverlayTile(static_cast<overlayAlias>(tileSetting + 2), &dst);
                    }
                    if(IN->tile.x == x - 1 && IN->tile.y == y - 1) {
                        SDL_SetTextureColorMod(overlayAtlas, 200, 255, 200);
                        renderOverlayTile(static_cast<overlayAlias>(tileSetting + 3), &dst);
                    }
                }
                SDL_SetTextureColorMod(overlayAtlas, 255, 180, 180);
            }

            if(toRender.overlay < FIGSEED || toRender.overlay > GRAPE) renderOverlayTile(toRender.overlay, &dst);
        }
    }
    if(setTile) {
        pass2();
        worldMap[IN->tile.x + worldSize * 0.5][IN->tile.y + worldSize * 0.5].active = true;
    }
    if(setTile && GUI->top && (!IN->keyDown(SDL_SCANCODE_LSHIFT) || tileSetting == PAL1_TL)) {
        if(tileSetting == PAL1_TL) {
            setStatus("Great! Now build a farm.", -1);
            workers = unemployed;
        }
        tileSetting = -1;
        IN->lmb = false;
    }
    SDL_SetTextureColorMod(atlas, 255, 255, 255);
    SDL_SetTextureColorMod(overlayAtlas, 255, 255, 255);
    if(!GUI->imageIsOn("black")) renderTile(SELECTOR, &selectorRect);
}

tileManager::overlayTile tileManager::getOverlayTile(const int tileIndex) {
    overlayTile returnTile = overlayTiles.at(tileIndex);
    if(returnTile.src.w > 0) {
        return returnTile;
    } else {
        return overlayTiles.at(NONE); // TODO: replace with not found null texture ats ome  point
    }
}

void tileManager::renderOverlayTile(enum overlayAlias tileName, SDL_Rect* dst, double angle, SDL_RendererFlip flipVal) {
    overlayTile toRender = getOverlayTile(static_cast<int>(tileName));

    if(tileName == NONE) return;
    if((tileName < FIGSEED || tileName > GRAPE) && (tileName < FARMLAND || tileName > PLAINS)) dst->y -= std::floor(4 * camZoom);

    SDL_RenderCopyEx(renderer, overlayAtlas, &toRender.src, dst, angle, NULL, flipVal);
    // https://wiki.libsdl.org/SDL2/SDL_RendererFlip
}

void tileManager::renderOverlayTile(int tile, int x, int y) {
    float tileSizeZoomed = std::floor(16 * camZoom);
    overlayTile toRender = getOverlayTile(tile);

    SDL_Rect dst;
    dst.x = (x - camX) * tileSizeZoomed;
    dst.y = (y - camY - 0.25) * tileSizeZoomed;
    dst.w = tileSizeZoomed;
    dst.h = tileSizeZoomed;

    SDL_RenderCopy(renderer, overlayAtlas, &toRender.src, &dst);
}

void tileManager::renderTile(int tile, int x, int y) {
    float tileSizeZoomed = std::floor(16 * camZoom);
    Tile toRender = getTile(tile);

    SDL_Rect dst;
    dst.x = (x - camX) * tileSizeZoomed;
    dst.y = (y - camY) * tileSizeZoomed;
    dst.w = tileSizeZoomed;
    dst.h = tileSizeZoomed;

    SDL_RenderCopy(renderer, atlas, &toRender.src, &dst);
}

void tileManager::renderDecorationTile(int x, int y, SDL_Rect dst) {
    // I know - this code looks like a bunch of spaghetti. That might be what it is, but it works well enough.
    // For each biome, I can just copy the respective code and change the specifics around a bit, like the texture to use. If I'm quick about it it'll take about 10 minutes to add a new biome to this part of the code.
    // Mediocre.
    // TODO: improve on this by stuffing this kind of data into the tiles.txt somehow, and generalise this function. Or don't "If it ain't broke, don't fix it."
    // Keep all the "blocks" collapsed.
    const int halfWorldSize = worldSize * 0.5;
    const SDL_RendererFlip hori = SDL_FLIP_HORIZONTAL; // helpers
    const SDL_RendererFlip vert = SDL_FLIP_VERTICAL;
    const SDL_RendererFlip none = SDL_FLIP_NONE;
    const SDL_RendererFlip both = (SDL_RendererFlip)(SDL_FLIP_HORIZONTAL | SDL_FLIP_VERTICAL);
    // Could store all the surrounding tiles in a 2D array, but doing it like this made more sense in my head + it's a bit easier to read (for me).
    // Holy shit this is pretty shitty
    Point topLeft     = getTile(x - 1, y - 1);
    Point top         = getTile(x    , y - 1);
    Point topRight    = getTile(x + 1, y - 1);
    Point left        = getTile(x - 1, y    );
    Point center      = getTile(x    , y    );
    Point right       = getTile(x + 1, y    );
    Point bottomLeft  = getTile(x - 1, y + 1);
    Point bottom      = getTile(x    , y + 1);
    Point bottomRight = getTile(x + 1, y + 1);
    int centerHeight = getTile(center.surface).height;
    Biome c  = getTileBiome(center.surface);
    Biome l  = getTileBiome(left.surface);
    Biome r  = getTileBiome(right.surface);
    Biome t  = getTileBiome(top.surface);
    Biome b  = getTileBiome(bottom.surface);
    Biome tl = getTileBiome(topLeft.surface);
    Biome tr = getTileBiome(topRight.surface);
    Biome br = getTileBiome(bottomRight.surface);
    Biome bl = getTileBiome(bottomLeft.surface);
    Biome toCheck;

    // DESERT
    toCheck = DESERT_BIOME;
    if((c != toCheck) && getTile(center.surface).height < getTile(DESERT_EMPTY).height) {
        do { // simple hack to break out of the biome instgead of returning
        if(l == toCheck && r == toCheck && t == toCheck && b == toCheck) { renderTile(DESERT_FILLED, &dst); break;}
        // ENCLOSED
        if(l == toCheck && r == toCheck && t == toCheck) { renderTile(DESERT_ENCLOSED1, &dst); break; }
        if(r == toCheck && b == toCheck && t == toCheck) { renderTile(DESERT_ENCLOSED2, &dst); break; }
        if(l == toCheck && r == toCheck && b == toCheck) { renderTile(DESERT_ENCLOSED3, &dst); break; }
        if(l == toCheck && b == toCheck && t == toCheck) { renderTile(DESERT_ENCLOSED4, &dst); break; }
        // DOUBLE SIDES
        if(l == toCheck && r == toCheck) { renderTile(DESERT_SIDE_E, &dst); renderTile(DESERT_SIDE_W, &dst); break; }
        if(t == toCheck && b == toCheck) { renderTile(DESERT_SIDE_S, &dst); renderTile(DESERT_SIDE_N, &dst); break; }
        // OUTER CORNERS
        if(tl == toCheck && t != toCheck && l != toCheck) { renderTile(DESERT_CORNER, &dst); }
        if(tr == toCheck && t != toCheck && r != toCheck) { renderTile(DESERT_CORNER, &dst, 0, hori); }
        if(br == toCheck && b != toCheck && r != toCheck) { renderTile(DESERT_CORNER, &dst, 0, both); }
        if(bl == toCheck && b != toCheck && l != toCheck) { renderTile(DESERT_CORNER, &dst, 0, vert); }
        // INNER CORNERS
        if(l == toCheck && t == toCheck) { renderTile(DESERT_INNER_CORNER_NW, &dst); break; }
        if(r == toCheck && t == toCheck) { renderTile(DESERT_INNER_CORNER_NE, &dst); break; }
        if(r == toCheck && b == toCheck) { renderTile(DESERT_INNER_CORNER_SE, &dst); break; }
        if(l == toCheck && b == toCheck) { renderTile(DESERT_INNER_CORNER_SW, &dst); break; }
        // SINGLE SIDES
        if(t == toCheck)      { renderTile(DESERT_SIDE_N, &dst); }
        if(r == toCheck)      { renderTile(DESERT_SIDE_E, &dst); }
        if(b == toCheck)      { renderTile(DESERT_SIDE_S, &dst); }
        if(l == toCheck)      { renderTile(DESERT_SIDE_W, &dst); }
        } while(false);
    }

    // PLAINS
    toCheck = PLAINS_BIOME;
    if((c != toCheck) && (getTile(center.surface).height < getTile(PLAINS_VAR1).height)) {
        do { // simple hack to break out of the biome instgead of returning
        if(l == toCheck && r == toCheck && t == toCheck && b == toCheck) { renderTile(PLAINS_FILLED, &dst); break;}
        // ENCLOSED
        if(l == toCheck && r == toCheck && t == toCheck) { renderTile(PLAINS_ENCLOSED, &dst); break; }
        if(r == toCheck && b == toCheck && t == toCheck) { renderTile(PLAINS_ENCLOSED, &dst, 90); break; }
        if(l == toCheck && r == toCheck && b == toCheck) { renderTile(PLAINS_ENCLOSED, &dst, 180); break; }
        if(l == toCheck && b == toCheck && t == toCheck) { renderTile(PLAINS_ENCLOSED, &dst, 270); break; }
        // DOUBLE SIDES
        if(l == toCheck && r == toCheck) { renderTile(PLAINS_SIDE, &dst, 0 ); renderTile(PLAINS_SIDE, &dst, 180); break; }
        if(t == toCheck && b == toCheck) { renderTile(PLAINS_SIDE, &dst, 90); renderTile(PLAINS_SIDE, &dst, 270); break; }
        // OUTER CORNERS
        if(tl == toCheck && t != toCheck && l != toCheck) { renderTile(PLAINS_CORNER, &dst); }
        if(tr == toCheck && t != toCheck && r != toCheck) { renderTile(PLAINS_CORNER, &dst, 0, hori); }
        if(br == toCheck && b != toCheck && r != toCheck) { renderTile(PLAINS_CORNER, &dst, 0, both); }
        if(bl == toCheck && b != toCheck && l != toCheck) { renderTile(PLAINS_CORNER, &dst, 0, vert); }
        // INNER CORNERS
        if(l == toCheck && t == toCheck) { renderTile(PLAINS_INNER_CORNER, &dst); break; }
        if(r == toCheck && t == toCheck) { renderTile(PLAINS_INNER_CORNER, &dst, 90); break; }
        if(r == toCheck && b == toCheck) { renderTile(PLAINS_INNER_CORNER, &dst, 180); break; }
        if(l == toCheck && b == toCheck) { renderTile(PLAINS_INNER_CORNER, &dst, 270); break; }
        // SINGLE SIDES
        if(b == toCheck)      { renderTile(PLAINS_SIDE, &dst, -90); }
        if(l == toCheck)      { renderTile(PLAINS_SIDE, &dst, 0); }
        if(t == toCheck)      { renderTile(PLAINS_SIDE, &dst, 90); }
        if(r == toCheck)      { renderTile(PLAINS_SIDE, &dst, 180); }
        } while(false);
    }

    // WATER
    toCheck = WATER_BIOME;
    if((c != toCheck) && getTile(center.surface).height < getTile(WATER_VAR1).height) {
        do { // simple hack to break out of the biome instgead of returning
            if(l == toCheck && r == toCheck && t == toCheck && b == toCheck) { renderTile(WATER_FILLED, &dst); break;}
            // ENCLOSED
            if(l == toCheck && r == toCheck && t == toCheck) { renderTile(WATER_ENCLOSED1, &dst); break; }
            if(l == toCheck && r == toCheck && b == toCheck) { renderTile(WATER_ENCLOSED1, &dst, 180); break; }
            if(r == toCheck && b == toCheck && t == toCheck) { renderTile(WATER_ENCLOSED2, &dst, 180); break; }
            if(l == toCheck && b == toCheck && t == toCheck) { renderTile(WATER_ENCLOSED2, &dst); break; }
            // DOUBLE SIDES
            if(l == toCheck && r == toCheck) { renderTile(WATER_SIDE_EW, &dst); renderTile(WATER_SIDE_EW, &dst, 180); break; }
            if(t == toCheck && b == toCheck) { renderTile(WATER_SIDE_NS, &dst); renderTile(WATER_SIDE_NS, &dst, 180); break; }
            // OUTER CORNERS
            if(tl == toCheck && t != toCheck && l != toCheck) { renderTile(WATER_CORNER, &dst); }
            if(tr == toCheck && t != toCheck && r != toCheck) { renderTile(WATER_CORNER, &dst, 0, hori); }
            if(br == toCheck && b != toCheck && r != toCheck) { renderTile(WATER_CORNER, &dst, 0, both); }
            if(bl == toCheck && b != toCheck && l != toCheck) { renderTile(WATER_CORNER, &dst, 0, vert); }
            // INNER CORNERS
            if(l == toCheck && t == toCheck) { renderTile(WATER_INNER_CORNER, &dst); break; }
            if(r == toCheck && t == toCheck) { renderTile(WATER_INNER_CORNER, &dst, 90); break; }
            if(r == toCheck && b == toCheck) { renderTile(WATER_INNER_CORNER, &dst, 180); break; }
            if(l == toCheck && b == toCheck) { renderTile(WATER_INNER_CORNER, &dst, 270); break; }
            // SINGLE SIDES
            if(t == toCheck)      { renderTile(WATER_SIDE_NS, &dst); }
            if(r == toCheck)      { renderTile(WATER_SIDE_EW, &dst, 180); }
            if(b == toCheck)      { renderTile(WATER_SIDE_NS, &dst, 180); }
            if(l == toCheck)      { renderTile(WATER_SIDE_EW, &dst); }
        } while(false);
    }

    // CLOUD
    toCheck = CLOUD_BIOME;
    if((c != toCheck) && getTile(center.surface).height < getTile(CLOUD).height) {
        do { // simple hack to break out of the biome instgead of returning
        // INNER CORNERS
        if(l == toCheck && t == toCheck) { renderTile(CLOUD_INNER_CORNER, &dst); break; }
        if(r == toCheck && t == toCheck) { renderTile(CLOUD_INNER_CORNER, &dst, 90); break; }
        if(r == toCheck && b == toCheck) { renderTile(CLOUD_INNER_CORNER, &dst, 180); break; }
        if(l == toCheck && b == toCheck) { renderTile(CLOUD_INNER_CORNER, &dst, 270); break; }
        // SINGLE SIDES
        if(b == toCheck)      { renderTile(CLOUD_SIDE, &dst); }
        if(l == toCheck)      { renderTile(CLOUD_SIDE, &dst, 90); }
        if(t == toCheck)      { renderTile(CLOUD_SIDE, &dst, 180); }
        if(r == toCheck)      { renderTile(CLOUD_SIDE, &dst, 270); }
        } while(false);
    }
}

tileManager::Point tileManager::getTile(int x, int y, bool half) {
    if(half) {
        const int halfWorldSize = worldSize * 0.5;

        if(x >= halfWorldSize || x < -halfWorldSize || y >= halfWorldSize || y < -halfWorldSize) return outOfBoundsPoint;


        return worldMap[x + halfWorldSize][y + halfWorldSize];
    } else {
        if(x >= worldSize || x <= 0 || y >= worldSize || y <= 0) return outOfBoundsPoint;

        return worldMap[x][y];
    }
}

tileManager::Tile tileManager::getTile(const int tileIndex) {
    Tile returnTile = tiles.at(tileIndex);
    if(returnTile.src.w > 0) {
        return returnTile;
    } else {
        return tiles.at(NOT_FOUND);
    }
}

tileManager::Tile tileManager::getTile(enum tileAlias tileName) {
    Tile returnTile = tiles.at(tileName);
    if(returnTile.src.w > 0) {
        return returnTile;
    } else {
        return tiles.at(NOT_FOUND);
    }
}

void tileManager::setTile(int x, int y, int tile) {
    if(x > worldSize / 2 - 1|| y > worldSize / 2 - 1 || x < -worldSize / 2 || y < -worldSize / 2) {/*std::cout << "error setting tile " << x << "," << y << "to " << tile << std::endl;*/return;}
    worldMap[x + worldSize / 2][y + worldSize /2].overlay = static_cast<overlayAlias>(tile);
}

void tileManager::setWorldTile(int x, int y, int tile) {
    if(x > worldSize / 2 - 1|| y > worldSize / 2 - 1 || x < -worldSize / 2 || y < -worldSize / 2) {/*std::cout << "error setting tile " << x << "," << y << "to " << tile << std::endl;*/return;}
    worldMap[x + worldSize / 2][y + worldSize /2].surface = static_cast<tileAlias>(tile);

    for(int i = -7; i < 8; i++) {
        for(int j = -7; j < 8; j++) {
            pass2xy(x + i + worldSize * 0.5, y + j + worldSize * 0.5); // update the few surrounding tiles
        }
    }
}

void tileManager::renderTile(enum tileAlias tileName, const SDL_Rect* dst, double angle, SDL_RendererFlip flipVal) {
    Tile toRender = getTile(tileName);

    SDL_RenderCopyEx(renderer, atlas, &toRender.src, dst, angle, NULL, flipVal); // idk if SDL_FLIP_NONE is neccessary
    // https://wiki.libsdl.org/SDL2/SDL_RendererFlip
}

#ifndef __TBG_HEADER__
#define __TBG_HEADER__

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

#include "command.hpp"
#include "item.hpp"
#include "npc.hpp"
#include "room.hpp"

struct Direction {
    std::string repr;
    std::string abbr;
    Direction& reverse;
};


class Player {
    public:

    Player();
};

class TextBasedGame {

    private:

    Player player;
    std::unordered_map<std::string, std::shared_ptr<Room>> rooms;
    std::unordered_map<std::string, std::shared_ptr<Item>> items;
    std::unordered_map<std::string, std::shared_ptr<NPC>> npcs;

    struct DirectionSet { Direction North, East, South, West; };

    public:

    static DirectionSet Directions;
    
    TextBasedGame();
    TextBasedGame(std::function<void(std::string)> _writeFunc);

    // IO - keep these public
    void EvalPlayerInput(std::string);
    std::function<void(std::string)> WriteGameOutput;

    std::vector<Command&> GetCommands();
    
};

#endif /* __TBG_HEADER__ */

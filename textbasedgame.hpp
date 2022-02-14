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
    
    struct DirectionSet {
        Direction North, East, South, West;
    };

    enum State {
        Title = 0,
        Gameplay,
    };

    State state;

    Player player;
    std::unordered_map<std::string, std::shared_ptr<Room>> rooms;
    std::unordered_map<std::string, std::shared_ptr<Item>> items;
    std::unordered_map<std::string, std::shared_ptr<NPC>> npcs;

    public:

    static DirectionSet Directions;
    
    TextBasedGame();
    TextBasedGame(std::function<void(std::string)> _writeFunc);

    /* IO - keep these public */
    void EvalPlayerInput(std::string);
    std::function<void(std::string)> WriteGameOutput;

    // returns a list of all currently valid commands
    std::vector<Command> GetCommands();

    // contains initialization logic for each state
    void SetState(State newState);
    
};

#endif /* __TBG_HEADER__ */

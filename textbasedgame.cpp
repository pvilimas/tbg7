#include "textbasedgame.hpp"

/* ------- PLAYER ------- */

Player::Player() {}

/* ------- TEXTBASEDGAME ------- */

TextBasedGame::DirectionSet TextBasedGame::Directions = {
    .North = Direction { .repr = "north", .abbr = "n", .reverse = TextBasedGame::Directions.South },
    .South = Direction { .repr = "south", .abbr = "s", .reverse = TextBasedGame::Directions.North },
    .East = Direction { .repr = "east", .abbr = "e", .reverse = TextBasedGame::Directions.West },
    .West = Direction { .repr = "west", .abbr = "w", .reverse = TextBasedGame::Directions.East },
};

TextBasedGame::TextBasedGame() {
    WriteGameOutput = nullptr;
}

TextBasedGame::TextBasedGame(std::function<void(std::string)> _writeFunc) {
    WriteGameOutput = _writeFunc;
}

void TextBasedGame::EvalPlayerInput(std::string s) {
    for (Command& c : GetCommands()) {
        if (c.Eval(s)) {
            break;
        }
    }
}

std::vector<Command> TextBasedGame::GetCommands() {
    std::vector<Command> cmds;
    
    // game commands
    cmds.push_back(Command("Help", false, "", "help( me)?", [&]{ WriteGameOutput("This is the help message."); }));
    cmds.push_back(Command("Quit Game", false, "", "(q(uit)?|exit)", [&]{ exit(0); }));

    // failsafes
    cmds.push_back(Command("Failsafe: Match All", true, "", ".*", [&]{ WriteGameOutput("Command not recognized."); }));

    return cmds;
}
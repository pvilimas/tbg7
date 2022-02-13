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
    std::cout << "evaling string: " << s << std::endl;
}
#include "textbasedgame.hpp"

/* ------- PLAYER ------- */

Player::Player() {
    currentRoom = nullptr;
}

// no failure check
void Player::Move(Direction dir) {
    currentRoom = currentRoom->GetPath(dir);
}

/* ------- TEXTBASEDGAME ------- */

TextBasedGame::DirectionSet TextBasedGame::Directions = {
    .North = Direction { .id = 0, .repr = "north", .abbr = "n", .reverse = TextBasedGame::Directions.South },
    .South = Direction { .id = 1, .repr = "south", .abbr = "s", .reverse = TextBasedGame::Directions.North },
    .East = Direction { .id = 2, .repr = "east", .abbr = "e", .reverse = TextBasedGame::Directions.West },
    .West = Direction { .id = 3, .repr = "west", .abbr = "w", .reverse = TextBasedGame::Directions.East },
};

TextBasedGame::TextBasedGame() {
    WriteGameOutput = nullptr;
}

TextBasedGame::TextBasedGame(std::function<void(std::string)> _writeFunc) {
    state = State::Title;
    WriteGameOutput = _writeFunc;
    WriteGameOutput("You are on the title screen.");

    // init rooms

    rooms = {
        {"Kitchen", std::make_shared<Room>("Kitchen")},
        {"Bedroom", std::make_shared<Room>("Bedroom")},
    };

    // link rooms

    rooms.at("Kitchen")->Link(Directions.North, *rooms.at("Bedroom"));

    player.currentRoom = &*rooms.at("Kitchen");

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

    if (state == State::Title) {
        cmds.push_back(Command("Start Game", false, "", "start( game)?", [&]{ SetState(State::Gameplay); }));
    }

    if (state == State::Gameplay) {
        cmds.push_back(Command("Go North", false, "", "((go|move) )?n(orth)?", [&] { TryMove(Directions.North); }));
        cmds.push_back(Command("Go South", false, "", "((go|move) )?s(outh)?", [&] { TryMove(Directions.South); }));
        cmds.push_back(Command("Go East", false, "", "((go|move) )?e(ast)?", [&] { TryMove(Directions.East); }));
        cmds.push_back(Command("Go West", false, "", "((go|move) )?w(est)?", [&] { TryMove(Directions.West); }));
    }

    // room commands

    if (state == State::Gameplay) {
        cmds.push_back(Command("Get Current Room", false, "", "c(urrent)?( )?r(oom)?", [&]{ WriteGameOutput("You are in the " + player.currentRoom->GetName()); }));
    }

    // failsafes
    cmds.push_back(Command("Failsafe: Match All", true, "", ".*", [&]{ WriteGameOutput("Command not recognized."); }));

    return cmds;
}

void TextBasedGame::SetState(TextBasedGame::State newState) {
    State oldState = state;

    // if/else stuff here with newState vs oldState

    // if starting the game
    if (oldState == State::Title && newState == State::Gameplay) {
        WriteGameOutput("You are now in the game.");
    }

    state = newState;
}

void TextBasedGame::TryMove(Direction dir) {
    if (player.currentRoom->GetPath(dir) != nullptr) {
        player.Move(dir);
    } else {
        WriteGameOutput("You can't go that way.");
    }
}
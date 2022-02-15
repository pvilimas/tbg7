#include "textbasedgame.hpp"

/* ------- PLAYER ------- */

Player::Player() {
    currentRoom = nullptr;
}

Room *Player::GetCurrentRoom() {
    return currentRoom;
}

void Player::SetCurrentRoom(Room* r) {
    currentRoom = r;
}

std::vector<Item*> Player::GetInventory() {
    return inventory;
}

std::string Player::ReprInventory() {
    std::string s = "Your inventory contains ";
    switch (inventory.size()) {
        case 0: return "Your inventory is empty.";
        case 1: return s + inventory.at(0)->GetRepr() + ".";
        case 2: return s + inventory.at(0)->GetRepr() + " and "  + inventory.at(1)->GetRepr() + ".";
        case 3: return s + inventory.at(0)->GetRepr() + ", "     + inventory.at(1)->GetRepr() + ", and " + inventory.at(2)->GetRepr() + ".";
        default: {
            size_t i; // scope issue, declare here
            for (i = 0; i < inventory.size() - 1; i++) {
                s += inventory.at(i)->GetRepr();
                s += ", ";
            }
            s += "and ";
            s += inventory.at(i)->GetRepr();
            s += ".";
            return s;
        }
    }
}

bool Player::HasItem(Item* i) {
    for (Item const *_i : inventory) {
        if (i == _i) {
            return true;
        }
    }
    return false;
}

void Player::TakeItem(Item* i) {
    inventory.push_back(i);
}

void Player::DropItem(Item* i) {
    int count = 0;
    for (auto& _i : inventory) {
        if (i == _i) {
            inventory.erase(inventory.begin()+count);
            return;
        }
        count++;
    }
}


// no failure check
void Player::Move(Direction dir) {
    currentRoom = currentRoom->GetPath(dir);
}

/* ------- TEXTBASEDGAME ------- */

std::string TextBasedGame::Messages::Title = "You are on the title screen.";
std::string TextBasedGame::Messages::Help = "This is the help message.";
std::string TextBasedGame::Messages::ErrorUnknownCmd = "Command not recognized.";

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

    // init rooms

    rooms = {
        { "Kitchen", std::make_shared<Room>(
            "Kitchen",
            std::unordered_map<Room::MessageType, std::string> {
                { Room::MessageType::OnEnter, "You have entered the kitchen." },
                { Room::MessageType::OnLook, "You are in the kitchen. A red key sits on the counter." },
                { Room::MessageType::OnStay, "You are in the kitchen." },
            }
        )},
        { "Bedroom", std::make_shared<Room>(
            "Bedroom",
            std::unordered_map<Room::MessageType, std::string> {
                { Room::MessageType::OnEnter, "You have entered the bedroom." },
                { Room::MessageType::OnLook, "You are in the bedroom. A red key door is across from the bed." },
                { Room::MessageType::OnStay, "You are in the bedroom." },
            }
        )},
    };

    // link rooms

    rooms.at("Kitchen")->Link(Directions.North, *rooms.at("Bedroom"));

    // init items

    items = {
        {"Red Key", std::make_shared<Item>(
            "Red Key", "a red key"
        )},
        {"Red Door", std::make_shared<Item>(
            "Red Door", "a red door"
        )},
    };


    player.SetCurrentRoom(&*rooms.at("Kitchen"));
    player.TakeItem(&*items.at("Red Key"));

    WriteGameOutput(TextBasedGame::Messages::Title);

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
    
    /* game commands */
    cmds.push_back(Command("Help", false, "help", "help( me)?", [&]{ WriteGameOutput(TextBasedGame::Messages::Help); }));
    cmds.push_back(Command("Quit Game", false, "quit", "(q(uit)?|exit)", [&]{ throw TextBasedGame::QuitGameException(); }));

    if (state == State::Title) {
        cmds.push_back(Command("Start Game", false, "start", "start( game)?", [&]{ SetState(State::Gameplay); }));
    }

    /* movement */
    if (state == State::Gameplay) {
        cmds.push_back(Command("Go North", false, "north", "((go|move) )?n(orth)?", [&] { TryMove(Directions.North); }));
        cmds.push_back(Command("Go South", false, "south", "((go|move) )?s(outh)?", [&] { TryMove(Directions.South); }));
        cmds.push_back(Command("Go East", false, "east", "((go|move) )?e(ast)?", [&] { TryMove(Directions.East); }));
        cmds.push_back(Command("Go West", false, "west", "((go|move) )?w(est)?", [&] { TryMove(Directions.West); }));
    }

    /* other top level actions */
    if (state == State::Gameplay) {
        cmds.push_back(Command("Look Around", false, "look around", "look( around)?", [&]{ WriteGameOutput(player.GetCurrentRoom()->GetMessage(Room::MessageType::OnLook)); }));
        cmds.push_back(Command("Get Current Room", false, "where am i", "where am i", [&]{ WriteGameOutput("You are in the " + player.GetCurrentRoom()->GetName() + "."); }));
        cmds.push_back(Command("Check Inventory", false, "check inventory", "(check )?inv(entory)?", [&]{ WriteGameOutput(player.ReprInventory()); }));
    }

    // room commands


    // item commands

    // npc commands

    // failsafes
    cmds.push_back(Command("Failsafe: Match All", true, "", ".*", [&]{ WriteGameOutput(TextBasedGame::Messages::ErrorUnknownCmd); }));

    return cmds;
}

void TextBasedGame::SetState(TextBasedGame::State newState) {
    State oldState = state;

    // if/else stuff here with newState vs oldState

    // if starting the game
    if (oldState == State::Title && newState == State::Gameplay) {
        WriteGameOutput(player.GetCurrentRoom()->GetMessage(Room::MessageType::OnStay));
    }

    state = newState;
}

void TextBasedGame::TryMove(Direction dir) {
    if (player.GetCurrentRoom()->GetPath(dir) != nullptr) {
        player.Move(dir);
        WriteGameOutput(player.GetCurrentRoom()->GetMessage(Room::MessageType::OnEnter));
    } else {
        WriteGameOutput("You can't go that way.");
    }
}
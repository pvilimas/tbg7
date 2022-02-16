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

inline std::string Player::fullRepr(std::string itemRepr) {
    return (itemRepr[0] == 'a' || itemRepr[0] == 'e' || itemRepr[0] == 'i' || itemRepr[0] == 'o' || itemRepr[0] == 'u')
        ? "an " + itemRepr
        : "a " + itemRepr;
}

std::string Player::ReprInventory() {

    std::string s = "Your inventory contains ";
    switch (inventory.size()) {
        case 0: return "Your inventory is empty.";
        case 1: return s + fullRepr(inventory.at(0)->GetRepr()) + ".";
        case 2: return s + fullRepr(inventory.at(0)->GetRepr()) + " and "  + fullRepr(inventory.at(1)->GetRepr()) + ".";
        case 3: return s + fullRepr(inventory.at(0)->GetRepr()) + ", "     + fullRepr(inventory.at(1)->GetRepr()) + ", and " + fullRepr(inventory.at(2)->GetRepr()) + ".";
        default: {
            size_t i; // scope issue, declare here
            for (i = 0; i < inventory.size() - 1; i++) {
                s += fullRepr(inventory.at(i)->GetRepr());
                s += ", ";
            }
            s += "and ";
            s += fullRepr(inventory.at(i)->GetRepr());
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

/* static stuff */

std::string TextBasedGame::Messages::Title = "You are on the title screen.";
std::string TextBasedGame::Messages::Help = "This is the help message.";
std::string TextBasedGame::Messages::PromptQuit = "Do you want to quit? (y/n)";

// TODO: functions here, to copy the exact verb the player used, unless it's invalid, in which case "what do you want to do?"
//                                   ^ eg. "what do you want to pick up?"
// maybe look at STR_KEYWORDS from tbg5/alpha/globals to get an idea
// "take key" -> "take key"
std::string TextBasedGame::Messages::ErrorInvalidTake = "You're already carrying that!";
// "take <item not in curr room>"
std::string TextBasedGame::Messages::ErrorInvalidTakeMissing = "I don't see that in here.";
// "use <unusable item>"
std::string TextBasedGame::Messages::ErrorInvalidUse = "You can't use that.";
// "drop key" -> "drop key"
std::string TextBasedGame::Messages::ErrorInvalidDrop = "You're not carrying that.";
// "take"
std::string TextBasedGame::Messages::ErrorMissingTake = "What do you want to take?";
// "use"
std::string TextBasedGame::Messages::ErrorMissingUse = "What do you want to use?";
// "drop"
std::string TextBasedGame::Messages::ErrorMissingDrop = "What do you want to drop?";
/*
    "use the sajdkadlas"
    "take sjdalskdad"
    "drop asdhjasfdjasme"
*/
std::string TextBasedGame::Messages::ErrorUnknownItem = "Item not found.";
// "sajkdhasd"
std::string TextBasedGame::Messages::ErrorUnknownCmd = "Command not recognized.";
// same as above but for the quit conf
std::string TextBasedGame::Messages::ErrorUnknownQuitCmd = "Command not recognized. Do you want to quit? (y/n)";

TextBasedGame::DirectionSet TextBasedGame::Directions = {
    .North = Direction { .id = 0, .repr = "north", .abbr = "n", .reverse = TextBasedGame::Directions.South },
    .South = Direction { .id = 1, .repr = "south", .abbr = "s", .reverse = TextBasedGame::Directions.North },
    .East = Direction { .id = 2, .repr = "east", .abbr = "e", .reverse = TextBasedGame::Directions.West },
    .West = Direction { .id = 3, .repr = "west", .abbr = "w", .reverse = TextBasedGame::Directions.East },
};

/* instance stuff */

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
            },
            std::vector<Command>{}
        )},
        { "Bedroom", std::make_shared<Room>(
            "Bedroom",
            std::unordered_map<Room::MessageType, std::string> {
                { Room::MessageType::OnEnter, "You have entered the bedroom." },
                { Room::MessageType::OnLook, "You are in the bedroom. A red key door is across from the bed." },
                { Room::MessageType::OnStay, "You are in the bedroom." },
            },
            std::vector<Command> {
                Command("Bedroom: Sleep", false, "sleep in bed", "sleep(in (the )?bed)?", [*this]{ WriteGameOutput("You slept in the bed. You woke up feeling refreshed."); })
            }
        )},
    };

    // link rooms

    rooms.at("Kitchen")->Link(Directions.North, *rooms.at("Bedroom"));

    // init items

    items = {
        {"Red Key", std::make_shared<Item>(
            "Red Key", "red key", std::vector<Command>{}
        )},
        {"Red Door", std::make_shared<Item>(
            "Red Door", "red door", std::vector<Command>{
                Command("Red Door: Open", false, "", "open (red )?door", [*this]{ WriteGameOutput("You opened the red door."); })
            }
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
    if (state == State::Title || state == State::Gameplay) {
        cmds.push_back(Command("Help", false, "help", "help( me)?", [&]{ WriteGameOutput(TextBasedGame::Messages::Help); }));
        cmds.push_back(Command("Quit Game", false, "quit", "(q(uit)?|exit)", [&]{ SetState(TextBasedGame::State::Quitting); }));
    }

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
        // TODO: boost::format?
        cmds.push_back(Command("Get Current Room", false, "where am i", "where am i", [&]{ WriteGameOutput("You are in the " + player.GetCurrentRoom()->GetName() + "."); }));
        cmds.push_back(Command("Check Inventory", false, "check inventory", "(check )?inv(entory)?", [&]{ WriteGameOutput(player.ReprInventory()); }));
    }

    // room commands

    // special room cmds
    if (state == State::Gameplay) {
        for (Command c : player.GetCurrentRoom()->GetCommands()) {
            cmds.push_back(c);
        }
    }

    // item commands
    if (state == State::Gameplay) {
        for (auto& it : items) {
            auto name = it.first;
            auto item = it.second;

            // standard item cmds
            cmds.push_back(Command("Take" + name, false, "", "take " + name, [&]{ TryTakeItem(&*item); }));
            cmds.push_back(Command("Use" + name, false, "", "use " + name, [&]{ WriteGameOutput("You used the " + item->GetRepr() + "."); }));
            cmds.push_back(Command("Drop" + name, false, "", "drop " + name, [&]{ TryDropItem(&*item); }));

            // special item cmds
            for (Command c : item->GetCommands()) {
                cmds.push_back(c);
            }
        }
    }


    // npc commands

    // failsafes
    if (state == State::Gameplay) {
        cmds.push_back(Command("Failsafe: Take/Drop/Use Invalid Item", true, "", "(take|drop|use) .*", [&]{ WriteGameOutput(TextBasedGame::Messages::ErrorUnknownItem); }));
        cmds.push_back(Command("Failsafe: Missing Take", true, "", "take", [&]{ WriteGameOutput(TextBasedGame::Messages::ErrorMissingTake); }));
        cmds.push_back(Command("Failsafe: Missing Drop", true, "", "drop", [&]{ WriteGameOutput(TextBasedGame::Messages::ErrorMissingDrop); }));
        cmds.push_back(Command("Failsafe: Missing Use", true, "", "use", [&]{ WriteGameOutput(TextBasedGame::Messages::ErrorMissingUse); }));
    }
    
    // quit conf
    if (state == State::Quitting) {
        cmds.push_back(Command("Quit Confirmation: Yes", true, "", "y(es)?", [&]{ throw TextBasedGame::QuitGameException(); }));
        cmds.push_back(Command("Quit Confirmation: No", true, "", "n(o)?", [&]{ SetState(preQuitState); }));
        cmds.push_back(Command("Quit Confirmation: Unknown", true, "", ".*", [&]{ WriteGameOutput(TextBasedGame::Messages::ErrorUnknownQuitCmd); }));
    } else {
        cmds.push_back(Command("Failsafe: Match All", true, "", ".*", [&]{ WriteGameOutput(TextBasedGame::Messages::ErrorUnknownCmd); }));
    }
    

    return cmds;
}

void TextBasedGame::SetState(TextBasedGame::State newState) {
    State oldState = state;

    if (oldState == newState) {
        return;
    }

    // if/else stuff here with newState vs oldState

    // if starting the game, write startup message
    if (oldState == State::Title && newState == State::Gameplay) {
        WriteGameOutput(player.GetCurrentRoom()->GetMessage(Room::MessageType::OnStay));
    }
    // if going to quit screen, save previous state to return
    else if (newState == State::Quitting) {
        preQuitState = oldState;
        WriteGameOutput(TextBasedGame::Messages::PromptQuit);
    }
    // if returning from quit conf
    else if (oldState == State::Quitting) {
        if (newState == State::Title) {
            WriteGameOutput(TextBasedGame::Messages::Title);
        } else if (newState == State::Gameplay) {
            WriteGameOutput(player.GetCurrentRoom()->GetMessage(Room::MessageType::OnStay));
        }
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

void TextBasedGame::TryTakeItem(Item* i) {
    if (player.HasItem(i)) {
        WriteGameOutput(TextBasedGame::Messages::ErrorInvalidTake);
    } else if (!player.GetCurrentRoom()->HasItem(i)) {
        WriteGameOutput(TextBasedGame::Messages::ErrorInvalidTakeMissing);
    } else {
        player.TakeItem(i);
        player.GetCurrentRoom()->RemoveItem(i);
        WriteGameOutput("You picked up the " + i->GetRepr() + ".");
    }
}

void TextBasedGame::TryDropItem(Item* i) {
    if (!player.HasItem(i)) {
        WriteGameOutput(TextBasedGame::Messages::ErrorInvalidDrop);
    } else {
        player.DropItem(i);
        player.GetCurrentRoom()->AddItem(i);
        WriteGameOutput("You dropped the " + i->GetRepr() + ".");
    }
}

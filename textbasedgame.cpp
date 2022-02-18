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

std::vector<std::shared_ptr<Item>> Player::GetInventory() {
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

bool Player::HasItem(std::shared_ptr<Item> i) {
    for (auto _i : inventory) {
        if (i == _i) {
            return true;
        }
    }
    return false;
}

void Player::TakeItem(std::shared_ptr<Item> i) {
    inventory.push_back(i);
}

void Player::DropItem(std::shared_ptr<Item> i) {
    int count = 0;
    for (auto _i : inventory) {
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

// "go north" when there is no room there
std::string TextBasedGame::Messages::ErrorBlockedDir = "You can't go that way.";
// "go askdjad"
std::string TextBasedGame::Messages::ErrorInvalidDir = "Which way do you want to go?";
// "go"
std::string TextBasedGame::Messages::ErrorMissingDir = "Which way do you want to go?";
// "take key" -> "take key"
std::string TextBasedGame::Messages::ErrorInvalidTake = "You're already carrying that!";
// "take <item not in curr room>"
std::string TextBasedGame::Messages::ErrorInvalidTakeMissing = "I don't see that in here.";
// "take <item with CanCarry=false>"
std::string TextBasedGame::Messages::ErrorIllegalTake = "You can't pick that up.";
// "take <item with CanUse=false>"
std::string TextBasedGame::Messages::ErrorIllegalUse = "You can't use that.";
// nonsense use like "unlock door with ice cube"
std::string TextBasedGame::Messages::ErrorIllegalUseThatWay = "You can't use it that way.";
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
                { Room::MessageType::OnLook, "You are in the bedroom. A blue door is across from the bed." },
                { Room::MessageType::OnStay, "You are in the bedroom." },
            },
            std::vector<Command> {
                Command("Bedroom: Sleep", false, "sleep in bed", "sleep(in (the )?bed)?", [&]{ WriteGameOutput("You slept in the bed. You woke up feeling refreshed."); })
            }
        )},
    };

    // link rooms

    rooms.at("Kitchen")->Link(Directions.North, *rooms.at("Bedroom"));

    // init items

    items = {
        {"Test Item 1", std::make_shared<Item>(
            "Test Item 1", "test item 1",
            std::vector<Command>{
//                Command("Test Item 1: Take", false, "", "take t1", [this]{ std::cout << __LINE__ << std::endl;TryTakeItem(items.at("Test Item 1"));std::cout << __LINE__ << std::endl; }),
                Command("Test Item 1: Use", false, "", "use t1", [&]{ WriteGameOutput("You used t1."); }),
//                Command("Test Item 1: Drop", false, "", "drop t1", [this]{ TryDropItem(items.at("Test Item 1")); }),
            },
            Item::Attrs {
                .CanCarry = true,
                .CanUse = true,
            }
        )},
        {"Red Key", std::make_shared<Item>(
            "Red Key", "red key",
            std::vector<Command>{
//                Command("Red Key: Take", false, "", "take (red )?key", [this]{ TryTakeItem(items.at("Red Key")); }),
//                Command("Red Key: Use", false, "", "use (red )?key", [&]{ WriteGameOutput("You can't use that."); }),
//                Command("Red Key: Drop", false, "", "drop (red )?key", [this]{ TryDropItem(items.at("Red Key")); }),
            },
            Item::Attrs {
                .CanCarry = true,
                .CanUse = true,
            }
        )},
        {"Blue Door", std::make_shared<Item>(
            "Blue Door", "blue door",
            std::vector<Command>{
//                Command("Blue Door: Take", false, "", "take (blue )?door", [&]{ WriteGameOutput("That's way too heavy."); }),
//                Command("Blue Door: Use", false, "", "use (blue )?door", [&]{ WriteGameOutput("You can't use that."); }),
//                Command("Blue Door: Drop", false, "", "drop (blue )?door", [&]{ WriteGameOutput("You aren't carrying that."); }),
//                Command("Blue Door: Open", false, "", "open (blue )?door", [&]{ WriteGameOutput("You opened the blue door."); })
            },
            Item::Attrs {
                .CanCarry = false,
                .CanUse = true,
            }
        )},
        {"Test Item 2", std::make_shared<Item>(
            "Test Item 2", "test item 2",
            std::vector<Command>{
//                Command("Test Item 2: Take", false, "", "take t2", [this]{ TryTakeItem(items.at("Test Item 2")); }),
//                Command("Test Item 2: Use", false, "", "use t2", [&]{ WriteGameOutput("You used t2."); }),
//                Command("Test Item 2: Drop", false, "", "drop t2", [this]{ TryDropItem(items.at("Test Item 2")); }),
            },
            Item::Attrs {
                .CanCarry = true,
                .CanUse = true,
            }
        )},
        {"Test Item 3", std::make_shared<Item>(
            "Test Item 3", "test item 3",
            std::vector<Command>{
//                Command("Test Item 3: Take", false, "", "take t3", [this]{ TryTakeItem(items.at("Test Item 3")); }),
//                Command("Test Item 3: Use", false, "", "use t3", [this]{ WriteGameOutput("You used t3."); }),
//                Command("Test Item 3: Drop", false, "", "drop t3", [this]{ TryDropItem(items.at("Test Item 3")); }),
            },
            Item::Attrs {
                .CanCarry = true,
                .CanUse = true,
            }
        )}
    };

    // add items to rooms

    rooms.at("Bedroom")->AddItem(items.at("Blue Door"));
    rooms.at("Kitchen")->AddItem(items.at("Test Item 1"));
    rooms.at("Kitchen")->AddItem(items.at("Test Item 2"));
    rooms.at("Kitchen")->AddItem(items.at("Test Item 3"));

    // init npcs

    // add npcs to rooms

    // set up player

    player.SetCurrentRoom(&*rooms.at("Kitchen"));
    player.TakeItem(items.at("Red Key"));

    WriteGameOutput(TextBasedGame::Messages::Title);

}

void TextBasedGame::EvalPlayerInput(std::string s) {

    for (Command& c : GetCommands()) {
        std::cout << c.GetName() << std::endl;
        if (c.Eval(s)) {
        std::cout << "> " << c.GetName() << std::endl;
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
            std::cout << "adding command for item " << name << " aka " << item->GetName() << std::endl;
            
            // standard item cmds
            cmds.push_back(Command("Take " + name, false, "", "take " + name, [&]{ std::cout<<"Will take " <<item->GetName()<<std::endl; TryTakeItem(item); }));
            cmds.push_back(Command("Use " + name, false, "", "use " + name, [&]{ WriteGameOutput("You used the " + item->GetRepr() + "."); }));
            cmds.push_back(Command("Drop " + name, false, "", "drop " + name, [&]{ TryDropItem(item); }));

            // special item cmds
            for (Command c : item->GetCommands()) {
                cmds.push_back(c);
            }
        }
    }


    // npc commands

    // failsafes
    if (state == State::Gameplay) {
        cmds.push_back(Command("Failsafe: Missing Take", true, "", "take", [&]{ WriteGameOutput(TextBasedGame::Messages::ErrorMissingTake); }));
        cmds.push_back(Command("Failsafe: Missing Drop", true, "", "drop", [&]{ WriteGameOutput(TextBasedGame::Messages::ErrorMissingDrop); }));
        cmds.push_back(Command("Failsafe: Missing Use", true, "", "use", [&]{ WriteGameOutput(TextBasedGame::Messages::ErrorMissingUse); }));
        cmds.push_back(Command("Failsafe: Take/Drop/Use Invalid Item", true, "", "(take|drop|use) .*", [&]{ WriteGameOutput(TextBasedGame::Messages::ErrorUnknownItem); }));
        cmds.push_back(Command("Failsafe: Missing Direction", true, "", "go", [&]{ WriteGameOutput(TextBasedGame::Messages::ErrorMissingDir); }));
        cmds.push_back(Command("Failsafe: Invalid Direction", true, "", "go .*", [&]{ WriteGameOutput(TextBasedGame::Messages::ErrorInvalidDir); }));
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
        WriteGameOutput(TextBasedGame::Messages::ErrorBlockedDir);
    }
}

void TextBasedGame::TryTakeItem(std::shared_ptr<Item> i) {
    std::cout << __LINE__ << std::endl;
    WriteGameOutput("You tried to take " + i->GetName() + ".");
    std::cout << __LINE__ << std::endl;
    if (player.HasItem(i)) {
    std::cout << __LINE__ << std::endl;
        WriteGameOutput(TextBasedGame::Messages::ErrorInvalidTake);
    } else if (!player.GetCurrentRoom()->HasItem(i)) {
    std::cout << __LINE__ << std::endl;
        WriteGameOutput(TextBasedGame::Messages::ErrorInvalidTakeMissing);
    } else {
    std::cout << __LINE__ << std::endl;
        player.TakeItem(i);
        player.GetCurrentRoom()->RemoveItem(i);
        WriteGameOutput("You picked up the " + i->GetRepr() + ".");
    std::cout << __LINE__ << std::endl;
    }
}

void TextBasedGame::TryDropItem(std::shared_ptr<Item> i) {
    WriteGameOutput("You tried to drop " + i->GetName() + ".");
    if (!player.HasItem(i)) {
        WriteGameOutput(TextBasedGame::Messages::ErrorInvalidDrop);
    } else {
        player.DropItem(i);
        player.GetCurrentRoom()->AddItem(i);
        WriteGameOutput("You dropped the " + i->GetRepr() + ".");
    }
}

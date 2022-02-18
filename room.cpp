#include "room.hpp"

Room::Room(std::string _name, std::unordered_map<MessageType, std::string> _messages, std::vector<Command> _commands) {
    name = _name;
    // hardcoding :(
    // use std::array ( + ::fill)?
    paths[0] = nullptr;
    paths[1] = nullptr;
    paths[2] = nullptr;
    paths[3] = nullptr;

    messages = _messages;
    commands = _commands;
}

std::string Room::GetName() {
    return name;
}

std::string Room::GetMessage(MessageType m) {
    return messages[m];
}

Room *Room::GetPath(Direction dir) {
    return paths[dir.id];
}

void Room::SetPath(Direction dir, Room *r) {
    paths[dir.id] = r;
}

bool Room::HasItem(std::shared_ptr<Item> i) {
    for (auto _i : items) {
        if (i->GetName() == _i->GetName()) {
            return true;
        }
    }
    return false;
}

void Room::AddItem(std::shared_ptr<Item> i) {
    items.push_back(i);
}

void Room::RemoveItem(std::shared_ptr<Item> i) {
    int count = 0;
    for (auto& _i : items) {
        if (i == _i) {
            items.erase(items.begin()+count);
            return;
        }
        count++;
    }
}


void Room::Link(Direction dir, Room& other, bool bothways) {
    SetPath(dir, &other);
    
    if (bothways) {
        other.SetPath(dir.reverse, this);
    }
}

std::vector<Command> Room::GetCommands() {
    return commands;
}
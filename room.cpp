#include "room.hpp"

Room::Room(std::string _name, std::string _messages[3]) {
    name = _name;
    // hardcoding :(
    // use std::array ( + ::fill)?
    paths[0] = nullptr;
    paths[1] = nullptr;
    paths[2] = nullptr;
    paths[3] = nullptr;

    messages[0] = _messages[0];
    messages[1] = _messages[1];
    messages[2] = _messages[2];
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

void Room::Link(Direction dir, Room& other, bool bothways) {
    SetPath(dir, &other);
    
    if (bothways) {
        other.SetPath(dir.reverse, this);
    }
    
}


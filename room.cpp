#include "room.hpp"

Room::Room(std::string _name) {
    name = _name;
    // hardcoding :(
    // use std::array ( + ::fill)?
    paths[0] = nullptr;
    paths[1] = nullptr;
    paths[2] = nullptr;
    paths[3] = nullptr;
}

std::string Room::GetName() {
    return name;
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


#include "room.hpp"

const Room& Room::InvalidRoom = Room();

Room::Room(std::string _name) {
    name = _name;
    paths = {
        .North = InvalidRoom,
        .South = InvalidRoom,
        .East = InvalidRoom,
        .West = InvalidRoom,
    };
}
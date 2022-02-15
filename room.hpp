#ifndef __ROOM_HEADER__
#define __ROOM_HEADER__

#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "item.hpp"

struct Direction {

    static constexpr size_t Count = 4;

    size_t id;
    std::string repr;
    std::string abbr;
    Direction& reverse;
};

class Room {

    public:

    // TODO: convert to functions
    enum MessageType : unsigned long { OnEnter, OnLook, OnStay };

    private:

    std::string name;
    std::unordered_map<MessageType, std::string> messages;
    // TODO: replace with map
    Room* paths[Direction::Count];

    public:

    Room(std::string _name, std::unordered_map<MessageType, std::string> _messages);

    std::string GetName();
    std::string GetMessage(MessageType);
    Room *GetPath(Direction dir);
    void SetPath(Direction dir, Room *r);

    void Link(Direction dir, Room& other, bool bothways = true);
};

#endif /* __ROOM_HEADER__ */
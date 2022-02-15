#ifndef __ROOM_HEADER__
#define __ROOM_HEADER__

#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

struct Direction {

    static constexpr size_t Count = 4;

    size_t id;
    std::string repr;
    std::string abbr;
    Direction& reverse;
};

class Room {

    private:

    std::string name;
    Room* paths[Direction::Count];

    public:

    Room(std::string _name);

    std::string GetName();
    
    Room *GetPath(Direction dir);
    void SetPath(Direction dir, Room *r);

    void Link(Direction dir, Room& other, bool bothways = true);
};

#endif /* __ROOM_HEADER__ */
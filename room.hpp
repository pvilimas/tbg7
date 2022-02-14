#ifndef __ROOM_HEADER__
#define __ROOM_HEADER__

#include <vector>

class Room {

    private:

    struct Paths { Room &North, &South, &East, &West; };

    public:

    static const Room& InvalidRoom;

    std::string name;
    Paths paths;

    Room(std::string _name);

};

#endif /* __ROOM_HEADER__ */
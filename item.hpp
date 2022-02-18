#ifndef __ITEM_HEADER__
#define __ITEM_HEADER__

#include <unordered_map>
#include <string>

#include "command.hpp"

class Item {

    public:
        struct Attrs {
            bool CanCarry;
            bool CanUse;
        };

    private:

        std::string name;
        std::string repr;
        std::vector<Command> commands;
        Attrs attrs;
        
    public:

        Item(std::string _name, std::string _repr, std::vector<Command> _commands, Attrs _attrs = Attrs());

        std::string GetName();
        std::string GetRepr();
        std::vector<Command> GetCommands();
        Attrs GetAttrs();
        
};

#endif /* __ITEM_HEADER__ */
#ifndef __ITEM_HEADER__
#define __ITEM_HEADER__

#include <string>

#include "command.hpp"

class Item {

    private:

        std::string name;
        std::string repr;

    public:

        Item(std::string _name, std::string _repr);

        std::string GetName();
        std::string GetRepr();

};

#endif /* __ITEM_HEADER__ */
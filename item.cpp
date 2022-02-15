#include "item.hpp"

Item::Item(std::string _name, std::string _repr) {
    name = _name;
    repr = _repr;
}

std::string Item::GetName() {
    return name;
}


std::string Item::GetRepr() {
    return repr;
}
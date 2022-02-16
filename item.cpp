#include "item.hpp"

Item::Item(std::string _name, std::string _repr, std::vector<Command> _commands) {
    name = _name;
    repr = _repr;
    commands = _commands;
}

std::string Item::GetName() {
    return name;
}

std::string Item::GetRepr() {
    return repr;
}

std::vector<Command> Item::GetCommands() {
    return commands;
}
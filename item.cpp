#include "item.hpp"

Item::Item(
    std::string _name,
    std::string _repr,
    std::vector<Command> _commands,
    Item::Attrs _attrs
) {
    name = _name;
    repr = _repr;
    commands = _commands;
    attrs = _attrs;
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

Item::Attrs Item::GetAttrs() {
    return attrs;
}
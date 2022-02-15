#include "command.hpp"

Command::Command(std::string _name, bool _hidden, std::string _repr, std::string _pattern, std::function<void()> _onMatch) {
    name = _name;
    if (_hidden) {
        repr = "";
    } else if (_repr == "") {
        repr = _name;
    }
    hidden = _hidden;
    // ignore case by default, ignore leading and trailing whitespace
    regex = std::regex("\\s*" + _pattern + "\\s*", std::regex_constants::ECMAScript | std::regex_constants::icase);
    onMatch = _onMatch;
}

std::string Command::GetName() {
    return name;
}

std::string Command::GetRepr() {
    return repr;
}


/* returns true if it FULLY matched false otherwise */
// TODO: make it a full match (or check if it actually is one)
bool Command::Eval(std::string _testString) {
    if (regex_match(_testString, regex)) {
        onMatch();
        return true;
    }
    return false;
}
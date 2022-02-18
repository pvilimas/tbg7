#ifndef __COMMAND_HEADER__
#define __COMMAND_HEADER__

#include <iostream>
#include <string>
#include <regex>

class Command {
    
    private:

        std::string name;
        bool hidden;
        std::string repr;
        std::regex regex;
        std::function<void()> onMatch;

    public:

        Command(std::string _name, bool _hidden, std::string _repr, std::string _pattern, std::function<void()> _onMatch);

        std::string GetName();
        std::string GetRepr();

        bool Eval(std::string _testString);

};

#endif /* __COMMAND_HEADER__ */
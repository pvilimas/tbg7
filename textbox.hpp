#ifndef __TEXTBOX_HEADER__
#define __TEXTBOX_HEADER__

#include <iostream>
#include <queue>
#include <regex>
#include <string>

#include "raylib.h"

class TextBox {
    private:

    static constexpr int LineCount = 3;
    static constexpr int LineLength = 58;
    static constexpr int FontSize = 20;
    static constexpr char PlayerPrompt[] = "> ";

    std::string textOut[TextBox::LineCount];
    std::string textIn;
    size_t cursorPos; // index into textIn

    Rectangle rec;
    std::function<void(std::string)> onEnter; // must be bound to textbasedgame.ReadPlayerInput

    std::queue<char> textQueue; // used to scroll text
    bool purgeQueue; // should the game clear the queue asap?

    void PollKeyInput();
    
    public:

    TextBox(std::function<void(std::string)> _onEnter = nullptr);

    // IO
    void Write(std::string);

    void Draw();
};

#endif /* __TEXTBOX_HEADER__ */

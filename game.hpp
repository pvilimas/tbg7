#ifndef __GAME_HEADER__
#define __GAME_HEADER__

#include "raylib.h"

#include <iostream>

#include "textbasedgame.hpp"
#include "textbox.hpp"

class Game {

    private:

        TextBasedGame tbg;
        TextBox textbox;
        Texture2D roomImgTexture;

        static constexpr int WinWidth = 640;
        static constexpr int WinHeight = 460;

    public:

        Game();
        ~Game();

        void Init();
        void Run();
};

#endif /* __GAME_HEADER__ */
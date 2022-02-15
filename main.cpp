#include "game.hpp"

/*
    TODO:


    GUIDELINES:

    - do not use 'using namespace x' anywhere
*/

int main() {
    SetTraceLogLevel(LOG_WARNING);
    ChangeDirectory(GetApplicationDirectory());

    Game *g = new Game();
    g->Init();
    try {
        g->Run();
    } catch (TextBasedGame::QuitGameException& q) {}
    delete g;

    return 0;
}
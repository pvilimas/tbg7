#include "game.hpp"

/*
    TODO:

    - implement items
    - player inventory
    - room items
    - item attrs: can take

    - use items - start with []{ WriteGameOutput("You used the " this.name); }
    - item attrs: can use

    - quit confirmation (function)

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
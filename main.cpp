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
    g->Run();
    delete g;
}
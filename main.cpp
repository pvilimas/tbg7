
#include "game.hpp"

int main() {

    SetTraceLogLevel(LOG_WARNING);
    ChangeDirectory(GetApplicationDirectory());

    Game *g = new Game();
    g->Init();
    g->Run();
    delete g;
}
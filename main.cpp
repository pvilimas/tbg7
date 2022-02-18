#include "game.hpp"

/*
    TODO:

    - fix item aliasing
    - item attrs: can take, can use, can use without picking up
    - standardize attrs 
    - paginate output ("...", hit enter to see next page)

    - regex collect
        # allows you to pass in a list of regex strings and joins them into one string (inefficiently - can be a long string)
        # ex. ['what\'s up', 'how are you doing', ...] -> r'(what\'s up)|(how are you doing)|...'
        def collect(*options: List[str]) -> RegexStr:

            """
            allows you to pass in a list of regex strings and joins them into one string (inefficiently - can be a long string).
            
            ex. ['what\'s up', 'how are you doing', ...] -> r'(what\'s up)|(how are you doing)|...'
            
            Used in the DialogOption and Command classes.
            """

            return '(' + ('|'.join([f'({o})' for o in options])) + ')'

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
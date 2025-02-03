#include "game.h"
#include <sstream>

#define SDL_MAIN_HANDLED

int main( int argc, char* argv[] )  {
    Canvas canvas( "Mines");

    int diff = 1;

    if( argc == 2 ) {
        std::istringstream arg( argv[1] );
        if( arg.str().length() == 1 )  {
            arg >> diff;
         }
     }

    std::vector<std::string> themes = { "Digital clock", "Steampunk clock", "Rolling clock" };
    std::make_shared<Game>( themes, canvas, diff )->Run();
 }



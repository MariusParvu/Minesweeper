#include "game.h"

#define SDL_MAIN_HANDLED

int main( int argc, char* argv[] )  {
    Canvas canvas( "Mines");
    std::vector<std::string> themes = { "Digital clock", "Rolling clock", "Steampunk clock" };
    std::make_shared<Game>( themes, canvas )->Run();
 }



#ifndef GAMEH
#define GAMEH
#include "canvas.h"
#include "mines.h"
#include <sstream>
#include <list>

class Theme  {
    std::string name, location;
    Animation pointer, detonation, erasure, reverseDetonation, reverseErasure;

    std::shared_ptr<Texture> detonationTexture, erasureTexture, reverseDetonationTexture, reverseErasureTexture, blackTexture, backgroundTexture;

    std::vector<Animation> detonations, erasures;
    int cellSize;

    void redrawAnimation( std::vector<Animation>&, Animation& );
 public:
    Theme();
    Theme( std::string, Canvas&, int );
    ~Theme();

    void redrawPointer();
    void pointerAnimation( int, int );

    void redrawDetonation();
    void detonationAnimation( int, int, Canvas&, bool = false );

    void redrawErasure();
    void erasureAnimation( int, int, Canvas&, bool = false );

    void redrawBackground( SDL_Rect, Canvas& );

    std::string getSkin();
 };

class Game  {
    int fieldLength, fieldHeight, gameLength, screenLength, screenHeight, mineCount, cellSize, borderCells, 
    origin_x, origin_y, secondChances, tileSize, cursor_x, cursor_y, backgroundOffset, backgroundOrigin_x, backgroundOrigin_y,
    x, y; //first click coordinates

    static constexpr int screenSubdivision = 16; //common divisor of 1920 and 2560

    float screenRatio;
    std::shared_ptr<mineField> field;
    bool newgame, cursorEnabled, hintEnabled;

    std::shared_ptr<Texture> tileMap, borderTiles, gameArea;

    Canvas canvas;

    std::shared_ptr<Theme> theme;

    SDL_Event event;
    SDL_Rect borderRect, gameAreaRect;

    std::vector<SDL_Rect> borderRectMapSrc, borderRectMapDest;
    std::vector<std::string> themeNames;

    struct AnimationQueue  {
	    std::vector<Cell::Coords> queue;

	    void Add( const std::vector<Cell::Coords>& );
	    void multiAdd( const std::vector<Cell::Coords>& );
	    void Clear();
     } animationQueue;

    bool inTextureBounds( int&, int& );
    bool inScreenBounds( int, int );
    int randomRange( int, int );

    void Paint();
    std::vector<Cell::Coords> Redraw( std::shared_ptr<Cell>, unsigned = -1 );
    int Redraw( const std::vector<Cell::Coords>& );
    void Redraw( const Cell::Coords& );
    void drawGameArea();
    void redrawScene();
    void redrawCursor();

    std::shared_ptr<Cell> firstClicked;
    std::vector<Cell::Coords> setFlagCount();

    void Reroll();
    void Initialize();

 public:
    Game( std::string, int, int, int, int, int, int, int, int, Canvas& );
    Game( std::vector<std::string>, Canvas& );
    ~Game();

    void Run();

    void Reset();
    void loadTheme( std::string );
    void flipEnableCursor();
    void flipEnableHint();

    void loadTextures();
 };

#endif

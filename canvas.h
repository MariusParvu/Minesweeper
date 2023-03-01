#ifndef CanvasH
#define CanvasH

#ifdef __linux__
 #include <SDL2/SDL.h>
 #include <SDL2/SDL_image.h>
#endif

#include <SDL.h>
#include <SDL_image.h>

#ifdef _WIN32
 #include <SDL.h>
 #include "SDL_image.h"
 #include <windows.h>
#endif 

#include <string>
#include <memory>
#include <vector>
#include <ctime>
#include <random>
#include <limits>
#include <cassert>
#include <algorithm>

class Texture  {
    SDL_Texture* texture;
    SDL_Rect coords;
    std::vector<SDL_Rect> textureMap;
    std::string filename;

    int length, width;

    void fillTextureMap();
public:
    Texture();
    Texture( int, int, SDL_Renderer* );
    Texture( std::string, SDL_Renderer* );
    ~Texture();

    SDL_Rect& getTileAt( int );
    const std::vector<SDL_Rect>& getTiles();
    int getTextureMapSize() const;

    const SDL_Rect& getCoords();
    SDL_Texture*& getTexture();

    void setColor( int, int, int );
    void resetColor( );

    int imageLength() const;
    int imageWidth() const;

    void Reverse();

    void setBlendMode( SDL_BlendMode );
 };


class Canvas  {
    SDL_Window* pWindow;
    SDL_Renderer* pRenderer;

    struct screenSize  {
		 float length, height; // used float because of the screen ratio
     } size;
public:
    Canvas() = delete;
    Canvas( std::string, int, int, int, int, unsigned );
    Canvas( std::string );
    ~Canvas();

    void Paint( Texture&, SDL_Rect&, SDL_Rect& );
    void Paint( Texture&, SDL_Rect& );
    void Paint( Texture& );
    void renderTarget( Texture& );
    void renderTargetSelf();

    SDL_Renderer*& getRenderer();
    SDL_Window*& getWindow();

    void Clear();
    void Refresh();
    void drawCursor( int, int, int, SDL_Color );
    float getScreenRatio() const;

    const screenSize& getScreenSize();
 };

class Animation  {
    std::shared_ptr<Texture> texture;
    bool reversed;
    SDL_Renderer* renderer;

    int frames, frameCounter;
    int x, y;
    int textureLength, textureHeight;
public:
    Animation();
    Animation( std::string, SDL_Renderer*, unsigned = -1, unsigned = -1 );
    Animation( std::shared_ptr<Texture>, SDL_Renderer*, unsigned = -1, unsigned = -1 );
    ~Animation();

    void Render();
    void setColor( int, int, int );
    void setBlendMode( SDL_BlendMode );
    void Activate( int, int );
    bool Finished();
    bool Reversed();
    const Animation& Reverse();
 };

class Timer  {
    unsigned long startTicks, pausedTicks;
    bool started, paused;
 public:
    Timer();
    ~Timer();

    bool isStarted();
    bool isPaused();

    unsigned long getTicks() const;
    void Start();
    void Reset();
    void togglePause();

    unsigned long operator-( const Timer& );
 };

#endif

#include <sstream>
#include "canvas.h"

//----------Canvas---------- 

Canvas::Canvas( std::string title_, int xpos_, int ypos_, int height_, int width_, unsigned flags_ ) : pRenderer( nullptr ), pWindow( nullptr ), size{ 0, 0 }  {
    SDL_Init( SDL_INIT_EVERYTHING );
    pWindow = SDL_CreateWindow( title_.c_str(), xpos_, ypos_, height_, width_, flags_ );
    pRenderer = SDL_CreateRenderer( pWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE | SDL_RENDERER_PRESENTVSYNC );
    SDL_SetRenderDrawColor( pRenderer, 0x0, 0x0, 0x0, SDL_ALPHA_OPAQUE );
 }

Canvas::Canvas( std::string title_ ) : pRenderer( nullptr ), pWindow( nullptr )  {
    SDL_Init( SDL_INIT_EVERYTHING );

    SDL_DisplayMode mode;
    SDL_GetCurrentDisplayMode( 0, &mode );

    size.length = mode.w;
    size.height = mode.h;

    pWindow = SDL_CreateWindow( title_.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, mode.w, mode.h,  0 );//SDL_WINDOW_FULLSCREEN );
    pRenderer = SDL_CreateRenderer( pWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE | SDL_RENDERER_PRESENTVSYNC );
    SDL_SetRenderDrawColor( pRenderer, 0x0, 0x0, 0x0, SDL_ALPHA_OPAQUE );
 }

Canvas::~Canvas() {
    SDL_DestroyWindow( pWindow );
    SDL_DestroyRenderer( pRenderer );
    SDL_QuitSubSystem( SDL_INIT_EVERYTHING );
    SDL_Quit();
 }

void Canvas::Paint( Texture& texture_, SDL_Rect& src_, SDL_Rect& dest_ )  {
//	SDL_RenderFillRect( pRenderer, &dest_ );
    SDL_RenderCopy( pRenderer, texture_.getTexture(), &src_, &dest_ );
 }

void Canvas::Paint( Texture& texture_, SDL_Rect& dest_ )  {
//	SDL_RenderFillRect( pRenderer, &dest_ );
    SDL_RenderCopy( pRenderer, texture_.getTexture(), nullptr, &dest_ );
 }

void Canvas::Paint( Texture& texture_ )  {
//	SDL_RenderFillRect( pRenderer, nullptr );
    SDL_RenderCopy( pRenderer, texture_.getTexture(), nullptr, nullptr );
 }

void Canvas::renderTarget( Texture& texture_ )  {
    SDL_SetRenderTarget( pRenderer, texture_.getTexture() );
 }

void Canvas::renderTargetSelf()  {
    SDL_SetRenderTarget( pRenderer, nullptr );
 }

SDL_Renderer*& Canvas::getRenderer()  {
    return pRenderer;
 }
	
SDL_Window*& Canvas::getWindow()  {
    return pWindow;
 }

void Canvas::Clear()  {
    SDL_RenderClear( pRenderer );
 }

void Canvas::Refresh()  {
	SDL_RenderPresent( pRenderer );
 }

void Canvas::drawCursor( int x_, int y_, int cellSize_, SDL_Color color_ )  {
//	SDL_SetRenderDrawBlendMode( pRenderer, SDL_BLENDMODE_BLEND );
	SDL_SetRenderDrawColor( pRenderer, color_.r, color_.g, color_.b, color_.a );
	SDL_Rect rect = { x_, y_, cellSize_, cellSize_ };
//	SDL_RenderFillRect( pRenderer, &rect );
	SDL_RenderDrawRect( pRenderer, &rect );
 }

float Canvas::getScreenRatio() const  {
   return ( float ) size.length / size.height;
//	return ( float ) 16 / 9;
 }

const Canvas::screenSize& Canvas::getScreenSize()  {
    return size;
 }

//----------Texture---------- 

Texture::Texture( int width_, int height_, SDL_Renderer* renderer_ ) : texture( nullptr ), length( 0 ), width( 0 )  {
    texture = SDL_CreateTexture( renderer_, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width_, height_ );
	SDL_SetTextureBlendMode( texture, SDL_BLENDMODE_BLEND );
 }

Texture::Texture( std::string filename_, SDL_Renderer* renderer_ ) : filename( filename_ ), texture( nullptr ) {
    if( filename_.size() )  {
		auto _surface = IMG_Load( filename_.c_str() );
		if( _surface != nullptr )  {
			length = _surface->w;
			width = _surface->h;

			texture = SDL_CreateTextureFromSurface( renderer_, _surface );
			SDL_SetTextureBlendMode( texture, SDL_BLENDMODE_BLEND );
			SDL_FreeSurface( _surface );

			fillTextureMap();
		 }
    }
 }

Texture::Texture() : texture( nullptr ), coords( { 0, 0, 0, 0 } ) {
 }

Texture::~Texture()  {
    if( texture != nullptr )  {
		SDL_DestroyTexture( texture );
     }
 }

const std::vector<SDL_Rect>& Texture::getTiles()  {
    return textureMap;
 }

void Texture::fillTextureMap( )  {
    for( int x = 0; x < length; x += width )  {
		textureMap.emplace_back( (SDL_Rect) { x, 0, width, width} );
     }
 }

void Texture::Reverse()  {
    std::reverse( std::begin( textureMap ), std::end( textureMap ) );
 }

void Texture::setBlendMode( SDL_BlendMode mode_ )  {
    SDL_SetTextureBlendMode( texture, mode_ ); 
 }

int Texture::imageLength() const  {
    return length;
 }

int Texture::imageWidth() const  {
    return width;
 }

SDL_Rect& Texture::getTileAt( int at_ )  {
    return textureMap.at( at_ );
 }

int Texture::getTextureMapSize() const  {
    return textureMap.size();
 }

void Texture::setColor( int r_, int g_, int b_ )  {
    SDL_SetTextureColorMod( texture, r_, g_, b_ );
 }

void Texture::resetColor()  {
    SDL_SetTextureColorMod( texture, 255, 255, 255 );
 }

const SDL_Rect& Texture::getCoords() {
    if( texture != nullptr )  {
		SDL_QueryTexture( texture, nullptr , nullptr , &coords.w, &coords.h );
     }
    return coords;
 }

SDL_Texture*& Texture::getTexture() {
    return texture;
 }

//----------Animation----------
Animation::Animation() : frameCounter( std::numeric_limits<int>::max() ), x( 0 ), y( 0 ), renderer( nullptr ), reversed( false ) {  }

Animation::Animation( std::string filename_, SDL_Renderer* renderer_ ,unsigned textureLength_, unsigned textureHeight_ ) : frameCounter( std::numeric_limits<int>::max() ), x( 0 ), y( 0 ), reversed( false )  {
    renderer = renderer_;
    texture = std::make_shared<Texture>( filename_, renderer );
    if( texture->getTexture() != nullptr )  {
		textureLength = textureLength_ == -1 ? texture->imageLength() / texture->getTextureMapSize() : textureLength_;
		textureHeight = textureHeight_ == -1 ? texture->imageWidth() : textureHeight_;

		frames = texture->getTextureMapSize();
     }
 }

Animation::Animation( std::shared_ptr<Texture> texture_, SDL_Renderer* renderer_ ,unsigned textureLength_, unsigned textureHeight_ ) : frameCounter( std::numeric_limits<int>::max() ), x( 0 ), y( 0 ), reversed( false )  {
    renderer = renderer_;
    texture = texture_;
    if( texture->getTexture() != nullptr )  {
	    textureLength = textureLength_ == -1 ? texture->imageLength() / texture->getTextureMapSize() : textureLength_;
	    textureHeight = textureHeight_ == -1 ? texture->imageWidth() : textureHeight_;

	    frames = texture->getTextureMapSize();
     }
 }

void Animation::Render()  {
    if( frameCounter < frames && texture->getTexture() != nullptr )  {
		SDL_Rect src = texture->getTileAt( frameCounter ),
		dest = { x - textureLength / 2, y - textureHeight / 2, textureLength, textureHeight };

		frameCounter++;

		SDL_RenderCopy( renderer, texture->getTexture(), &src, &dest ); 
     }
 }

Animation::~Animation() { 
 }

void Animation::Activate( int x_, int y_ )  {
    x = x_;
    y = y_;
    frameCounter = 0;
 }

void Animation::setColor( int r_, int g_, int b_ )  {
    texture->setColor( r_, g_, b_ );
 }

bool Animation::Finished()  {
    return frameCounter == frames;
 }

bool Animation::Reversed()  {
    return reversed;
 }

const Animation& Animation::Reverse()  {
    reversed ^= 1;
    texture->Reverse();
    return *this;
 }

//----------Timer---------- 


Timer::Timer() : startTicks( 0 ), pausedTicks( 0 ), started( false ), paused( false ) {  }

Timer::~Timer() {  }

bool Timer::isStarted()  {
    return started;
 }
bool Timer::isPaused()  {
    return paused;
 }

unsigned long Timer::getTicks() const  {
    if( started )  {
		if( paused )  {
			return pausedTicks;
		 }
		else return SDL_GetTicks() - startTicks;
     }

    return 0;
 }

void Timer::Start()  {
    started = true;
    paused = false;

    startTicks = SDL_GetTicks();
 }

void Timer::Reset()  {
    started = true;
    paused = false;
    startTicks = SDL_GetTicks();
    pausedTicks = 0;
 }

void Timer::togglePause()  {
    if( started )  {
		paused ^= true;

		if( paused )  {
			pausedTicks = SDL_GetTicks() - startTicks;
			startTicks = 0;
		 }
		else  {
			startTicks = SDL_GetTicks() - pausedTicks;
			pausedTicks = 0;
		 }
    }
 }

unsigned long Timer::operator-( const Timer& second_ ) {
    return getTicks() - second_.getTicks();
 }

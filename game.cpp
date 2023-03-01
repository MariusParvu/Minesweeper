#include "game.h"

Game::Game( std::vector<std::string> themeNames_, Canvas& canvas_ ) : themeNames( themeNames_ ), canvas( canvas_ ),  
   x( 0 ), y( 0 ), borderCells( 1 ),
   newgame( true ), cursorEnabled( true ), hintEnabled( true ),
   secondChances( 0 ), cursor_x( 0 ), cursor_y( 0 )  {

   Reroll();

   field = std::make_shared<mineField>( fieldLength, fieldHeight, mineCount );

   loadTheme( themeNames.at( randomRange( 0, themeNames.size() - 1 ) ) );
   loadTextures();

   Initialize();

   drawGameArea();
   redrawScene();
 }

Game::Game( std::string themeName_, int backgroundOffset_, int origin_x_, int origin_y_, int fieldLength_, int fieldHeight_, int mineCount_, int cellSize_, int borderCells_, Canvas& canvas_ ) :
   fieldLength( fieldLength_ ), fieldHeight( fieldHeight_ ), borderCells( borderCells_ ), x( 0 ), y( 0 ),
   origin_x( origin_x_ + backgroundOffset_ * cellSize_ ), origin_y( origin_y_ + backgroundOffset_ * cellSize_ ), backgroundOffset( backgroundOffset_ ),
   backgroundOrigin_x( origin_x_ ), backgroundOrigin_y( origin_y_ ),
   mineCount( mineCount_ ), cellSize( cellSize_ ), 
   newgame( true ), cursorEnabled( true ), hintEnabled( true ),
   secondChances( 0 ), cursor_x( 0 ), cursor_y( 0 ),
   field( std::make_shared<mineField> ( fieldLength, fieldHeight, mineCount ) ), 
   canvas( canvas_ )  {

   //the length of the board will fit the screen's length

   Reroll();

   field = std::make_shared<mineField>( fieldLength, fieldHeight, mineCount );

   loadTheme( themeName_ );
   loadTextures();

   Initialize();

   drawGameArea();
   redrawScene();
 }

Game::~Game()  {
 }

void Game::Reroll()  {
   x = 0; 
   y = 0;
   borderCells = 1;
   newgame = true;
   cursorEnabled = true; 
   hintEnabled = true;
   secondChances = 0;
   cursor_x = 0; 
   cursor_y = 0;

   firstClicked.reset();

   float gameHeight = canvas.getScreenSize().length / canvas.getScreenRatio();

   auto roll = randomRange( 1, 3 ); //difficulty range ( affects field size )
   auto cellRatio = screenSubdivision * roll;
   std::vector<float> difficulty = { 18.29, 20.72, 22.36 };

   backgroundOffset = cellRatio / screenSubdivision;
   cellSize = canvas.getScreenSize().length / cellRatio;

   fieldLength = canvas.getScreenSize().length / cellSize - 2 * borderCells - backgroundOffset * 2;
   fieldHeight = gameHeight / cellSize - 2 * borderCells - backgroundOffset * 2;

   mineCount = ( fieldLength * fieldHeight ) * ( difficulty.at( roll - 1 ) / 100 );

   if( fieldHeight % 2 == 0 )  {  //we need an odd number of rows and an even number of columns ( 5 x 11; 11 x 25; 19 x 40 )
      fieldHeight--;
      gameHeight -= cellSize;;
    }

    origin_y = ( canvas.getScreenSize().height - gameHeight ) / 2 + backgroundOffset * cellSize;
    origin_x = ( canvas.getScreenSize().length - canvas.getScreenSize().length ) / 2 + backgroundOffset * cellSize;

    backgroundOrigin_x = 0;
    backgroundOrigin_y = ( canvas.getScreenSize().height - gameHeight ) / 2; //needs work
 }

void Game::Initialize()  {
    borderRectMapSrc = {
   /*topLeftSrc*/     {( fieldLength - borderCells ) * tileSize, 
            ( fieldHeight - borderCells ) * tileSize, 
            borderCells * tileSize,
            borderCells * tileSize},

   /*topSrc*/     {0,
            ( fieldHeight - borderCells ) * tileSize,
            fieldLength * tileSize,
            borderCells * tileSize},

   /*topRightSrc*/    {0,
            ( fieldHeight - borderCells ) * tileSize,
            borderCells * tileSize,
            borderCells * tileSize},

   /*rightSrc*/       {0,
            0,
            borderCells * tileSize,
            fieldHeight * tileSize },

   /*bottomRightSrc*/ {0,
            0,
            borderCells * tileSize,
            borderCells * tileSize },

   /*bottomSrc*/      {0,
            0,
            fieldLength * tileSize,
            borderCells * tileSize },

   /*bottomLeftSrc*/  {( fieldLength - borderCells ) * tileSize,
                0,
            borderCells * tileSize,
            borderCells * tileSize },

   /*leftSrc*/        {( fieldLength - borderCells ) * tileSize,
            0,
            borderCells * tileSize,
            fieldHeight * tileSize} };

   

   borderRectMapDest = {
   /*topLeftDest*/    {0, 
            0, 
            borderCells * tileSize,
            borderCells * tileSize },

   /*topDest*/        {borderCells * tileSize,
            0,
            fieldLength * tileSize,
            tileSize * borderCells },

   /*topRightDest*/   {( fieldLength + borderCells ) * tileSize,
            0,
            borderCells * tileSize,
            borderCells * tileSize },

   /*rightDest*/      {( fieldLength + borderCells ) * tileSize,
            borderCells * tileSize,
            borderCells * tileSize,
            fieldHeight * tileSize },

   /*bottomRightDest*/{( borderCells + fieldLength ) * tileSize,
            ( borderCells + fieldHeight ) * tileSize,
            borderCells * tileSize,
            borderCells * tileSize },

   /*bottomDest*/     {borderCells * tileSize,
            ( fieldHeight + borderCells ) * tileSize,
            fieldLength * tileSize,
            borderCells * tileSize },

   /*bottomLeftDest*/ { 0,
            ( fieldHeight + borderCells ) * tileSize,
            borderCells * tileSize,
            borderCells * tileSize },

   /*leftDest*/       {0,
            borderCells * tileSize,
            borderCells * tileSize,
            fieldHeight * tileSize } };

    gameAreaRect = { borderCells * cellSize + origin_x , borderCells * cellSize + origin_y ,fieldLength * cellSize, fieldHeight * cellSize };

    borderRect = { origin_x, origin_y, ( borderCells * 2 + fieldLength ) * cellSize, ( borderCells * 2 + fieldHeight ) * cellSize };
 }

void Game::Reset()  {
    animationQueue.Clear();
    field.reset();

    Reroll();

    field = std::make_shared<mineField>( fieldLength, fieldHeight, mineCount );

    loadTheme( themeNames.at( randomRange( 0, themeNames.size() - 1 ) ) );
    loadTextures();

    Initialize();

    drawGameArea();
    redrawScene();
 }

std::vector<Cell::Coords> Game::Redraw( std::shared_ptr<Cell> cell_ , unsigned tile_ )  {
   std::vector<Cell::Coords> restored;

   canvas.renderTarget( *gameArea );
   SDL_Rect tileRect = { cell_->getCoords().x * tileSize, cell_->getCoords().y * tileSize, tileSize, tileSize };

   canvas.Paint( *tileMap, tileMap->getTileAt( tile_ == -1 ? cell_->drawTile() : tile_ ) , tileRect );

   if( tile_ == Cell::State::cell )  {
      for( auto n : cell_->getNeighbors() )  {
         if( n->Erased() )  {
            n->Restore();
            restored.emplace_back( n->getCoords() );

            SDL_Rect rect = { n->getCoords().x * tileSize, n->getCoords().y * tileSize, tileSize, tileSize };
            canvas.Paint( *tileMap, tileMap->getTileAt( n->drawTile() ) , rect );
           }
        }
     }
    return restored;
 }

int Game::Redraw( const std::vector<Cell::Coords>& coords_ )  {
    if( coords_.size() )  { 
      canvas.renderTarget( *gameArea );
      for( auto f : coords_ )  {
         auto cell = field->getCellAt( f.x, f.y );
         SDL_Rect tileRect = { f.x * tileSize, f.y * tileSize, tileSize, tileSize };

         canvas.Paint( *tileMap, tileMap->getTileAt( cell->drawTile() ) , tileRect );
       }
    }
    return 0;
 }

void Game::Redraw( const Cell::Coords& coords_ )  {
   canvas.renderTarget( *gameArea );
   auto cell = field->getCellAt( coords_.x, coords_.y );
   SDL_Rect tileRect = { coords_.x * tileSize, coords_.y * tileSize, tileSize, tileSize };

   bool colorChanged = false; 
   if( cell == firstClicked || cell == firstClicked->getLeftCell() || cell == firstClicked->getRightCell() )  {
       if( cell->getMineCount() > 0 )  {
          tileMap->setColor( 150, 150, 150 ); //graying out the mine counter cells
          colorChanged = true;
        }
    }

   canvas.Paint( *tileMap, tileMap->getTileAt( cell->drawTile() ) , tileRect );

   if( colorChanged )  {
       tileMap->resetColor();
     }
 }

void Game::drawGameArea()  {
   canvas.renderTarget( *gameArea );
   bool colorChanged = false;

   SDL_Rect tileRect;
   for( int x = 0; x < fieldLength; x++ )  {
      for( int y = 0; y < fieldHeight; y++ )  {
         tileRect = { x * tileSize, y * tileSize, tileSize, tileSize };
         auto cell = field->getCellAt( x, y );
         if( !cell->Cleared() && !cell->Flagged() )  {
            canvas.Paint( *tileMap, tileMap->getTileAt( randomRange( Cell::State::cell, tileMap->getTextureMapSize() - 1 ) ) , tileRect );
          }
         else  {
         if( cell == firstClicked || cell == firstClicked->getLeftCell() || cell == firstClicked->getRightCell() )  {
            if( cell->getMineCount() > 0 )  {
               tileMap->setColor( 150, 150, 150 );  //graying out the mine counter cells
               colorChanged = true;
             }
          }
         canvas.Paint( *tileMap, tileMap->getTileAt( cell->drawTile() ) , tileRect );

         if( colorChanged )
            tileMap->resetColor();
          }
       }
    }
 }

void Game::Paint()  {
   canvas.Clear();

   if( animationQueue.queue.size() )  {
      auto coords = animationQueue.queue.back();

      auto cell = field->getCellAt( coords.x, coords.y );

      if( !cell->Erased() && !cell->Restored() )  {
         theme->detonationAnimation( ( ( coords.x + borderCells ) * cellSize ) + cellSize / 2 + origin_x, ( ( coords.y + borderCells ) * cellSize ) + cellSize / 2 + origin_y, canvas );
       }

      else if( cell->Erased() )
         theme->erasureAnimation( ( ( coords.x + borderCells ) * cellSize ) + cellSize / 2 + origin_x, ( ( coords.y + borderCells ) * cellSize ) + cellSize / 2 + origin_y, canvas );
      else if( cell->Restored() ) //reverse
         theme->erasureAnimation( ( ( coords.x + borderCells ) * cellSize ) + cellSize / 2 + origin_x, ( ( coords.y + borderCells ) * cellSize ) + cellSize / 2 + origin_y, canvas, true );

      Redraw( animationQueue.queue.back() );
      animationQueue.queue.pop_back();
    }

   redrawScene(); //draw before this function and animate after it

   //    theme->redrawPointer();
   theme->redrawDetonation();
   theme->redrawErasure();
   redrawCursor();
   canvas.Refresh();

   if( !animationQueue.queue.size() && secondChances < 0 ) Reset();
 }

void Game::redrawCursor()  {
    //Cursor and hint rendering
    if( cursorEnabled | hintEnabled )  {
      int x = cursor_x / cellSize - borderCells, y = cursor_y / cellSize - borderCells;
      if( inTextureBounds( x, y ) )  {
         auto cell = field->getCellAt( x, y );
         unsigned char a = randomRange( 0, 140 ); //cursor color flashes

         if( cursorEnabled )  {
            if( !cell->Cleared() && !cell->Flagged() )  {
               canvas.drawCursor( cursor_x + origin_x, cursor_y + origin_y , cellSize, { 0xFF, (unsigned char) a, (unsigned char) a, 0xFF } );
             }
          }
         if( hintEnabled )  {
            if( cell->Cleared() && !cell->Flagged() && cell->getState() == Cell::State::numbered )  {
               for( auto p : cell->getNeighbors() )  {
                  if( !p->Cleared() && !p->Flagged() )  {
                     auto n = p->getCoords();
                     canvas.drawCursor( ( n.x + borderCells ) * cellSize + origin_x, ( n.y + borderCells ) * cellSize + origin_y, cellSize, { (unsigned char) a, (unsigned char) a, 0xFF, 0xFF } );
                   }
                }
             }
          }
       }
    }
 }

int Game::randomRange( int from_, int to_ )  {
    std::uniform_int_distribution<> range( from_, to_ );
    static std::mt19937 generator( std::time( nullptr ) );
    return range( generator );
 }

void Game::redrawScene()  {
    canvas.renderTarget( *borderTiles );

    gameArea->setBlendMode( SDL_BLENDMODE_NONE );
    for( auto src = std::begin( borderRectMapSrc ), dest = std::begin( borderRectMapDest ); src != std::end( borderRectMapSrc ); ++src, ++dest )  {
      canvas.Paint( *gameArea, *src, *dest );
     }
    gameArea->setBlendMode( SDL_BLENDMODE_BLEND ); 

    canvas.renderTargetSelf();

    theme->redrawBackground( { backgroundOrigin_x, backgroundOrigin_y, fieldLength * cellSize + ( borderCells * 2 ) * cellSize + 2 * backgroundOffset * cellSize , fieldHeight * cellSize + ( borderCells * 2 ) * cellSize + 2 * backgroundOffset * cellSize }, canvas );
    canvas.Paint( *borderTiles, borderRect );
    canvas.Paint( *gameArea, gameAreaRect ); 
 }

std::vector<Cell::Coords> Game::setFlagCount()  {  //updates the counter on the field and returns the coordinates of the cells that need to be redrawn
   std::vector<Cell::Coords> vec;

   if( firstClicked != nullptr && field->getFlaggedMines() >= 0 && field->getFlaggedMines() <= mineCount )  {
      firstClicked->setState( Cell::numbered );
      firstClicked->getLeftCell()->setState( Cell::numbered );
      firstClicked->getRightCell()->setState( Cell::numbered );

      int hundreds = field->getFlaggedMines() / 100;
      int tens = ( field->getFlaggedMines() - hundreds * 100 ) / 10;
      if ( tens == 0 && hundreds > 0 ) tens = 10;
      int digits = field->getFlaggedMines() - hundreds * 100 - tens * 10;
      if ( digits < 1 ) digits = 10;


      firstClicked->getLeftCell()->setMineCount( hundreds );  //the number of mines is stored as a mine count in the cells that will display the counter
      firstClicked->setMineCount( tens );
      firstClicked->getRightCell()->setMineCount( digits );

      firstClicked->Restore();
      firstClicked->getLeftCell()->Restore();
      firstClicked->getRightCell()->Restore();

      vec.emplace_back( firstClicked->getRightCell()->getCoords() ); //there will always be some mines to flag ( < 10 )
      if( firstClicked->getLeftCell()->getMineCount() > 0 )  {
         vec.emplace_back( firstClicked->getLeftCell()->getCoords() ); //if there are over a hundred mines then add the left cell to the drawing queue
         if( firstClicked->getMineCount() == 0 )  { 
            vec.emplace_back( firstClicked->getCoords() ); //for numbers between say 100 and 109 when the middle 0 needs to be updated
          }
       }

      else {
         firstClicked->getLeftCell()->setState( Cell::empty );
         Redraw( firstClicked->getLeftCell() );
       }

      if( firstClicked->getMineCount() > 0 ) vec.emplace_back( firstClicked->getCoords() ); // if there are over ten do the same for the middle one
      else {
         firstClicked->setState( Cell::empty );
         Redraw( firstClicked->getCoords() );
       }
     }
    return vec;
 }

void Game::AnimationQueue::Add( const std::vector<Cell::Coords>& cells_ ) {  
    queue.insert( queue.end(), cells_.begin(), cells_.end() );
    std::reverse( std::begin( queue ), std::end( queue ) );
 }

void Game::AnimationQueue::Clear() {  
    queue.clear();
 }

bool Game::inTextureBounds( int& x_, int& y_ )  {
///* Comment out for not being able to click the greyed cells
    if( x_ < 0 ) x_ += fieldLength;
   else if( x_ >= fieldLength ) x_ -= fieldLength;

    if( y_ < 0 ) y_ += fieldHeight;
   else if( y_ >= fieldHeight ) y_ -= fieldHeight;
//*/
    if( x_ >= 0 && x_ < fieldLength && y_ >= 0 && y_ < fieldHeight )
       return true;
   return false;
 }

bool Game::inScreenBounds( int x_, int y_ )  {
    return  ( x_ >= origin_x && y_ >= origin_y &&
       x_ <= origin_x + ( fieldLength + borderCells * 2 ) * cellSize && 
       y_ <= origin_y + ( fieldHeight + borderCells * 2 ) * cellSize );
 }

void Game::Run()  {
    std::shared_ptr<Cell> cell;

    SDL_Event breakEvent;
    breakEvent.type = SDL_WINDOWEVENT;

   //moving the window from screen to screen introduces a flicker
    SDL_DisplayMode mode;
    SDL_GetDisplayMode( 0, 0 /*3*/, &mode );
    float fpsCap = 1000 / ( mode.refresh_rate );  //fps

    while( true )  {
      Timer startCounter;
      startCounter.Start();
      Paint();

      SDL_PollEvent( &event );

      if( event.type == SDL_QUIT ) break;

      switch( event.type )  {
         //On windows the renderer cannot be recovered so the textures have to be reloaded
   #ifdef _WIN32
         case SDL_WINDOWEVENT:  {
            if( event.window.event == SDL_WINDOWEVENT_RESTORED )  {
               loadTextures(); 
               drawGameArea();
               redrawScene();
             }
            break;
          }
   #endif
         case SDL_MOUSEMOTION:  {
            if( inScreenBounds( event.motion.x, event.motion.y ) )  {
               cursor_x = ( ( event.motion.x - origin_x ) / cellSize ) * cellSize;
               cursor_y = ( ( event.motion.y - origin_y ) / cellSize ) * cellSize;
             }
            break;
          }
            
         case SDL_MOUSEBUTTONUP:  {
            SDL_PushEvent( &breakEvent );
               switch( event.button.button )  {
                  case SDL_BUTTON_LEFT :  {
                     if( inScreenBounds( event.button.x, event.button.y ) )  {
                        x = ( event.button.x - origin_x ) / cellSize - borderCells; 
                        y = ( event.button.y - origin_y ) / cellSize - borderCells;

                        if( inTextureBounds( x, y ) ) {
                           if( newgame )  {
                              field->Mine( mineCount, { x, y } );
                              newgame = false;

                              auto cell = field->getCellAt( x, y);
                              firstClicked = cell;
                              animationQueue.Add( cell->detonate() );
                            }

                           cell = field->getCellAt( x, y );
                           if( !cell->Cleared() && !cell->Flagged() )  {
                              if ( cell->getState() == Cell::State::mined )  { 
                                 cell->Clear();
                                 field->Flag();

                                 animationQueue.Add( setFlagCount() );

                                 animationQueue.Add( { cell->getCoords() } );

                                 if( secondChances > 0 && !field->allCleared() )  {
                                    secondChances--;
                                    break;
                                  }

                                 else  {
                                    secondChances--;
                                    animationQueue.Add( cell->detonateAll() );
                                    //Reset();
                                  }
                               }
                              else {
                                 animationQueue.Add( cell->detonate() );
                               }

                              if( firstClicked != nullptr )  {
                                 firstClicked->Restore();
                                 firstClicked->getLeftCell()->Restore();
                                 firstClicked->getRightCell()->Restore();
                               }

                              if( field->allCleared() )  {
                                 Reset( ); //If game won
                               }
                            } 
                         }
                      }
                  break;
               }

               case SDL_BUTTON_RIGHT:  {
               x = ( event.button.x - origin_x ) / cellSize - borderCells; 
               y = ( event.button.y - origin_y ) / cellSize - borderCells;

               if( inTextureBounds( x, y ) ) {

                  cell = field->getCellAt( x, y);
                  if( !cell->Cleared() )  {
                     if( firstClicked != nullptr )  {

                        theme->pointerAnimation( ( event.button.x / cellSize ) * cellSize + cellSize / 2, ( event.button.y / cellSize ) * cellSize + cellSize / 2 );

                        if( cell->Flagged() )  {
                           field->Unflag();
                         }
                        else  {
                           field->Flag();
                         }

                        int flag = cell->Flag();

                        animationQueue.Add( Redraw( cell, flag ) );
                        auto coords = cell->getCoords();
                        theme->detonationAnimation( 
                           ( coords.x + borderCells ) * cellSize + cellSize / 2 + origin_x, 
                           ( coords.y + borderCells ) * cellSize + cellSize / 2 + origin_y, 
                           canvas, flag == Cell::State::cell );

                        animationQueue.Add( cell->hideErased() );
                        animationQueue.Add( setFlagCount() );

                        if( field->allCleared() )  {
                           Reset( );
                         }
                      }
                   }
                }
               break;
               }
            }

            redrawScene();
            break;
         }
      }
      auto frameTicks = startCounter.getTicks();

      if( frameTicks < fpsCap ) { // This needs to be looked at once more
         SDL_Delay( fpsCap - frameTicks );
       } 
    }
 }

void Game::loadTheme( std::string themeName_ )  {
    theme.reset();
    theme = std::make_shared<Theme>( std::string( "resources/" ) + themeName_ + std::string( "/" ), canvas, cellSize );
 }

void Game::flipEnableCursor()  {
    cursorEnabled ^= false;
 }

void Game::flipEnableHint()  {
    hintEnabled ^= false;
 }

void Game::loadTextures()  {
    tileMap = std::make_shared<Texture>( theme->getSkin(), canvas.getRenderer() );
    tileSize = tileMap->imageWidth();
    gameArea = std::make_shared<Texture>( fieldLength * tileSize, fieldHeight * tileSize, canvas.getRenderer() );

    borderTiles = std::make_shared<Texture>( ( 2 * borderCells + fieldLength ) * tileSize, ( 2 * borderCells + fieldHeight ) * tileSize, canvas.getRenderer() );
    borderTiles->setColor( 150, 150, 150 ); //gray
    tileMap->setBlendMode( SDL_BLENDMODE_NONE ); 
 }

//----------Theme----------

Theme::Theme() {  }

Theme::Theme( std::string location_, Canvas& canvas_, int cellSize_ ) : cellSize( cellSize_ ), location( location_ ), pointer( Animation( std::string( "resources/" ) + std::string( "pointer.png" ), canvas_.getRenderer() ) )  {
    detonationTexture = std::make_shared<Texture>( location_ + std::string( "detonation.png" ), canvas_.getRenderer() );

    reverseDetonationTexture = std::make_shared<Texture>( location_ + std::string( "detonation.png" ), canvas_.getRenderer() );

    erasureTexture = std::make_shared<Texture>( location_ + std::string( "erasure.png" ), canvas_.getRenderer() );

    reverseErasureTexture = std::make_shared<Texture>( location_ + std::string( "erasure.png" ), canvas_.getRenderer() );

    backgroundTexture = std::make_shared<Texture>( location_ + std::string( "background.png" ), canvas_.getRenderer() );
    blackTexture = std::make_shared<Texture>( std::string( "resources/" ) + std::string( "black.png" ), canvas_.getRenderer() );

    detonation = Animation( detonationTexture, canvas_.getRenderer(), cellSize, cellSize );
    erasure = Animation( erasureTexture, canvas_.getRenderer(), cellSize, cellSize );

    reverseDetonation = Animation( reverseDetonationTexture, canvas_.getRenderer(), cellSize, cellSize ).Reverse();
    reverseErasure = Animation( reverseErasureTexture, canvas_.getRenderer(), cellSize, cellSize ).Reverse();
 }

Theme::~Theme() {  }

void Theme::redrawPointer()  {
    pointer.Render();
 }

void Theme::pointerAnimation( int x_, int y_ )  {
    pointer.Activate( x_, y_ );
 }

void Theme::redrawDetonation()  {
    redrawAnimation( detonations, detonation );
 }

void Theme::redrawErasure()  {
    redrawAnimation( erasures, erasure );
 }

void Theme::redrawAnimation( std::vector<Animation>& animations_, Animation& animation_ )  {
   bool clear = true;

   if( animations_.size() )  {
      for( std::vector<Animation>::iterator f = std::begin( animations_ ); f != std::end( animations_ ); ++f )  {
          if( !f->Finished() )  {
            f->Render();
             }

            clear &= f->Finished();
          }
        }
   else 
       animation_.Render();

   if( clear ) animations_.clear();
 }

void Theme::redrawBackground( SDL_Rect dest_, Canvas& canvas_ )  {
   if( backgroundTexture != nullptr && blackTexture != nullptr )  {
      canvas_.renderTargetSelf();
      canvas_.Paint( *blackTexture );
      canvas_.Paint( *backgroundTexture, dest_ );
    }
 }

void Theme::detonationAnimation( int x_, int y_, Canvas& canvas_, bool reverse_ )  {
    if( detonationTexture->getTexture() != nullptr )  {
      if( reverse_ )  {
         detonations.emplace_back( Animation( reverseDetonationTexture, canvas_.getRenderer(), cellSize, cellSize ) );
       }

      else  {
         detonations.emplace_back( Animation( detonationTexture, canvas_.getRenderer(), cellSize, cellSize ) );
       }
      detonations.back().Activate( x_, y_ );
     }
 }

void Theme::erasureAnimation( int x_, int y_, Canvas& canvas_, bool reverse_ )  {
    if( erasureTexture->getTexture() != nullptr )  {
      if( reverse_ )  {
         erasures.emplace_back( Animation( reverseErasureTexture, canvas_.getRenderer(), cellSize, cellSize ) );
       }

      else  {
         erasures.emplace_back( Animation( erasureTexture, canvas_.getRenderer(), cellSize, cellSize ) );
       }
      erasures.back().Activate( x_, y_ );
     }
 }
std::string Theme::getSkin()  {
    return location + std::string( "skin.png" );
 }



#include "mines.h"
#include <ctime>
#include <sstream>

//--------------------cell-------------------- 

int Cell::clearedCells = 0;

Cell::Cell() : state( Cell::State::empty ), id( 0 ), mineCount( 0 ), coords{ 0, 0 }, flagged( false ), cleared( false ), erased( false ), visited( false ), restored( false ) {
 }

Cell::~Cell() {
    clearedCells = 0;
 }

Cell::State Cell::getState() const {
    return state;
 }


void Cell::setState( Cell::State state_ ) {
    state = state_;
 }

void Cell::countMines()  {
   if( state != Cell::State::mined )  {
      mineCount = ( getUpCell()->getState() == Cell::State::mined ) + 
               ( getDownCell()->getState() == Cell::State::mined ) + 
               ( getLeftCell()->getState() == Cell::State::mined ) + 
               ( getRightCell()->getState() == Cell::State::mined ) + 
               ( getUpCell()->getLeftCell()->getState() == Cell::State::mined ) + 
               ( getUpCell()->getRightCell()->getState() == Cell::State::mined ) + 
               ( getDownCell()->getLeftCell()->getState() == Cell::State::mined ) + 
               ( getDownCell()->getRightCell()->getState() == Cell::State::mined );
      if( mineCount > 0 ) 
         state = Cell::State::numbered;
      else state = Cell::State::empty;
    }
 }

int Cell::getMineCount() const {
    return mineCount;
 }

void Cell::setNeighbors()  {
    neighbors.emplace_back( leftCell );
    neighbors.emplace_back( leftCell->getDownCell() );
    neighbors.emplace_back( rightCell );
    neighbors.emplace_back( rightCell->getUpCell()  );
    neighbors.emplace_back( upCell );
    neighbors.emplace_back( upCell->getLeftCell()  );
    neighbors.emplace_back( downCell );
    neighbors.emplace_back( downCell->getRightCell()  );
 }

const std::vector<std::shared_ptr<Cell>>& Cell::getNeighbors()  {
    return neighbors;
 }

std::shared_ptr<Cell> Cell::getUpCell() {
    return upCell;
 }

std::shared_ptr<Cell> Cell::getDownCell() {
    return downCell;
 }

std::shared_ptr<Cell> Cell::getLeftCell() {
    return leftCell;
 }

std::shared_ptr<Cell> Cell::getRightCell() {
    return rightCell;
 }

void Cell::linkUp( std::shared_ptr<Cell> upCell_ ) {
    upCell = upCell_;
 }

void Cell::linkDown( std::shared_ptr<Cell> downCell_ ) {
    downCell = downCell_;
 }

void Cell::linkLeft( std::shared_ptr<Cell> leftCell_ ) {
    leftCell = leftCell_;
 }

void Cell::linkRight( std::shared_ptr<Cell> rightCell_ ) {
    rightCell = rightCell_;
 }

std::vector<Cell::Coords> Cell::detonate()  {
    std::vector<Cell::Coords> cells;

    if( !Flagged() && !Cleared() )  {
       if( state == Cell::State::empty )  {
          Clear();
          clearedCells++;
          cells.emplace_back( coords );

          for( auto f : neighbors )  {
             if( f->getState() == Cell::State::numbered && !f->Flagged() && !f->Cleared() )  {
                f->Clear();
                clearedCells++;
                cells.emplace_back( f->getCoords() );
              }
             else if( f->getState() == Cell::State::empty | !f->Cleared() ) {
                auto vec = f->detonate();
                cells.insert( cells.end(), vec.begin(), vec.end() );
              }
           }
        }

       else if( state == Cell::State::numbered )  {
          Clear();
          clearedCells++;
          cells.emplace_back( coords );
        }
     }
    
    auto erasedCells = hideErased();
    cells.insert( cells.end(), erasedCells.begin(), erasedCells.end() );
    return cells;
 }

void Cell::setCoords( int x_, int y_ )  {
    coords = { x_, y_ };
 }

const Cell::Coords& Cell::getCoords()  {
    return coords;
 }

void Cell::setId( int id_ ) {
    id = id_;
 }

int Cell::getId() const {
    return id;
 }

int Cell::Flag()  {
    if( !flagged && !cleared )  {
       flagged = true;
       return State::flag;
     }
    else if( !cleared && flagged ) {
       flagged = false;
       return cell;
     }
    return -1;
 }
   
bool Cell::Flagged()  {
    return flagged;
 }

void Cell::Erase()  {
    restored = false;
    erased = true;
 }

bool Cell::Erased()  {
    return erased;
 }

void Cell::Restore()  {
    restored = true;
    erased = false;
 }

void Cell::Clear()  {
    cleared = true;
 }

bool Cell::Cleared()  {
    return cleared;
 }

bool Cell::Restored()  {
    return restored;
 }

bool Cell::Visited()  {
    return visited;
 }

void Cell::Visit()  {
    visited = true;
 }

int Cell::drawTile()  {
   if( flagged ) return State::flag;
   else if( erased ) return 0;
   else if( state == empty ) return 0;
   else if( state == mined ) return State::mined;
   else if( cleared )  {
      if( mineCount > 0 ) return mineCount;
    }
   else return State::cell;
}

void Cell::setMineCount( int mineCount_ )  {
    mineCount = mineCount_;
 }

int Cell::getClearedCells()  {
    return clearedCells;
 }

std::vector<Cell::Coords> Cell::hideErased()  {
    std::vector<Cell::Coords> vec;

    bool clear = flagged | cleared,
   clear1,
   clearSelf = flagged | cleared,

   notAllFlagged = false,
   notAllFlagged1 = false;

   if( !erased )  {
      for( auto n : neighbors )  {
         if( n->Cleared() | n->Flagged() && ( n->getState() == State::numbered ) && !n->Erased() )  {
            for( auto p : n->getNeighbors() )  {
               if( !p->Erased() )  {
                  clear &= p->Flagged() | p->Cleared();
                  notAllFlagged |= p->Cleared();
                }

               if( p->Cleared() | p->Flagged() && ( p->getState() == State::numbered ) && !p->Erased() )  {
                  for( auto q : p->getNeighbors() )  {
                     if( !q->Erased() )  {
                        clear1 &= q->Flagged() | q->Cleared();
                        notAllFlagged1 |= q->Cleared() ;
                      }
                   } 

                  if( clear1 && notAllFlagged1 && !p->Flagged() )  {
                     p->Erase();
                     vec.emplace_back( p->getCoords() );
                   }

                  clear1 = p->Flagged() | p->Cleared();
                  notAllFlagged1 = false;
                }
             }

            if( clear && !n->Flagged() )  {
               n->Erase();
               vec.emplace_back( n->getCoords() );
             }
          }

         clearSelf &= n->Flagged() | n->Cleared() | n->Erased();

         clear = flagged | cleared;
         notAllFlagged = false;
       }

      if( clearSelf && !flagged && state == numbered )  {
         Erase();
         vec.emplace_back( coords);
       }
    }
   return vec;
 }

std::vector<Cell::Coords> Cell::detonateAll()  {
   std::vector<Cell::Coords> cells;
   if( !Flagged() )  {

      Clear();
      cells.emplace_back( coords );

      for( auto f : neighbors )  {
         if( !f->Cleared() ) {
            auto vec = f->detonateAll();
            cells.insert( cells.end(), vec.begin(), vec.end() );
          }
       }
    }
    return cells;
 }

//--------------------cellRow--------------------

cellRow::cellRow() {

 }

cellRow::cellRow( int count_ )  {
   row.emplace_back( std::make_shared<Cell>( Cell() ) );
   std::deque<std::shared_ptr<Cell>>::iterator it = row.begin();

   for( int n = 0; n < count_ - 1; n++ ) {
      auto newCell = std::make_shared<Cell>( Cell() );

      newCell->linkLeft( *it );

      (*it)->linkRight( newCell );

      row.emplace_back( newCell );
      ++it;
     }

   (*it)->linkRight( row.front() );
   row.front()->linkLeft( *it );
 } 

cellRow::~cellRow() {

 }

const std::deque<std::shared_ptr<Cell>>& cellRow::getRow() const {
    return row;
 }

int cellRow::rowSize() const {
    return row.size();
 }

//--------------------cellGrid--------------------  

cellGrid::cellGrid() {

 }

cellGrid::cellGrid( int length_, int height_ )  {//: grid( height_, cellRow( length_ ) ) //copies shared pointers too, the damn bastard

   for( int n = 0; n < height_; n++ )  {
      grid.emplace_back( cellRow( length_ ) );
    }
   int id = 0, row = 0, col = 0;

   auto link = [&]( std::shared_ptr<Cell> up_, std::shared_ptr<Cell> down_ ) -> void  {
      (*up_).linkDown( ( down_ ) );
      (*down_).linkUp( ( up_ ) );

      (*up_).setId( id );

      cellMap.emplace( id, up_ );
      id++;
    };

   std::deque<cellRow>::iterator firstRow = std::begin( grid );
   std::deque<cellRow>::iterator nextRow;
   if( grid.size() >= 1 )  nextRow = firstRow + 1;

   for( row = 0; nextRow != std::end( grid ); ++firstRow, ++nextRow, row++ )  {
      auto upperCell = std::begin( firstRow->getRow() );
      auto lowerCell = std::begin( nextRow->getRow() );

      for( col = 0; upperCell != std::end( firstRow->getRow() ); ++upperCell, ++lowerCell, col++ )  { 
         link( *upperCell, *lowerCell );
         (*upperCell)->setCoords( col, row );
       }
    }

   auto upperCell = std::begin( grid.back().getRow() );
   auto lowerCell = std::begin( grid.front().getRow() );

   for( col = 0; lowerCell != std::end( grid.front().getRow() ); ++upperCell, ++lowerCell, col++ )  { 
      link( *upperCell, *lowerCell );
          (*upperCell)->setCoords( col, row );
     }
 }

cellGrid::~cellGrid() {

 }

int cellGrid::gridSize() const {
    return grid.size();
 }

const std::deque<cellRow>& cellGrid::getGrid()  {
    return grid;
 }

const std::map<int, std::shared_ptr<Cell>>& cellGrid::getMap() {
    return cellMap;
 }

//--------------------mineField--------------------    

mineField::mineField()  {

 }

mineField::mineField( int length_, int height_, int mineCount_ ) : length( length_ ), height( height_ ), mineCount( mineCount_ ), grid( length_, height_ ), mapSize( grid.getMap().size() ), range( 0, mapSize - 1 ), flaggedMines( 0 )  {
   for( auto f : grid.getMap()  )  {
      f.second->setNeighbors();
      cellsCoords.emplace_back( f.second->getCoords() );
    }
 } 

mineField::~mineField()  {

 }

void mineField::Mine( int amount_, Cell::Coords coords_ )  {
   std::mt19937 generator( std::time( nullptr ) );

   bool clear = true;
   std::vector<std::shared_ptr<Cell>> vec;

   if( coords_.x != -1 && coords_.y != -1 )  {
      for( auto n : getCellAt( coords_.x, coords_.y )->getNeighbors() )  {
         if( amount_ >= 9 )  {
            for( auto p : n->getNeighbors() )  {
               if( !p->Visited() )  {
                  vec.emplace_back( p );
                  p->Visit();
               }
            }
         }
         n->Visit();
         vec.emplace_back( n );
       }
    }

   if( mapSize >= mineCount ) {
      while( amount_ > 0 )  {
         auto pointer = grid.getMap().at( range( generator ) );

         for( auto f : vec )  {
            clear &= f != pointer;
          }

         if( clear )  {
            if( (bool) ( pointer->getState() != Cell::State::mined ) )  {
               pointer->setState( Cell::State::mined );
               amount_--;
             }   
          }
         clear = true;
       }
    } 

   for( auto f : grid.getMap()  )  {
      f.second->countMines();
    }
 }


const std::shared_ptr<Cell>& mineField::getCellAt( int x_, int y_ )  {
   if( grid.gridSize() ) { 
      if( y_ >= 0 && y_ < grid.gridSize() && x_ >= 0 && x_ < grid.getGrid().front().rowSize() ) {
         return grid.getGrid().at( y_ ).getRow().at( x_ );
       }
    }
 }

int mineField::getSize() {
    return mapSize;
 }

void mineField::Flag()  {
    flaggedMines++;
 }

void mineField::Unflag()  {
    flaggedMines--;
 }

int mineField::getFlaggedMines()  {
    return mineCount - flaggedMines;
 }

bool mineField::allCleared()  {
    return mapSize - mineCount == getCellAt( 0, 0 )->getClearedCells() && getFlaggedMines() == 0;
 }

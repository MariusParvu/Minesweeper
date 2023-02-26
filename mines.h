#ifndef MINES_H
#define MINES_H

#include <iostream>
#include <deque>
#include <map>
#include <memory>
#include <random>

//----------cell----------  

class Cell {
    public:

    Cell();
    Cell( std::shared_ptr<Cell> );
    ~Cell();
    
    struct Coords;
    enum State { empty = 0, numbered, detonated, mined = 11, flag, cell };

    std::vector<Cell::Coords> detonate();
    void clear();

    int getMineCount() const;
    void countMines();
    void setNeighbors();
    const std::vector<std::shared_ptr<Cell>>& getNeighbors();
    void setId( int );
    int getId() const;

    const Coords& getCoords();
    void setCoords( int, int );

    State getState() const;
    void setState( Cell::State );
    
    void linkUp( std::shared_ptr<Cell> );
    void linkDown( std::shared_ptr<Cell> );
    void linkLeft( std::shared_ptr<Cell> );
    void linkRight( std::shared_ptr<Cell> );

    void Clear();
    void Unclear();
    int Flag();
    void Erase();
    bool Erased();
    void Restore();
    bool Flagged();
    bool Cleared();
    bool Restored();

    bool Visited();
    void Visit();

    int drawTile();
    int getClearedCells();
    void setMineCount( int );
    std::vector<Cell::Coords> hideErased();
    std::vector<Cell::Coords> detonateAll();

    std::shared_ptr<Cell> getUpCell();
    std::shared_ptr<Cell> getDownCell();
    std::shared_ptr<Cell> getLeftCell();
    std::shared_ptr<Cell> getRightCell();

    struct Coords { int x, y; } coords;
    
    private:

    State state;
    bool cleared, flagged, erased, visited, restored /* used when the game starts */;
    static int clearedCells;
                                        
    std::shared_ptr<Cell> upCell;
    std::shared_ptr<Cell> downCell;
    std::shared_ptr<Cell> leftCell;
    std::shared_ptr<Cell> rightCell;

    std::vector<std::shared_ptr<Cell>> neighbors;

    int mineCount, id;
    
 };

//----------cellRow----------  

class cellRow {
    std::deque<std::shared_ptr<Cell>> row;
    
    public:

    cellRow();
    cellRow( int );
    ~cellRow();
    
    const std::deque<std::shared_ptr<Cell>>& getRow() const;
    int rowSize() const;
 };

//----------cellGrid----------  

class cellGrid {
    std::deque<cellRow> grid;
    std::map<int, std::shared_ptr<Cell>> cellMap;
    
    public:

    cellGrid();
    cellGrid( int, int );
    ~cellGrid();

    int gridSize() const;
    const std::deque<cellRow>& getGrid();
    const std::map<int, std::shared_ptr<Cell>>& getMap();
 };

//----------mineField----------  

class mineField  {
    cellGrid grid;
    int length, height, mineCount, mapSize, flaggedMines;
    std::uniform_int_distribution<> range;
    std::vector<Cell::Coords> cellsCoords;

    public:
    mineField();
    mineField( int, int, int );
    ~mineField();

    const std::shared_ptr<Cell>& getCellAt( int, int );
    int getSize();
    void Mine( int, Cell::Coords = { -1, -1 } );
    void Flag();
    void Unflag();
    bool allCleared();
    int getFlaggedMines();
 };

#endif

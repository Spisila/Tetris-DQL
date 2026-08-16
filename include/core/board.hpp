#pragma once

#include <array>
#include <tetromino.hpp>

#include <shared.hpp>
#include <types.hpp>

constexpr int BOARD_SIZE_X = 10;
constexpr int BOARD_SIZE_Y = 24;

class Board
{
private:

  std::array<std::array<CellState, BOARD_SIZE_Y>, BOARD_SIZE_X> board;
  Tetromino tetromino{};

  std::array<Position, 4> spawn_positions = {
      Position(5, 1),
      Position(0, 0),
      Position(0, 0),
      Position(0, 0)};

public:
  Board(/* args */);
  ~Board();

  // Getters

  std::array<std::array<CellState, BOARD_SIZE_Y>, BOARD_SIZE_X> getBoardCells() const;
  std::array<int, BOARD_SIZE_X> getBoardHeight() const;
  int getAggregateHeight() const;
  int getAmountOfHoles() const;
  int getRugosity() const;
  const std::vector<int> getBoardState() const;

  Tetromino &getBoardTetromino();
  const Tetromino &getBoardTetromino() const;

  //Setters

  void setTetrominoCellsStates(CellState state, std::array<Position, 4> positions);

  //Methods
    
  void moveTetromino(MovementDirection dir);
  void rotateTetromino(Rotation rot);

  bool setPiece(PieceType new_piece);

  bool checkCollisions(std::array<Position, 4> projected_positions);
  bool checkTouchedFloor(std::array<Position, 4> projected_positions);
  bool checkOutOfLateralBounds(std::array<Position, 4> projected_positions);

  bool checkShouldSetPiece(std::array<Position, 4> projected_position);

  bool checkLoss(std::array<Position, 4> new_piece_positions);

  void tickGravity();
  int clearLines();

  void clearBoard();
};



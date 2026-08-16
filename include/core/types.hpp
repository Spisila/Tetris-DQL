#pragma once

enum class CellState
{
  EMPTY,
  SPAWN,
  ACTIVE,
  FILLED
};

enum class Actions
{
  MOVE_LEFT,
  MOVE_RIGHT,
  SOFT_DROP,
  ROTATE_CLOCKWISE,
  ROTATE_COUNTER_CLOCKWISE,
  HARD_DROP,
  HOLD
};


// index + (currn +/- 1) mod(4)
enum class PieceType
{
  // Index 0
  I_PIECE_0,
  I_PIECE_R,
  I_PIECE_2,
  I_PIECE_L,

  // Index 4
  J_PIECE_0,
  J_PIECE_R,
  J_PIECE_2,
  J_PIECE_L,

  // Index 8
  L_PIECE_0,
  L_PIECE_R,
  L_PIECE_2,
  L_PIECE_L,

  // Index 12
  O_PIECE_0,
  O_PIECE_R,
  O_PIECE_2,
  O_PIECE_L,

  // Index 16
  S_PIECE_0,
  S_PIECE_R,
  S_PIECE_2,
  S_PIECE_L,

  // Index 20
  T_PIECE_0,
  T_PIECE_R,
  T_PIECE_2,
  T_PIECE_L,

  // Index 24
  Z_PIECE_0,
  Z_PIECE_R,
  Z_PIECE_2,
  Z_PIECE_L
};


enum class MovementDirection
{
  LEFT,
  RIGHT,
  DOWN,
  UP,
  TOP_RIGHT,
  TOP_LEFT,
  BOTTOM_RIGHT,
  BOTTOM_LEFT
};

enum class Rotation
{
  CLOCKWISE,
  COUNTER_CLOCKWISE,
  ONE_EIGHTY
};

struct StepData
{
  float reward;

  bool piece_placed{false};
  bool lost{false};
};

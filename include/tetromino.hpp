#pragma once

#include <unordered_map>
#include <shared.hpp>
#include <array>

#include <shared.hpp>

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

inline std::unordered_map<PieceType, std::array<Position, 3>> rotationToPositionOffsets = {

    {PieceType::I_PIECE_0, std::array<Position, 3>{Position(-1, 0), Position(+1, 0), Position(+2, 0)}},
    {PieceType::I_PIECE_R, std::array<Position, 3>{Position(0, -1), Position(0, +1), Position(0, +2)}},
    {PieceType::I_PIECE_2, std::array<Position, 3>{Position(-1, 0), Position(-2, 0), Position(+1, 0)}},
    {PieceType::I_PIECE_L, std::array<Position, 3>{Position(0, +1), Position(0, -1), Position(0, -2)}},

    {PieceType::J_PIECE_0, std::array<Position, 3>{Position(-1, 0), Position(-1, -1), Position(+1, 0)}},
    {PieceType::J_PIECE_R, std::array<Position, 3>{Position(0, -1), Position(+1, -1), Position(0, +1)}},
    {PieceType::J_PIECE_2, std::array<Position, 3>{Position(-1, 0), Position(+1, 0), Position(+1, +1)}},
    {PieceType::J_PIECE_L, std::array<Position, 3>{Position(0, -1), Position(0, +1), Position(-1, +1)}},

    {PieceType::L_PIECE_0, std::array<Position, 3>{Position(-1, 0), Position(+1, 0), Position(+1, -1)}},
    {PieceType::L_PIECE_R, std::array<Position, 3>{Position(0, -1), Position(0, +1), Position(+1, +1)}},
    {PieceType::L_PIECE_2, std::array<Position, 3>{Position(-1, +1), Position(-1, 0), Position(+1, 0)}},
    {PieceType::L_PIECE_L, std::array<Position, 3>{Position(-1, -1), Position(0, -1), Position(0, +1)}},

    {PieceType::O_PIECE_0, std::array<Position, 3>{Position(+1, 0), Position(+1, -1), Position(0, -1)}},
    {PieceType::O_PIECE_R, std::array<Position, 3>{Position(+1, 0), Position(+1, +1), Position(0, +1)}},
    {PieceType::O_PIECE_2, std::array<Position, 3>{Position(-1, 0), Position(-1, +1), Position(0, +1)}},
    {PieceType::O_PIECE_L, std::array<Position, 3>{Position(-1, 0), Position(-1, -1), Position(0, -1)}},

    {PieceType::S_PIECE_0, std::array<Position, 3>{Position(-1, 0), Position(0, -1), Position(+1, -1)}},
    {PieceType::S_PIECE_R, std::array<Position, 3>{Position(0, -1), Position(+1, 0), Position(+1, +1)}},
    {PieceType::S_PIECE_2, std::array<Position, 3>{Position(-1, +1), Position(0, +1), Position(+1, 0)}},
    {PieceType::S_PIECE_L, std::array<Position, 3>{Position(-1, -1), Position(-1, 0), Position(0, +1)}},

    {PieceType::T_PIECE_0, std::array<Position, 3>{Position(-1, 0), Position(0, -1), Position(+1, 0)}},
    {PieceType::T_PIECE_R, std::array<Position, 3>{Position(0, -1), Position(0, +1), Position(+1, 0)}},
    {PieceType::T_PIECE_2, std::array<Position, 3>{Position(-1, 0), Position(0, +1), Position(+1, 0)}},
    {PieceType::T_PIECE_L, std::array<Position, 3>{Position(-1, 0), Position(0, -1), Position(0, +1)}},

    {PieceType::Z_PIECE_0, std::array<Position, 3>{Position(-1, -1), Position(0, -1), Position(+1, 0)}},
    {PieceType::Z_PIECE_R, std::array<Position, 3>{Position(+1, 0), Position(0, +1), Position(+1, -1)}},
    {PieceType::Z_PIECE_2, std::array<Position, 3>{Position(-1, 0), Position(0, +1), Position(+1, +1)}},
    {PieceType::Z_PIECE_L, std::array<Position, 3>{Position(-1, 0), Position(-1, +1), Position(0, -1)}}

};

class Tetromino
{
private:
  std::array<Position, 4> pieces_positions{Position(5, 1), Position(0, 0), Position(0, 0), Position(0, 0)};
  PieceType current_piece_type = PieceType::I_PIECE_0;

public:
  Tetromino(/* args */);
  ~Tetromino();

  int getCurrentPieceTypeFormatted() const;
  int getCurrentPieceType() const;
  
  Position getPivotPosition() const;
  const std::array<Position, 4> getAllPositions() const;
  

  void setTetrominoType(PieceType new_type);
  
  void setPositions(std::array<Position, 4> new_pos);
  PieceType changeTetrominoeRotation(Rotation rot);

  std::array<Position, 4> projectMovement(MovementDirection dir);
  std::array<Position, 4> projectRotation(Rotation rot);
};

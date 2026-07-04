#pragma once

struct Position
{
  int x;
  int y;

  Position(int _x, int _y) : x(_x), y(_y) {}
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

inline int formatPiece(int piece)
{
  switch (piece)
  {
  case 0:
    return 0;
  case 4:
    return 1;
  case 8:
    return 2;
  case 12:
    return 3;
  case 16:
    return 4;
  case 20:
    return 5;
  case 24:
    return 6;
  default:
    return -1;
  }
}
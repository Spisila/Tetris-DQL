#pragma once

#include <raylib.h>

#include <vector>
#include <array>
#include <unordered_map>
#include <random>

constexpr int BOARD_SIZE_X = 10;
constexpr int BOARD_SIZE_Y = 24;

constexpr int ENTIRE_BOARD_SIZE = BOARD_SIZE_X * BOARD_SIZE_Y;

constexpr Color SPAWN_COLOR = Color{75, 75, 75, 255};
constexpr Color BOARD_COLOR = Color{15, 15, 15, 255};
constexpr Color FILLED_COLOR = Color{100, 100, 100, 255};

constexpr int CELL_SIZE = 30;

enum class Cell_state
{
  EMPTY,
  SPAWN,
  ACTIVE,
  FILLED
};

// index + (currn +/- 1) mod(4)
enum class Piece_type
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

enum class Movement_direction
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

struct Position
{
  int x;
  int y;

  Position(int _x, int _y) : x(_x), y(_y) {}
};

// First position is the pivot
struct Tetromino
{
  std::array<Position, 4> pieces_positions{Position(5, 1), Position(0, 0), Position(0, 0), Position(0, 0)};
  Piece_type current_type = Piece_type::I_PIECE_0;
};

// If rotating clockwise, from even to odd multiply by -1
// If rotating counter-clockwise

// Map the piece types to positions relative to the pivot
inline std::unordered_map<Piece_type, std::array<Position, 3>> tetromino_type_rotation_to_piece_positions = {

    {Piece_type::I_PIECE_0, std::array<Position, 3>{Position(-1, 0), Position(+1, 0), Position(+2, 0)}},
    {Piece_type::I_PIECE_R, std::array<Position, 3>{Position(0, -1), Position(0, +1), Position(0, +2)}},
    {Piece_type::I_PIECE_2, std::array<Position, 3>{Position(-1, 0), Position(-2, 0), Position(+1, 0)}},
    {Piece_type::I_PIECE_L, std::array<Position, 3>{Position(0, +1), Position(0, -1), Position(0, -2)}},

    {Piece_type::J_PIECE_0, std::array<Position, 3>{Position(-1, 0), Position(-1, -1), Position(+1, 0)}},
    {Piece_type::J_PIECE_R, std::array<Position, 3>{Position(0, -1), Position(+1, -1), Position(0, +1)}},
    {Piece_type::J_PIECE_2, std::array<Position, 3>{Position(-1, 0), Position(+1, 0), Position(+1, +1)}},
    {Piece_type::J_PIECE_L, std::array<Position, 3>{Position(0, -1), Position(0, +1), Position(-1, +1)}},

    {Piece_type::L_PIECE_0, std::array<Position, 3>{Position(-1, 0), Position(+1, 0), Position(+1, -1)}},
    {Piece_type::L_PIECE_R, std::array<Position, 3>{Position(0, -1), Position(0, +1), Position(+1, +1)}},
    {Piece_type::L_PIECE_2, std::array<Position, 3>{Position(-1, +1), Position(-1, 0), Position(+1, 0)}},
    {Piece_type::L_PIECE_L, std::array<Position, 3>{Position(-1, -1), Position(0, -1), Position(0, +1)}},

    {Piece_type::O_PIECE_0, std::array<Position, 3>{Position(+1, 0), Position(+1, -1), Position(0, -1)}},
    {Piece_type::O_PIECE_R, std::array<Position, 3>{Position(+1, 0), Position(+1, +1), Position(0, +1)}},
    {Piece_type::O_PIECE_2, std::array<Position, 3>{Position(-1, 0), Position(-1, +1), Position(0, +1)}},
    {Piece_type::O_PIECE_L, std::array<Position, 3>{Position(-1, 0), Position(-1, -1), Position(0, -1)}},

    {Piece_type::S_PIECE_0, std::array<Position, 3>{Position(-1, 0), Position(0, -1), Position(+1, -1)}},
    {Piece_type::S_PIECE_R, std::array<Position, 3>{Position(0, -1), Position(+1, 0), Position(+1, +1)}},
    {Piece_type::S_PIECE_2, std::array<Position, 3>{Position(-1, +1), Position(0, +1), Position(+1, 0)}},
    {Piece_type::S_PIECE_L, std::array<Position, 3>{Position(-1, -1), Position(-1, 0), Position(0, +1)}},

    {Piece_type::T_PIECE_0, std::array<Position, 3>{Position(-1, 0), Position(0, -1), Position(+1, 0)}},
    {Piece_type::T_PIECE_R, std::array<Position, 3>{Position(0, -1), Position(0, +1), Position(+1, 0)}},
    {Piece_type::T_PIECE_2, std::array<Position, 3>{Position(-1, 0), Position(0, +1), Position(+1, 0)}},
    {Piece_type::T_PIECE_L, std::array<Position, 3>{Position(-1, 0), Position(0, -1), Position(0, +1)}},

    {Piece_type::Z_PIECE_0, std::array<Position, 3>{Position(-1, -1), Position(0, -1), Position(+1, 0)}},
    {Piece_type::Z_PIECE_R, std::array<Position, 3>{Position(+1, 0), Position(0, +1), Position(+1, -1)}},
    {Piece_type::Z_PIECE_2, std::array<Position, 3>{Position(-1, 0), Position(0, +1), Position(+1, +1)}},
    {Piece_type::Z_PIECE_L, std::array<Position, 3>{Position(-1, 0), Position(-1, +1), Position(0, -1)}}

};

class Game
{
private:
  int GRAVITY_TICKS = 30;

  std::random_device rd;
  std::mt19937 gen{rd()};

public:
  Tetromino active_tetromino;
  std::array<std::array<Cell_state, BOARD_SIZE_Y>, BOARD_SIZE_X> board;

  int hold_piece_index = -1;
  bool hold_used = false;

  int queue_index = 0;
  std::array<Piece_type, 14> piece_queue;
  std::array<Piece_type, 5> known_piece_queue;

  int gravity_counter = 0;
  int score = 0;

  Game(/* args */);

  std::array<int, BOARD_SIZE_X> get_board_height();
  int get_current_peice_type();
  std::array<Position, 4> get_active_tetromino_pieces_positions();
  std::array<int, 5> get_piece_queue();

  int return_data();

  void set_tetromino_cell_state(Cell_state state);
  void set_piece(Piece_type new_piece);
  void update_active_tetromino();

  void tick_gravity();

  Piece_type change_tetrominoe_rotation(Rotation rot);

  void move_tetromino(Movement_direction dir);
  void rotate_tetromino(Rotation rot);
  void hard_drop();

  void hold_current_piece();

  void generate_piece_queue();
  void update_piece_queue();

  int clear_lines();

  std::array<Position, 4> project_movement(Movement_direction dir);
  std::array<Position, 4> project_rotation(Rotation rot);

  bool check_collision(const std::array<Position, 4>);
  bool check_out_of_bounds(const std::array<Position, 4> projected_positions);
  bool check_touched_floor(const std::array<Position, 4> projected_positions);

  void draw_board();

  ~Game();
};

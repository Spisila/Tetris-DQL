#pragma once

#include <raylib.h>

#include <vector>
#include <array>
#include <unordered_map>

#include <board.hpp>
#include <piece_queue.hpp>
#include <types.hpp>

#include <shared.hpp>


constexpr float LOSS_SCORE_WEIGHT = -100.0f;
constexpr float LINES_CLEARED_WEIGHT = 200.0f;
constexpr float TOTAL_HEIGHT_WEIGHT = 0.5f;
constexpr float HOLES_WEIGHT = 4.0f;
constexpr float RUGOSITY_WEIGHT = 0.2f;

constexpr int piece_queue_size = 5;

struct StepData
{
  float reward;

  bool piece_placed{false};
  bool lost{false};
};

// First position is the pivot

// If rotating clockwise, from even to odd multiply by -1
// If rotating counter-clockwise

// Map the piece types to positions relative to the pivot



class Game
{
private:
  int id;

  int GRAVITY_TICKS = 30;

  Board game_board{};
  PieceQueue piece_queue{};

public:
  Actions next_action{Actions::HOLD};

  int lines_cleared = 0;

  int hold_piece_index = -1;
  bool hold_used = false;

  bool lost = false;
  bool piece_set = false;

  int gravity_counter = 0;
  int score = 0;

  Game(int _id);

  void step();

  void reset();

  StepData calculateReward();

  // ---------------

 
  // ---------------

  int getId();

  int getScore();

  std::vector<int> getGameState();

  PieceQueue &getPieceQueue();
  const PieceQueue &getPieceQueue() const;

  Board &getBoard();
  const Board &getBoard() const;

  void increase_score(int lines);
  void reset_score();
  void increase_gravity_counter();

  void update_active_tetromino();

  void tick_gravity();

  void hardDrop();

  // TODO: Implement hold again
  void holdCurrentPiece();

  int pieceWasSet();

  // int clear_lines();

  ~Game();
};

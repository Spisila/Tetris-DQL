
#include <array>
#include <vector>

#include <span>
#include <thread>
#include <future>

#include <barrier>

#include <game.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

namespace py = pybind11;

class MultiGame
{
private:
  size_t GAME_AMOUNT = 8;

  std::vector<Game> games;
  std::vector<std::span<Game>> segments;

  bool running{false};
  std::vector<std::thread> game_workers;

  std::barrier<> barrier;

public:

  std::vector<std::vector<int>> states_cache; 

  MultiGame(size_t parallel_game_count);
  ~MultiGame();

  void threadLoop(size_t thread_segment_index);

  std::vector<StepData> stepAll(std::vector<Actions> _actions);
  void stepThis(int index, Actions _action);

  void resetAll();
  void resetThis(int index);

  void checkPiecePlaced();
  void checkGameLost();

  int getSumScore();

  std::vector<std::vector<int>> getStates();
};

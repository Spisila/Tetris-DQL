#include <rl_env.hpp>
#include <iostream>

RLEnv::RLEnv(int _id) : game_env(_id)
{
}

RLEnv::~RLEnv()
{
}

void RLEnv::takePreciseAction(PreciseActions action)
{
  int precise_action_num = static_cast<int>(action);

  if (precise_action_num <= 39)
  {
    int rotation = precise_action_num % 4;
    int column = precise_action_num / 4;

    game_env.getBoard().moveTetrominoToColumnWithRotation(column, rotation);
    game_env.hardDrop();
  }
  else
  {
    if (game_env.hold_used == false)
    {
      game_env.hold_used = true;
      game_env.holdCurrentPiece();
    }
  }
}

auto RLEnv::testStep()
{

  int actions_start = static_cast<int>(PreciseActions::ROTATION_0_COLUMN_0);
  int actions_end = static_cast<int>(PreciseActions::HOLD);

  std::array<std::vector<int>, 40> state_attempts;

  for (int i = actions_start; i < actions_end; i++)
  {

    auto precise_action_i = static_cast<PreciseActions>(i);

    takePreciseAction(precise_action_i);

    state_attempts.at(i) = getGameState();

    game_env.getBoard().setTetrominoCellsStates(CellState::EMPTY, game_env.getLastPiecePlacedPositions());
  }

  return state_attempts;
}

void RLEnv::step()
{
  takePreciseAction(game_env.next_precise_action);
}

void RLEnv::reset()
{
  game_env.lost = false;

  game_env.hold_used = false;
  game_env.hold_piece_index = -1;

  game_env.getBoard().clearBoard();

  game_env.getPieceQueue().generatePieceQueue();
  game_env.getPieceQueue().updatePieceQueue();

  game_env.getBoard().setPiece(static_cast<PieceType>(game_env.getPieceQueue().getPieceQueue().at(0)));
  game_env.getBoard().setTetrominoCellsStates(CellState::ACTIVE, game_env.getBoard().getBoardTetromino().getAllPositions());
}

StepData RLEnv::calculateReward()
{
  if (getGameEnv().lost)
  {
    StepData loss_step = {LOSS_SCORE_WEIGHT, true, true};
    return loss_step;
  }

  if (getGameEnv().piece_set == true)
  {
    getGameEnv().piece_set = false;

    int lines = getGameEnv().getBoard().clearLines();
    int total_height = getGameEnv().getBoard().getAggregateHeight();
    int total_holes = getGameEnv().getBoard().getAmountOfHoles();
    int rugosity = getGameEnv().getBoard().getRugosity();

    float reward =
        (lines * LINES_CLEARED_WEIGHT) -
        (total_height * TOTAL_HEIGHT_WEIGHT) -
        (total_holes * HOLES_WEIGHT) -
        (rugosity * RUGOSITY_WEIGHT);

    StepData piece_set_step = {reward, true, false};
    return piece_set_step;
  }

  StepData gravity_step = {-0.005f, false, false};

  return gravity_step;
}

Game &RLEnv::getGameEnv()
{
  return game_env;
}

int RLEnv::getGameEnvID() const
{
  return game_env.getId();
}

// TODO: Change vector into
std::vector<int> RLEnv::getGameState()
{

  std::vector<int> state;

  std::array<int, 3> board_state = game_env.getBoard().getBoardState();

  for (int i : board_state)
  {
    state.push_back(i);
  }

  const Tetromino &current_tetromino = game_env.getBoard().getBoardTetromino();
  const int current_piece_type = current_tetromino.getCurrentPieceType();

  state.push_back(current_piece_type);

  const PieceQueue &c_q = game_env.getPieceQueue();
  const auto &p_q = c_q.getPieceQueue();

  for (int piece : p_q)
  {
    state.push_back(formatPiece(piece));
  }

  return state;
}

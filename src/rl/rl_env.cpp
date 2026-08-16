#include <rl_env.hpp>

RLEnv::RLEnv(int _id) : game_env(_id)
{
}

RLEnv::~RLEnv()
{
}

void RLEnv::step()
{

  switch (game_env.next_action)
  {
  case Actions::MOVE_RIGHT:
    game_env.getBoard().moveTetromino(MovementDirection::RIGHT);
    break;
  case Actions::MOVE_LEFT:
    game_env.getBoard().moveTetromino(MovementDirection::LEFT);
    break;
  case Actions::SOFT_DROP:
    game_env.getBoard().moveTetromino(MovementDirection::DOWN);
    break;
  case Actions::ROTATE_CLOCKWISE:
    game_env.getBoard().rotateTetromino(Rotation::CLOCKWISE);
    break;
  case Actions::ROTATE_COUNTER_CLOCKWISE:
    game_env.getBoard().rotateTetromino(Rotation::COUNTER_CLOCKWISE);
    break;
  case Actions::HARD_DROP:
    game_env.hardDrop();
    break;
  case Actions::HOLD:
    if (game_env.hold_used == false)
    {
      game_env.hold_used = true;
      game_env.holdCurrentPiece();
    }
    break;
  default:
    break;
  }
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

std::vector<int> RLEnv::getGameState()
{

  std::vector<int> state;

  std::vector<int> board_state = game_env.getBoard().getBoardState();

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


#include <game.hpp>

#include <raylib.h>

#include <iostream>

/* TODO:
Implement wall kicks
*/

Game::Game(int _id)
{

    id = _id;

    piece_queue.generatePieceQueue();
    piece_queue.updatePieceQueue();
    game_board.setPiece(static_cast<PieceType>(piece_queue.getPieceQueue().at(0)));
}

Game::~Game()
{
}

void Game::step()
{

    switch (next_action)
    {
    case Actions::MOVE_RIGHT:
        game_board.moveTetromino(MovementDirection::RIGHT);
        break;
    case Actions::MOVE_LEFT:
        game_board.moveTetromino(MovementDirection::LEFT);
        break;
    case Actions::SOFT_DROP:
        game_board.moveTetromino(MovementDirection::DOWN);
        break;
    case Actions::ROTATE_CLOCKWISE:
        game_board.rotateTetromino(Rotation::CLOCKWISE);
        break;
    case Actions::ROTATE_COUNTER_CLOCKWISE:
        game_board.rotateTetromino(Rotation::COUNTER_CLOCKWISE);
        break;
    case Actions::HARD_DROP:
        hardDrop();
        break;
    case Actions::HOLD:
        if (hold_used == false)
        {
            hold_used = true;
            holdCurrentPiece();
        }
        break;
    default:
        break;
    }
}

void Game::reset()
{
    lost = false;

    game_board.clearBoard();

    piece_queue.generatePieceQueue();
    piece_queue.updatePieceQueue();

    game_board.setPiece(static_cast<PieceType>(piece_queue.getPieceQueue().at(0)));
}

StepData Game::calculateReward()
{

    if (piece_set == true)
    {
        piece_set = false;

        int lines = game_board.clearLines();
        int total_height = game_board.getAggregateHeight();
        int total_holes = game_board.getAmountOfHoles();
        int rugosity = game_board.getRugosity();

        std::array<Position, 4> spawn_positions = {
            Position(5, 1),
            Position(0, 0),
            Position(0, 0),
            Position(0, 0)};

        if (game_board.checkCollisions(spawn_positions))
        {
            StepData loss_step = {LOSS_SCORE_WEIGHT, true, true};
            return loss_step;
        }

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



#pragma region GETTERS

int Game::getId()
{
    return id;
}

int Game::getScore()
{
    return score;
}

std::vector<int> Game::getGameState()
{

    std::vector<int> state;

    std::vector<int> board_state = game_board.getBoardState();

    for (int i : board_state)
    {
        state.push_back(i);
    }

    const Tetromino &current_tetromino = game_board.getBoardTetromino();
    const int current_piece_type = current_tetromino.getCurrentPieceType();

    state.push_back(current_piece_type);

    const PieceQueue &c_q = getPieceQueue();
    const auto &p_q = c_q.getPieceQueue();

    for (int piece : p_q)
    {
        state.push_back(formatPiece(piece));
    }

    return state;
}

const PieceQueue &Game::getPieceQueue() const
{
    return piece_queue;
}

Board &Game::getBoard()
{
    return game_board;
}

const Board &Game::getBoard() const
{
    return game_board;
}

#pragma endregion

#pragma region SETTERS

void Game::reset_score()
{
    score = 0;
}

#pragma endregion

void Game::hardDrop()
{

    Tetromino &t = game_board.getBoardTetromino();

    while (true)
    {
        game_board.setTetrominoCellsStates(CellState::EMPTY, t.getAllPositions());
        auto projected_position = t.projectMovement(MovementDirection::DOWN);

        if (game_board.checkShouldSetPiece(projected_position))
        {
            lines_cleared += pieceWasSet();
            break;
        }

        t.setPositions(projected_position);
        game_board.setTetrominoCellsStates(CellState::ACTIVE, projected_position);
    }
}

void Game::increase_score(int lines)
{

    switch (lines)
    {
    case 0:
        break;
    case 1:
        score += 100;
        break;
    case 2:
        score += 200;
        break;
    case 3:
        score += 400;
        break;
    case 4:
        score += 1000;
        break;

    default:
        break;
    }
}

int Game::pieceWasSet()
{
    piece_set = true;
    hold_used = false;
    game_board.setTetrominoCellsStates(CellState::FILLED, game_board.getBoardTetromino().getAllPositions());
    game_board.setPiece(static_cast<PieceType>(piece_queue.getPieceQueue().at(0)));
    piece_queue.updatePieceQueue();
    return game_board.clearLines();
}

void Game::holdCurrentPiece()
{
    game_board.setTetrominoCellsStates(CellState::EMPTY, game_board.getBoardTetromino().getAllPositions());

    Tetromino &t = game_board.getBoardTetromino();

    int current_index = static_cast<int>(t.getCurrentPieceType());

    if (hold_piece_index == -1)
    {
        hold_piece_index = static_cast<int>(current_index);
        game_board.setPiece(static_cast<PieceType>(getPieceQueue().getPieceQueue().at(0))); // Spawn new random piece
    }
    else
    {
        int temp = hold_piece_index;
        hold_piece_index = static_cast<int>(current_index);
        game_board.setPiece(static_cast<PieceType>(temp));
    }
}

// void Board::tickGravity()
// {
//     tetromino.setTetrominoCellState(CellState::EMPTY);

//     std::array<Position, 4> projected_position = project_movement(Movement_direction::DOWN);

//     if (check_should_set_piece(projected_position))
//     {
//         piece_was_set();
//         return;
//     }

//     for (int i = 0; i < active_tetromino.pieces_positions.size(); i++)
//     {
//         active_tetromino.pieces_positions.at(i) = projected_position.at(i);
//     }

//     set_tetromino_CellState(CellState::ACTIVE);
// }
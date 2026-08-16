
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

#pragma region GETTERS

int Game::getId() const
{
    return id;
}

int Game::getScore()
{
    return score;
}

PieceQueue &Game::getPieceQueue()
{
    return piece_queue;
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

    int counter = 0;

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
    game_board.setTetrominoCellsStates(CellState::FILLED, game_board.getBoardTetromino().getAllPositions());
    if (!game_board.setPiece(static_cast<PieceType>(piece_queue.getPieceQueue().at(0))))
    {
        lost = true;
        return 0;
    }

    piece_set = true;
    hold_used = false;
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
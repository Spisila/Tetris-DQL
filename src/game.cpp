
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

// void Game::step()
// {

//     switch (next_action)
//     {
//     case Actions::MOVE_RIGHT:
//         move_tetromino(Movement_direction::RIGHT);
//         break;
//     case Actions::MOVE_LEFT:
//         move_tetromino(Movement_direction::LEFT);
//         break;
//     case Actions::SOFT_DROP:
//         move_tetromino(Movement_direction::DOWN);
//         break;
//     case Actions::ROTATE_CLOCKWISE:
//         rotate_tetromino(Rotation::CLOCKWISE);
//         break;
//     case Actions::ROTATE_COUNTER_CLOCKWISE:
//         rotate_tetromino(Rotation::COUNTER_CLOCKWISE);
//         break;
//     case Actions::HARD_DROP:
//         hard_drop();
//         break;
//     case Actions::HOLD:
//         if (hold_used == false)
//         {
//             hold_used = true;
//             hold_current_piece();
//         }
//         break;
//     default:
//         break;
//     }
// }

// void Game::reset()
// {
//     lost = false;

//     for (int x = 0; x < board.size(); x++)
//     {
//         for (int y = 0; y < board.at(x).size(); y++)
//         {
//             board.at(x).at(y) = Cell_state::EMPTY;
//         }
//     }

//     generate_piece_queue();
//     update_piece_queue();

//     set_piece(Piece_type::J_PIECE_0);
// }

// StepData Game::calculate_reward()
// {

//     if (piece_set == true)
//     {
//         piece_set = false;

//         int lines = clear_lines();
//         int total_height = get_aggregate_height();
//         int total_holes = get_amount_of_holes();
//         int rugosity = get_rugosity();

//         std::array<Position, 4> spawn_positions = {Position(5, 1), Position(0, 0), Position(0, 0), Position(0, 0)};

//         if (check_collision(spawn_positions))
//         {
//             StepData loss_step = {LOSS_SCORE_WEIGHT, true, true};
//             return loss_step;
//         }

//         float reward = (lines * LINES_CLEARED_WEIGHT) - (total_height * TOTAL_HEIGHT_WEIGHT) - (total_holes * HOLES_WEIGHT) - (rugosity * RUGOSITY_WEIGHT);

//         StepData piece_set_step = {reward, true, false};

//         return piece_set_step;
//     }

//     gravity_counter += 1;

//     if (gravity_counter >= 10)
//     {
//         tick_gravity();
//         gravity_counter = 0;
//     }

//     StepData gravity_step = {-0.005f, false, false};

//     return gravity_step;
// }

// ----------

// void Game::init_graphics()
// {
//     SetTargetFPS(60);

//     SetTraceLogLevel(LOG_WARNING);

//     InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Test");
// }

// void Game::render(std::string &generation_counter)
// {
//     PollInputEvents();

//     if (!WindowShouldClose())
//     {
//         BeginDrawing();
//         ClearBackground(BACKGROUND_COLOR);

//         DrawText(TextFormat("Generation = %s", generation_counter), SCREEN_WIDTH / 2 + 400, SCREEN_HEIGHT / 2 - 400, 30, RED);

//         for (int x = 0; x < board.size(); x++)
//         {
//             for (int y = 0; y < board.at(x).size(); y++)
//             {
//                 Color cell_color;

//                 if (y < 4)
//                 {
//                     cell_color = SPAWN_COLOR;
//                 }
//                 else
//                 {
//                     cell_color = BOARD_COLOR;
//                 }

//                 if (board.at(x).at(y) == Cell_state::FILLED)
//                 {
//                     cell_color = FILLED_COLOR;
//                 }

//                 if (board.at(x).at(y) == Cell_state::ACTIVE)
//                 {
//                     int active_piece = static_cast<int>(active_tetromino.current_type);

//                     if (active_piece < 4)
//                         cell_color = SKYBLUE;
//                     else if (active_piece >= 4 && active_piece < 8)
//                         cell_color = DARKBLUE;
//                     else if (active_piece >= 8 && active_piece < 12)
//                         cell_color = ORANGE;
//                     else if (active_piece >= 12 && active_piece < 16)
//                         cell_color = YELLOW;
//                     else if (active_piece >= 16 && active_piece < 20)
//                         cell_color = GREEN;
//                     else if (active_piece >= 20 && active_piece < 24)
//                         cell_color = PURPLE;
//                     else if (active_piece >= 24 && active_piece < 28)
//                         cell_color = RED;
//                 }

//                 DrawRectangle(OFFSET_X + x * CELL_SIZE, OFFSET_Y + y * CELL_SIZE, CELL_SIZE - 1, CELL_SIZE - 1, cell_color);
//             }
//         }

//         EndDrawing();
//     }
// }

// void Game::close_graphics()
// {
//     CloseWindow();
// }

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

    const PieceQueue &current_queue = getPieceQueue();
    const auto &piece_queue = current_queue.getPieceQueue();

    for (int piece : piece_queue)
    {
        state.push_back(formatPiece(piece_queue[piece]));
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
        game_board.setTetrominoCellsStates(Cell_state::EMPTY, t.getAllPositions());
        auto projected_position = t.projectMovement(MovementDirection::DOWN);

        if (game_board.checkShouldSetPiece(projected_position))
        {
            pieceWasSet();
            break;
        }

        t.setPositions(projected_position);
        game_board.setTetrominoCellsStates(Cell_state::ACTIVE, projected_position);
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
    game_board.setTetrominoCellsStates(Cell_state::FILLED, game_board.getBoardTetromino().getAllPositions());
    game_board.setPiece(static_cast<PieceType>(piece_queue.getPieceQueue().at(0)));
    piece_queue.updatePieceQueue();
    return game_board.clearLines();
}

void Game::holdCurrentPiece()
{
    game_board.setTetrominoCellsStates(Cell_state::EMPTY, game_board.getBoardTetromino().getAllPositions());

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

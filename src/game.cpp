
#include <iostream>
#include <algorithm>
#include <random>

#include <raylib.h>

// #include <pybind11/pybind11.h>
// #include <pybind11/stl.h>

// namespace py = pybind11;

#include <game.hpp>

/* TODO:
Implement wall kicks
*/

Game::Game()
{

    for (int x = 0; x < board.size(); x++)
    {
        for (int y = 0; y < board.at(x).size(); y++)
        {
            board.at(x).at(y) = Cell_state::EMPTY;
        }
    }
}

Game::~Game()
{
}

class DataContainer
{
public:
    DataContainer() {}

    int return_data()
    {
        return 5;
    }

private:
};

// PYBIND11_MODULE(agent_data, ag)
// {
//     ag.doc() = "Agent data";
//     py::class_<DataContainer>(ag, "DataContainer")
//         .def(py::init<>())
//         .def("return_data", &DataContainer::return_data, "PEPE");
// }

std::array<int, BOARD_SIZE_X> Game::get_board_height()
{

    std::array<int, BOARD_SIZE_X> heights{};

    for (int x = 0; x < board.size(); x++)
    {
        for (int y = 0; y < board.at(x).size(); y++)
        {
            if (board.at(x).at(y) == Cell_state::FILLED)
            {
                heights.at(x)++;
            }
        }
    }

    return heights;
}

int Game::get_current_peice_type()
{

    return static_cast<int>(active_tetromino.current_type);
}

std::array<Position, 4> Game::get_active_tetromino_pieces_positions()
{
    return active_tetromino.pieces_positions;
}

std::array<int, 5> Game::get_piece_queue()
{
    std::array<int, 5> int_queue{};

    for (int i = 0; i < 5; i++)
    {
        int_queue.at(i) = static_cast<int>(known_piece_queue.at(i));
    }

    return int_queue;
}

void Game::set_tetromino_cell_state(Cell_state state)
{
    for (int i = 0; i < active_tetromino.pieces_positions.size(); i++)
    {
        board[active_tetromino.pieces_positions.at(i).x][active_tetromino.pieces_positions.at(i).y] = state;
    }
}

void Game::set_piece(Piece_type new_piece)
{

    active_tetromino.pieces_positions.at(0) = Position(5, 1);

    std::array<Position, 3> other_pieces_positions = tetromino_type_rotation_to_piece_positions.at(new_piece);

    std::array<Position, 4> spawn_positions = {Position(5, 1), Position(0, 0), Position(0, 0), Position(0, 0)};

    for (int j = 0; j < other_pieces_positions.size(); j++)
    {
        spawn_positions.at(j + 1).x = spawn_positions.at(0).x + other_pieces_positions.at(j).x;
        spawn_positions.at(j + 1).y = spawn_positions.at(0).y + other_pieces_positions.at(j).y;
    }

    if (check_collision(spawn_positions))
    {
        std::cout << "GAME OVER!" << std::endl;
        // exit(0);
        return;
    }

    for (int j = 0; j < other_pieces_positions.size(); j++)
    {
        active_tetromino.pieces_positions.at(j + 1).x = active_tetromino.pieces_positions.at(0).x + other_pieces_positions.at(j).x;
        active_tetromino.pieces_positions.at(j + 1).y = active_tetromino.pieces_positions.at(0).y + other_pieces_positions.at(j).y;
    }

    active_tetromino.current_type = new_piece;

    set_tetromino_cell_state(Cell_state::ACTIVE);
}

std::array<Position, 4> Game::project_movement(Movement_direction dir)
{

    std::array<Position, 4> projected_position{Position(0, 0), Position(0, 0), Position(0, 0), Position(0, 0)};

    // 0 is the pivot of the projected tetromino
    projected_position.at(0) = active_tetromino.pieces_positions.at(0);

    switch (dir)
    {
    case Movement_direction::UP:
        projected_position.at(0).y--;
        break;
    case Movement_direction::DOWN:
        projected_position.at(0).y++;
        break;
    case Movement_direction::LEFT:
        projected_position.at(0).x--;
        break;
    case Movement_direction::RIGHT:
        projected_position.at(0).x++;
        break;
    case Movement_direction::TOP_RIGHT:
        projected_position.at(0).x++;
        projected_position.at(0).y--;
        break;
    case Movement_direction::TOP_LEFT:
        projected_position.at(0).x--;
        projected_position.at(0).y--;
        break;
    case Movement_direction::BOTTOM_RIGHT:
        projected_position.at(0).x++;
        projected_position.at(0).y++;
        break;
    case Movement_direction::BOTTOM_LEFT:
        projected_position.at(0).x--;
        projected_position.at(0).y++;
        break;

    default:
        break;
    }

    std::array<Position, 3> pos = tetromino_type_rotation_to_piece_positions.at(active_tetromino.current_type);

    for (int j = 0; j < pos.size(); j++)
    {
        projected_position.at(j + 1).x = projected_position.at(0).x + pos.at(j).x;
        projected_position.at(j + 1).y = projected_position.at(0).y + pos.at(j).y;
    }

    return projected_position;
}

void Game::move_tetromino(Movement_direction dir)
{

    std::array<Position, 4> projected_position = project_movement(dir);

    if (check_out_of_bounds(projected_position) || check_collision(projected_position))
    {
        return;
    }

    set_tetromino_cell_state(Cell_state::EMPTY);

    for (int i = 0; i < active_tetromino.pieces_positions.size(); i++)
    {
        active_tetromino.pieces_positions.at(i) = projected_position.at(i);
    }
    set_tetromino_cell_state(Cell_state::ACTIVE);
}
// index + (current +/- 1) mod(4)
Piece_type Game::change_tetrominoe_rotation(Rotation rot)
{

    int current_index = static_cast<int>(active_tetromino.current_type);
    int general_index;

    if (current_index < 4)
        general_index = 0;
    else if (current_index >= 4 && current_index < 8)
        general_index = 4;
    else if (current_index >= 8 && current_index < 12)
        general_index = 8;
    else if (current_index >= 12 && current_index < 16)
        general_index = 12;
    else if (current_index >= 16 && current_index < 20)
        general_index = 16;
    else if (current_index >= 20 && current_index < 24)
        general_index = 20;
    else if (current_index >= 24 && current_index < 28)
        general_index = 24;

    int step;

    switch (rot)
    {
    case Rotation::CLOCKWISE:
        step = 1;
        break;
    case Rotation::COUNTER_CLOCKWISE:
        step = -1;
        break;
    case Rotation::ONE_EIGHTY:
        step = 2;
        break;
    default:
        break;
    } 

    int new_rotated_piece = general_index + ((current_index + step) % 4);

    if (new_rotated_piece < 0)
        new_rotated_piece += 4;

    return static_cast<Piece_type>(new_rotated_piece);
}

std::array<Position, 4> Game::project_rotation(Rotation rot)
{

    std::array<Position, 4> projected_rotation{Position(0, 0), Position(0, 0), Position(0, 0), Position(0, 0)};

    Piece_type new_rotation = change_tetrominoe_rotation(rot);

    projected_rotation.at(0) = active_tetromino.pieces_positions.at(0);

    std::array<Position, 3> pos = tetromino_type_rotation_to_piece_positions.at(new_rotation);

    for (int i = 0; i < pos.size(); i++)
    {
        projected_rotation.at(i + 1).x = projected_rotation.at(0).x + pos.at(i).x;
        projected_rotation.at(i + 1).y = projected_rotation.at(0).y + pos.at(i).y;
    }

    return projected_rotation;
}

void Game::rotate_tetromino(Rotation rot)
{

    std::array<Position, 4> projected_rotation = project_rotation(rot);

    if (check_out_of_bounds(projected_rotation) || check_collision(projected_rotation))
    {
        return;
    }

    set_tetromino_cell_state(Cell_state::EMPTY);

    for (int i = 0; i < active_tetromino.pieces_positions.size(); i++)
    {
        active_tetromino.pieces_positions.at(i) = projected_rotation.at(i);
    }

    active_tetromino.current_type = change_tetrominoe_rotation(rot);

    set_tetromino_cell_state(Cell_state::ACTIVE);
}

bool Game::check_collision(std::array<Position, 4> projected_positions)
{

    for (int i = 0; i < 4; i++)
    {
        if (board[projected_positions.at(i).x][projected_positions.at(i).y] == Cell_state::FILLED)
        {
            return true;
        }
    }

    return false;
}

bool Game::check_out_of_bounds(std::array<Position, 4> projected_positions)
{

    for (int i = 0; i < 4; i++)
    {
        if (projected_positions.at(i).x < 0 || projected_positions.at(i).x >= BOARD_SIZE_X || projected_positions.at(i).y < 0 || projected_positions.at(i).y >= BOARD_SIZE_Y)
        {
            return true;
        }
    }

    return false;
}

bool Game::check_touched_floor(std::array<Position, 4> projected_positions)
{

    for (int i = 0; i < 4; i++)
    {
        if (projected_positions.at(i).y >= BOARD_SIZE_Y)
        {
            return true;
        }
    }

    return false;
}

void Game::tick_gravity()
{
    if (gravity_counter >= GRAVITY_TICKS)
    {
        set_tetromino_cell_state(Cell_state::EMPTY);

        std::array<Position, 4> projected_position = project_movement(Movement_direction::DOWN);

        if (check_touched_floor(projected_position) || check_collision(projected_position))
        {
            set_tetromino_cell_state(Cell_state::FILLED);
            set_piece(piece_queue.at(queue_index));
            update_piece_queue();
            hold_used = false;
            return;
        }

        for (int i = 0; i < active_tetromino.pieces_positions.size(); i++)
        {
            active_tetromino.pieces_positions.at(i) = projected_position.at(i);
        }
        gravity_counter = 0;

        set_tetromino_cell_state(Cell_state::ACTIVE);
        return;
    }

    gravity_counter++;
}

void Game::hard_drop()
{
    while (true)
    {
        std::array<Position, 4> projected_position = project_movement(Movement_direction::DOWN);

        if (check_touched_floor(projected_position) || check_collision(projected_position))
        {
            set_tetromino_cell_state(Cell_state::FILLED);
            set_piece(piece_queue.at(queue_index));
            update_piece_queue();
            hold_used = false;
            return;
        }

        set_tetromino_cell_state(Cell_state::EMPTY);

        for (int i = 0; i < active_tetromino.pieces_positions.size(); i++)
        {
            active_tetromino.pieces_positions.at(i) = projected_position.at(i);
        }

        set_tetromino_cell_state(Cell_state::ACTIVE);
    }
}

int Game::clear_lines()
{
    int cleared_lines = 0;

    // Loop through all lines and count filled cells
    for (int line = 0; line < BOARD_SIZE_Y; line++)
    {
        int filled_count = 0;

        // Checa se a linha esta cheia
        for (int x = 0; x < BOARD_SIZE_X; x++)
        {

            if (board[x][line] == Cell_state::FILLED)
            {
                filled_count++;
            }
        }

        // If line is full, clear it and move everything above down

        if (filled_count == BOARD_SIZE_X)
        {
            cleared_lines++;
            int cleared_line = line;

            for (int line_above = line - 1; line_above > 0; line_above--)
            {
                for (int cell = 0; cell < BOARD_SIZE_X; cell++)
                {

                    if (board[cell][line_above] != Cell_state::ACTIVE)
                    {
                        board[cell][cleared_line] = board[cell][line_above];
                    }
                }
                cleared_line = line_above;
            }
        }
    }

    return cleared_lines;
}

void Game::hold_current_piece()
{
    set_tetromino_cell_state(Cell_state::EMPTY);

    int current_index = static_cast<int>(active_tetromino.current_type);
    int general_index;

    if (current_index < 4)
        general_index = 0;
    else if (current_index >= 4 && current_index < 8)
        general_index = 4;
    else if (current_index >= 8 && current_index < 12)
        general_index = 8;
    else if (current_index >= 12 && current_index < 16)
        general_index = 12;
    else if (current_index >= 16 && current_index < 20)
        general_index = 16;
    else if (current_index >= 20 && current_index < 24)
        general_index = 20;
    else if (current_index >= 24 && current_index < 28)
        general_index = 24;

    if (hold_piece_index == -1)
    {
        hold_piece_index = static_cast<int>(general_index);
        set_piece(static_cast<Piece_type>(rand() % 7 * 4)); // Spawn new random piece
    }
    else
    {
        int temp = hold_piece_index;
        hold_piece_index = static_cast<int>(general_index);
        set_piece(static_cast<Piece_type>(temp));
    }
}

void Game::generate_piece_queue()
{
    std::array<Piece_type, 7> bag = {
        Piece_type::I_PIECE_0,
        Piece_type::J_PIECE_0,
        Piece_type::L_PIECE_0,
        Piece_type::O_PIECE_0,
        Piece_type::S_PIECE_0,
        Piece_type::T_PIECE_0,
        Piece_type::Z_PIECE_0};

    std::shuffle(bag.begin(), bag.end(), gen);

    std::array<Piece_type, 14> new_queue;

    for (int i = 0; i < 7; i++)
    {
        new_queue.at(i) = bag.at(i);
    }

    std::shuffle(bag.begin(), bag.end(), gen);

    for (int i = 7; i < 14; i++)
    {
        new_queue.at(i) = bag.at(i - 7);
    }

    for (int i = 0; i < 14; i++)
    {
        piece_queue.at(i) = new_queue.at(i);
    }
}

void Game::update_piece_queue()
{
    std::array<Piece_type, 7> bag = {
        Piece_type::I_PIECE_0,
        Piece_type::J_PIECE_0,
        Piece_type::L_PIECE_0,
        Piece_type::O_PIECE_0,
        Piece_type::S_PIECE_0,
        Piece_type::T_PIECE_0,
        Piece_type::Z_PIECE_0};

    if (queue_index + 7 == piece_queue.size() + 1)
    {

        std::shuffle(bag.begin(), bag.end(), gen);

        for (int i = 0; i < 7; i++)
        {
            piece_queue.at(i) = bag.at(i);
        }
    }

    if (queue_index >= piece_queue.size() - 1)
    {
        std::shuffle(bag.begin(), bag.end(), gen);

        for (int i = 7; i < 14; i++)
        {
            piece_queue.at(i) = bag.at(i - 7);
        }

        queue_index = 0;
    }

    if (queue_index + 5 < 14)
    {
        known_piece_queue[0] = piece_queue[queue_index + 1];
        known_piece_queue[1] = piece_queue[queue_index + 2];
        known_piece_queue[2] = piece_queue[queue_index + 3];
        known_piece_queue[3] = piece_queue[queue_index + 4];
        known_piece_queue[4] = piece_queue[queue_index + 5];
    }
    else if (queue_index + 5 == 14)
    {
        known_piece_queue[0] = piece_queue[queue_index + 1];
        known_piece_queue[1] = piece_queue[queue_index + 2];
        known_piece_queue[2] = piece_queue[queue_index + 3];
        known_piece_queue[3] = piece_queue[queue_index + 4];
        known_piece_queue[4] = piece_queue[0];
    }
    else if (queue_index + 5 == 15)
    {
        known_piece_queue[0] = piece_queue[queue_index + 1];
        known_piece_queue[1] = piece_queue[queue_index + 2];
        known_piece_queue[2] = piece_queue[queue_index + 3];
        known_piece_queue[3] = piece_queue[0];
        known_piece_queue[4] = piece_queue[1];
    }
    else if (queue_index + 5 == 16)
    {
        known_piece_queue[0] = piece_queue[queue_index + 1];
        known_piece_queue[1] = piece_queue[queue_index + 2];
        known_piece_queue[2] = piece_queue[0];
        known_piece_queue[3] = piece_queue[1];
        known_piece_queue[4] = piece_queue[2];
    }
    else if (queue_index + 5 == 17)
    {
        known_piece_queue[0] = piece_queue[queue_index + 1];
        known_piece_queue[1] = piece_queue[0];
        known_piece_queue[2] = piece_queue[1];
        known_piece_queue[3] = piece_queue[2];
        known_piece_queue[4] = piece_queue[3];
    }

    queue_index++;
}
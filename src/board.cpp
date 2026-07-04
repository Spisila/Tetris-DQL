#include <board.hpp>
#include <shared.hpp>
#include <iostream>

Board::Board(/* args */)
{

    // initialize the board as empty
    for (int x = 0; x < board.size(); x++)
    {
        for (int y = 0; y < board.at(x).size(); y++)
        {
            board.at(x).at(y) = Cell_state::EMPTY;
        }
    }
}

Board::~Board()
{
}

std::array<std::array<Cell_state, BOARD_SIZE_Y>, BOARD_SIZE_X> Board::getBoardCells() const
{
    return board;
}

std::array<int, BOARD_SIZE_X> Board::getBoardHeight() const
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

int Board::getAggregateHeight() const
{

    auto columns_heights = getBoardHeight();

    int total_height = 0;

    for (int i = 0; i < columns_heights.size(); i++)
    {
        total_height += columns_heights.at(i);
    }

    return total_height;
}

int Board::getAmountOfHoles() const
{

    int holes = 0;

    for (int x = 0; x < board.size(); x++)
    {
        for (int y = 0; y < board.at(x).size(); y++)
        {

            // If is the first line there can be no empty cell above it
            if (y == 0)
            {
                continue;
            }

            auto cell_i = board.at(x).at(y);
            auto cell_above = board.at(x).at(y - 1);

            if (cell_i == Cell_state::EMPTY)
            {

                if (cell_above == Cell_state::FILLED)
                {
                    holes++;
                }
            }
        }
    }

    return holes;
}

int Board::getRugosity() const
{

    auto column_heights = getBoardHeight();

    int rugosity = 0;

    for (int i = 0; i < column_heights.size() - 1; i++)
    {

        int difference = 0;

        difference = column_heights.at(i) - column_heights.at(i + 1);

        rugosity += std::abs(difference);
    }

    return rugosity;
}

const std::vector<int> Board::getBoardState() const
{

    int agg_height = getAggregateHeight();
    int holes = getAmountOfHoles();
    int rugosity = getRugosity();

    std::vector<int> data_basic = {agg_height, holes, rugosity};

    return data_basic;
}

const Tetromino &Board::getBoardTetromino() const
{

    return tetromino;
}

Tetromino &Board::getBoardTetromino()
{
    return tetromino;
}

void Board::setTetrominoCellsStates(Cell_state state, std::array<Position, 4> positions)
{
    for (Position pos : positions)
    {
        board[pos.x][pos.y] = state;
    }
}

void Board::moveTetromino(MovementDirection dir)
{
    std::array<Position, 4> projected_position = tetromino.projectMovement(dir);

    // Todo: Fix collision checking not letting move, piece not updating visuals
    if (checkOutOfLateralBounds(projected_position) || checkCollisions(projected_position))
    {
        return;
    }

    setTetrominoCellsStates(Cell_state::EMPTY, tetromino.getAllPositions());

    tetromino.setPositions(projected_position);

    setTetrominoCellsStates(Cell_state::ACTIVE, projected_position);
}

void Board::rotateTetromino(Rotation rot)
{

    std::array<Position, 4> projected_rotation = tetromino.projectRotation(rot);

    if (checkOutOfLateralBounds(projected_rotation) || checkCollisions(projected_rotation))
    {
        return;
    }

    setTetrominoCellsStates(Cell_state::EMPTY, tetromino.getAllPositions());

    tetromino.setPositions(projected_rotation);

    tetromino.setTetrominoType(tetromino.changeTetrominoeRotation(rot));

    setTetrominoCellsStates(Cell_state::ACTIVE, projected_rotation);
}

void Board::setPiece(PieceType new_piece)
{

    // std::array<Position, 4> positions = tetromino.getAllPositions();
    // std::cout << positions[1].x << std::endl;
    // Position pivot_position = positions[0];

    // std::array<Position, 3> other_pieces_positions = {positions[1], positions[2], positions[3]};

    auto new_piece_offsets = rotationToPositionOffsets.at(new_piece);

    Position new_pivot_position = spawn_positions[0];

    std::array<Position, 4> new_piece_positions = {
        new_pivot_position,
        new_pivot_position,
        new_pivot_position,
        new_pivot_position};

    // Sets the other pieces around the spawn pivot position
    for (int j = 0; j < new_piece_positions.size(); j++)
    {
        new_piece_positions[j + 1].x += new_piece_offsets[j].x;
        new_piece_positions[j + 1].y += new_piece_offsets[j].y;
    }

    // TODO: Needs to set game lost
    if (checkCollisions(spawn_positions))
    {
        // std::cout << "GAME OVER!" << std::endl;
        // exit(0);
        // lost = true;
        return;
    }

    // for (int j = 0; j < other_pieces_positions.size(); j++)
    // {
    //     active_tetromino.pieces_positions.at(j + 1).x = active_tetromino.pieces_positions.at(0).x + other_pieces_positions.at(j).x;
    //     active_tetromino.pieces_positions.at(j + 1).y = active_tetromino.pieces_positions.at(0).y + other_pieces_positions.at(j).y;
    // }

    tetromino.setPositions(new_piece_positions);

    tetromino.setTetrominoType(new_piece);

    setTetrominoCellsStates(Cell_state::ACTIVE, tetromino.getAllPositions());
}

bool Board::checkCollisions(std::array<Position, 4> projected_positions)
{

    for (Position pos : projected_positions)
    {
        if (board[pos.x][pos.y] == Cell_state::FILLED)
        {
            return true;
        }
    }

    return false;
}

bool Board::checkOutOfLateralBounds(std::array<Position, 4> projected_positions)
{

    for (Position pos : projected_positions)
    {
        if (pos.x < 0 || pos.x >= BOARD_SIZE_X || pos.y < 0 || pos.y >= BOARD_SIZE_Y)
        {
            return true;
        }
    }

    return false;
}

bool Board::checkTouchedFloor(std::array<Position, 4> projected_positions)
{

    for (Position pos : projected_positions)
    {
        if (pos.y >= BOARD_SIZE_Y)
        {
            return true;
        }
    }

    return false;
}

bool Board::checkShouldSetPiece(std::array<Position, 4> projected_position)
{
    bool test_floor = checkTouchedFloor(projected_position);
    bool test_collision = checkCollisions(projected_position);

    return test_floor || test_collision;
}

// void Board::tickGravity()
// {
//     tetromino.setTetrominoCellState(Cell_state::EMPTY);

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

//     set_tetromino_cell_state(Cell_state::ACTIVE);
// }

int Board::clearLines()
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
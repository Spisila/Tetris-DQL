#include <gtest/gtest.h>
#include <board.hpp>

TEST(BoardTests, checkOutOfLateralBounds_invalidPositions)
{
    Board b{};

    std::array<Position, 4> out_of_bounds_left{
        Position{-1, 5},
        Position{1, 5},
        Position{2, 4},
        Position{3, 4}};

    std::array<Position, 4> out_of_bounds_right{
        Position{BOARD_SIZE_X + 1, 15},
        Position{BOARD_SIZE_X + 2, 10},
        Position{8, 8},
        Position{7, 7}};

    std::array<Position, 4> out_of_bounds_down{
        Position{7, BOARD_SIZE_Y + 1},
        Position{6, BOARD_SIZE_Y + 1},
        Position{8, 15},
        Position{7, 15}};

    EXPECT_TRUE(b.checkOutOfLateralBounds(out_of_bounds_left));
    EXPECT_TRUE(b.checkOutOfLateralBounds(out_of_bounds_right));
    EXPECT_TRUE(b.checkOutOfLateralBounds(out_of_bounds_down));
}

TEST(BoardTests, checkOutOfLateralBounds_validPositions)
{

    Board b{};

    std::array<Position, 4> in_bounds_pos{
        Position{2, 1},
        Position{3, 2},
        Position{4, 3},
        Position{5, 4}};

    EXPECT_FALSE(b.checkOutOfLateralBounds(in_bounds_pos));
}

TEST(BoardTests, checkCollisions_validPositions)
{

    Board b{};

    std::array<Position, 4> in_bounds_pos{
        Position{2, 1},
        Position{3, 2},
        Position{4, 3},
        Position{5, 4}};

    for (Position pos : in_bounds_pos)
    {
        b.getBoardCells()[pos.x][pos.y] = CellState::EMPTY;
    }

    EXPECT_FALSE(b.checkOutOfLateralBounds(in_bounds_pos));
}

TEST(BoardTests, checkCollisions_invalidPositions)
{

    Board b{};

    std::array<Position, 4> in_bounds_pos{
        Position{2, 1},
        Position{3, 2},
        Position{4, 3},
        Position{5, 4}};

    b.getBoardCells()[in_bounds_pos[0].x][in_bounds_pos[0].y] = CellState::FILLED;

    EXPECT_FALSE(b.checkOutOfLateralBounds(in_bounds_pos));
}

TEST(BoardTests, chechTouchedFloor_validPositions)
{

    Board b{};

    std::array<Position, 4> in_bounds_pos{
        Position{2, 1},
        Position{3, 2},
        Position{4, 3},
        Position{5, 4}};

    EXPECT_FALSE(b.checkTouchedFloor(in_bounds_pos));
}

TEST(BoardTests, chechTouchedFloor_invalidPositions)
{

    Board b{};

    std::array<Position, 4> out_of_bounds_pos{
        Position{2, BOARD_SIZE_Y + 1},
        Position{3, 2},
        Position{4, 3},
        Position{5, 4}};

    EXPECT_TRUE(b.checkTouchedFloor(out_of_bounds_pos));
}
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "game.hpp"

namespace py = pybind11;

PYBIND11_MODULE(Tetris_AGENT, m)
{
    m.doc() = "Tetris module";

    py::enum_<Cell_state>(m, "CellState")
        .value("EMPTY", Cell_state::EMPTY)
        .value("SPAWN", Cell_state::SPAWN)
        .value("ACTIVE", Cell_state::ACTIVE)
        .value("FILLED", Cell_state::FILLED)
        .export_values();

    py::enum_<Actions>(m, "Actions")
        .value("MOVE_LEFT", Actions::MOVE_LEFT)
        .value("MOVE_RIGHT", Actions::MOVE_RIGHT)
        .value("SOFT_DROP", Actions::SOFT_DROP)
        .value("ROTATE_CLOCKWISE", Actions::ROTATE_CLOCKWISE)
        .value("ROTATE_COUNTER_CLOCKWISE", Actions::ROTATE_COUNTER_CLOCKWISE)
        .value("HOLD", Actions::HOLD)
        .value("HARD_DROP", Actions::HARD_DROP)
        .export_values();

    py::class_<Game>(m, "Game")
        .def(py::init<>())

        .def_readwrite("lost", &Game::lost, "Boolean to check if game is lost")
        .def_readwrite("piece_set", &Game::piece_set, "Boolean to check if piece was set")

        .def("init_graphics", &Game::init_graphics, "Initializes window")
        .def("render", &Game::render, "Renders the board")
        .def("close_graphics", &Game::close_graphics, "Closes window")

        .def("step", &Game::step, "Step and returns reward")

        // TODO: Implement reset method
        .def("reset", &Game::reset, "Restarts environment")

        .def("get_board_height", &Game::get_board_height, "Returns all the heights")
        .def("get_aggregate_height", &Game::get_aggregate_height, "Returns the sum of the height of all columns")
        .def("get_amount_of_holes", &Game::get_amount_of_holes, "Returns the amount of holes in the board")
        .def("get_rugosity", &Game::get_rugosity, "Returns the rugosity of the board")
        .def("get_current_peice_type", &Game::get_current_peice_type, "Returns the current piece being placed")
        .def("get_piece_queue", &Game::get_piece_queue, "Returns the queue of pieces")
        .def("get_score", &Game::get_score, "Returns the scroe");

    // .def("get_board", &Game::get_board, "Return entire board");
}
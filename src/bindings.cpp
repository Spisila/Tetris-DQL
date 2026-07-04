#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "game.hpp"

#include <board.hpp>
#include <shared.hpp>

#include <MultiGame.hpp>

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

    py::class_<StepData>(m, "StepData")
        .def_readwrite("reward", &StepData::reward)
        .def_readwrite("piece_placed", &StepData::piece_placed)
        .def_readwrite("lost", &StepData::lost);

    py::class_<MultiGame>(m, "MultiGame")
        .def(py::init<int>())

        // .def("init_graphics", &Game::init_graphics, "Initializes window")
        // .def("render", &Game::render, "Renders the board")
        // .def("close_graphics", &Game::close_graphics, "Closes window")

        .def("stepAll", &MultiGame::stepAll, "Runs a step on all the enviroments")

        .def("resetAll", &MultiGame::resetAll, "Restarts all of the environments")
        .def("resetThis", &MultiGame::resetThis, "Reset a specific environment")

        .def("getSumScore", &MultiGame::getSumScore, "Sum score")

        .def("getStates", &MultiGame::getStates, "Gets the state of all the environments");

}

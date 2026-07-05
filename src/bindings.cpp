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

    py::enum_<CellState>(m, "CellState")
        .value("EMPTY", CellState::EMPTY)
        .value("SPAWN", CellState::SPAWN)
        .value("ACTIVE", CellState::ACTIVE)
        .value("FILLED", CellState::FILLED)
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

        .def("initGraphics", &MultiGame::initGraphics, "Initializes window")
        .def("render", &MultiGame::render, "Renders the board")
        .def("closeGraphics", &MultiGame::closeGraphics, "Closes window")

        .def("stepAll", &MultiGame::stepAll, "Runs a step on all the enviroments")

        .def("resetAll", &MultiGame::resetAll, "Restarts all of the environments")
        .def("resetThis", &MultiGame::resetThis, "Reset a specific environment")

        .def("getLinesCleared", &MultiGame::getLinesCleared, "Lines cleared")

        .def("getStates", &MultiGame::getStates, "Gets the state of all the environments");
}

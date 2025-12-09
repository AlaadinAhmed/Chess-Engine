#pragma once

#include "position.hpp"
#include "history.hpp"

class MovePicker {
public:
    MovePicker(Position &pos, Move tt_move, History &history, int ply, Move prev_move);
    Move next_move();

private:
    enum Stage {
        TT_MOVE,
        GOOD_CAPTURES,
        KILLER_MOVES,
        QUIET_MOVES,
        BAD_CAPTURES
    };

    void score_captures();

    Position &pos;
    Move tt_move;
    History &history;
    int ply;
    Stage stage;
    MoveList moves;
    MoveList bad_captures;
    int move_index;
    Move prev_move;
};

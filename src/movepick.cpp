#include "movepick.hpp"
#include "movegen.hpp"
#include "utils.hpp"
#include <algorithm>

MovePicker::MovePicker(Position &pos, Move tt_move, History &history, int ply)
    : pos(pos), tt_move(tt_move), history(history), ply(ply), stage(TT_MOVE), move_index(0) {
    
    generate_moves(pos, moves);
    for (int i = 0; i < moves.count; i++) {
        if (moves.moves[i].from == tt_move.from && moves.moves[i].to == tt_move.to) {
            moves.moves[i].score = 1000000;
        } else if (get_piece_at(pos, moves.moves[i].to) != NO_PIECE) {
            moves.moves[i].score = see(pos, moves.moves[i]) + 100000;
        } else if (history.is_killer(moves.moves[i], ply)) {
            moves.moves[i].score = 50000;
        } else {
            moves.moves[i].score = history.history_scores[get_piece_at(pos, moves.moves[i].from)][moves.moves[i].to];
        }
    }
}

Move MovePicker::next_move() {
    if (stage == TT_MOVE) {
        stage = QUIET_MOVES; // a single stage for all other moves
        if (tt_move.from != 0 || tt_move.to != 0) {
            // make sure tt_move is legal
            for(int i=0; i<moves.count; ++i) {
                if (moves.moves[i].from == tt_move.from && moves.moves[i].to == tt_move.to) {
                    return tt_move;
                }
            }
        }
    }

    if (move_index >= moves.count) {
        return {};
    }

    int best_idx = move_index;
    for (int i = move_index + 1; i < moves.count; i++) {
        if (moves.moves[i].score > moves.moves[best_idx].score) {
            best_idx = i;
        }
    }

    Move tmp = moves.moves[move_index];
    moves.moves[move_index] = moves.moves[best_idx];
    moves.moves[best_idx] = tmp;

    // don't return tt_move again
    if (moves.moves[move_index].from == tt_move.from && moves.moves[move_index].to == tt_move.to) {
        move_index++;
        return next_move();
    }

    return moves.moves[move_index++];
}
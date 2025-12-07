#include "movepick.hpp"
#include "movegen.hpp"
#include "utils.hpp"
#include <algorithm>

MovePicker::MovePicker(Position &pos, Move tt_move, History &history, int ply, Move prev_move)
    : pos(pos), tt_move(tt_move), history(history), ply(ply), stage(TT_MOVE), move_index(0) {
}

void MovePicker::score_captures() {
    auto piece_value = [&](Pieces p){
        switch(p){
            case W_PAWN: case B_PAWN: return 100; 
            case W_KNIGHT: case B_KNIGHT: return 300;
            case W_BISHOP: case B_BISHOP: return 320;
            case W_ROOK: case B_ROOK: return 500;
            case W_QUEEN: case B_QUEEN: return 900;
            default: return 0;
        }
    };

    for (int i = 0; i < moves.count; i++) {
        Pieces victim = get_piece_at(pos, moves.moves[i].to);
        Pieces aggressor = get_piece_at(pos, moves.moves[i].from);
        moves.moves[i].score = piece_value(victim) - piece_value(aggressor) + 100000;
    }
}

Move MovePicker::next_move() {
    if (stage == TT_MOVE) {
        stage = GOOD_CAPTURES;
        if (tt_move.from != 0 || tt_move.to != 0) {
            if (get_piece_at(pos, tt_move.from) != NO_PIECE) {
                return tt_move;
            }
        }
    }

    if (stage == GOOD_CAPTURES) {
        if (move_index == 0) {
            moves.count = 0;
            generate_captures(pos, moves);
            score_captures();
        }

        if (move_index < moves.count) {
            int best_idx = move_index;
            for (int i = move_index + 1; i < moves.count; i++) {
                if (moves.moves[i].score > moves.moves[best_idx].score) {
                    best_idx = i;
                }
            }
            
            Move best = moves.moves[best_idx];
            moves.moves[best_idx] = moves.moves[move_index];
            moves.moves[move_index] = best;
            move_index++;

            if (best.from == tt_move.from && best.to == tt_move.to) {
                return next_move();
            }
            return best;
        }
        
        stage = KILLER_MOVES;
        move_index = 0;
    }

    if (stage == KILLER_MOVES) {
        stage = QUIET_MOVES;
    }

    if (stage == QUIET_MOVES) {
        if (move_index == 0) {
            moves.count = 0;
            generate_quiet_moves(pos, moves);
            
            for (int i = 0; i < moves.count; i++) {
                if (history.is_killer(moves.moves[i], ply)) {
                    moves.moves[i].score = 50000;
                } else {
                    moves.moves[i].score = history.history_scores[get_piece_at(pos, moves.moves[i].from)][moves.moves[i].to];
                }
            }
        }

        if (move_index < moves.count) {
            int best_idx = move_index;
            for (int i = move_index + 1; i < moves.count; i++) {
                if (moves.moves[i].score > moves.moves[best_idx].score) {
                    best_idx = i;
                }
            }
            
            Move best = moves.moves[best_idx];
            moves.moves[best_idx] = moves.moves[move_index];
            moves.moves[move_index] = best;
            move_index++;

            if (best.from == tt_move.from && best.to == tt_move.to) {
                return next_move();
            }
            return best;
        }
        
        stage = BAD_CAPTURES;
    }

    return {};
}

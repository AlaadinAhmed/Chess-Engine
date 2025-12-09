#include "movepick.hpp"
#include "movegen.hpp"
#include "utils.hpp"
#include "pst.hpp"
#include "see.hpp"
#include <algorithm>

static bool gives_check(Position& pos, Move m) {
    makemove(pos, m);
    bool opponent_is_white = pos.whiteToMove;
    uint64_t king_bb = opponent_is_white ? pos.WhiteKing : pos.BlackKing;
    int king_sq = __builtin_ctzll(king_bb);
    bool check = is_square_attacked(pos, king_sq, !opponent_is_white);
    undomove(pos, m);
    return check;
}

int get_pst_val(Pieces p, int sq) {
    switch(p) {
        case W_PAWN: return pawn_pst_mg[sq];
        case B_PAWN: return pawn_pst_mg[63^sq];
        case W_KNIGHT: return knight_pst_mg[sq];
        case B_KNIGHT: return knight_pst_mg[63^sq];
        case W_BISHOP: return bishop_pst_mg[sq];
        case B_BISHOP: return bishop_pst_mg[63^sq];
        case W_ROOK: return rook_pst_mg[sq];
        case B_ROOK: return rook_pst_mg[63^sq];
        case W_QUEEN: return 0; 
        case B_QUEEN: return 0;
        case W_KING: return king_pst_mg[sq];
        case B_KING: return king_pst_mg[63^sq];
        default: return 0;
    }
}

MovePicker::MovePicker(Position &pos, Move tt_move, History &history, int ply, Move prev_move)
    : pos(pos), tt_move(tt_move), history(history), ply(ply), stage(TT_MOVE), move_index(0), prev_move(prev_move) {
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

    int good_count = 0;
    bad_captures.count = 0;

    for (int i = 0; i < moves.count; i++) {
        Pieces victim = get_piece_at(pos, moves.moves[i].to);
        Pieces aggressor = get_piece_at(pos, moves.moves[i].from);
        int score = piece_value(victim) - piece_value(aggressor) + 100000;
        
        if (gives_check(pos, moves.moves[i])) {
            score += 10000;
        }

        moves.moves[i].score = score;

        if (see(pos, moves.moves[i]) < 0) {
            bad_captures.moves[bad_captures.count++] = moves.moves[i];
        } else {
            moves.moves[good_count++] = moves.moves[i];
        }
    }
    moves.count = good_count;
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
        move_index = 0;
    }

    if (stage == QUIET_MOVES) {
        if (move_index == 0) {
            moves.count = 0;
            generate_quiet_moves(pos, moves);
            
            Move counter_move = history.get_counter_move(prev_move);

            for (int i = 0; i < moves.count; i++) {
                int score = 0;
                if (history.is_killer(moves.moves[i], ply)) {
                    score = 50000; // Killer moves high priority
                } else {
                    score = history.history_scores[get_piece_at(pos, moves.moves[i].from)][moves.moves[i].to];
                    score += history.get_counter_move_history_score(prev_move, moves.moves[i], pos);
                    if (moves.moves[i].from == counter_move.from && moves.moves[i].to == counter_move.to) {
                        score += 10000; // Counter move bonus (high priority)
                    }
                    // Add PSQT bonus
                    Pieces p = get_piece_at(pos, moves.moves[i].from);
                    score += get_pst_val(p, moves.moves[i].to) - get_pst_val(p, moves.moves[i].from);
                    
                    // Check bonus
                    if (gives_check(pos, moves.moves[i])) {
                        score += 10000;
                    }
                }
                moves.moves[i].score = score;
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
        move_index = 0;
    }

    if (stage == BAD_CAPTURES) {
        if (move_index < bad_captures.count) {
            Move m = bad_captures.moves[move_index++];
            if (m.from == tt_move.from && m.to == tt_move.to) {
                return next_move();
            }
            return m;
        }
    }

    return {};
}

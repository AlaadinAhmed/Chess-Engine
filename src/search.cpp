#include "search.hpp"

bool searching = false;

UndoInfo history[256];
#include "eval.hpp"
#include "movegen.hpp"
#include "tt.hpp"

extern TranspositionTable tt;
int quiescence(Position &pos, int alpha, int beta) {
    int stand_pat = evaluate(pos);
    if (stand_pat >= beta) {
        return beta;
    }
    if (alpha < stand_pat) {
        alpha = stand_pat;
    }

    MoveList move_list;
    generate_captures(pos, move_list);

    for (int i = 0; i < move_list.count; i++) {
        Position next_pos = pos;
        makemove(next_pos, move_list.moves[i]);
        int score = -quiescence(next_pos, -beta, -alpha);
        if (score >= beta) {
            return beta;
        }
        if (score > alpha) {
            alpha = score;
        }
    }
    return alpha;
}

int search(Position &pos, int depth, int alpha, int beta, Move &best_move) {
    if (!searching) {
        return 0;
    }
    bool found;
    TTEntry *entry = tt.probe(pos.zobrist_key, found);
    if (found && entry->depth >= depth) {
        if (entry->flag == HASH_FLAG_EXACT) {
            best_move = entry->best_move;
            return entry->score;
        } else if (entry->flag == HASH_FLAG_ALPHA) {
            if (entry->score <= alpha) {
                best_move = entry->best_move;
                return alpha;
            }
        } else if (entry->flag == HASH_FLAG_BETA) {
            if (entry->score >= beta) {
                best_move = entry->best_move;
                return beta;
            }
        }
    }

    if (depth == 0) {
        return quiescence(pos, alpha, beta);
    }

    MoveList move_list;
    generate_moves(pos, move_list);

    int best_score = -100000;
    HashFlag flag = HASH_FLAG_ALPHA;

    for (int i = 0; i < move_list.count; i++) {
        Position next_pos = pos;
        makemove(next_pos, move_list.moves[i]);
        int score = -search(next_pos, depth - 1, -beta, -alpha, best_move);
        if (score > best_score) {
            best_score = score;
            best_move = move_list.moves[i];
        }
        if (best_score > alpha) {
            alpha = best_score;
            flag = HASH_FLAG_EXACT;
        }
        if (alpha >= beta) {
            tt.save(pos.zobrist_key, depth, HASH_FLAG_BETA, beta, best_move);
            return beta;
        }
    }

    tt.save(pos.zobrist_key, depth, flag, best_score, best_move);
    return best_score;
}
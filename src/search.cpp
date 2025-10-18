#include "position.hpp"
#include "search.hpp"
#include "utils.hpp"

bool searching = false;
long long nodes_searched = 0;
#include "eval.hpp"
#include "movegen.hpp"
#include "tt.hpp"

#include "globals.hpp"
#include <iostream>
#include <ostream>
#include <chrono> // For time management
#include <thread> // For potential future use with time management

int quiescence(Position &pos, int alpha, int beta) {
    log_debug("Entering quiescence");
    if (!searching) {
        return 0; // Stop early if search is interrupted
    }
    nodes_searched++; // Increment nodes searched
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
        makemove(pos, move_list.moves[i]);
        int score = -quiescence(pos, -beta, -alpha);
        undomove(pos, move_list.moves[i]);
        if (score >= beta) {
            return beta;
        }
        if (score > alpha) {
            alpha = score;
        }
    }
    return alpha;
}

int alpha_beta_search(Position &pos, int current_depth, int max_depth, int alpha, int beta, Move &best_move) {
    log_debug("Entering alpha_beta_search function (current_depth " + std::to_string(current_depth) + ", max_depth " + std::to_string(max_depth) + ")");
    if (!searching) {
        return 0;
    }
    nodes_searched++; // Increment nodes searched
    bool found;
    TTEntry *entry = tt.probe(pos.zobrist_key, found);
    if (found && entry->depth >= current_depth) {
        if (entry->flag == HASH_FLAG_EXACT) {
            best_move = entry->best_move;
            return entry->score;
        } else if (entry->flag == HASH_FLAG_ALPHA) {
            if (entry->score <= alpha) {
                best_move = entry->best_move;
                return alpha;
            }
        }
        else if (entry->flag == HASH_FLAG_BETA) {
            if (entry->score >= beta) {
                best_move = entry->best_move;
                return beta;
            }
        }
    }

    if (current_depth == 0) {
        return quiescence(pos, alpha, beta);
    }

    MoveList move_list;
    generate_moves(pos, move_list);

    HashFlag flag = HASH_FLAG_ALPHA;

    if (move_list.count == 0) {
        int king_square = pos.whiteToMove ? __builtin_ctzll(pos.WhiteKing) : __builtin_ctzll(pos.BlackKing);
        if (is_square_attacked(pos, king_square, !pos.whiteToMove)) {
            return -100000; // Checkmate
        } else {
            return 0; // Stalemate
        }
    }

    int best_score = -100000;

    log_debug("Found " + std::to_string(move_list.count) + " moves.");
    for (int i = 0; i < move_list.count; i++) {
        log_debug("Searching move: " + move_to_uci(move_list.moves[i]));
        makemove(pos, move_list.moves[i]);
        Move temp_best_move; // Temporary best move for recursive calls
        int score = -alpha_beta_search(pos, current_depth - 1, max_depth, -beta, -alpha, temp_best_move);
        log_debug("Undoing move: " + move_to_uci(move_list.moves[i]));
        undomove(pos, move_list.moves[i]);
        if (score > best_score) {
            best_score = score;
            best_move = move_list.moves[i];
        }
        if (best_score > alpha) {
            alpha = best_score;
            flag = HASH_FLAG_EXACT;
        }
        if (alpha >= beta) {
            tt.save(pos.zobrist_key, current_depth, HASH_FLAG_BETA, beta, move_list.moves[i]); // Save the move that caused beta cutoff
            return beta;
        }
    }

    tt.save(pos.zobrist_key, current_depth, flag, best_score, best_move);
    return best_score;
}

int search(Position &pos, int max_depth, long long move_time, Move &best_move) {
    Move current_best_move;
    int current_best_score = 0;

    auto start_time = std::chrono::high_resolution_clock::now();
    nodes_searched = 0; // Reset nodes searched for each new search

    for (int depth = 1; depth <= max_depth; ++depth) {
        if (!searching) break; // Stop iterative deepening if search is interrupted

        Move iteration_best_move;
        int iteration_score = alpha_beta_search(pos, depth, max_depth, -1000000, 1000000, iteration_best_move);

        if (searching) { // Check if we are still searching (e.g., not stopped by 'stop' command)
            current_best_score = iteration_score;
            current_best_move = iteration_best_move;

            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

            // Reconstruct PV
            std::string pv_string = "";
            Position temp_pos = pos;
            Move pv_move = current_best_move;
            for (int i = 0; i < depth; ++i) {
                if (pv_move.from == 0 && pv_move.to == 0) break; // Null move, end of PV
                pv_string += move_to_uci(pv_move) + " ";
                makemove(temp_pos, pv_move);
                Move next_pv_move;
                alpha_beta_search(temp_pos, 1, max_depth, -1000000, 1000000, next_pv_move); // Search for next move in PV
                pv_move = next_pv_move;
            }

            long long nps = 0;
            if (duration > 0) {
                nps = (nodes_searched * 1000) / duration;
            }

            // Output info string
            std::cout << "info depth " << depth
                      << " score cp " << current_best_score
                      << " time " << duration
                      << " nodes " << nodes_searched
                      << " nps " << nps
                      << " pv " << pv_string
                      << std::endl;
            
            if (move_time != -1 && duration >= move_time) {
                searching = false; // Time limit exceeded, stop search
            }
        } else {
            break; // Stop iterative deepening if searching is false
        }
    }
    best_move = current_best_move;
    return current_best_score;
}

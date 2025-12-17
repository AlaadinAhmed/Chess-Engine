#include "position.hpp"
#include "search.hpp"
#include "utils.hpp"
#include "book.hpp"
#include "see.hpp"
#include "nnue.hpp"

#include <atomic>
#include <algorithm>
#include <cmath>

std::atomic<bool> searching = false;
std::atomic<long long> nodes_searched = 0;
std::atomic<int> seldepth = 0;
std::atomic<int> max_seldepth = 0;
#include "eval.hpp"
#include "movegen.hpp"
#include "tt.hpp"
#include "movepick.hpp"
#include "history.hpp"

History history;
int lmr_table[64][64];

struct EvalCacheEntry {
    uint64_t key;
    int score;
};
EvalCacheEntry eval_cache[16384];

int cached_evaluate(Position& pos) {
    uint64_t key = pos.zobrist_key;
    int index = key % 16384;
    if (eval_cache[index].key == key) {
        return eval_cache[index].score;
    }
    int score = evaluate(pos);
    eval_cache[index] = {key, score};
    return score;
}

void init_lmr() {
    for (int depth = 0; depth < 64; depth++) {
        for (int move = 0; move < 64; move++) {
            if (depth == 0 || move == 0) {
                lmr_table[depth][move] = 0;
            } else {
                // Balanced LMR: divisor 1.0 for good reductions without too many re-searches
                lmr_table[depth][move] = 1.0 + std::log(depth) * std::log(move) / 1.0;
            }
        }
    }
}

#include "globals.hpp"
#include <iostream>
#include <ostream>
#include <chrono> // For time management
#include <thread> // For threads
#include <mutex>
#include <vector>

int quiescence(Position &pos, int alpha, int beta, const std::chrono::high_resolution_clock::time_point& start_time, long long move_time, int ply) {
    
    // Track selective depth (atomic update)
    int current_max = max_seldepth.load();
    while (ply > current_max) {
        if (max_seldepth.compare_exchange_weak(current_max, ply)) {
            break;
        }
    }
    
    if (!searching) {
        return 0; // Stop early if search is interrupted
    }

    // Prevent infinite recursion
    if (ply > 100) {
        return cached_evaluate(pos);
    }

    // Check time limit more frequently (every 2048 nodes)
    if (move_time != -1 && (nodes_searched & 2047) == 0) {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time).count();
        if (duration >= move_time) {
            searching = false;
            return 0;
        }
    }

    nodes_searched++; // Increment nodes searched
    int stand_pat = cached_evaluate(pos);
    if (stand_pat >= beta) {
        return beta;
    }
    if (alpha < stand_pat) {
        alpha = stand_pat;
    }

    // Delta Pruning
    int big_delta = 900; // Queen value
    if (stand_pat < alpha - big_delta) {
        return alpha;
    }
    
    MoveList move_list;
    generate_captures(pos, move_list);
    // Simple capture ordering by MVV-LVA heuristic using piece values
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
    for (int i = 0; i < move_list.count; ++i) {
        Pieces victim = get_piece_at(pos, move_list.moves[i].to);
        Pieces aggressor = get_piece_at(pos, move_list.moves[i].from);
        
        // Delta Pruning Check
        if (stand_pat + piece_value(victim) + 200 < alpha && move_list.moves[i].promotion == NO_PIECE) {
            move_list.moves[i].score = -1000000; // Mark for skipping
        } else {
            move_list.moves[i].score = piece_value(victim) - piece_value(aggressor);
        }
    }

    for (int i = 0; i < move_list.count; i++) {
        // Find best move
        int best_idx = i;
        for (int j = i + 1; j < move_list.count; j++) {
            if (move_list.moves[j].score > move_list.moves[best_idx].score) {
                best_idx = j;
            }
        }
        // Swap
        Move tmp = move_list.moves[i];
        move_list.moves[i] = move_list.moves[best_idx];
        move_list.moves[best_idx] = tmp;

        if (move_list.moves[i].score == -1000000) continue; // Skip pruned moves

        // SEE Pruning
        if (see(pos, move_list.moves[i]) < 0) continue;

        makemove(pos, move_list.moves[i]);
        // Skip illegal captures that leave own king in check
        bool mover_is_white = !pos.whiteToMove;
        int king_sq = mover_is_white ? __builtin_ctzll(pos.WhiteKing) : __builtin_ctzll(pos.BlackKing);
        if (is_square_attacked(pos, king_sq, !mover_is_white)) {
            undomove(pos, move_list.moves[i]);
            continue;
        }
        int score = -quiescence(pos, -beta, -alpha, start_time, move_time, ply + 1);
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

void score_moves(MoveList &move_list, Position &pos, Move tt_move) {
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

    for (int i = 0; i < move_list.count; i++) {
        if (move_list.moves[i].from == tt_move.from && move_list.moves[i].to == tt_move.to) {
            move_list.moves[i].score = 1000000; // TT move gets highest score
            continue;
        }
        Pieces victim = get_piece_at(pos, move_list.moves[i].to);
        if (victim != NO_PIECE) { // It's a capture
            Pieces aggressor = get_piece_at(pos, move_list.moves[i].from);
            move_list.moves[i].score = piece_value(victim) - piece_value(aggressor) + 100000; // MVV-LVA
        } else {
            // Quiet move scoring can be added here (e.g., history heuristic)
            move_list.moves[i].score = 0;
        }
    }
}

int alpha_beta_search(Position &pos, int current_depth, int max_depth, int alpha, int beta, Move &best_move, std::chrono::time_point<std::chrono::high_resolution_clock> start_time, int move_time, bool allow_null, Move prev_move) {
    // log_debug("Entering alpha_beta_search function (current_depth " + std::to_string(current_depth) + ", max_depth " + std::to_string(max_depth) + ")");

    // Check if search should stop (check more frequently)
    if (!searching) {
        return 0;
    }
    
    // Check time limit more frequently (every 2048 nodes instead of 1024)
    if (move_time != -1 && (nodes_searched & 2047) == 0) {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time).count();
        if (duration >= move_time) {
            searching = false;
            return 0;
        }
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
    
    // ProbCut
    if (current_depth >= 5 && beta < 30000) { // Not in mate score range
        Move prob_cut_best;
        int prob_beta = beta + 200;
        int prob_depth = current_depth - 4;
        // Perform a shallow search with a wider window
        int prob_score = -alpha_beta_search(pos, prob_depth, max_depth, -prob_beta, -prob_beta + 1, prob_cut_best, start_time, move_time, false, prev_move);
        if (prob_score >= prob_beta) {
            return prob_beta;
        }
    }
    
    // Static Eval for pruning (hoisted)
    int static_eval = 0;
    bool eval_calculated = false;
    if (current_depth <= 7) {
        // Lazy Eval
        if (!is_square_attacked(pos, pos.whiteToMove ? __builtin_ctzll(pos.WhiteKing) : __builtin_ctzll(pos.BlackKing), !pos.whiteToMove)) {
             int classical = evaluate_classical(pos);
             int margin = 500;
             if (classical - margin >= beta) {
                 return beta;
             }
             if (classical + margin <= alpha) {
                 return alpha;
             }
        }

        static_eval = cached_evaluate(pos);
        eval_calculated = true;
    }

    // Null Move Pruning
    if (allow_null && current_depth >= 3 && !is_square_attacked(pos, pos.whiteToMove ? __builtin_ctzll(pos.WhiteKing) : __builtin_ctzll(pos.BlackKing), !pos.whiteToMove) && pos.has_non_pawn_material(pos.whiteToMove)) {
        int se = eval_calculated ? static_eval : cached_evaluate(pos);
        // Very aggressive NMP: R = 8 + depth/3 for maximum cutoffs
        int R = 8 + current_depth / 3 + std::min(4, std::max(0, (se - beta) / 80)); 
        
        int null_depth = current_depth - 1 - R;
        if (null_depth > 0) {
            uint64_t saved_ep = pos.enPassant;
            pos.make_null_move();
            
            Move null_move_best;
            int score = -alpha_beta_search(pos, null_depth, max_depth, -beta, -beta + 1, null_move_best, start_time, move_time, false, Move{});
            
            pos.unmake_null_move(saved_ep);
            
            if (score >= beta) {
                return beta;
            }
        }
    }
    
    // Razoring - extended to depth 4
    if (current_depth <= 4 && !is_square_attacked(pos, pos.whiteToMove ? __builtin_ctzll(pos.WhiteKing) : __builtin_ctzll(pos.BlackKing), !pos.whiteToMove) && alpha < beta - 1) {
        int se = eval_calculated ? static_eval : cached_evaluate(pos);
        int razor_margin = 150 + 80 * current_depth * current_depth;
        if (se + razor_margin < alpha) {
            int q_score = quiescence(pos, alpha, beta, start_time, move_time, max_depth); 
            if (q_score < alpha) {
                return alpha; 
            }
        }
    }

    // Probcut - If we have a good enough capture, skip the rest
    // Try a shallow search and if it beats beta + margin, assume a full search would too
    if (current_depth >= 5 && !is_square_attacked(pos, pos.whiteToMove ? __builtin_ctzll(pos.WhiteKing) : __builtin_ctzll(pos.BlackKing), !pos.whiteToMove)) {
        int probcut_beta = beta + 200;
        int probcut_depth = current_depth - 4;
        
        MoveList probcut_moves;
        generate_captures(pos, probcut_moves);
        
        for (int i = 0; i < probcut_moves.count; i++) {
            Move m = probcut_moves.moves[i];
            makemove(pos, m);
            
            // Check legality
            bool mover_white = !pos.whiteToMove;
            uint64_t kbb = mover_white ? pos.WhiteKing : pos.BlackKing;
            if (kbb == 0 || is_square_attacked(pos, __builtin_ctzll(kbb), !mover_white)) {
                undomove(pos, m);
                continue;
            }
            
            Move temp;
            int score = -alpha_beta_search(pos, probcut_depth, max_depth, -probcut_beta, -probcut_beta + 1, temp, start_time, move_time, false, m);
            undomove(pos, m);
            
            if (score >= probcut_beta) {
                return probcut_beta;
            }
        }
    }

    // Futility Pruning (Reverse Futility Pruning) - Extended to depth 15
    if (current_depth <= 15 && !is_square_attacked(pos, pos.whiteToMove ? __builtin_ctzll(pos.WhiteKing) : __builtin_ctzll(pos.BlackKing), !pos.whiteToMove) && alpha < beta - 1) {
        int se = eval_calculated ? static_eval : cached_evaluate(pos);
        // Very aggressive margin: 80 + 70*depth
        int margin = 80 + 70 * current_depth;
        if (se - margin >= beta) {
            return se; // Reverse Futility Pruning (Static Null Move Pruning)
        }
    }

    // Check Extension
    bool in_check = is_square_attacked(pos, pos.whiteToMove ? __builtin_ctzll(pos.WhiteKing) : __builtin_ctzll(pos.BlackKing), !pos.whiteToMove);
    if (in_check) {
        current_depth++;
    }

    if (current_depth <= 0) {
        int ply = max_depth;
        return quiescence(pos, alpha, beta, start_time, move_time, ply);
    }

    // Internal Iterative Deepening (IID)
    if (found && entry->depth >= current_depth) {
        // ... existing TT logic ...
    }
    
    Move tt_move = (found) ? entry->best_move : Move{};
    
    if (current_depth >= 4 && (tt_move.from == 0 && tt_move.to == 0)) {
        int iid_depth = current_depth - 2;
    Move iid_best_move;
    alpha_beta_search(pos, iid_depth, max_depth, alpha, beta, iid_best_move, start_time, move_time, true, prev_move);
    tt_move = iid_best_move;
        
        // Probe TT again to get the entry from IID
        entry = tt.probe(pos.zobrist_key, found);
        if (found) {
             tt_move = entry->best_move;
        }
    }

    // Singular Extension: If the TT move is clearly better than alternatives, extend it
    int singular_extension = 0;
    bool pvNode = (beta - alpha > 1);
    
    // Singular Extension: If the TT move is clearly better than alternatives, extend it
    if (current_depth >= 6 && found && entry->depth >= current_depth - 3 && 
        entry->flag != HASH_FLAG_ALPHA && (tt_move.from != 0 || tt_move.to != 0) && !in_check) {
        
        // Singular margin: scale with depth
        int singular_beta = entry->score - 2 * current_depth;
        int singular_depth = (current_depth - 1) / 2;
        
        if (singular_depth > 0) {
            // Do a reduced search excluding the TT move
            // We use a null window search around singular_beta
            Move excluded_best;
            MovePicker singular_picker(pos, Move{}, history, singular_depth, prev_move);
            int singular_score = -50000;
            
            while (true) {
                Move m = singular_picker.next_move();
                if (m.from == 0 && m.to == 0) break;
                
                // Skip the TT move
                if (m.from == tt_move.from && m.to == tt_move.to && m.promotion == tt_move.promotion) continue;
                
                makemove(pos, m);
                
                // Check legality
                bool mover_white = !pos.whiteToMove;
                uint64_t kbb = mover_white ? pos.WhiteKing : pos.BlackKing;
                if (kbb == 0 || is_square_attacked(pos, __builtin_ctzll(kbb), !mover_white)) {
                    undomove(pos, m);
                    continue;
                }
                
                Move temp;
                int score = -alpha_beta_search(pos, singular_depth, max_depth, -singular_beta, -singular_beta + 1, temp, start_time, move_time, true, m);
                undomove(pos, m);
                
                if (score >= singular_beta) {
                    singular_score = score;
                    break;  // Found a move that beats singular_beta, TT move is not singular
                }
                if (score > singular_score) {
                    singular_score = score;
                }
            }
            
            // If no other move beats singular_beta, the TT move is singular
            if (singular_score < singular_beta) {
                singular_extension = 1;
            }
        }
    }

    int best_score = -50000;
    Move local_best_move = {}; 
    bool found_legal_move = false;
    int illegal_count = 0; 
    HashFlag flag = HASH_FLAG_ALPHA;
    Move temp_best_move;

    MovePicker move_picker(pos, tt_move, history, current_depth, prev_move);
    
    int moves_searched = 0;
    
    while (true) {
        Move current_move = move_picker.next_move();
        if (current_move.from == 0 && current_move.to == 0) break;

        moves_searched++;
        
        // Check if capture BEFORE makemove
        bool is_capture = (get_piece_at(pos, current_move.to) != NO_PIECE);
        Pieces moved_piece = get_piece_at(pos, current_move.from);
        
        // log_debug("Searching move: " + move_to_uci(current_move));
        makemove(pos, current_move);
        
        // Skip illegal moves that leave own king in check
        bool mover_is_white = !pos.whiteToMove;
        uint64_t king_bb = mover_is_white ? pos.WhiteKing : pos.BlackKing;
        if (king_bb == 0) {
            illegal_count++;
            undomove(pos, current_move);
            continue;
        }
        int king_sq = __builtin_ctzll(king_bb);
        bool in_check_after_move = is_square_attacked(pos, king_sq, !mover_is_white);
        if (in_check_after_move) {
            illegal_count++;
            undomove(pos, current_move);
            continue;
        }
        
        // If this is the first legal move found, initialize local_best_move
        if (!found_legal_move) {
            local_best_move = current_move;
            found_legal_move = true;
        }
        
        int score;
        bool do_full_search = true;
        
        // Late Move Pruning (LMP) - Stable thresholds
        // Don't prune the TT move if it was marked as singular
        bool is_tt = (current_move.from == tt_move.from && current_move.to == tt_move.to && current_move.promotion == tt_move.promotion);
        if (!in_check_after_move && !is_capture && current_move.promotion == NO_PIECE && current_depth <= 5 && !is_tt) {
            int lmp_threshold = 3 + current_depth * current_depth;
            if (moves_searched > lmp_threshold) {
                undomove(pos, current_move);
                continue;
            }
        }

        // History Pruning
        int history_score_val = history.history_scores[moved_piece][current_move.to];
        if (!in_check_after_move && !is_capture && current_move.promotion == NO_PIECE && current_depth <= 4 && history_score_val < -4000) {
            undomove(pos, current_move);
            continue;
        }
        
        // Note: SEE-based pruning for quiet moves removed - SEE on non-captures returns 0

        // Late Move Reduction (LMR)
        if (moves_searched > 3 && current_depth >= 3 && !in_check_after_move && !is_capture && current_move.promotion == NO_PIECE) {
            int d = std::min(current_depth, 63);
            int m = std::min(moves_searched, 63);
            int reduction = lmr_table[d][m];
            
            // History-based LMR adjustment
            if (history_score_val > 2000) reduction -= 1;
            else if (history_score_val < -2000) reduction += 1;
            
            // PV node adjustment
            bool pvNode = (beta - alpha > 1);
            if (pvNode) reduction -= 1;
            if (reduction < 0) reduction = 0;
            
            int reduced_depth = std::max(1, current_depth - 1 - reduction);
            
            score = -alpha_beta_search(pos, reduced_depth, max_depth, -alpha - 1, -alpha, temp_best_move, start_time, move_time, true, current_move);
            
            if (score > alpha) {
                do_full_search = true; // Re-search at full depth
            } else {
                do_full_search = false;
            }
        }
        
        if (do_full_search) {
            // Calculate extension (singular extension applies to TT move)
            int extension = 0;
            bool is_tt_move = (current_move.from == tt_move.from && current_move.to == tt_move.to && current_move.promotion == tt_move.promotion);
            if (is_tt_move && singular_extension) {
                extension = singular_extension;
            }
            
            int new_depth = current_depth - 1 + extension;
            
            // Principal Variation Search (PVS)
            if (moves_searched > 1) {
                // Null window search
                score = -alpha_beta_search(pos, new_depth, max_depth, -alpha - 1, -alpha, temp_best_move, start_time, move_time, true, current_move);
                if (score > alpha && score < beta) {
                    // Re-search with full window
                    score = -alpha_beta_search(pos, new_depth, max_depth, -beta, -alpha, temp_best_move, start_time, move_time, true, current_move);
                }
            } else {
                // Full window search for PV move
                score = -alpha_beta_search(pos, new_depth, max_depth, -beta, -alpha, temp_best_move, start_time, move_time, true, current_move);
            }
        }

        // log_debug("Undoing move: " + move_to_uci(current_move));
        undomove(pos, current_move);
        
        if (score > best_score) {
            best_score = score;
            local_best_move = current_move;
            if (score > alpha) {
                alpha = score;
                flag = HASH_FLAG_EXACT;
                
                // Update History
                if (get_piece_at(pos, current_move.to) == NO_PIECE) {
                     history.update_history_score(pos, current_move, current_depth);
                }
            }
        }
        if (score >= beta) {
            // Update Killers
            if (get_piece_at(pos, current_move.to) == NO_PIECE) {
                history.update_killer_move(current_move, current_depth); // Using depth as ply proxy
                history.update_history_score(pos, current_move, current_depth);
                history.update_counter_move(prev_move, current_move);
                history.update_counter_move_history(prev_move, current_move, current_depth, pos);
            }
            
            tt.save(pos.zobrist_key, current_depth, HASH_FLAG_BETA, beta, local_best_move); 
            best_move = local_best_move; 
            return beta;
        }
    }
    
    if (found_legal_move) {
        best_move = local_best_move; 
    } else {
        best_move = {}; 
        if (debug_mode && current_depth <= 3) {
             // Warning suppressed
        }
    }
    tt.save(pos.zobrist_key, current_depth, flag, best_score, best_move);
    return best_score;
}

int search(Position &pos, int max_depth, long long move_time, Move &best_move) {
    searching = true;
    
    // Refresh NNUE accumulator for the root position
    if (nnue::is_initialized()) {
        nnue::refresh_accumulator(pos);
    }

    static bool lmr_init = false;
    if (!lmr_init) {
        init_lmr();
        lmr_init = true;
    }

    // Check for book move (just a hint, we still do real search)
    Move book_move;
    if (own_book_enabled) {
        book_move = opening_book.get_move(pos);
        // Book move will be used as hint but we still search
    }

    Move current_best_move;
    int current_best_score = 0;

    auto start_time = std::chrono::high_resolution_clock::now();
    nodes_searched = 0; // Reset nodes searched for each new search
    max_seldepth = 0; // Reset max seldepth

    for (int depth = 1; depth <= max_depth; ++depth) {
        log_debug("Starting search for depth " + std::to_string(depth) + ", searching is " + (searching ? "true" : "false"));
        if (!searching) break; // Stop iterative deepening if search is interrupted

        Move iteration_best_move;
        history_ply = 0; // ensure clean state per iteration
        seldepth = 0; // Reset seldepth for this iteration (not max_seldepth!)
        int iteration_score;
        int alpha = -50000;
        int beta = 50000;
        
        if (depth >= 5) {
            alpha = current_best_score - 50;
            beta = current_best_score + 50;
        }
        
        while (true) {
            iteration_score = alpha_beta_search(pos, depth, depth, alpha, beta, iteration_best_move, start_time, move_time, true, Move{});
            
            if (!searching) break;
            
            if (iteration_score <= alpha) {
                alpha -= alpha / 2; // Widen window downwards (or just -infinity)
                if (alpha < -50000) alpha = -50000;
                log_debug("Aspiration fail low, widening alpha to " + std::to_string(alpha));
            } else if (iteration_score >= beta) {
                beta += beta / 2; // Widen window upwards
                if (beta > 50000) beta = 50000;
                log_debug("Aspiration fail high, widening beta to " + std::to_string(beta));
            } else {
                break; // Score within window
            }
        }

        if (searching) { // Check if we are still searching (e.g., not stopped by 'stop' command)
            current_best_score = iteration_score;
            current_best_move = iteration_best_move;
            
            // Update max_seldepth
            int current_sel = seldepth.load();
            int current_max = max_seldepth.load();
            if (current_sel > current_max) {
                max_seldepth.store(current_sel);
            }

            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
            if (duration == 0) duration = 1; // Avoid division by zero

            // Reconstruct PV with legality checks to avoid bogus TT chains
            Move pv_moves[256];
            int pv_count = 0;
            Position temp_pos = pos;
            int saved_history_ply = history_ply; // Save before PV extraction
            uint64_t pv_keys[256]; // Track keys to detect cycles
            int pv_keys_count = 0;
            
            // Start with the best move from this iteration
            if (iteration_best_move.from != 0 || iteration_best_move.to != 0) {
                pv_moves[pv_count++] = iteration_best_move;
                makemove(temp_pos, iteration_best_move);
                pv_keys[pv_keys_count++] = pos.zobrist_key;
                
                // Then follow the TT chain
                for (int i = 1; i < depth && pv_count < depth; ++i) {
                    // Check for cycles
                    bool cycle_found = false;
                    for (int k = 0; k < pv_keys_count; ++k) {
                        if (pv_keys[k] == temp_pos.zobrist_key) {
                            cycle_found = true;
                            break;
                        }
                    }
                    if (cycle_found) break; // Position repeated, stop PV
                    
                    pv_keys[pv_keys_count++] = temp_pos.zobrist_key;
                    
                    bool found;
                    TTEntry* entry = tt.probe(temp_pos.zobrist_key, found);
                    if (!found || entry->depth == 0) break;
                    Move pv_move = entry->best_move;
                    
                    // Validate the move has non-zero squares
                    if (pv_move.from == 0 && pv_move.to == 0) break;
                    
                    // Validate move legality in current position
                    MoveList legal;
                    generate_moves(temp_pos, legal);
                    bool legal_found = false;
                    for (int mi = 0; mi < legal.count; ++mi) {
                        if (legal.moves[mi].from == pv_move.from && legal.moves[mi].to == pv_move.to && legal.moves[mi].promotion == pv_move.promotion) {
                            legal_found = true; break;
                        }
                    }
                    if (!legal_found) {
                        break;
                    }
                    pv_moves[pv_count++] = pv_move;
                    makemove(temp_pos, pv_move);
                }
            }
            history_ply = saved_history_ply; // Restore after PV extraction
            
            // Build PV string
            std::string pv_string = "";
            for (int i = 0; i < pv_count; ++i) {
                pv_string += move_to_uci(pv_moves[i]) + " ";
            }

            long long nps = 0;
            long long total_nodes = nodes_searched.load();
            if (duration > 0) {
                nps = (total_nodes * 1000) / duration;
            }

            // Output info string
            std::cout << "info depth " << depth
                      << " seldepth " << max_seldepth.load()
                      << " score cp " << current_best_score
                      << " time " << duration
                      << " nodes " << total_nodes
                      << " nps " << nps
                      << " pv " << pv_string
                      << std::endl;
            
            if (move_time != -1 && duration >= move_time) {
                searching = false; // Time limit exceeded, stop search
            }
        } else {
            log_debug("Stopping search because searching is false.");
            break; // Stop iterative deepening if searching is false
        }
    }
    best_move = current_best_move;
    return current_best_score;
}

// Lazy SMP: All threads search the same position with iterative deepening
// Thread 0 is the main thread that prints info strings
int search_root_parallel(Position &pos, int max_depth, long long move_time, Move &best_move) {
    if (num_threads <= 1) {
        return search(pos, max_depth, move_time, best_move);
    }
    
    // Shared state across threads
    struct SharedData {
        std::mutex mtx;
        Move best_move;
        int best_score = -1000000;
        int completed_depth = 0;
    };
    
    SharedData shared;
    searching = true;
    nodes_searched = 0;
    auto start_time = std::chrono::high_resolution_clock::now();
    
    // Generate legal moves at root
    MoveList root_moves;
    generate_moves(pos, root_moves);
    if (root_moves.count == 0) {
        best_move = {};
        return 0;
    }
    shared.best_move = root_moves.moves[0];
    
    // Worker function: each thread does iterative deepening
    auto worker = [&](int thread_id) {
        Position local_pos = pos;

        Move local_best;
        int local_best_score = -1000000;
        
        // Each helper thread starts from a slightly different depth to add diversity
        int start_depth = (thread_id == 0) ? 1 : 1 + (thread_id % 3);
        
        for (int depth = start_depth; depth <= max_depth; ++depth) {
            if (!searching) break;
            
            Move iteration_best;
            history_ply = 0;
            
            // Reset seldepth only for thread 0 at each depth
            if (thread_id == 0) {
                seldepth = 0;
            }
            
            int iteration_score = alpha_beta_search(local_pos, depth, depth, -1000000, 1000000, iteration_best, start_time, move_time, true, Move{});
            
            if (!searching) break;
            
            // Update shared best move if this is better
            {
                std::lock_guard<std::mutex> lock(shared.mtx);
                if (iteration_score > shared.best_score || 
                    (iteration_score == shared.best_score && thread_id == 0)) {
                    shared.best_score = iteration_score;
                    shared.best_move = iteration_best;
                    local_best = iteration_best;
                    local_best_score = iteration_score;
                    
                    // Only thread 0 prints info strings
                    if (thread_id == 0) {
                        shared.completed_depth = depth;
                        
                        auto end_time = std::chrono::high_resolution_clock::now();
                        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
                        
                        // Build PV string
                        std::string pv_string = "";
                        Position temp_pos = pos;
                        int saved_history_ply = history_ply;
                        uint64_t pv_keys[256];
                        int pv_keys_count = 0;
                        
                        if (iteration_best.from != 0 || iteration_best.to != 0) {
                            pv_string += move_to_uci(iteration_best) + " ";
                            makemove(temp_pos, iteration_best);
                            pv_keys[pv_keys_count++] = pos.zobrist_key;
                            
                            for (int i = 1; i < depth; ++i) {
                                bool cycle_found = false;
                                for (int k = 0; k < pv_keys_count; ++k) {
                                    if (pv_keys[k] == temp_pos.zobrist_key) {
                                        cycle_found = true;
                                        break;
                                    }
                                }
                                if (cycle_found) break;

                                pv_keys[pv_keys_count++] = temp_pos.zobrist_key;
                                
                                bool found;
                                TTEntry* entry = tt.probe(temp_pos.zobrist_key, found);
                                if (!found || entry->depth == 0) break;
                                Move pv_move = entry->best_move;
                                
                                if (pv_move.from == 0 && pv_move.to == 0) break;
                                
                                MoveList legal;
                                generate_moves(temp_pos, legal);
                                bool legal_found = false;
                                for (int mi = 0; mi < legal.count; ++mi) {
                                    if (legal.moves[mi].from == pv_move.from && 
                                        legal.moves[mi].to == pv_move.to && 
                                        legal.moves[mi].promotion == pv_move.promotion) {
                                        legal_found = true;
                                        break;
                                    }
                                }
                                if (!legal_found) break;
                                
                                pv_string += move_to_uci(pv_move) + " ";
                                makemove(temp_pos, pv_move);
                            }
                        }
                        history_ply = saved_history_ply;
                        
                        // Reset NNUE accumulator to root state after PV extraction modified it
                        if (nnue::is_initialized()) {
                            nnue::refresh_accumulator(pos);
                            // nnue::print_stats();
                        }
                        
                        long long nps = 0;
                        if (duration > 0) {
                            nps = (nodes_searched.load() * 1000) / duration;
                        }
                        
                        std::cout << "info depth " << depth
                                  << " seldepth " << seldepth.load()
                                  << " score cp " << iteration_score
                                  << " time " << duration
                                  << " nodes " << nodes_searched.load()
                                  << " nps " << nps
                                  << " pv " << pv_string << std::endl;
                    }
                }
            }
            
            // Check time
            auto now = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time).count();
            if (move_time != -1 && duration >= move_time) {
                searching = false;
                break;
            }
        }
    };
    
    // Launch threads
    std::vector<std::thread> threads;
    for (int t = 0; t < num_threads; ++t) {
        threads.emplace_back(worker, t);
    }
    
    // Wait for all threads
    for (auto &th : threads) {
        th.join();
    }
    
    best_move = shared.best_move;
    return shared.best_score;
}
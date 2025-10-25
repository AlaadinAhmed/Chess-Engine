#include "position.hpp"
#include "search.hpp"
#include "utils.hpp"

#include <atomic>
#include <algorithm>

std::atomic<bool> searching = false;
std::atomic<long long> nodes_searched = 0;
std::atomic<int> seldepth = 0;
#include "eval.hpp"
#include "movegen.hpp"
#include "tt.hpp"

#include "globals.hpp"
#include <iostream>
#include <ostream>
#include <chrono> // For time management
#include <thread> // For threads
#include <mutex>
#include <vector>

int quiescence(Position &pos, int alpha, int beta, const std::chrono::high_resolution_clock::time_point& start_time, long long move_time, int ply) {
    log_debug("Entering quiescence");
    
    // Track selective depth (atomic update)
    int current_seldepth = seldepth.load();
    while (ply > current_seldepth) {
        if (seldepth.compare_exchange_weak(current_seldepth, ply)) {
            break;
        }
    }
    
    if (!searching) {
        return 0; // Stop early if search is interrupted
    }

    // Check time limit
    if (move_time != -1 && (nodes_searched & 2047) == 0) {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time).count();
        if (duration >= move_time) {
            searching = false;
            return 0;
        }
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
        move_list.moves[i].score = piece_value(victim) - piece_value(aggressor);
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

int alpha_beta_search(Position &pos, int current_depth, int max_depth, int alpha, int beta, Move &best_move, const std::chrono::high_resolution_clock::time_point& start_time, long long move_time) {
    log_debug("Entering alpha_beta_search function (current_depth " + std::to_string(current_depth) + ", max_depth " + std::to_string(max_depth) + ")");

    if (!searching) {
        return 0;
    }
    
    // Check time limit
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

    if (current_depth == 0) {
        int ply = max_depth;
        return quiescence(pos, alpha, beta, start_time, move_time, ply);
    }

    MoveList move_list;
    generate_moves(pos, move_list);
    
    Move tt_move = (found) ? entry->best_move : Move{};
    score_moves(move_list, pos, tt_move);

    HashFlag flag = HASH_FLAG_ALPHA;

    if (move_list.count == 0) {
        int king_square = pos.whiteToMove ? __builtin_ctzll(pos.WhiteKing) : __builtin_ctzll(pos.BlackKing);
        if (is_square_attacked(pos, king_square, !pos.whiteToMove)) {
            // Checkmate - return mate score adjusted for distance
            return -100000 + (max_depth - current_depth);
        } else {
            return 0; // Stalemate
        }
    }

    int best_score = -100000;
    Move local_best_move = {}; // Initialize with default values
    bool found_legal_move = false;
    int illegal_count = 0; // Count illegal moves

    log_debug("Found " + std::to_string(move_list.count) + " moves.");
    for (int i = 0; i < move_list.count; i++) {
        // Find best move
        int best_idx = i;
        for (int j = i + 1; j < move_list.count; j++) {
            if (move_list.moves[j].score > move_list.moves[best_idx].score) {
                best_idx = j;
            }
        }
        // Swap
        Move current_move = move_list.moves[best_idx];
        move_list.moves[best_idx] = move_list.moves[i];
        move_list.moves[i] = current_move;

        log_debug("Searching move: " + move_to_uci(current_move));
        makemove(pos, current_move);
        // Skip illegal moves that leave own king in check
        bool mover_is_white = !pos.whiteToMove;
        uint64_t king_bb = mover_is_white ? pos.WhiteKing : pos.BlackKing;
        if (king_bb == 0) {
            // King was captured - this should never happen!
            log_debug("ERROR: King captured after move " + move_to_uci(current_move));
            illegal_count++;
            undomove(pos, current_move);
            continue;
        }
        int king_sq = __builtin_ctzll(king_bb);
        bool in_check = is_square_attacked(pos, king_sq, !mover_is_white);
        if (in_check) {
            illegal_count++;
            undomove(pos, current_move);
            continue;
        }
        // If this is the first legal move found, initialize local_best_move
        if (!found_legal_move) {
            local_best_move = current_move;
            found_legal_move = true;
        }
        Move temp_best_move; // Temporary best move for recursive calls
        int score = -alpha_beta_search(pos, current_depth - 1, max_depth, -beta, -alpha, temp_best_move, start_time, move_time);
        log_debug("Undoing move: " + move_to_uci(current_move));
        undomove(pos, current_move);
        if (score > best_score) {
            best_score = score;
            local_best_move = current_move;
        }
        if (best_score > alpha) {
            alpha = best_score;
            flag = HASH_FLAG_EXACT;
        }
        if (alpha >= beta) {
            tt.save(pos.zobrist_key, current_depth, HASH_FLAG_BETA, beta, local_best_move); // Save the best move that caused beta cutoff
            best_move = local_best_move; // Assign the local best move before returning
            return beta;
        }
    }

    if (found_legal_move) {
        best_move = local_best_move; // Assign the local best move to the reference parameter
    } else {
        best_move = {}; // No legal moves found, set to null move
        if (debug_mode && current_depth <= 3) {
            std::cerr << "WARNING: No legal moves found at depth " << current_depth 
                      << " (generated " << move_list.count << ", " << illegal_count << " illegal)" << std::endl;
        }
    }
    tt.save(pos.zobrist_key, current_depth, flag, best_score, best_move);
    return best_score;
}

int search(Position &pos, int max_depth, long long move_time, Move &best_move) {
    searching = true;
    Move current_best_move;
    int current_best_score = 0;

    auto start_time = std::chrono::high_resolution_clock::now();
    nodes_searched = 0; // Reset nodes searched for each new search

    for (int depth = 1; depth <= max_depth; ++depth) {
        log_debug("Starting search for depth " + std::to_string(depth) + ", searching is " + (searching ? "true" : "false"));
        if (!searching) break; // Stop iterative deepening if search is interrupted

        Move iteration_best_move;
        history_ply = 0; // ensure clean state per iteration
        seldepth = 0; // Reset seldepth for this iteration
        int iteration_score = alpha_beta_search(pos, depth, depth, -1000000, 1000000, iteration_best_move, start_time, move_time);

        if (searching) { // Check if we are still searching (e.g., not stopped by 'stop' command)
            current_best_score = iteration_score;
            current_best_move = iteration_best_move;

            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

            // Reconstruct PV with legality checks to avoid bogus TT chains
            std::string pv_string = "";
            Position temp_pos = pos;
            int saved_history_ply = history_ply; // Save before PV extraction
            std::vector<uint64_t> pv_keys; // Track keys to detect cycles
            
            // Start with the best move from this iteration
            if (iteration_best_move.from != 0 || iteration_best_move.to != 0) {
                pv_string += move_to_uci(iteration_best_move) + " ";
                makemove(temp_pos, iteration_best_move);
                pv_keys.push_back(pos.zobrist_key);
                
                // Then follow the TT chain
                for (int i = 1; i < depth; ++i) {
                    // Check for cycles
                    if (std::find(pv_keys.begin(), pv_keys.end(), temp_pos.zobrist_key) != pv_keys.end()) {
                        break; // Position repeated, stop PV
                    }
                    pv_keys.push_back(temp_pos.zobrist_key);
                    
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
                    pv_string += move_to_uci(pv_move) + " ";
                    makemove(temp_pos, pv_move);
                }
            }
            history_ply = saved_history_ply; // Restore after PV extraction

            long long nps = 0;
            long long total_nodes = nodes_searched.load();
            if (duration > 0) {
                nps = (total_nodes * 1000) / duration;
            }

            // Output info string
            std::cout << "info depth " << depth
                      << " seldepth " << seldepth.load()
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
            
            int iteration_score = alpha_beta_search(local_pos, depth, -1000000, 1000000, iteration_best, start_time, move_time);
            
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
                        std::vector<uint64_t> pv_keys;
                        
                        if (iteration_best.from != 0 || iteration_best.to != 0) {
                            pv_string += move_to_uci(iteration_best) + " ";
                            makemove(temp_pos, iteration_best);
                            pv_keys.push_back(pos.zobrist_key);
                            
                            for (int i = 1; i < depth; ++i) {
                                if (std::find(pv_keys.begin(), pv_keys.end(), temp_pos.zobrist_key) != pv_keys.end()) {
                                    break;
                                }
                                pv_keys.push_back(temp_pos.zobrist_key);
                                
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
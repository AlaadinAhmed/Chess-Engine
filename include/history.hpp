#pragma once

#include "position.hpp"
#include "globals.hpp"

struct History {
    int history_scores[16][64]; // [piece][to_square]
    int counter_move_history[16][64][16][64]; // [prev_piece][prev_to][piece][to]
    Move killer_moves[2][64];   // [slot][ply]
    Move counter_moves[64][64]; // [from][to] of previous move -> counter move

    History() {
        clear();
    }

    void clear() {
        for (int i = 0; i < 16; i++) {
            for (int j = 0; j < 64; j++) {
                history_scores[i][j] = 0;
                for (int k = 0; k < 16; k++) {
                    for (int l = 0; l < 64; l++) {
                        counter_move_history[i][j][k][l] = 0;
                    }
                }
            }
        }
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 64; j++) {
                killer_moves[i][j] = Move{}; // Initialize with empty moves
            }
        }
        for (int i = 0; i < 64; i++) {
            for (int j = 0; j < 64; j++) {
                counter_moves[i][j] = Move{};
            }
        }
    }

    void update_history_score(const Position &pos, Move move, int depth) {
        int bonus = depth * depth;
        int& score = history_scores[get_piece_at(pos, move.from)][move.to];
        score += bonus;
        if (score > 10000) score = 10000; // Cap at 10000
    }

    void update_counter_move_history(Move prev_move, Move move, int depth, const Position& pos) {
        if (prev_move.from == 0 && prev_move.to == 0) return;
        int prev_piece = get_piece_at(pos, prev_move.to); // Piece is at 'to' square now
        // Wait, prev_move was made, so piece is at prev_move.to.
        // But we need the piece type.
        // If it was a capture, get_piece_at(pos, prev_move.to) returns the piece that moved.
        // Yes.
        
        int piece = get_piece_at(pos, move.from);
        int bonus = depth * depth;
        int& score = counter_move_history[prev_piece][prev_move.to][piece][move.to];
        score += bonus;
        if (score > 10000) score = 10000;
    }
    
    int get_counter_move_history_score(Move prev_move, Move move, const Position& pos) {
        if (prev_move.from == 0 && prev_move.to == 0) return 0;
        // We need the piece type of prev_move.
        // But prev_move was made in previous turn.
        // In current position, the piece is at prev_move.to.
        // Unless it was captured? No, prev_move was just made by opponent.
        // So piece is at prev_move.to.
        int prev_piece = get_piece_at(pos, prev_move.to);
        int piece = get_piece_at(pos, move.from);
        return counter_move_history[prev_piece][prev_move.to][piece][move.to];
    }

    void penalize_history_score(const Position &pos, Move move, int depth) {
        int penalty = depth * depth;
        int& score = history_scores[get_piece_at(pos, move.from)][move.to];
        score -= penalty;
        if (score < -10000) score = -10000; // Cap at -10000
    }
    
    void update_killer_move(Move move, int ply) {
        if (ply >= 64) return;
        if (killer_moves[0][ply].from != move.from || killer_moves[0][ply].to != move.to) {
            killer_moves[1][ply] = killer_moves[0][ply];
            killer_moves[0][ply] = move;
        }
    }
    
    void update_counter_move(Move prev_move, Move move) {
        if (prev_move.from != 0 || prev_move.to != 0) {
            counter_moves[prev_move.from][prev_move.to] = move;
        }
    }
    
    Move get_counter_move(Move prev_move) {
        if (prev_move.from != 0 || prev_move.to != 0) {
            return counter_moves[prev_move.from][prev_move.to];
        }
        return Move{};
    }

    bool is_killer(Move move, int ply) {
        if (ply >= 64) return false;
        return (killer_moves[0][ply].from == move.from && killer_moves[0][ply].to == move.to) ||
               (killer_moves[1][ply].from == move.from && killer_moves[1][ply].to == move.to);
    }
};

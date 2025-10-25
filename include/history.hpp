#pragma once

#include "position.hpp"
#include "globals.hpp"

struct History {
    Move killer_moves[MAX_PLY][2];
    int history_scores[12][64];

    History() {
        for (int i = 0; i < MAX_PLY; i++) {
            killer_moves[i][0] = {};
            killer_moves[i][1] = {};
        }
        for (int i = 0; i < 12; i++) {
            for (int j = 0; j < 64; j++) {
                history_scores[i][j] = 0;
            }
        }
    }

    void update_killer_move(Move move, int ply) {
        if (killer_moves[ply][0].from != move.from || killer_moves[ply][0].to != move.to) {
            killer_moves[ply][1] = killer_moves[ply][0];
            killer_moves[ply][0] = move;
        }
    }

    void update_history_score(Position &pos, Move move, int depth) {
        history_scores[get_piece_at(pos, move.from)][move.to] += depth * depth;
    }

    bool is_killer(Move move, int ply) {
        return (killer_moves[ply][0].from == move.from && killer_moves[ply][0].to == move.to) ||
               (killer_moves[ply][1].from == move.from && killer_moves[ply][1].to == move.to);
    }
};

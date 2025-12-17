/**
 * @file nnue.cpp
 * @brief NNUE wrapper implementation using stockfish_nnue_probe library with incremental updates
 */

#include "nnue.hpp"
#include <iostream>
#include <vector>
#include <memory>
#include <cstring>
#include <chrono>
#include "fen.hpp"

// Include Stockfish headers
// Use full paths to avoid ambiguity
#include "../extern/stockfish_nnue_probe/types.h"
#include "../extern/stockfish_nnue_probe/position.h"
#include "../extern/stockfish_nnue_probe/evaluate.h"
#include "../extern/stockfish_nnue_probe/nnue/evaluate_nnue.h"
#include "../extern/stockfish_nnue_probe/probe.h"

#include <mutex>

namespace nnue {

static bool initialized = false;

// Thread-local shadow state for incremental updates
struct ShadowState {
    Stockfish::Position pos;
    std::vector<std::unique_ptr<Stockfish::StateInfo>> state_stack;
    int state_index = 0;

    ShadowState() {
        // Allocate initial state
        state_stack.push_back(std::make_unique<Stockfish::StateInfo>());
        state_index = 0;
    }
};

thread_local std::unique_ptr<ShadowState> shadow;
std::recursive_mutex nnue_mutex;

bool init(const char* big_net_file, const char* small_net_file) {
    if (initialized) return true;
    
    #ifdef __AVX2__
    std::cout << "info string AVX2 enabled" << std::endl;
    #else
    std::cout << "info string AVX2 DISABLED" << std::endl;
    #endif
    
    // Check if files exist
    FILE* f1 = fopen(big_net_file, "rb");
    FILE* f2 = fopen(small_net_file, "rb");
    if (!f1 || !f2) {
        if (f1) fclose(f1);
        if (f2) fclose(f2);
        return false;
    }
    fclose(f1);
    fclose(f2);
    
    Stockfish::Probe::init(big_net_file, small_net_file);
    initialized = true;
    std::cout << "info string NNUE loaded: " << big_net_file << " + " << small_net_file << std::endl;
    return true;
}

bool init(const char* network_file) { return false; }

bool is_initialized() { return initialized; }

void shutdown() { initialized = false; }

// Convert OctoKnight piece to Stockfish piece
static Stockfish::Piece to_sf_piece(Pieces p) {
    switch (p) {
        case W_PAWN:   return Stockfish::W_PAWN;
        case W_KNIGHT: return Stockfish::W_KNIGHT;
        case W_BISHOP: return Stockfish::W_BISHOP;
        case W_ROOK:   return Stockfish::W_ROOK;
        case W_QUEEN:  return Stockfish::W_QUEEN;
        case W_KING:   return Stockfish::W_KING;
        case B_PAWN:   return Stockfish::B_PAWN;
        case B_KNIGHT: return Stockfish::B_KNIGHT;
        case B_BISHOP: return Stockfish::B_BISHOP;
        case B_ROOK:   return Stockfish::B_ROOK;
        case B_QUEEN:  return Stockfish::B_QUEEN;
        case B_KING:   return Stockfish::B_KING;
        default:       return Stockfish::NO_PIECE;
    }
}

static Stockfish::Square to_sf_square(int sq) {
    return static_cast<Stockfish::Square>(sq);
}

// Rebuild shadow position from OctoKnight position
void refresh_accumulator(const Position& pos) {
    if (!initialized) return;
    
    if (!shadow) {
        shadow = std::make_unique<ShadowState>();
    }

    // Reset stack
    shadow->state_index = 0;
    if (shadow->state_stack.empty()) {
        shadow->state_stack.push_back(std::make_unique<Stockfish::StateInfo>());
    }
    
    // Build piece arrays for Stockfish::Position::set
    int pieces[33];
    int squares[33];
    int idx = 0;
    
    // Helper to add pieces
    auto add_pieces = [&](uint64_t bb, Stockfish::Piece p) {
        while (bb) {
            int sq = __builtin_ctzll(bb);
            pieces[idx] = p;
            squares[idx] = sq;
            idx++;
            bb &= bb - 1;
        }
    };
    
    add_pieces(pos.WhitePawns, Stockfish::W_PAWN);
    add_pieces(pos.WhiteKnights, Stockfish::W_KNIGHT);
    add_pieces(pos.WhiteBishops, Stockfish::W_BISHOP);
    add_pieces(pos.WhiteRooks, Stockfish::W_ROOK);
    add_pieces(pos.WhiteQueen, Stockfish::W_QUEEN);
    if (pos.WhiteKing) { pieces[idx] = Stockfish::W_KING; squares[idx] = __builtin_ctzll(pos.WhiteKing); idx++; }
    
    add_pieces(pos.BlackPawns, Stockfish::B_PAWN);
    add_pieces(pos.BlackKnights, Stockfish::B_KNIGHT);
    add_pieces(pos.BlackBishops, Stockfish::B_BISHOP);
    add_pieces(pos.BlackRooks, Stockfish::B_ROOK);
    add_pieces(pos.BlackQueen, Stockfish::B_QUEEN);
    if (pos.BlackKing) { pieces[idx] = Stockfish::B_KING; squares[idx] = __builtin_ctzll(pos.BlackKing); idx++; }
    
    shadow->pos.set(pieces, squares, idx, pos.whiteToMove, pos.move50rule, shadow->state_stack[0].get());
}

void make_move(const Position& pos, Move m) {
    if (!initialized) return;
    if (!shadow) refresh_accumulator(pos);
    
    // Ensure we have space in stack
    shadow->state_index++;
    if (shadow->state_index >= shadow->state_stack.size()) {
        shadow->state_stack.push_back(std::make_unique<Stockfish::StateInfo>());
    }
    
    Stockfish::StateInfo* st = shadow->state_stack[shadow->state_index].get();
    Stockfish::StateInfo* prev = shadow->state_stack[shadow->state_index - 1].get();
    
    // 1. Copy state from previous
    st->previous = prev;
    st->castlingRights = prev->castlingRights;
    st->rule50 = prev->rule50 + 1;
    st->pliesFromNull = prev->pliesFromNull + 1;
    st->epSquare = Stockfish::SQ_NONE;
    st->nonPawnMaterial[Stockfish::WHITE] = prev->nonPawnMaterial[Stockfish::WHITE];
    st->nonPawnMaterial[Stockfish::BLACK] = prev->nonPawnMaterial[Stockfish::BLACK];
    st->key = prev->key;
    
    Stockfish::Square from = to_sf_square(m.from);
    Stockfish::Square to = to_sf_square(m.to);
    Stockfish::Piece moved_piece = shadow->pos.piece_on(from);
    
    if (moved_piece == Stockfish::NO_PIECE) {
        // Desync detected, force refresh
        refresh_accumulator(pos);
        moved_piece = shadow->pos.piece_on(from);
        if (moved_piece == Stockfish::NO_PIECE) return;
        
        // Re-initialize st after refresh
        st = shadow->state_stack[shadow->state_index].get();
        prev = shadow->state_stack[shadow->state_index - 1].get();
        // Re-copy state
        st->previous = prev;
        st->castlingRights = prev->castlingRights;
        st->rule50 = prev->rule50 + 1;
        st->pliesFromNull = prev->pliesFromNull + 1;
        st->epSquare = Stockfish::SQ_NONE;
        st->nonPawnMaterial[Stockfish::WHITE] = prev->nonPawnMaterial[Stockfish::WHITE];
        st->nonPawnMaterial[Stockfish::BLACK] = prev->nonPawnMaterial[Stockfish::BLACK];
        st->key = prev->key;
    }
    
    Stockfish::PieceType pt = Stockfish::type_of(moved_piece);
    Stockfish::Color us = shadow->pos.side_to_move();
    Stockfish::Color them = ~us;
    
    // Reset rule50 if pawn move or capture
    bool is_capture = (shadow->pos.piece_on(to) != Stockfish::NO_PIECE);
    if (pt == Stockfish::PAWN || is_capture) {
        st->rule50 = 0;
    }
    
    // Prepare DirtyPiece
    Stockfish::DirtyPiece& dp = st->dirtyPiece;
    dp.dirty_num = 0;
    
    // Handle Capture
    Stockfish::Piece captured = shadow->pos.piece_on(to);
    if (captured != Stockfish::NO_PIECE) {
        dp.piece[dp.dirty_num] = captured;
        dp.from[dp.dirty_num] = to;
        dp.to[dp.dirty_num] = Stockfish::SQ_NONE;
        dp.dirty_num++;
        
        shadow->pos.remove_piece(to);
        st->capturedPiece = captured;
        
        if (Stockfish::type_of(captured) != Stockfish::PAWN) {
            st->nonPawnMaterial[them] -= Stockfish::PieceValue[captured];
        }
        
        // Update castling rights if rook captured
        if (Stockfish::type_of(captured) == Stockfish::ROOK) {
             if (to == Stockfish::SQ_A1) st->castlingRights &= ~2;
             if (to == Stockfish::SQ_H1) st->castlingRights &= ~1;
             if (to == Stockfish::SQ_A8) st->castlingRights &= ~8;
             if (to == Stockfish::SQ_H8) st->castlingRights &= ~4;
        }
    } else {
        st->capturedPiece = Stockfish::NO_PIECE;
    }
    
    // Handle En Passant Capture
    bool is_ep = false;
    if (pt == Stockfish::PAWN && captured == Stockfish::NO_PIECE) {
        if (Stockfish::file_of(from) != Stockfish::file_of(to)) {
            is_ep = true;
            Stockfish::Square cap_sq = Stockfish::make_square(Stockfish::file_of(to), Stockfish::rank_of(from));
            Stockfish::Piece cap_pawn = shadow->pos.piece_on(cap_sq);
            
            dp.piece[dp.dirty_num] = cap_pawn;
            dp.from[dp.dirty_num] = cap_sq;
            dp.to[dp.dirty_num] = Stockfish::SQ_NONE;
            dp.dirty_num++;
            
            shadow->pos.remove_piece(cap_sq);
            st->capturedPiece = cap_pawn;
        }
    }
    
    // Handle Promotion
    if (m.promotion != NO_PIECE) {
        Stockfish::Piece promo = to_sf_piece(m.promotion);
        
        dp.piece[dp.dirty_num] = moved_piece; // Pawn
        dp.from[dp.dirty_num] = from;
        dp.to[dp.dirty_num] = Stockfish::SQ_NONE;
        dp.dirty_num++;
        
        dp.piece[dp.dirty_num] = promo;
        dp.from[dp.dirty_num] = Stockfish::SQ_NONE;
        dp.to[dp.dirty_num] = to;
        dp.dirty_num++;
        
        shadow->pos.remove_piece(from);
        shadow->pos.put_piece(promo, to);
        
        st->nonPawnMaterial[us] += Stockfish::PieceValue[promo];
    } else {
        // Normal Move
        dp.piece[dp.dirty_num] = moved_piece;
        dp.from[dp.dirty_num] = from;
        dp.to[dp.dirty_num] = to;
        dp.dirty_num++;
        
        shadow->pos.remove_piece(from);
        shadow->pos.put_piece(moved_piece, to);
    }
    
    // Handle Castling Move
    if (pt == Stockfish::KING) {
        // Remove castling rights for moving king
        if (us == Stockfish::WHITE) st->castlingRights &= ~(1 | 2);
        else st->castlingRights &= ~(4 | 8);
        
        if (std::abs(to - from) == 2) {
            Stockfish::Square r_from, r_to;
            if (to > from) { // Kingside
                r_from = Stockfish::make_square(Stockfish::FILE_H, Stockfish::rank_of(from));
                r_to = Stockfish::make_square(Stockfish::FILE_F, Stockfish::rank_of(from));
            } else { // Queenside
                r_from = Stockfish::make_square(Stockfish::FILE_A, Stockfish::rank_of(from));
                r_to = Stockfish::make_square(Stockfish::FILE_D, Stockfish::rank_of(from));
            }
            
            Stockfish::Piece rook = shadow->pos.piece_on(r_from);
            
            dp.piece[dp.dirty_num] = rook;
            dp.from[dp.dirty_num] = r_from;
            dp.to[dp.dirty_num] = r_to;
            dp.dirty_num++;
            
            shadow->pos.remove_piece(r_from);
            shadow->pos.put_piece(rook, r_to);
        }
    }
    
    // Update Castling Rights for Rook Move
    if (pt == Stockfish::ROOK) {
         if (from == Stockfish::SQ_A1) st->castlingRights &= ~2;
         if (from == Stockfish::SQ_H1) st->castlingRights &= ~1;
         if (from == Stockfish::SQ_A8) st->castlingRights &= ~8;
         if (from == Stockfish::SQ_H8) st->castlingRights &= ~4;
    }
    
    // Update EP Square
    if (pt == Stockfish::PAWN && std::abs(to - from) == 16) {
        st->epSquare = static_cast<Stockfish::Square>((from + to) / 2);
    }
    
    // Update Position members
    shadow->pos.st = st;
    shadow->pos.sideToMove = ~shadow->pos.sideToMove;
    shadow->pos.gamePly++;
}

void undo_move(const Position& pos, Move m) {
    if (!initialized || !shadow || shadow->state_index == 0) return;
    
    Stockfish::StateInfo* st = shadow->state_stack[shadow->state_index].get();
    const Stockfish::DirtyPiece& dp = st->dirtyPiece;
    
    // Restore pieces in reverse order of DirtyPiece
    for (int i = dp.dirty_num - 1; i >= 0; i--) {
        Stockfish::Piece p = dp.piece[i];
        Stockfish::Square f = dp.from[i];
        Stockfish::Square t = dp.to[i];
        
        if (t != Stockfish::SQ_NONE) shadow->pos.remove_piece(t);
        if (f != Stockfish::SQ_NONE) shadow->pos.put_piece(p, f);
    }
    
    shadow->state_index--;
    shadow->pos.st = shadow->state_stack[shadow->state_index].get();
    shadow->pos.sideToMove = ~shadow->pos.sideToMove;
    shadow->pos.gamePly--;
}

void make_null_move(const Position& pos) {
    if (!initialized) return;
    if (!shadow) refresh_accumulator(pos);

    // Ensure we have space in stack
    shadow->state_index++;
    if (shadow->state_index >= shadow->state_stack.size()) {
        shadow->state_stack.push_back(std::make_unique<Stockfish::StateInfo>());
    }
    
    Stockfish::StateInfo* st = shadow->state_stack[shadow->state_index].get();
    Stockfish::StateInfo* prev = shadow->state_stack[shadow->state_index - 1].get();
    
    st->previous = prev;
    st->castlingRights = prev->castlingRights;
    st->rule50 = prev->rule50 + 1;
    st->pliesFromNull = 0;
    st->epSquare = Stockfish::SQ_NONE;
    st->nonPawnMaterial[Stockfish::WHITE] = prev->nonPawnMaterial[Stockfish::WHITE];
    st->nonPawnMaterial[Stockfish::BLACK] = prev->nonPawnMaterial[Stockfish::BLACK];
    st->key = prev->key;
    
    st->dirtyPiece.dirty_num = 0;
    st->capturedPiece = Stockfish::NO_PIECE;
    
    shadow->pos.st = st;
    shadow->pos.sideToMove = ~shadow->pos.sideToMove;
    shadow->pos.gamePly++;
}

void undo_null_move(const Position& pos) {
    if (!initialized || !shadow || shadow->state_index == 0) return;
    
    // Restore state
    shadow->state_index--;
    shadow->pos.st = shadow->state_stack[shadow->state_index].get();
    shadow->pos.sideToMove = ~shadow->pos.sideToMove;
    shadow->pos.gamePly--;
}

static long long eval_count = 0;
static long long eval_time_ns = 0;

int evaluate(const Position& pos) {
    if (!initialized) return 0;
    if (!shadow) refresh_accumulator(pos);
    
    auto start = std::chrono::high_resolution_clock::now();
    int score = Stockfish::Eval::evaluate(shadow->pos);
    auto end = std::chrono::high_resolution_clock::now();
    
    eval_count++;
    eval_time_ns += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    
    return score;
}

void print_stats() {
    std::cout << "NNUE Stats: " << eval_count << " calls, " 
              << (eval_time_ns / 1000000.0) << " ms, "
              << (eval_count > 0 ? (eval_time_ns / eval_count) : 0) << " ns/call" << std::endl;
}

} // namespace nnue

// Extern functions
int nnue_evaluate_position(Position& pos) {
    return nnue::evaluate(pos);
}

bool nnue_is_initialized() {
    return nnue::is_initialized();
}

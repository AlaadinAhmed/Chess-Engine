#include "see.hpp"
#include "utils.hpp"
#include "bitboard.hpp"
#include "magics.hpp"
#include "globals.hpp"
#include <algorithm>

static int piece_values[] = {
    300, 320, 500, 100, 900, 20000, // White: N, B, R, P, Q, K
    300, 320, 500, 100, 900, 20000, // Black: N, B, R, P, Q, K
    0 // NO_PIECE
};

static int get_piece_value(Pieces p) {
    if (p == NO_PIECE) return 0;
    return piece_values[p];
}

// Helper to find the least valuable attacker
static Pieces get_smallest_attacker(const Position& pos, int sq, uint64_t occupied, uint64_t& attackers, bool white_to_move) {
    uint64_t pawns = white_to_move ? pos.WhitePawns : pos.BlackPawns;
    uint64_t knights = white_to_move ? pos.WhiteKnights : pos.BlackKnights;
    uint64_t bishops = white_to_move ? pos.WhiteBishops : pos.BlackBishops;
    uint64_t rooks = white_to_move ? pos.WhiteRooks : pos.BlackRooks;
    uint64_t queens = white_to_move ? pos.WhiteQueen : pos.BlackQueen;
    uint64_t king = white_to_move ? pos.WhiteKing : pos.BlackKing;

    // Check pawns
    // White pawns attack from sq-7 and sq-9 (if occupied)
    // Wait, we want to find WHICH piece attacks 'sq'.
    // If 'sq' is attacked by a white pawn, that pawn must be at sq-7 or sq-9 (from white's perspective, pawns move +8).
    // White pawn at x attacks x+7 and x+9.
    // So if we are at 'sq', a white pawn attacking us must be at sq-7 or sq-9.
    // Black pawn at x attacks x-7 and x-9.
    // So if we are at 'sq', a black pawn attacking us must be at sq+7 or sq+9.
    
    uint64_t potential_pawns;
    if (white_to_move) {
        // We are looking for a white pawn attacking 'sq'.
        // It comes from 'sq' - 7 or 'sq' - 9 (checking boundaries).
        // Actually, simpler: use pawn attacks table.
        // whitePawnAttacks[s] gives squares attacked BY a white pawn at s.
        // We want squares that attack s.
        // The squares that attack 's' with a white pawn are the squares where a BLACK pawn at 's' would attack.
        potential_pawns = blackPawnAttacks[sq] & pawns & occupied;
    } else {
        // Looking for black pawn attacking 'sq'.
        potential_pawns = whitePawnAttacks[sq] & pawns & occupied;
    }
    
    if (potential_pawns) {
        attackers = potential_pawns;
        return white_to_move ? W_PAWN : B_PAWN;
    }

    // Knights
    uint64_t potential_knights = knightAttacks[sq] & knights & occupied;
    if (potential_knights) {
        attackers = potential_knights;
        return white_to_move ? W_KNIGHT : B_KNIGHT;
    }

    // Bishops
    uint64_t bishop_attacks = get_bishop_attacks(sq, occupied);
    uint64_t potential_bishops = bishop_attacks & bishops & occupied;
    if (potential_bishops) {
        attackers = potential_bishops;
        return white_to_move ? W_BISHOP : B_BISHOP;
    }

    // Rooks
    uint64_t rook_attacks = get_rook_attacks(sq, occupied);
    uint64_t potential_rooks = rook_attacks & rooks & occupied;
    if (potential_rooks) {
        attackers = potential_rooks;
        return white_to_move ? W_ROOK : B_ROOK;
    }

    // Queens
    uint64_t potential_queens = (bishop_attacks | rook_attacks) & queens & occupied;
    if (potential_queens) {
        attackers = potential_queens;
        return white_to_move ? W_QUEEN : B_QUEEN;
    }

    // King
    uint64_t potential_king = kingAttacks[sq] & king & occupied;
    if (potential_king) {
        attackers = potential_king;
        return white_to_move ? W_KING : B_KING;
    }

    return NO_PIECE;
}

int see(const Position& pos, Move move) {
    int gain[32];
    int d = 0;
    
    int from = move.from;
    int to = move.to;
    Pieces piece = get_piece_at(pos, from);
    Pieces captured = get_piece_at(pos, to);
    
    gain[d] = get_piece_value(captured);
    
    // Handle en passant
    if (piece == W_PAWN || piece == B_PAWN) {
        if (pos.enPassant && to == __builtin_ctzll(pos.enPassant)) {
            gain[d] = 100; // Pawn value
        }
    }
    
    // Handle promotion
    if (move.promotion != NO_PIECE) {
        gain[d] += get_piece_value(move.promotion) - get_piece_value(piece);
        piece = move.promotion;
    }
    
    uint64_t occupied = pos.occupiedSquares;
    // Remove the moving piece
    occupied &= ~(1ULL << from);
    // Add the moving piece to destination (it's there now)
    occupied |= (1ULL << to);
    
    bool side = !pos.whiteToMove; // Side to move after the initial capture
    
    // Find attackers to the 'to' square
    // We need to find the least valuable attacker from 'side'
    
    while (true) {
        d++;
        gain[d] = get_piece_value(piece) - gain[d-1];
        
        if (std::max(-gain[d-1], gain[d]) < 0) break;
        
        // Find least valuable attacker
        uint64_t attackers_bb = 0;
        Pieces attacker_type = NO_PIECE;
        
        // We need to find an attacker from 'side'
        // But we need to be careful about X-rays.
        // The 'occupied' bitboard changes.
        
        // Re-scanning for attackers is expensive but correct for SEE.
        // Optimized SEE uses an attackers bitboard and updates it.
        // For simplicity, let's re-scan or use a simpler approach.
        
        // Let's try to find the smallest attacker.
        // We need to know which square the attacker is on to remove it.
        
        // Pawns
        uint64_t pawns = side ? pos.WhitePawns : pos.BlackPawns; // side=true means White (0 is white in my code? No, side is bool)
        // Wait, pos.whiteToMove is bool.
        // In my code: bool side = !pos.whiteToMove;
        // If pos.whiteToMove is true (White), then side is false (Black).
        // So side=false means Black?
        // Let's stick to: side is the color of the player whose turn it is to recapture.
        // If initial move was White, side is Black.
        
        bool is_white = (side == (pos.whiteToMove ? false : true)); // Wait.
        // If pos.whiteToMove is true, we made a move. Now it's Black's turn to recapture.
        // So side should be Black.
        // My `side` variable is initialized to `!pos.whiteToMove`.
        // If pos.whiteToMove is true, side is false (Black).
        // So side=false means Black?
        // Let's check `get_smallest_attacker` usage.
        // It takes `bool white_to_move`.
        // So I should pass `side` directly if side=true means White.
        // If pos.whiteToMove is true, side is false.
        // So side=false means Black.
        // But `get_smallest_attacker` expects true for White.
        // So `side` variable name is confusing. Let's use `stm` (side to move).
        
        bool stm = !pos.whiteToMove; // The side that will recapture
        
        // Find LVA
        uint64_t attackers_mask;
        Pieces lva = get_smallest_attacker(pos, to, occupied, attackers_mask, stm);
        
        if (lva == NO_PIECE) break;
        
        // Get the square of the attacker
        int attacker_sq = __builtin_ctzll(attackers_mask);
        
        // Remove attacker from occupied
        occupied &= ~(1ULL << attacker_sq);
        
        piece = lva;
        stm = !stm;
    }
    
    while (--d > 0) {
        gain[d-1] = -std::max(-gain[d-1], gain[d]);
    }
    
    return gain[0];
}

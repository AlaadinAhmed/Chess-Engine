#include "position.hpp"
#include "movegen.hpp"
#include "fen.hpp"
#include "globals.hpp"
#include "utils.hpp"
#include <iostream>

int main() {
    Zobrist zkey_local;
    zkey_local.initKeys();
    history_ply = 0;
    
    Position pos;
    parseFEN(pos, "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    pos.zobrist_key = calculate_initial_hash(pos);
    
    uint64_t orig_zobrist = pos.zobrist_key;
    uint64_t orig_wking = pos.WhiteKing;
    int orig_ksq = __builtin_ctzll(orig_wking);
    
    std::cout << "Original: zobrist=" << std::hex << orig_zobrist << std::dec 
              << ", king sq=" << orig_ksq << std::endl;
    
    // Generate and make/unmake all moves
    MoveList moves;
    generate_moves(pos, moves);
    std::cout << "Testing " << moves.count << " moves..." << std::endl;
    
    for (int i = 0; i < moves.count; i++) {
        makemove(pos, moves.moves[i]);
        undomove(pos, moves.moves[i]);
        
        if (pos.zobrist_key != orig_zobrist) {
            std::cout << "ZOBRIST MISMATCH after move " << i << std::endl;
            return 1;
        }
        if (pos.WhiteKing != orig_wking) {
            std::cout << "KING BITBOARD MISMATCH after move " << i << ": "
                      << move_to_uci(moves.moves[i]) << std::endl;
            std::cout << "Expected: " << std::hex << orig_wking 
                      << ", got: " << pos.WhiteKing << std::dec << std::endl;
            return 1;
        }
    }
    
    std::cout << "All moves tested successfully!" << std::endl;
    std::cout << "Final: zobrist=" << std::hex << pos.zobrist_key << std::dec 
              << ", king sq=" << __builtin_ctzll(pos.WhiteKing) << std::endl;
    
    return 0;
}

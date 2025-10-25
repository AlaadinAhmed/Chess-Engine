#include "position.hpp"
#include "movegen.hpp"
#include "fen.hpp"
#include "globals.hpp"
#include "utils.hpp"
#include <iostream>
#include <iomanip>

void print_bitboards(const Position& pos, const std::string& label) {
    std::cout << "\n=== " << label << " ===" << std::endl;
    std::cout << "WhiteKing: 0x" << std::hex << std::setw(16) << std::setfill('0') << pos.WhiteKing << std::dec << std::endl;
    std::cout << "BlackKing: 0x" << std::hex << std::setw(16) << std::setfill('0') << pos.BlackKing << std::dec << std::endl;
    std::cout << "WhiteoccupiedSquares: 0x" << std::hex << std::setw(16) << std::setfill('0') << pos.WhiteoccupiedSquares << std::dec << std::endl;
    std::cout << "BlackoccupiedSquares: 0x" << std::hex << std::setw(16) << std::setfill('0') << pos.BlackoccupiedSquares << std::dec << std::endl;
    std::cout << "occupiedSquares: 0x" << std::hex << std::setw(16) << std::setfill('0') << pos.occupiedSquares << std::dec << std::endl;
    std::cout << "whiteToMove: " << pos.whiteToMove << std::endl;
    std::cout << "castling: " << (int)pos.castelingRights << std::endl;
    std::cout << "enPassant: " << pos.enPassant << std::endl;
}

int main() {
    history_ply = 0;
    
    Position pos;
    parseFEN(pos, "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    
    print_bitboards(pos, "ORIGINAL");
    
    // Generate first legal move
    MoveList moves;
    generate_moves(pos, moves);
    
    Move first_move = moves.moves[0];
    std::cout << "\nFirst move: " << move_to_uci(first_move) << std::endl;
    
    makemove(pos, first_move);
    print_bitboards(pos, "AFTER MAKEMOVE");
    
    undomove(pos, first_move);
    print_bitboards(pos, "AFTER UNDOMOVE");
    
    return 0;
}

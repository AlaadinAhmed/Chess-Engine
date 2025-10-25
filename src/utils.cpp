#include "utils.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include "globals.hpp"
#include "fen.hpp"
#include "bitboard.hpp"
#include "magics.hpp"
#include "movegen.hpp"

Move uci_to_move(std::string uci_move) {
    int from_file = uci_move[0] - 'a';
    int from_rank = uci_move[1] - '1';
    int to_file = uci_move[2] - 'a';
    int to_rank = uci_move[3] - '1';
    int from = from_rank * 8 + from_file;
    int to = to_rank * 8 + to_file;
    return {from, to};
}

std::string move_to_uci(Move move) {
    std::string uci_move = "";
    int from_file = move.from % 8;
    int from_rank = move.from / 8;
    int to_file = move.to % 8;
    int to_rank = move.to / 8;
    uci_move += (char)('a' + from_file);
    uci_move += (char)('1' + from_rank);
    uci_move += (char)('a' + to_file);
    uci_move += (char)('1' + to_rank);
    return uci_move;
}

void log_debug(const std::string& message) {
    if (debug_mode) {
        std::ofstream log_file("debug.log", std::ios_base::app);
        if (log_file.is_open()) {
            log_file << message << std::endl;
        }
        std::cout << "info string Debug: " << message << std::endl;
    }
}

// Simple text-based opening book: lines of "<fen>;<uci>"
std::string get_book_move(const Position &pos, const std::string &book_file_path) {
    std::ifstream fin(book_file_path);
    if (!fin.is_open()) {
        return std::string();
    }
    std::string line;
    while (std::getline(fin, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string fen_str, uci;
        if (!std::getline(ss, fen_str, ';')) continue;
        if (!std::getline(ss, uci)) continue;
        Position cmp;
        parseFEN(cmp, fen_str);
        if (cmp.WhitePawns == pos.WhitePawns && cmp.BlackPawns == pos.BlackPawns &&
            cmp.WhiteKnights == pos.WhiteKnights && cmp.BlackKnights == pos.BlackKnights &&
            cmp.WhiteBishops == pos.WhiteBishops && cmp.BlackBishops == pos.BlackBishops &&
            cmp.WhiteRooks == pos.WhiteRooks && cmp.BlackRooks == pos.BlackRooks &&
            cmp.WhiteQueen == pos.WhiteQueen && cmp.BlackQueen == pos.BlackQueen &&
            cmp.WhiteKing == pos.WhiteKing && cmp.BlackKing == pos.BlackKing &&
            cmp.whiteToMove == pos.whiteToMove && cmp.castelingRights == pos.castelingRights &&
            cmp.enPassant == pos.enPassant) return uci;
    }
    return std::string();
}

const int see_piece_values[] = {0, 100, 320, 330, 500, 900, 10000}; // NO_PIECE_TYPE, PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING

// This is a helper function for SEE
static Bitboard get_attackers(const Position& pos, int square, int side, Bitboard occupied) {
    Bitboard pawns, knights, bishops, rooks, queens, kings;
    if (side == 0) { // WHITE
        pawns = pos.WhitePawns;
        knights = pos.WhiteKnights;
        bishops = pos.WhiteBishops;
        rooks = pos.WhiteRooks;
        queens = pos.WhiteQueen;
        kings = pos.WhiteKing;
    } else { // BLACK
        pawns = pos.BlackPawns;
        knights = pos.BlackKnights;
        bishops = pos.BlackBishops;
        rooks = pos.BlackRooks;
        queens = pos.BlackQueen;
        kings = pos.BlackKing;
    }

    Bitboard attackers = 0;
    attackers |= GetPawnAttacks(pos, square, !side) & pawns;
    attackers |= GetKnightAttacks(pos, square) & knights;
    attackers |= get_bishop_attacks(square, occupied) & (bishops | queens);
    attackers |= get_rook_attacks(square, occupied) & (rooks | queens);
    attackers |= kingAttacks[square] & kings;
    return attackers;
}

int see(const Position &pos, Move move) {
    int gain[32];
    int d = 0;

    Bitboard from_bb = 1ULL << move.from;
    Bitboard occupied = pos.occupiedSquares;
    Pieces from_piece = get_piece_at(pos, move.from);
    Pieces to_piece = get_piece_at(pos, move.to);

    if (to_piece == NO_PIECE) return 0;

    gain[d] = see_piece_values[get_piece_type(to_piece)];

    int side = pos.whiteToMove;
    Bitboard attackers = get_attackers(pos, move.to, !side, occupied ^ from_bb) | from_bb;

    while (true) {
        d++;
        side = !side;
        attackers &= occupied;

        Bitboard side_pieces = (side == 0) ? pos.WhiteoccupiedSquares : pos.BlackoccupiedSquares;
        Bitboard side_attackers = attackers & side_pieces;

        if (!side_attackers) break;

        int attacker_piece_type = NO_PIECE_TYPE;
        Bitboard attacker_bb = 0;

        // Find the smallest attacker
        Bitboard pawns = (side == 0) ? pos.WhitePawns : pos.BlackPawns;
        if (side_attackers & pawns) {
            attacker_piece_type = PAWN;
            attacker_bb = side_attackers & pawns;
        } else {
            Bitboard knights = (side == 0) ? pos.WhiteKnights : pos.BlackKnights;
            if (side_attackers & knights) {
                attacker_piece_type = KNIGHT;
                attacker_bb = side_attackers & knights;
            } else {
                Bitboard bishops = (side == 0) ? pos.WhiteBishops : pos.BlackBishops;
                if (side_attackers & bishops) {
                    attacker_piece_type = BISHOP;
                    attacker_bb = side_attackers & bishops;
                } else {
                    Bitboard rooks = (side == 0) ? pos.WhiteRooks : pos.BlackRooks;
                    if (side_attackers & rooks) {
                        attacker_piece_type = ROOK;
                        attacker_bb = side_attackers & rooks;
                    } else {
                        Bitboard queens = (side == 0) ? pos.WhiteQueen : pos.BlackQueen;
                        if (side_attackers & queens) {
                            attacker_piece_type = QUEEN;
                            attacker_bb = side_attackers & queens;
                        } else {
                            Bitboard kings = (side == 0) ? pos.WhiteKing : pos.BlackKing;
                            if (side_attackers & kings) {
                                attacker_piece_type = KING;
                                attacker_bb = side_attackers & kings;
                            } else {
                                break; // No more attackers
                            }
                        }
                    }
                }
            }
        }

        attacker_bb &= -attacker_bb; // get the lsb

        gain[d] = see_piece_values[attacker_piece_type] - gain[d-1];
        if (std::max(-gain[d-1], gain[d]) < 0) break;

        occupied ^= attacker_bb;
        attackers |= get_bishop_attacks(move.to, occupied) | get_rook_attacks(move.to, occupied);
    }

    while (--d) {
        gain[d-1] = -std::max(-gain[d-1], gain[d]);
    }

    return gain[0];
}
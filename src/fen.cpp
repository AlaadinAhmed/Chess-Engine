#include <string>
#include <iostream>
#include <sstream>
#include "position.hpp"
#include "print.hpp"
#include "fen.hpp"
#include "globals.hpp"

void parseFEN(Position &pos, std::string fen){
    pos.reset();
    std::stringstream ss(fen);
    std::string board_str, side_to_move_str, castling_str, en_passant_str, halfmove_clock_str, fullmove_number_str;

    ss >> board_str >> side_to_move_str >> castling_str >> en_passant_str >> halfmove_clock_str >> fullmove_number_str;

    int current_rank = 0;
    int current_file = 0;
    for (char c : board_str) {
        if (c == '/') {
            current_rank++;
            current_file = 0;
        } else if (isdigit(c)) {
            current_file += (c - '0');
        } else {
            uint64_t mask = 1ULL << ((7 - current_rank) * 8 + current_file);
            switch (c) {
                case 'P': pos.WhitePawns |= mask; break;
                case 'R': pos.WhiteRooks |= mask; break;
                case 'N': pos.WhiteKnights |= mask; break;
                case 'B': pos.WhiteBishops |= mask; break;
                case 'Q': pos.WhiteQueen |= mask; break;
                case 'K': pos.WhiteKing |= mask; break;
                case 'p': pos.BlackPawns |= mask; break;
                case 'r': pos.BlackRooks |= mask; break;
                case 'n': pos.BlackKnights |= mask; break;
                case 'b': pos.BlackBishops |= mask; break;
                case 'q': pos.BlackQueen |= mask; break;
                case 'k': pos.BlackKing |= mask; break;
                default:
                    std::cout << "info string Error parsing FEN: Invalid character in board string: " << c << std::endl;
                    return;
            }
            current_file++;
        }
    }

    pos.whiteToMove = (side_to_move_str == "w");

    pos.castelingRights = 0;
    for (char c : castling_str) {
        switch (c) {
            case 'K': pos.castelingRights |= 1; break;
            case 'Q': pos.castelingRights |= 2; break;
            case 'k': pos.castelingRights |= 4; break;
            case 'q': pos.castelingRights |= 8; break;
            case '-': break;
            default:
                std::cout << "info string Error parsing FEN: Invalid character in castling rights string: " << c << std::endl;
                return;
        }
    }

    if (en_passant_str == "-") {
        pos.enPassant = 0;
    } else {
        if (en_passant_str.length() == 2) {
            int file = en_passant_str[0] - 'a';
            int rank = en_passant_str[1] - '1';
            if (file >= 0 && file < 8 && rank >= 0 && rank < 8) {
                pos.enPassant = 1ULL << (rank * 8 + file);
            } else {
                std::cout << "info string Error parsing FEN: Invalid en passant square: " << en_passant_str << std::endl;
                return;
            }
        } else {
            std::cout << "info string Error parsing FEN: Invalid en passant string: " << en_passant_str << std::endl;
            return;
        }
    }

    try {
        pos.move50rule = std::stoi(halfmove_clock_str);
    } catch (const std::invalid_argument& ia) {
        std::cout << "info string Error parsing FEN: Invalid halfmove clock value: " << halfmove_clock_str << std::endl;
        return;
    }

    try {
        pos.move = std::stoi(fullmove_number_str);
    } catch (const std::invalid_argument& ia) {
        std::cout << "info string Error parsing FEN: Invalid fullmove number value: " << fullmove_number_str << std::endl;
        return;
    }


    pos.BlackoccupiedSquares = 0;
    pos.BlackoccupiedSquares |= (uint64_t)pos.BlackBishops;
    pos.BlackoccupiedSquares |= (uint64_t)pos.BlackKing;
    pos.BlackoccupiedSquares |= (uint64_t)pos.BlackKnights;
    pos.BlackoccupiedSquares |= (uint64_t)pos.BlackPawns;
    pos.BlackoccupiedSquares |= (uint64_t)pos.BlackQueen;
    pos.BlackoccupiedSquares |= (uint64_t)pos.BlackRooks;
    
    pos.WhiteoccupiedSquares = pos.WhiteBishops | pos.WhiteKing | pos.WhiteKnights | pos.WhitePawns | pos.WhiteQueen | pos.WhiteRooks;
    pos.occupiedSquares = pos.BlackoccupiedSquares | pos.WhiteoccupiedSquares;
    pos.emptySquares = ~pos.occupiedSquares;
}

void positionToFEN(const Position &pos, char *fen_string) {
    int char_idx = 0;
    for (int rank = 7; rank >= 0; rank--) {
        int empty_squares = 0;
        for (int file = 0; file < 8; file++) {
            int square = rank * 8 + file;
            Pieces piece = get_piece_at(pos, square); // Assuming get_piece_at is available

            if (piece == NO_PIECE) {
                empty_squares++;
            } else {
                if (empty_squares > 0) {
                    fen_string[char_idx++] = '0' + empty_squares;
                    empty_squares = 0;
                }
                switch (piece) {
                    case W_PAWN: fen_string[char_idx++] = 'P'; break;
                    case W_KNIGHT: fen_string[char_idx++] = 'N'; break;
                    case W_BISHOP: fen_string[char_idx++] = 'B'; break;
                    case W_ROOK: fen_string[char_idx++] = 'R'; break;
                    case W_QUEEN: fen_string[char_idx++] = 'Q'; break;
                    case W_KING: fen_string[char_idx++] = 'K'; break;
                    case B_PAWN: fen_string[char_idx++] = 'p'; break;
                    case B_KNIGHT: fen_string[char_idx++] = 'n'; break;
                    case B_BISHOP: fen_string[char_idx++] = 'b'; break;
                    case B_ROOK: fen_string[char_idx++] = 'r'; break;
                    case B_QUEEN: fen_string[char_idx++] = 'q'; break;
                    case B_KING: fen_string[char_idx++] = 'k'; break;
                    case NO_PIECE: break; // Should not happen
                }
            }
        }
        if (empty_squares > 0) {
            fen_string[char_idx++] = '0' + empty_squares;
        }
        if (rank > 0) {
            fen_string[char_idx++] = '/';
        }
    }

    fen_string[char_idx++] = ' ';
    fen_string[char_idx++] = pos.whiteToMove ? 'w' : 'b';
    fen_string[char_idx++] = ' ';

    if (pos.castelingRights == 0) {
        fen_string[char_idx++] = '-';
    } else {
        if (pos.castelingRights & 1) fen_string[char_idx++] = 'K';
        if (pos.castelingRights & 2) fen_string[char_idx++] = 'Q';
        if (pos.castelingRights & 4) fen_string[char_idx++] = 'k';
        if (pos.castelingRights & 8) fen_string[char_idx++] = 'q';
    }
    fen_string[char_idx++] = ' ';

    if (pos.enPassant == 0) {
        fen_string[char_idx++] = '-';
    } else {
        int ep_sq = __builtin_ctzll(pos.enPassant);
        fen_string[char_idx++] = ('a' + (ep_sq % 8));
        fen_string[char_idx++] = ('1' + (ep_sq / 8));
    }
    fen_string[char_idx++] = ' ';

    char temp[10];
    sprintf(temp, "%d", pos.move50rule);
    for (int i = 0; temp[i] != '\0'; i++) {
        fen_string[char_idx++] = temp[i];
    }
    fen_string[char_idx++] = ' ';

    sprintf(temp, "%d", pos.move);
    for (int i = 0; temp[i] != '\0'; i++) {
        fen_string[char_idx++] = temp[i];
    }
    fen_string[char_idx] = '\0';
}

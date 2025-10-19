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
        }
    }

    if (en_passant_str == "-") {
        pos.enPassant = 0;
    } else {
        int file = en_passant_str[0] - 'a';
        int rank = en_passant_str[1] - '1';
        pos.enPassant = rank * 8 + file;
    }

    pos.move50rule = std::stoi(halfmove_clock_str);
    pos.move = std::stoi(fullmove_number_str);

    pos.BlackoccupiedSquares = pos.BlackBishops | pos.BlackKing | pos.BlackKnights | pos.BlackPawns | pos.BlackQueen | pos.BlackRooks;
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
        fen_string[char_idx++] = ('a' + (pos.enPassant % 8));
        fen_string[char_idx++] = ('1' + (pos.enPassant / 8));
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

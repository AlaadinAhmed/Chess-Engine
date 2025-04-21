#include "fen.hpp"
void parseFEN(Position &pos, std::string fen = defaultFEN){
    int rank = 0;
    for (int i = 0; i < fen.length(); i++){
        int file = 8;
        if (fen[i] == '/'){
            rank++;
        }
        else if (isdigit(fen[i])){
            file = file - (fen[i] - '0');
        }
        else{
            uint64_t mask = 1ULL << (rank*8 + (8-file));
            switch (fen[i])
            {
            case 'P': pos.BlackPawns |= mask; break;
            case 'R': pos.BlackRooks |= mask; break;
            case 'N': pos.BlackKnights |= mask; break;
            case 'B': pos.BlackBishops |= mask; break;
            case 'Q': pos.BlackQueen |= mask; break;
            case 'K': pos.BlackKing |= mask; break;
            
            case 'r': pos.WhiteRooks |= mask; break;
            case 'n': pos.WhiteKnights |= mask; break;
            case 'b': pos.WhiteBishops |= mask; break;
            case 'q': pos.WhiteQueen |= mask; break;
            case 'k': pos.WhiteKing |= mask; break;
            case 'p': pos.WhitePawns |= mask; break;
            
            default:  break;
            }
            file++;
        }
        if (rank > 6){
            switch (fen[i])
            {
            case 'w': pos.whiteToMove = true; break;
            case 'b': pos.whiteToMove = false; break;
            default:  break;
            }
            rank ++;
        }
        if (rank = 7){
            switch (fen[i])
            {
            case 'K': pos.castlingRights |= 0b001000; break;
            case 'Q': pos.castlingRights |= 0b000100; break;
            
            case 'k': pos.castlingRights |= 0b000010; break;
            case 'q': pos.castlingRights |= 0b000001; break;
            
            default:  break;
            }
            rank++;
        }
        if (rank = 8){
            if (fen[i] == '-'){
                pos.unpassant = 0;
            }
            else {
                int square = 0;
                square = square + (fen[i] - 'a');
                if (isdigit(fen[i])){
                    square = (fen[i] - '1')*8;
                }
                uint64_t mask = 1ULL << square;
                pos.unpassant |= mask;
            }
            rank++;
        }
        if (rank = 9){
            pos.move50rule = fen[i] - '0';
            rank++;
        }
        if (rank = 10){
            pos.move = fen[i] - '0';
        }
    }
    pos.BlackoccupiedSquares = pos.BlackBishops | pos.BlackKing | pos.BlackKnights | pos.BlackPawns | pos.BlackQueen | pos.BlackRooks;
    pos.WhiteoccupiedSquares = pos.WhiteBishops | pos.WhiteKing | pos.WhiteKnights | pos.WhitePawns | pos.WhiteQueen | pos.WhiteRooks;
    pos.occupiedSquares = pos.BlackoccupiedSquares | pos.WhiteoccupiedSquares;
};
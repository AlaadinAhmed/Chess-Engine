#include <string>
#include <iostream>
#include "position.hpp"
#include "print.hpp"
#include "fen.hpp"
void parseFEN(Position &pos, std::string fen = defaultFEN){
    int rank = 0;
    int file = 8;
    for (int i = 0; i < fen.length(); i++){
        if (fen[i] == '/'){
            rank++;
            file = 8;
        }
        else if (isdigit(fen[i])){
            file = file - (fen[i] - '0');
        }
        else{
            uint64_t mask = 0x8000000000000000ULL >> ((rank*8) + (8-file));
            if (rank < 8){
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
            

            
            default: rank++; i++; break;
            }
        }
            file--;
        }
        if (rank == 8){
            switch (fen[i])
            {
            case 'w': pos.whiteToMove = true; break;
            case 'b': pos.whiteToMove = false; break;
            default:  break;
            }
            rank ++;
            i++;
        }
        else if (rank == 9){
            switch (fen[i])
            {
            case 'K': pos.castelingRights |= 0b001000; break;
            case 'Q': pos.castelingRights |= 0b000100; break;
            
            case 'k': pos.castelingRights |= 0b000010; break;
            case 'q': pos.castelingRights |= 0b000001; break;
            
            default:  break;
            }
            rank++;
            i++;
        }
        else if (rank == 10){
            if (fen[i] == '-'){
                pos.enPassant = 0;
            }
            else {
                int square = 0;
                square = square + (fen[i] - 'a');
                if (isdigit(fen[i])){
                    square = (fen[i] - '1')*8;
                }
                uint64_t mask = 1ULL << square;
                pos.enPassant |= mask;
            }
            rank++;
            i++;
        }
        else if (rank == 11){
            pos.move50rule = fen[i] - '0';
            rank++;
            i++;
        }
        else if (rank == 12){
            pos.move = fen[i] - '0';
        }
        // std::cout << i << " : " << std::endl;
        // std::cout << "Black Knight" << std::endl;
        // printBoard(pos.occupiedSquares);
        // std::cout << "White Knight" << std::endl;
        // printBoard(pos.occupiedSquares);
        // std::cout << "Knight" << std::endl;
        // printBoard(pos.occupiedSquares);
        // std::cout << "Knight" << std::endl;
        // printBoard(pos.occupiedSquares);
        // std::cout << "Knight" << std::endl;
        // printBoard(pos.occupiedSquares);
        // std::cout << "Knight" << std::endl;
        // printBoard(pos.occupiedSquares);
        // std::cout << "Knight" << std::endl;
        // printBoard(pos.occupiedSquares);
        // std::cout << "Knight" << std::endl;
        // printBoard(pos.occupiedSquares);
        // std::cout << "Knight" << std::endl;
        // printBoard(pos.occupiedSquares);
        // std::cout << "Knight" << std::endl;
        // printBoard(pos.occupiedSquares);
        // std::cout << "Knight" << std::endl;
        // printBoard(pos.occupiedSquares);
        // std::cout << "Knight" << std::endl;
        // printBoard(pos.occupiedSquares);
    }
    pos.BlackoccupiedSquares = pos.BlackBishops | pos.BlackKing | pos.BlackKnights | pos.BlackPawns | pos.BlackQueen | pos.BlackRooks;
    pos.WhiteoccupiedSquares = pos.WhiteBishops | pos.WhiteKing | pos.WhiteKnights | pos.WhitePawns | pos.WhiteQueen | pos.WhiteRooks;
    pos.occupiedSquares = pos.BlackoccupiedSquares | pos.WhiteoccupiedSquares;
    pos.emptySquares = ~pos.occupiedSquares;
};
#pragma once
inline void printBoard(uint64_t pieces) {
    for (int i = 0; i < 64; i++) {
        // Print a piece if the current bit is set
        switch (i)
        {
        case 0:
            std::cout << "8   ";
        break;
        case 8:
            std::cout << "7   ";
        break;
        case 16:
            std::cout << "6   ";
        break;
        case 24:
            std::cout << "5   ";
        break;
        case 32:
            std::cout << "4   ";
        break;
        case 40:
            std::cout << "3   ";
        break;
        case 48:
            std::cout << "2   ";
        break;
        case 56:
            std::cout << "1   ";
        break;
        
        default:
            break;
        }
        if (pieces & (1ULL << (63 - i))) {
            std::cout << "1 ";  // "X" represents a piece
        } else {
            std::cout << "0 ";  // "." represents an empty square
        }
        // After every 8 bits (one rank), print a new line
        if ((i + 1) % 8 == 0) {
            std::cout << std::endl;
        }
    }
    std::cout << "\n    a b c d e f g h \n";
}
inline void printSequentialBoard(const uint64_t pieces[]) {
    for (int j = 0; j < 64; j++){
        std::string square;
        int rank = 0;
        rank = j / 8;
        switch (j - rank*8)
        {
        case 7:
            square += "h";
            break;
        case 6:
            square += "g";
            break;
        case 5:
            square += "f";
            break;
        case 4:
            square += "e";
            break;
        case 3:
            square += "d";
            break;
        case 2:
            square += "c";
            break;
        case 1:
            square += "b";
            break;
        case 0:
            square += "a";
            break;
        
        default:
            break;
        }
        square += std::to_string(8 - rank);
        std::cout << square << " :   " << std::endl;
        printBoard(pieces[j]);
        std::cout << std::endl << std::endl;
    }
}
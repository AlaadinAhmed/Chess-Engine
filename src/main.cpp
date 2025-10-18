#include "bitboard.hpp"
#include "fen.hpp"
#include "hash.hpp"
#include "magics.hpp"
#include "movegen.hpp"
#include "position.hpp"
#include "print.hpp"
#include "search.hpp"
#include "tt.hpp"
#include "utils.hpp"
#include <cstdint>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include "globals.hpp"

void uci_loop() {
    Position board;
    std::string line;
    while (std::getline(std::cin, line)) {
        std::vector<std::string> tokens;
        std::string current_token;
        for (char c : line) {
            if (c == ' ') {
                tokens.push_back(current_token);
                current_token = "";
            } else {
                current_token += c;
            }
        }
        tokens.push_back(current_token);

        if (tokens[0] == "uci") {
            std::cout << "id name OctoKnight" << std::endl;
            std::cout << "id author Aladdin" << std::endl;
            std::cout << "uciok" << std::endl;
        } else if (tokens[0] == "isready") {
            std::cout << "readyok" << std::endl;
        } else if (tokens[0] == "ucinewgame") {
            board.reset();
            tt.clear();
        } else if (tokens[0] == "position") {
            if (tokens[1] == "startpos") {
                parseFEN(board, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
                currentHashKey = calculate_initial_hash(board);
                if (tokens.size() > 2 && tokens[2] == "moves") {
                    for (int i = 3; i < tokens.size(); i++) {
                        Move m = uci_to_move(tokens[i]);
                        makemove(board, m);
                    }
                }
            } else if (tokens[1] == "fen") {
                std::string fen = "";
                int i = 2;
                while (i < tokens.size() && tokens[i] != "moves") {
                    fen += tokens[i] + " ";
                    i++;
                }
                parseFEN(board, fen);
                currentHashKey = calculate_initial_hash(board);
                if (i < tokens.size() && tokens[i] == "moves") {
                    for (int j = i + 1; j < tokens.size(); j++) {
                        Move m = uci_to_move(tokens[j]);
                        makemove(board, m);
                    }
                }
            }
        } else if (tokens[0] == "go") {
            searching = true;
            std::thread search_thread([&]() {
                Move best_move;
                for (int depth = 1; depth <= 10; depth++) {
                    if (!searching) {
                        break;
                    }
                    int score = search(board, depth, -100000, 100000, best_move);
                    if (searching) {
                        std::cout << "info depth " << depth << " score cp " << score << " pv " << move_to_uci(best_move) << std::endl;
                    }
                }
                if (searching) {
                    std::cout << "bestmove " << move_to_uci(best_move) << std::endl;
                }
                searching = false;
            });
            search_thread.detach();
        } else if (tokens[0] == "stop") {
            searching = false;
        } else if (tokens[0] == "quit") {
            break;
        }
    }
}

void print_help() {
    std::cout << "OctoKnight - A UCI-compliant chess engine." << std::endl;
    std::cout << "Usage: ./OctoKnight [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --help, -h    Show this help message." << std::endl;
    std::cout << std::endl;
    std::cout << "For more information on how to use the engine, see MANUAL.md." << std::endl;
}

int main(int argc, char* argv[]) {
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--help" || arg == "-h") {
            print_help();
            return 0;
        }
    }

    zkey.initKeys();
    init_magics();
    initKingAttacks();
    tt.clear();

    uci_loop();

    return 0;
}
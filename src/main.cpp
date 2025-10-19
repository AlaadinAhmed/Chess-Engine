#include "bitboard.hpp"
#include "eval.hpp"
#include "fen.hpp"
#include "globals.hpp"
#include "hash.hpp"
#include "magics.hpp"
#include "movegen.hpp"
#include "position.hpp"
#include "print.hpp"
#include "search.hpp"
#include "tt.hpp"
#include "utils.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <sstream>

int main() {
    init_magics();
    initKingAttacks();
    zkey.initKeys(); // Initialize Zobrist keys

    std::string line;
    Position current_pos;
    current_pos.setStartingPosition();
    current_pos.zobrist_key = calculate_initial_hash(current_pos); // Initialize zobrist_key

    int search_depth = 6; // Default search depth, moved to wider scope
    long long move_time = -1; // Default no movetime limit, moved to wider scope

    while (std::getline(std::cin, line)) {
        std::stringstream ss(line);
        std::string command;
        ss >> command;

        if (command == "uci") {
            std::cout << "id name OctoKnight" << std::endl;
            std::cout << "id author Aladdin" << std::endl;
            std::cout << "option name Debug Log File type string default " << std::endl;
            std::cout << "option name Debug type check default false" << std::endl;
            std::cout << "uciok" << std::endl;
        } else if (command == "isready") {
            std::cout << "readyok" << std::endl;
        } else if (command == "setoption") {
            std::string name_str, value_str;
            ss >> name_str; // "name"
            ss >> name_str; // option name
            ss >> value_str; // "value"
            ss >> value_str; // option value

            if (name_str == "Debug") {
                if (value_str == "true") {
                    debug_mode = true;
                } else if (value_str == "false") {
                    debug_mode = false;
                }
            } else if (name_str == "Hash") {
                int hash_size_mb = std::stoi(value_str);
                tt.resize(hash_size_mb);
            } else if (name_str == "Ponder") {
                if (value_str == "true") {
                    ponder_mode = true;
                } else if (value_str == "false") {
                    ponder_mode = false;
                }
            }
        } // Added closing brace for 'setoption' command block
        else if (command == "debug") { // Handle direct 'debug on/off' command
            std::string debug_arg;
            ss >> debug_arg;
            if (debug_arg == "on") {
                debug_mode = true;
            } else if (debug_arg == "off") {
                debug_mode = false;
            }
        } else if (command == "position") {
            std::string arg;
            ss >> arg;
            if (arg == "startpos") {
                current_pos.zobrist_key = calculate_initial_hash(current_pos); // Update hash key
            } else if (arg == "fen") {
                std::string fen_string;
                while (ss >> arg && arg != "moves") {
                    fen_string += arg + " ";
                }
                current_pos.setFen(fen_string);
                current_pos.zobrist_key = calculate_initial_hash(current_pos); // Update hash key
            }
            if (arg == "moves") {
                while (ss >> arg) {
                    Move m = uci_to_move(arg);
                    makemove(current_pos, m);
                }
            }
        } else if (command == "ucinewgame") {
            tt.clear(); // Clear transposition table for a new game
        } else if (command == "go") {
            searching = true;
            search_depth = 6; // Default search depth
            move_time = -1; // Default no movetime limit
            bool infinite_search = false;
            bool ponder_this_search = false;

            std::string token;
            while (ss >> token) {
                if (token == "depth") {
                    ss >> search_depth;
                } else if (token == "movetime") {
                    ss >> move_time;
                } else if (token == "infinite") {
                    infinite_search = true;
                } else if (token == "ponder") {
                    ponder_this_search = true;
                }
                // Add other 'go' command parameters here as needed (e.g., nodes, mate, etc.)
            }

            if (infinite_search) {
                search_depth = 100; // A large enough number to represent infinite depth
            }
            if (ponder_this_search) {
                ponder_mode = true; // Set ponder mode for this search
            } else {
                ponder_mode = false; // Ensure ponder mode is off if not explicitly set
            } // Correctly close the 'go' command block

            Move best_move;
            // The search function will be modified to output info as it searches
            int score = search(current_pos, search_depth, move_time, best_move);
            searching = false;
            std::cout << "bestmove " << move_to_uci(best_move) << std::endl;
        } else if (command == "stop") {
            searching = false;
        } else if (command == "quit") {
            break;
        } else {
            log_debug("Unknown command: " + command);
        }
    }

    return 0;
}

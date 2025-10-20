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
#include <sstream>
#include <string>
#include <vector>

#include <thread>
#include <atomic>
#include <mutex>

std::thread search_thread;
std::mutex position_mutex;

int main() {
  init_magics();
  initKingAttacks();
  zkey.initKeys(); // Initialize Zobrist keys

  std::string line;
  Position current_pos;
  current_pos.setStartingPosition();
  current_pos.zobrist_key =
      calculate_initial_hash(current_pos); // Initialize zobrist_key

  int search_depth = 64;    // Default search depth, moved to wider scope
  long long move_time = -1; // Default no movetime limit, moved to wider scope

  while (std::getline(std::cin, line)) {
    std::stringstream ss(line);
    std::string command;
    ss >> command;

    if (command == "uci") {
      std::cout << "id name OctoKnight" << std::endl;
      std::cout << "id author Aladdin" << std::endl;
      std::cout << "option name Debug Log File type string default "
                << std::endl;
      std::cout << "option name Debug type check default false" << std::endl;
      std::cout << "option name Hash type spin default 700 min 1 max 65536"
                << std::endl;
      std::cout << "option name Ponder type check default false" << std::endl;
      std::cout << "option name OwnBook type check default false" << std::endl;
      std::cout << "option name BookFile type string default book/book.txt"
                << std::endl;
      std::cout << "option name Threads type spin default 1 min 1 max 256"
                << std::endl;
      std::cout << "uciok" << std::endl;
    } else if (command == "isready") {
      std::cout << "readyok" << std::endl;
    } else if (command == "setoption") {
      std::string token;
      ss >> token; // Should be "name"
      std::string option_name;
      while (ss >> token && token != "value") {
        option_name += token;
      }
      std::string option_value;
      if (token == "value") {
        ss >> option_value;
      }

      if (option_name == "Debug") {
        if (option_value == "true")
          debug_mode = true;
        else if (option_value == "false")
          debug_mode = false;
        if (debug_mode) {
          std::cout << "info string Debug mode set to: "
                    << (debug_mode ? "true" : "false") << std::endl;
        }
      } else if (option_name == "Hash") {
        if (!option_value.empty()) {
          int hash_size_mb = std::stoi(option_value);
          tt.resize(hash_size_mb);
        }
      } else if (option_name == "Ponder") {
        if (option_value == "true")
          ponder_mode = true;
        else if (option_value == "false")
          ponder_mode = false;
      } else if (option_name == "OwnBook") {
        if (option_value == "true")
          own_book_enabled = true;
        else if (option_value == "false")
          own_book_enabled = false;
      } else if (option_name == "BookFile") {
        if (!option_value.empty())
          book_path = option_value;
      } else if (option_name == "Threads") {
        if (!option_value.empty()) {
          int t = std::stoi(option_value);
          if (t < 1)
            t = 1;
          if (t > 256)
            t = 256;
          num_threads = t;
        }
      }
    } else if (command == "debug") { // Handle direct 'debug on/off' command
      std::string debug_arg;
      ss >> debug_arg;
      if (debug_arg == "on") {
        debug_mode = true;
      } else if (debug_arg == "off") {
        debug_mode = false;
      }
    } else if (command == "position") {
      std::lock_guard<std::mutex> lock(position_mutex);
      std::string arg;
      ss >> arg;
      if (arg == "startpos") {
        current_pos.setStartingPosition();
        current_pos.zobrist_key =
            calculate_initial_hash(current_pos); // Update hash key
        tt.clear();                              // Clear TT on position change
      } else if (arg == "fen") {
        std::string fen_string;
        while (ss >> arg && arg != "moves") {
          fen_string += arg + " ";
        }
        current_pos.setFen(fen_string);
        if (debug_mode) {
          std::cout << "info string FEN parsed: " << fen_string << std::endl;
        }
        current_pos.zobrist_key =
            calculate_initial_hash(current_pos); // Update hash key
        if (debug_mode) {
          std::cout << "info string WhitePawns after setFen: "
                    << current_pos.WhitePawns << std::endl;
        }
        tt.clear(); // Clear TT on position change
      }
      if (arg == "moves") {
        while (ss >> arg) {
          Move m = uci_to_move(arg);
          makemove(current_pos, m);
        }
      }
    } else if (command == "ucinewgame") {
      std::lock_guard<std::mutex> lock(position_mutex);
      tt.clear(); // Clear transposition table for a new game
      current_pos.setStartingPosition();
      current_pos.zobrist_key = calculate_initial_hash(current_pos);
    } else if (command == "go") {
      if (searching) {
        continue;
      }
      searching = true;
      search_depth = 64; // Default search depth
      move_time = -1;    // Default no movetime limit
      bool infinite_search = false;
      bool ponder_this_search = false;
      bool depth_specified = false; // Flag to check if depth was explicitly set
      long long wtime = -1, btime = -1, winc = 0, binc = 0;
      int movestogo = 0;

      std::string token;
      while (ss >> token) {
        if (token == "depth") {
          ss >> search_depth;
          depth_specified = true;
        } else if (token == "movetime") {
          ss >> move_time;
        } else if (token == "infinite") {
          infinite_search = true;
        } else if (token == "ponder") {
          ponder_this_search = true;
        } else if (token == "wtime") {
          ss >> wtime;
        } else if (token == "btime") {
          ss >> btime;
        } else if (token == "winc") {
          ss >> winc;
        } else if (token == "binc") {
          ss >> binc;
        } else if (token == "movestogo") {
          ss >> movestogo;
        } else {
        }
        // Add other 'go' command parameters here as needed (e.g., nodes, mate,
        // etc.)
      }

      if (infinite_search) {
        search_depth = 100; // A large enough number to represent infinite depth
      }

      // Debug: Print search parameters
      std::cout << "info string Search params: depth=" << search_depth
                << " move_time=" << move_time << " infinite=" << infinite_search
                << std::endl;
      if (ponder_this_search) {
        ponder_mode = true; // Set ponder mode for this search
      } else {
        ponder_mode = false; // Ensure ponder mode is off if not explicitly set
      } // Correctly close the 'go' command block

      // Try opening book if enabled
      if (own_book_enabled) {
        std::string bm = get_book_move(current_pos, book_path);
        if (!bm.empty()) {
          std::cout << "bestmove " << bm << std::endl;
          searching = false;
          continue;
        }
      }
      // If clock provided, compute a simple time allocation per move
      if (move_time == -1 && (wtime >= 0 || btime >= 0)) {
        long long mytime = current_pos.whiteToMove ? wtime : btime;
        long long myinc = current_pos.whiteToMove ? winc : binc;
        long long reserve = mytime / 10; // keep 10% in reserve
        int remaining = movestogo > 0 ? movestogo : 30;
        move_time = std::max(10LL, (mytime - reserve) / remaining + myinc / 2);
      } else if (move_time == -1 && !infinite_search && !depth_specified) {
        move_time = 1000; // default 1s/move when no limits supplied
      }

      search_thread = std::thread([&]() mutable {
        Move best_move;
        std::lock_guard<std::mutex> lock(position_mutex);
        search(current_pos, search_depth, move_time, best_move);
        std::cout << "bestmove " << move_to_uci(best_move) << std::endl;
        searching = false;
      });
      search_thread.detach();

    } else if (command == "stop") {
      searching = false;
    } else if (command == "quit") {
      searching = false;
      if (search_thread.joinable()) {
        search_thread.join();
      }
      break;
    } else {
      log_debug("Unknown command: " + command);
    }
  }

  return 0;
}

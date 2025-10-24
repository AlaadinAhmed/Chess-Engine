#include "bitboard.hpp"
#include "eval.hpp"
#include "fen.hpp"
#include "globals.hpp"
#include "hash.hpp"
#include "history.hpp"
#include "magics.hpp"
#include "movegen.hpp"
#include "movepick.hpp"
#include "position.hpp"
#include "print.hpp"
#include "search.hpp"
#include "tt.hpp"
#include "utils.hpp"

#include "utils.hpp"
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
#include <mutex>

std::mutex cout_mutex;

void uci_loop() {
  Position pos;
  pos.setStartingPosition();
  std::string line;
  std::thread search_thread;

  while (std::getline(std::cin, line)) {
    std::istringstream iss(line);
    std::string token;
    iss >> token;
    if (token == "uci") {
      std::cout << "id name OctoKnight" << std::endl;
      std::cout << "id author Aladdin" << std::endl;
      std::cout << "option name Threads type spin default " << num_threads
                << " min 1 max 1024" << std::endl;
      std::cout << "option name Debug type check default false" << std::endl;
      std::cout << "uciok" << std::endl;
    } else if (token == "isready") {
      if (search_thread.joinable()) {
        searching = false;
        search_thread.join();
      }
      std::cout << "readyok" << std::endl;
    } else if (token == "ucinewgame") {
      if (search_thread.joinable()) {
        searching = false;
        search_thread.join();
      }
      tt.clear(); // Clear transposition table for new game
      pos.setStartingPosition(); // Reset to starting position
    } else if (token == "setoption") {
      std::string name_token, value_token;
      iss >> name_token; // "name"
      std::string name;
      iss >> name;        // option name
      iss >> value_token; // "value"
      if (name == "Threads") {
        iss >> num_threads;
      } else if (name == "Debug") {
        std::string value;
        iss >> value;
        debug_mode = (value == "true");
      }
    } else if (token == "position") {
      if (search_thread.joinable()) {
        searching = false;
        search_thread.join();
      }
      std::string sub_token;
      iss >> sub_token;
      if (sub_token == "startpos") {
        pos.setStartingPosition();
        iss >> sub_token; // "moves" or nothing
      } else {
        std::string fen_string;
        if (sub_token == "fen") {
          // "fen" keyword is present, so we read the fen string
          iss >> sub_token; // read first part of fen
        }
        fen_string = sub_token;
        while (iss >> sub_token && sub_token != "moves") {
          fen_string += " " + sub_token;
        }
        pos.setFen(fen_string);
      }

      if (sub_token == "moves") {
        std::string move_token;
        while (iss >> move_token) {
          Move m = uci_to_move(move_token);
          makemove(pos, m);
        }
      }
    } else if (token == "go") {
      // Stop any existing search first
      if (search_thread.joinable()) {
        searching = false;
        search_thread.join();
      }
      
      std::string go_params;
      std::getline(iss, go_params);
      long long move_time = -1;
      int max_depth = 1000;
      std::istringstream go_iss(go_params);
      std::string token;
      while (go_iss >> token) {
        if (token == "wtime" || token == "btime" || token == "movetime") {
          go_iss >> move_time;
        } else if (token == "depth") {
          go_iss >> max_depth;
        }
      }
      searching = true;
      
      // Launch search in a separate thread so we can handle stop/quit
      // Make a copy of position for the search thread
      Position pos_copy = pos;
      search_thread = std::thread([pos_copy, max_depth, move_time]() mutable {
        Move best_move;
        search(pos_copy, max_depth, move_time, best_move);
        if (searching) {
          std::lock_guard<std::mutex> lock(cout_mutex);
          std::cout << "bestmove " << move_to_uci(best_move) << std::endl;
        }
        searching = false;
      });
    } else if (token == "stop") {
      searching = false;
      if (search_thread.joinable()) {
        search_thread.join();
      }
    } else if (token == "quit") {
      searching = false;
      if (search_thread.joinable()) {
        search_thread.join();
      }
      break;
    }
  }
  
  // Ensure search thread is stopped before exiting
  searching = false;
  if (search_thread.joinable()) {
    search_thread.join();
  }
}

int main() {
  // log_debug("Engine starting...");
  zkey.initKeys();
  num_threads = std::thread::hardware_concurrency();
  uci_loop();
  log_debug("Engine finished.");
  return 0;
}

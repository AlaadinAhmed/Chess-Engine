# ♞ OctoKnight

**OctoKnight** is a custom-built chess engine designed to think tactically, play aggressively, and learn from every position. Named after the knight’s eight possible moves, this engine blends classic chess logic with modern programming principles.

---

## 🚀 Features

- ♟️ Alpha-beta pruning with iterative deepening
- 🧠 Static evaluation function with material and positional heuristics
- ⚔️ Tactical pattern recognition (forks, pins, skewers, etc.)
- 🕹️ UCI-compatible interface for use with GUIs like Arena or CuteChess
- 📈 Search depth control and move history
- 🧪 Opening book (WIP)

---

## 🔧 Build & Run

### 🛠️ Requirements
- C++17 or later (or your engine’s language)
- CMake (optional but recommended)
- Git
- Optional: GUI (like [Arena](http://www.playwitharena.de/) or [CuteChess](https://github.com/cutechess/cutechess))

### 💻 Clone & Build

```bash
git clone https://github.com/AladdinAhmed/octoknight.git
cd octoknight
make          # or use cmake .

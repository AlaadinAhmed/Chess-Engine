/**
@mainpage The Complete Chess Engine Programming Guide

@image html chess_header.png width=800px
@image latex chess_header.png width=\textwidth

@tableofcontents

---

@section guide_introduction Introduction

@subsection what_is_chess_engine What is a Chess Engine?

A **chess engine** is a sophisticated computer program capable of playing chess at superhuman levels. Modern engines like Stockfish, AlphaZero, and Leela Chess Zero can defeat the world's best human players consistently.

This comprehensive guide will teach you how to build a competitive chess engine from scratch, explaining every concept in exhaustive detail with:
- 📊 **Detailed diagrams and flowcharts**
- 📈 **Performance comparisons and benchmarks**
- 💻 **Complete, working code examples**
- 🧠 **Deep explanations of "WHY" for every decision**
- 🚀 **Modern techniques from Stockfish**

@subsection engine_architecture Engine Architecture Overview

A chess engine consists of several interconnected subsystems:

@dot "High-Level Chess Engine Architecture"
digraph G {
    rankdir=TB;
    graph [bgcolor=transparent, fontname="Arial"];
    node [shape=box, style="filled,rounded", fontname="Arial", fillcolor="#E8F4F8"];
    edge [fontname="Arial"];
    
    subgraph cluster_input {
        label="Input Layer";
        style=filled;
        fillcolor="#F0F0F0";
        
        UCI [label="UCI Protocol\nCommunication Interface", fillcolor="#90EE90"];
        OpeningBook [label="Opening Book\n(Optional)", fillcolor="#FFE4B5"];
    }
    
    subgraph cluster_core {
        label="Core Engine";
        style=filled;
        fillcolor="#F8F8FF";
        
        Board [label="Board\nRepresentation", fillcolor="#87CEEB"];
        MoveGen [label="Move\nGenerator", fillcolor="#DDA0DD"];
        Eval [label="Evaluation\nFunction", fillcolor="#FFB6C1"];
        Search [label="Search\nAlgorithm", fillcolor="#FFD700"];
    }
    
    subgraph cluster_optimization {
        label="Optimization Layer";
        style=filled;
        fillcolor="#F0F0F0";
        
        TT [label="Transposition\nTable", fillcolor="#FF6347"];
        History [label="History\nHeuristic", fillcolor="#FFA07A"];
        Killers [label="Killer\nMoves", fillcolor="#FA8072"];
    }
    
    subgraph cluster_output {
        label="Output";
        style=filled;
        fillcolor="#F0F0F0";
        
        BestMove [label="Best Move\n+ PV", fillcolor="#98FB98"];
    }
    
    UCI -> Board [label="position"];
    UCI -> Search [label="go"];
    OpeningBook -> Search [label="book move"];
    
    Board -> MoveGen [label="current\nposition"];
    MoveGen -> Search [label="legal\nmoves"];
    Board -> Eval [label="position\nstate"];
    Eval -> Search [label="score"];
    
    Search -> TT [label="store/\nlookup"];
    Search -> History [label="update"];
    Search -> Killers [label="update"];
    
    TT -> Search [label="cached\nresult"];
    History -> Search [label="move\nordering"];
    Killers -> Search [label="move\nordering"];
    
    Search -> BestMove [label="result"];
    BestMove -> UCI [label="bestmove"];
}
@enddot

@subsection component_breakdown Component Breakdown

| Component | Purpose | Key Techniques | Complexity |
|-----------|---------|----------------|------------|
| **Board Representation** | Store position state | Bitboards, Zobrist hashing | Medium |
| **Move Generator** | Find all legal moves | Magic bitboards, attack tables | High |
| **Evaluation** | Score positions | Material, PST, pawn structure, NNUE | Very High |
| **Search** | Find best move | Alpha-beta, quiescence, iterative deepening | Very High |
| **Transposition Table** | Cache results | Hash table, replacement schemes | Medium |
| **UCI Protocol** | GUI communication | Text parsing, move formatting | Low |

@subsection learning_path What You'll Learn

By completing this guide, you will master:

@dot "Learning Progression"
digraph learning {
    rankdir=LR;
    node [shape=box, style="filled,rounded", fontname="Arial"];
    
    basics [label="Basics\n• Bitboards\n• Move Gen", fillcolor="#90EE90"];
    intermediate [label="Intermediate\n• Alpha-Beta\n• Evaluation", fillcolor="#FFD700"];
    advanced [label="Advanced\n• TT, Ordering\n• Pruning", fillcolor="#FFA500"];
    expert [label="Expert\n• NNUE\n• Parallel Search", fillcolor="#FF6347"];
    
    basics -> intermediate -> advanced -> expert;
}
@enddot

**Skill Levels:**

- ✅ **Beginner (Chapters 1-4)**: Board representation, move generation, make/unmake
- ✅ **Intermediate (Chapters 5-6)**: Evaluation, basic search (minimax, alpha-beta)
- ✅ **Advanced (Chapters 7-8)**: Transposition tables, move ordering, pruning
- ✅ **Expert (Chapters 9-10)**: UCI protocol, NNUE, parallel search, Stockfish techniques

@subsection prerequisites Prerequisites and Setup

@subsubsection required_knowledge Required Knowledge

| Area | Requirement | Why You Need It |
|------|-------------|-----------------|
| **C++ Programming** | Intermediate level | Templates, classes, STL containers |
| **Data Structures** | Arrays, hash tables, stacks | Position storage, caching |
| **Algorithms** | Recursion, tree search | Search algorithm implementation |
| **Bitwise Operations** | AND, OR, XOR, shifts | Bitboard manipulation |
| **Chess Rules** | Complete understanding | Move generation, legality checking |
| **Basic Math** | Algebra, functions | Evaluation formulas |

@subsubsection development_environment Development Environment

@code{.bash}
# Required: C++ compiler with C++17 support
g++ --version  # Minimum: GCC 7.0 or Clang 5.0

# Recommended: Latest versions for best optimization
# GCC 11+ or Clang 14+ recommended for maximum performance

# Build system (optional but recommended)
cmake --version  # For project management

# Version control
git --version

# Debugger
gdb --version  # or lldb for Clang

# Chess GUI for testing
# Download one of:
# - Cute Chess: https://cutechess.com/
# - Arena: http://www.playwitharena.de/
# - BanksiaGUI: https://banksiagui.com/
@endcode

@subsubsection performance_expectations Performance Expectations

**Typical engine strength progression:**

@dot "Engine Strength vs Features"
digraph strength {
    rankdir=TB;
    node [shape=box, style="filled,rounded"];
    
    basic [label="Basic Engine\nMaterial + Simple Search\n~1000 Elo", fillcolor="#FFE4B5"];
    alpha [label="+ Alpha-Beta + Quiescence\n~1600 Elo", fillcolor="#F0E68C"];
    tt [label="+ Transposition Table\n~1900 Elo", fillcolor="#FFD700"];
    eval [label="+ Advanced Eval\n~2200 Elo", fillcolor="#FFA500"];
    pruning [label="+ Null Move, LMR\n~2400 Elo", fillcolor="#FF8C00"];
    nnue [label="+ NNUE Evaluation\n~3000+ Elo", fillcolor="#FF6347"];
    
    basic -> alpha -> tt -> eval -> pruning -> nnue;
}
@enddot

@note **Elo Rating**: Standard chess rating system. Stockfish 16 is rated ~3600 Elo, world champion ~2800 Elo, beginner ~800 Elo.

---

@section chapter_1 Chapter 1: Understanding Chess Programming Fundamentals

@subsection search_space_problem 1.1 The Search Space Explosion Problem

@subsubsection exponential_growth Understanding Exponential Growth

Chess is a **combinatorially explosive** game. Let's visualize why this matters:

@dot "Exponential Search Tree Growth"
digraph searchtree {
    rankdir=TB;
    node [shape=circle, fixedsize=true, width=0.8, fontsize=10];
    
    root [label="Start", fillcolor="#90EE90", style=filled];
    
    // Depth 1
    d1_1 [label="Move 1", fillcolor="#F0E68C", style=filled];
    d1_2 [label="Move 2", fillcolor="#F0E68C", style=filled];
    d1_3 [label="...", shape=plaintext];
    d1_20 [label="Move 20", fillcolor="#F0E68C", style=filled];
    
    root -> d1_1;
    root -> d1_2;
    root -> d1_3 [style=dotted];
    root -> d1_20;
    
    // Depth 2 (only show for one branch)
    d2_1 [label="1", fillcolor="#FFD700", style=filled];
    d2_2 [label="2", fillcolor="#FFD700", style=filled];
    d2_etc [label="... 20", shape=plaintext];
    
    d1_1 -> d2_1;
    d1_1 -> d2_2;
    d1_1 -> d2_etc [style=dotted];
    
    // Depth 3 (only hint)
    d3_1 [label="400+", fillcolor="#FFA500", style=filled];
    d2_1 -> d3_1;
    d2_2 -> d3_1 [style=invis];
    
    label="Each level multiplies positions by ~20-40";
}
@enddot

**Mathematical Analysis:**

The number of positions at depth \f$d\f$ is approximately:

\f[
\text{Positions}(d) \approx b^d
\f]

Where \f$b\f$ is the average branching factor (~35 for chess).

| Depth | Approximate Positions | Search Time @ 1M nodes/sec |
|-------|----------------------|---------------------------|
| 1 | 20 | 0.00002 seconds |
| 2 | 400 | 0.0004 seconds |
| 3 | 8,000 | 0.008 seconds |
| 4 | 160,000 | 0.16 seconds |
| 5 | 3,200,000 | 3.2 seconds |
| 6 | 64,000,000 | 64 seconds |
| 7 | 1,280,000,000 | 21 minutes |
| 8 | 25,600,000,000 | 7 hours |
| 10 | ~10^15 | **32 years!** |

@warning Without optimization, searching to depth 10 would take decades! This is why we need alpha-beta pruning and other techniques.

@subsubsection shannon_number The Shannon Number

Claude Shannon calculated the **game-tree complexity** of chess:

\f[
\text{Shannon Number} \approx 10^{120}
\f]

This is the approximate number of possible chess games.

**Comparison with the universe:**

@dot "Scale Comparison"
digraph scale {
    rankdir=LR;
    node [shape=box, style="filled,rounded"];
    
    atoms [label="Atoms in\nObservable Universe\n~10^80", fillcolor="#87CEEB"];
    chess [label="Possible\nChess Games\n~10^120", fillcolor="#FFD700"];
    
    atoms -> chess [label="Chess has\n10^40 times more!"];
}
@enddot

@note Even if we had a computer the size of the universe, examining one position per atom, we couldn't search all chess games before heat death of the universe!

@subsection core_concepts_detail 1.2 Core Concepts Explained in Detail

@subsubsection ply_vs_move_detailed Concept 1: Ply vs Move - The Critical Distinction

**Definition:**
- **Ply**: A single turn by one player
- **Move**: A complete turn by both players (white + black)

@startuml "Detailed Ply vs Move Timeline"
!theme cerulean

participant "White" as W
participant "Board State" as B
participant "Black" as Bl
participant "Move Counter" as M

== Move 1 ==
W -> B: e2-e4
activate B
note right of B
  Ply count: 1
  Full move: 1 (incomplete)
  Position hash: XYZ123
end note
B -> W: Updated position
deactivate B

Bl -> B: e7-e5
activate B
note right of B
  Ply count: 2
  Full move: 1 (complete)
  Position hash: ABC456
end note
B -> Bl: Updated position
deactivate B
M -> M: Increment move to 2

== Move 2 ==
W -> B: Ng1-f3
activate B
note right of B
  Ply count: 3
  Full move: 2 (incomplete)
end note
B -> W: Updated position
deactivate B
@enduml

**Why This Matters in Code:**

@code{.cpp}
// WRONG: Using "moves" as depth unit
int search_wrong(Position& pos, int moves_ahead) {
    if (moves_ahead == 0) return evaluate(pos);
    
    // Confusion: Do we search white only? Both players?
    // What about odd depths?
}

// CORRECT: Using "ply" as depth unit
int search_correct(Position& pos, int ply) {
    if (ply == 0) return evaluate(pos);
    
    // Clear: Search exactly ply half-moves ahead
    // ply=1: opponent's next move
    // ply=2: our response to opponent
    // ply=10: 5 full moves of lookahead
    
    for (Move m : generate_moves(pos)) {
        make_move(pos, m);
        int score = -search_correct(pos, ply - 1);  // Recurse
        undo_move(pos, m);
    }
}
@endcode

**Practical Example:**

@dot "Search Tree with Ply Labeling"
digraph plytree {
    rankdir=TB;
    node [shape=box, style="filled,rounded"];
    
    root [label="Current Position\n(White to move)\nPly 0", fillcolor="#90EE90"];
    
    w1 [label="After White Move 1\nPly 1", fillcolor="#F0E68C"];
    w2 [label="After White Move 2\nPly 1", fillcolor="#F0E68C"];
    
    b1 [label="After Black Response\nPly 2", fillcolor="#FFD700"];
    b2 [label="After Black Response\nPly 2", fillcolor="#FFD700"];
    
    w1_2 [label="White's 2nd move\nPly 3", fillcolor="#FFA500"];
    
    root -> w1 [label="Nf3"];
    root -> w2 [label="e4"];
    
    w1 -> b1 [label="Nc6"];
    w1 -> b2 [label="d5"];
    
    b1 -> w1_2 [label="d4"];
    
    label="Ply increases by 1 for each half-move";
}
@enddot


@subsubsection centipawn_system Concept 2: The Centipawn Scoring System

**Why Centipawns?**

Chess engines need a numerical way to represent "who's winning". We use **centipawns** (cp):

\f[
1 \text{ centipawn} = \frac{1}{100} \text{ pawn}
\f]

**Standard Piece Values:**

| Piece | Value (centipawns) | Reasoning |
|-------|-------------------|-----------|
| Pawn | 100 | Base unit |
| Knight | 320 | Worth ~3.2 pawns |
| Bishop | 330 | Slightly better than knight (long diagonals) |
| Rook | 500 | Worth 5 pawns |
| Queen | 900 | Worth 9 pawns |
| King | ∞ | Losing king = losing game |

**Score Interpretation:**

@dot "Score Scale Visualization"
digraph score_scale {
    rankdir=LR;
    node [shape=box, style="filled,rounded", fontsize=10];
    
    massive_adv [label="+3000\nMassive\nAdvantage", fillcolor="#006400", fontcolor=white];
    winning [label="+500\nWinning", fillcolor="#228B22", fontcolor=white];
    advantage [label="+200\nAdvantage", fillcolor="#90EE90"];
    equal [label="0\nEqual", fillcolor="#F0E68C"];
    disadvantage [label="-200\nDisadvantage", fillcolor="#FFB6C1"];
    losing [label="-500\nLosing", fillcolor="#DC143C", fontcolor=white];
    massive_dis [label="-3000\nMassive\nDisadvantage", fillcolor="#8B0000", fontcolor=white];
    
    massive_dis -> losing -> disadvantage -> equal -> advantage -> winning -> massive_adv;
}
@enddot

**Example Calculations:**

@code{.cpp}
// Position evaluation examples

// Example 1: Material only
// White: K, Q, 2R, 2B, 2N, 8P
// Black: K, Q, 2R, 2B, 2N, 7P (missing one pawn)
int score = +100;  // White is up one pawn
// Interpretation: Slight advantage

// Example 2: White has extra knight
// White: K, Q, 2R, 2B, 3N, 8P
// Black: K, Q, 2R, 2B, 2N, 8P
int score = +320;  // White is up a knight
// Interpretation: Winning advantage

// Example 3: Complex position
// Material equal but white has better position
// Base: 0 (material equal)
// + Center control: +30
// + Better pawn structure: +20
// + King safety advantage: +15
// = Total: +65 centipawns
// Interpretation: Small but meaningful advantage
@endcode

**Special Scores:**

@code{.cpp}
const int MATE_SCORE = 30000;        // Checkmate value
const int DRAW_SCORE = 0;            // Draw value
const int MATE_IN_N = MATE_SCORE - ply;  // Prefer faster mates

// Example: Mating scores
int mate_in_1 = MATE_SCORE - 1;  // 29999 (immediate mate)
int mate_in_5 = MATE_SCORE - 5;  // 29995 (mate in 5 plies)

// Engine prefers mate_in_1 over mate_in_5
// Both are "mate", but faster is better!
@endcode

@subsubsection minimax_principle Concept 3: The Minimax Principle

**Core Idea:**

You want to **maximize** your score, your opponent wants to **minimize** your score (which maximizes theirs).

@startuml "Minimax Game Tree"
!theme cerulean

object "Position A (Max)" as PA {
  Score = ?
  Your Turn
}

object "Position B (Min)" as PB1 {
  Score = 50
  Opponent Turn
}

object "Position C (Min)" as PB2 {
  Score = -20
  Opponent Turn  
}

object "Position D (Min)" as PB3 {
  Score = 100
  Opponent Turn
}

object "Final 1" as F1 {
  Score = 50
}

object "Final 2" as F2 {
  Score = 30
}

object "Final 3" as F3 {
  Score = -20
}

object "Final 4" as F4 {
  Score = 10
}

object "Final 5" as F5 {
  Score = 100
}

object "Final 6" as F6 {
  Score = 80
}

PA --> PB1: Move 1
PA --> PB2: Move 2
PA --> PB3: Move 3

PB1 --> F1: Opp Move A
PB1 --> F2: Opp Move B

PB2 --> F3: Opp Move C
PB2 --> F4: Opp Move D

PB3 --> F5: Opp Move E
PB3 --> F6: Opp Move F

note right of PB1
  Opponent picks MIN
  min(50, 30) = 30
end note

note right of PB2
  Opponent picks MIN
  min(-20, 10) = -20
end note

note right of PB3
  Opponent picks MIN
  min(100, 80) = 80
end note

note left of PA
  You pick MAX
  max(30, -20, 80) = 80
  Choose Move 3!
end note
@enduml

**The Negamax Formulation:**

Instead of alternating between max and min, we can negate scores:

\f[
\text{score}(\text{pos}) = -\text{score}(\text{opponent's pos})
\f]

@code{.cpp}
// Traditional Minimax (alternating max/min)
int minimax(Position& pos, int depth, bool maximizing) {
    if (depth == 0) return evaluate(pos);
    
    if (maximizing) {
        int best = -INFINITY;
        for (Move m : moves) {
            make_move(pos, m);
            int score = minimax(pos, depth-1, false);
            undo_move(pos, m);
            best = max(best, score);
        }
        return best;
    } else {
        int best = +INFINITY;
        for (Move m : moves) {
            make_move(pos, m);
            int score = minimax(pos, depth-1, true);
            undo_move(pos, m);
            best = min(best, score);
        }
        return best;
    }
}

// Negamax (simpler - always maximize, negate scores)
int negamax(Position& pos, int depth) {
    if (depth == 0) return evaluate(pos);
    
    int best = -INFINITY;
    for (Move m : moves) {
        make_move(pos, m);
        
        // Negate opponent's score (their max is our min)
        int score = -negamax(pos, depth-1);
        
        undo_move(pos, m);
        best = max(best, score);
    }
    return best;
}

// Same result, but negamax is cleaner!
@endcode

@dot "Negamax Score Propagation"
digraph negamax {
    rankdir=TB;
    node [shape=record, style="filled,rounded"];
    
    root [label="{Position|Max Layer|Score = ?}", fillcolor="#90EE90"];
    
    c1 [label="{Move 1|Min Layer|Score = ?}", fillcolor="#FFD700"];
    c2 [label="{Move 2|Min Layer|Score = ?}", fillcolor="#FFD700"];
    
    g1 [label="{Opp Move A|Max Layer|Eval = +50}", fillcolor="#FFA500"];
    g2 [label="{Opp Move B|Max Layer|Eval = +30}", fillcolor="#FFA500"];
    
    g3 [label="{Opp Move C|Max Layer|Eval = -20}", fillcolor="#FFA500"];
    g4 [label="{Opp Move D|Max Layer|Eval = +10}", fillcolor="#FFA500"];
    
    root -> c1 [label="try"];
    root -> c2 [label="try"];
    
    c1 -> g1 [label="opp tries"];
    c1 -> g2 [label="opp tries"];
    
    c2 -> g3 [label="opp tries"];
    c2 -> g4 [label="opp tries"];
    
    // Return path
    g1 -> c1 [label="-(-50)=+50", color=red, style=dashed];
    g2 -> c1 [label="-(-30)=+30", color=red, style=dashed];
    
    g3 -> c2 [label="-(-20)=+20", color=blue, style=dashed];
    g4 -> c2 [label="-(-10)=+10", color=blue, style=dashed];
    
    c1 -> root [label="max(50,30)=50", color=red, style=dashed];
    c2 -> root [label="max(20,10)=20", color=blue, style=dashed];
    
    label="Negamax: Negate scores, always maximize";
}
@enddot

---

@section chapter_2 Chapter 2: Board Representation - The Foundation

@subsection bitboard_introduction 2.1 Bitboards - The Modern Approach

@subsubsection why_not_arrays Why Not Use Arrays?

**Traditional Array Approach:**

@code{.cpp}
// Old-school mailbox representation
struct BoardArray {
    char squares[64];  // 'P' = white pawn, 'p' = black pawn, etc.
    
    // Problems:
    // 1. Slow to find pieces (must scan all 64 squares)
    // 2. Hard to do set operations
    // 3. Takes 64 bytes
    // 4. Copying is slow
};

// Finding all white pawns
std::vector<int> find_white_pawns(const BoardArray& board) {
    std::vector<int> pawns;
    for (int sq = 0; sq < 64; sq++) {
        if (board.squares[sq] == 'P') {
            pawns.push_back(sq);
        }
    }
    return pawns;  // O(64) time!
}
@endcode

**Performance Comparison:**

| Operation | Array Method | Bitboard Method | Speedup |
|-----------|-------------|-----------------|---------|
| Find all pawns | Scan 64 squares | Single variable | 64x |
| Count pawns | Loop + counter | POPCNT instruction | 10-20x |
| Check if square occupied | Array access | Bit test | 1x |
| Copy position | Copy 64 bytes | Copy 12 uint64_t | 1x |
| Find attacked squares | Complex loops | Bitwise AND/OR | 5-10x |

@subsubsection bitboard_concept The Bitboard Concept

A **bitboard** is a 64-bit integer where each bit represents a chessboard square:

@verbatim
Bitboard Layout (Little-Endian Rank-File Mapping):

Bit Index          Chess Square
---------          ------------
  a  b  c  d  e  f  g  h
8 56 57 58 59 60 61 62 63  <- Rank 8
7 48 49 50 51 52 53 54 55  <- Rank 7
6 40 41 42 43 44 45 46 47  <- Rank 6
5 32 33 34 35 36 37 38 39  <- Rank 5
4 24 25 26 27 28 29 30 31  <- Rank 4
3 16 17 18 19 20 21 22 23  <- Rank 3
2  8  9 10 11 12 13 14 15  <- Rank 2
1  0  1  2  3  4  5  6  7  <- Rank 1

Formula: square_index = rank * 8 + file
Example: e4 = 4 * 8 + 4 = 32
         a1 = 0 * 8 + 0 = 0
         h8 = 7 * 8 + 7 = 63
@endverbatim

**Visual Example:**

@code{.cpp}
// White pawns on starting position
uint64_t whitePawns = 0x000000000000FF00ULL;

// Binary representation (showing only relevant bits):
// Bit:  63 62 61 ... 15 14 13 12 11 10  9  8  7  6 ... 0
// Val:   0  0  0 ...  1  1  1  1  1  1  1  1  0  0 ... 0
//                     ^  ^  ^  ^  ^  ^  ^  ^
//                     h2 g2 f2 e2 d2 c2 b2 a2

// In memory (64-bit hex):
// 0x000000000000FF00
//   ^^^^^^^^ ^^^^^^^^
//   High bits Low bits
//   (Rank 8-5)(Rank 4-1)
@endcode

@dot "Bitboard Visual Representation"
digraph bitboard_viz {
    rankdir=TB;
    node [shape=record, fontname="Courier"];
    
    bits [label="<b63>63|<b62>62|...|<b15>15|<b14>14|...|<b8>8|<b7>7|...|<b0>0", fillcolor="#E8F4F8", style=filled];
    
    squares [label="<h8>h8|<g8>g8|...|<h2>h2|<g2>g2|...|<a2>a2|<h1>h1|...|<a1>a1", fillcolor="#FFD700", style=filled];
    
    bits:b15 -> squares:h2 [label="bit 15\n= square h2"];
    bits:b8 -> squares:a2 [label="bit 8\n= square a2"];
    bits:b0 -> squares:a1 [label="bit 0\n= square a1"];
    
    label="Each bit corresponds to one square";
}
@enddot


@subsubsection bitboard_operations Bitboard Operations - The Toolbox

**Core Operations with Visual Examples:**

@dot "Bitboard Operations Overview"
digraph operations {
    rankdir=TB;
    node [shape=box, style="filled,rounded"];
    
    bb [label="Bitboard\n(64-bit integer)", fillcolor="#90EE90"];
    
    set [label="SET BIT\nTurn bit ON", fillcolor="#FFD700"];
    clear [label="CLEAR BIT\nTurn bit OFF", fillcolor="#FFA500"];
    test [label="TEST BIT\nCheck if ON", fillcolor="#87CEEB"];
    count [label="COUNT BITS\nHow many 1s?", fillcolor="#DDA0DD"];
    find [label="FIND BIT\nFirst 1 position", fillcolor="#FFB6C1"];
    iterate [label="ITERATE\nProcess each 1", fillcolor="#F0E68C"];
    
    bb -> set;
    bb -> clear;
    bb -> test;
    bb -> count;
    bb -> find;
    bb -> iterate;
}
@enddot

**1. Setting a Bit (Placing a Piece):**

@code{.cpp}
// Add a white pawn to e4 (square 28)
uint64_t whitePawns = 0;  // Start with empty board

// Method: OR with bit mask
whitePawns |= (1ULL << 28);

// Visual breakdown:
// Before: 0000000000000000000000000000000000000000000000000000000000000000
// Mask:   0000000000000000000000000001000000000000000000000000000000000000
//                                    ^ bit 28
// After:  0000000000000000000000000001000000000000000000000000000000000000

// Why |= operator:
// | = OR: Combines bits (turns ON without affecting others)
// |= means "OR with this and store result"

// Multiple pieces:
whitePawns |= (1ULL << 8);   // Add pawn on a2
whitePawns |= (1ULL << 9);   // Add pawn on b2
// Result: pawns on a2, b2, and e4
@endcode

@verbatim
Visual Example - Adding Pawn to e4:

Before:                        After:
  a b c d e f g h                a b c d e f g h
8 . . . . . . . .              8 . . . . . . . .
7 . . . . . . . .              7 . . . . . . . .
6 . . . . . . . .              6 . . . . . . . .
5 . . . . . . . .              5 . . . . . . . .
4 . . . . . . . .              4 . . . . P . . .  <- Pawn added
3 . . . . . . . .              3 . . . . . . . .
2 . . . . . . . .              2 . . . . . . . .
1 . . . . . . . .              1 . . . . . . . .

Bitboard value: 0 -> 0x0000001000000000
@endverbatim

**2. Clearing a Bit (Removing a Piece):**

@code{.cpp}
// Remove pawn from e4
whitePawns &= ~(1ULL << 28);

// Breakdown:
// Step 1: Create mask with bit 28 set
// 1ULL << 28  = 0000...0001000000...0000

// Step 2: Invert mask (NOT operator)
// ~(1ULL << 28) = 1111...1110111111...1111
//                           ^ bit 28 is now 0, all others are 1

// Step 3: AND with bitboard
// whitePawns   = 0000...0001000000...0000
// AND mask     = 1111...1110111111...1111
// Result       = 0000...0000000000...0000
//                          ^ bit 28 cleared

// Why &= ~mask:
// & = AND: Only keeps bits that are 1 in BOTH operands
// ~mask has 0 only at bit 28, so ANDing clears just that bit
@endcode

**3. Testing a Bit (Checking for a Piece):**

@code{.cpp}
// Check if there's a pawn on e4
bool has_pawn_e4 = (whitePawns & (1ULL << 28)) != 0;

// Breakdown:
// whitePawns       = xxxx...x1xx...xxxx (bit 28 is 1)
// Mask (1ULL << 28) = 0000...0100...0000
// Result of AND     = 0000...0100...0000 (non-zero!)
// != 0 converts to true

// If no pawn:
// whitePawns       = xxxx...x0xx...xxxx (bit 28 is 0)
// Mask             = 0000...0100...0000
// Result of AND    = 0000...0000...0000 (zero!)
// != 0 converts to false
@endcode

**4. Counting Bits (Population Count):**

@code{.cpp}
// Count how many pawns
int pawn_count = __builtin_popcountll(whitePawns);

// Example:
// whitePawns = 0x000000000000FF00 (8 bits set = 8 pawns on rank 2)
// __builtin_popcountll returns: 8

// How it works internally (modern CPUs):
// Uses POPCNT instruction (1 CPU cycle!)
// Hardware counts 1-bits in parallel

// Manual implementation (educational):
int manual_popcount(uint64_t bb) {
    int count = 0;
    while (bb) {
        count += bb & 1;  // Add lowest bit
        bb >>= 1;         // Shift right
    }
    return count;
}

// Better manual implementation (Brian Kernighan):
int kernighan_popcount(uint64_t bb) {
    int count = 0;
    while (bb) {
        bb &= bb - 1;  // Clear lowest 1-bit
        count++;
    }
    return count;
}

// Why bb & (bb-1) clears lowest bit:
// bb     = ...10110100
// bb - 1 = ...10110011 (flips bits after lowest 1)
// AND    = ...10110000 (lowest 1 cleared!)
@endcode

@dot "Population Count Performance"
digraph popcount_perf {
    rankdir=LR;
    node [shape=box, style=filled];
    
    naive [label="Naive Loop\n64 iterations", fillcolor="#FFB6C1"];
    kernighan [label="Kernighan\n~8 iterations\n(# of 1-bits)", fillcolor="#FFD700"];
    builtin [label="__builtin_popcountll\n1 CPU cycle\nPOPCNT instruction", fillcolor="#90EE90"];
    
    naive -> kernighan [label="8x faster"];
    kernighan -> builtin [label="8x faster"];
    
    label="Population count optimizations";
}
@enddot

**5. Finding First Bit (Bit Scan Forward):**

@code{.cpp}
// Find index of first (lowest) set bit
int first_square = __builtin_ctzll(whitePawns);

// Example:
// whitePawns = 0x0000000000001100 (bits 8 and 12 set)
//              = ...0001000100000000 (binary)
// __builtin_ctzll returns: 8 (index of first 1-bit)

// ctz = "Count Trailing Zeros"
// Uses CPU's BSF (Bit Scan Forward) instruction

// Visual:
// Binary: ...0001000100000000
//                      ^^^^^^^^ 8 trailing zeros
// Returns: 8

// What if bitboard is 0?
// Result is undefined! Always check:
if (whitePawns != 0) {
    int sq = __builtin_ctzll(whitePawns);
}
@endcode

**6. Iterating Through Set Bits:**

@code{.cpp}
// Process all pawns
uint64_t pawns = whitePawns;
while (pawns) {
    // Find first pawn
    int square = __builtin_ctzll(pawns);
    
    // Do something with this pawn
    std::cout << "Pawn on square " << square << std::endl;
    
    // Remove this pawn from our copy
    pawns &= pawns - 1;  // Clear lowest bit
}

// Why pawns &= pawns - 1 works:
// Iteration 1:
//   pawns     = ...0001000100000000 (bits 8, 12 set)
//   pawns - 1 = ...0001000011111111
//   AND       = ...0001000000000000 (bit 8 cleared, 12 remains)
//
// Iteration 2:
//   pawns     = ...0001000000000000 (bit 12 set)
//   pawns - 1 = ...0000111111111111
//   AND       = ...0000000000000000 (bit 12 cleared, done!)
@endcode

@startuml "Bitboard Iteration Process"
!theme cerulean

start

:bitboard = whitePawns;

while (bitboard != 0?) is (yes)
  :square = __builtin_ctzll(bitboard);
  :process(square);
  :bitboard &= bitboard - 1;
endwhile (no)

stop

note right
  This loop processes each
  pawn exactly once, in
  order from a1 to h8
end note
@enduml

@subsubsection complete_position_structure 2.2 Complete Position Structure

Now let's build the complete data structure for a chess position:

@code{.cpp}
/**
 * @brief Complete chess position representation
 * 
 * This structure stores everything needed to represent
 * any legal chess position, including game state that
 * cannot be derived from piece placement alone.
 */
struct Position {
    // ========================================
    // PIECE BITBOARDS (12 total)
    // ========================================
    
    /// White pawns bitboard
    uint64_t WhitePawns;
    
    /// White knights bitboard  
    uint64_t WhiteKnights;
    
    /// White bishops bitboard
    uint64_t WhiteBishops;
    
    /// White rooks bitboard
    uint64_t WhiteRooks;
    
    /// White queens bitboard (usually 0 or 1 bit set)
    uint64_t WhiteQueen;
    
    /// White king bitboard (always exactly 1 bit set)
    uint64_t WhiteKing;
    
    /// Black pawns bitboard
    uint64_t BlackPawns;
    
    /// Black knights bitboard
    uint64_t BlackKnights;
    
    /// Black bishops bitboard
    uint64_t BlackBishops;
    
    /// Black rooks bitboard
    uint64_t BlackRooks;
    
    /// Black queens bitboard
    uint64_t BlackQueen;
    
    /// Black king bitboard (always exactly 1 bit set)
    uint64_t BlackKing;
    
    // ========================================
    // COMBINED BITBOARDS (for efficiency)
    // ========================================
    
    /// All white pieces combined (OR of all white piece bitboards)
    /// @note Updated after every piece movement
    uint64_t WhiteoccupiedSquares;
    
    /// All black pieces combined
    uint64_t BlackoccupiedSquares;
    
    /// All pieces regardless of color
    /// @note Equals WhiteoccupiedSquares | BlackoccupiedSquares
    uint64_t occupiedSquares;
    
    /// Empty squares
    /// @note Equals ~occupiedSquares
    uint64_t emptySquares;
    
    // ========================================
    // GAME STATE FLAGS
    // ========================================
    
    /// Side to move (true = white, false = black)
    bool whiteToMove;
    
    /**
     * @brief Castling rights encoded as 4 bits: KQkq
     * 
     * Bit layout:
     * - Bit 0 (0x1): White kingside  (K)
     * - Bit 1 (0x2): White queenside (Q)
     * - Bit 2 (0x4): Black kingside  (k)
     * - Bit 3 (0x8): Black queenside (q)
     * 
     * Example values:
     * - 0xF (1111): All castling rights available
     * - 0x3 (0011): Only white can castle (both sides)
     * - 0x0 (0000): No castling rights
     * 
     * @note Rights are REMOVED when:
     * - King moves (lose both sides)
     * - Rook moves (lose that side)
     * - Rook is captured (lose that side)
     */
    uint8_t castelingRights;
    
    /**
     * @brief En passant target square
     * 
     * Bitboard with at most 1 bit set, indicating
     * the square where en passant capture is legal.
     * 
     * Set when: Pawn moves 2 squares forward
     * Value: The square the pawn "jumped over"
     * 
     * Example:
     * - White plays e2-e4
     * - enPassant = (1ULL << 20) = e3 square
     * - Black can capture en passant by playing d5-e4
     * 
     * @note Cleared after every move except pawn double-push
     * @note Only valid for exactly ONE move!
     */
    uint64_t enPassant;
    
    /**
     * @brief Halfmove clock for 50-move rule
     * 
     * Counts halfmoves (plies) since last:
     * - Pawn move, OR
     * - Capture
     * 
     * When this reaches 100 (50 full moves), position is a draw.
     * 
     * Reset to 0 on:
     * - Any pawn move
     * - Any capture
     * 
     * Incremented on:
     * - Every other move
     */
    int move50rule;
    
    /**
     * @brief Full move number
     * 
     * Starts at 1, increments after each black move.
     * 
     * Examples:
     * - After 1.e4: fullMoveNumber = 1
     * - After 1...e5: fullMoveNumber = 2
     * - After 2.Nf3: fullMoveNumber = 2
     * - After 2...Nc6: fullMoveNumber = 3
     */
    int fullMoveNumber;
    
    // ========================================
    // HASH KEY (for transposition table)
    // ========================================
    
    /**
     * @brief Zobrist hash key
     * 
     * A 64-bit hash that uniquely identifies this position.
     * Used for:
     * - Transposition table lookups
     * - Three-fold repetition detection
     * - Position comparison
     * 
     * Updated incrementally (not recomputed from scratch):
     * - XOR out old piece positions
     * - XOR in new piece positions
     * - XOR castling rights change
     * - XOR en passant change
     * - XOR side to move flip
     * 
     * @note Zobrist hashing explained in detail later
     */
    uint64_t zobrist_key;
    
    // ========================================
    // HELPER FUNCTIONS
    // ========================================
    
    /**
     * @brief Update combined bitboards after piece movement
     * 
     * Call this after any modification to piece bitboards.
     * Recomputes: WhiteoccupiedSquares, BlackoccupiedSquares,
     *             occupiedSquares, emptySquares
     */
    void updateOccupancy() {
        WhiteoccupiedSquares = WhitePawns | WhiteKnights | WhiteBishops |
                               WhiteRooks | WhiteQueen | WhiteKing;
        
        BlackoccupiedSquares = BlackPawns | BlackKnights | BlackBishops |
                               BlackRooks | BlackQueen | BlackKing;
        
        occupiedSquares = WhiteoccupiedSquares | BlackoccupiedSquares;
        emptySquares = ~occupiedSquares;
    }
    
    /**
     * @brief Get piece type at a specific square
     * @param square Square index (0-63)
     * @return Piece type enum or NO_PIECE if square empty
     */
    Pieces pieceAt(int square) const {
        uint64_t bit = (1ULL << square);
        
        if (WhitePawns   & bit) return W_PAWN;
        if (WhiteKnights & bit) return W_KNIGHT;
        if (WhiteBishops & bit) return W_BISHOP;
        if (WhiteRooks   & bit) return W_ROOK;
        if (WhiteQueen   & bit) return W_QUEEN;
        if (WhiteKing    & bit) return W_KING;
        
        if (BlackPawns   & bit) return B_PAWN;
        if (BlackKnights & bit) return B_KNIGHT;
        if (BlackBishops & bit) return B_BISHOP;
        if (BlackRooks   & bit) return B_ROOK;
        if (BlackQueen   & bit) return B_QUEEN;
        if (BlackKing    & bit) return B_KING;
        
        return NO_PIECE;
    }
};
@endcode

@dot "Position Structure Memory Layout"
digraph memory {
    rankdir=TB;
    node [shape=record, style=filled];
    
    pos [label="{Position Structure|{Piece Bitboards|12 × 8 bytes = 96 bytes}|{Combined Bitboards|4 × 8 bytes = 32 bytes}|{Game State|~16 bytes}|{Hash Key|8 bytes}|Total: ~152 bytes}", fillcolor="#E8F4F8"];
    
    label="Compact and cache-friendly!";
}
@enddot

**Why Each Field is Necessary:**

@dot "Field Dependencies"
digraph fields {
    rankdir=LR;
    node [shape=box, style=filled];
    
    pieces [label="Piece\nBitboards", fillcolor="#90EE90"];
    combined [label="Combined\nBitboards", fillcolor="#FFD700"];
    castling [label="Castling\nRights", fillcolor="#FFB6C1"];
    ep [label="En Passant", fillcolor="#DDA0DD"];
    fifty [label="50-Move\nClock", fillcolor="#87CEEB"];
    hash [label="Zobrist\nHash", fillcolor="#FFA500"];
    
    movegen [label="Move\nGeneration", shape=ellipse, fillcolor="#F0E68C"];
    search [label="Search", shape=ellipse, fillcolor="#F0E68C"];
    eval [label="Evaluation", shape=ellipse, fillcolor="#F0E68C"];
    tt [label="Transposition\nTable", shape=ellipse, fillcolor="#F0E68C"];
    
    pieces -> combined [label="compute"];
    pieces -> movegen;
    combined -> movegen;
    castling -> movegen [label="legal\ncastles"];
    ep -> movegen [label="legal\nen passant"];
    
    pieces -> eval;
    combined -> eval;
    
    hash -> tt [label="lookup"];
    fifty -> search [label="draw\ndetection"];
}
@enddot


@subsection fen_parsing 2.3 FEN - Forsyth-Edwards Notation

@subsubsection fen_format FEN Format Specification

**FEN (Forsyth-Edwards Notation)** is the standard way to describe chess positions in a compact string format.

@dot "FEN Components"
digraph fen {
    rankdir=LR;
    node [shape=box, style="filled,rounded"];
    
    fen [label="FEN String", fillcolor="#90EE90", shape=ellipse];
    
    placement [label="1. Piece\nPlacement", fillcolor="#FFD700"];
    turn [label="2. Active\nColor", fillcolor="#DDA0DD"];
    castle [label="3. Castling\nRights", fillcolor="#87CEEB"];
    ep [label="4. En Passant\nSquare", fillcolor="#FFB6C1"];
    halfmove [label="5. Halfmove\nClock", fillcolor="#FFA500"];
    fullmove [label="6. Fullmove\nNumber", fillcolor="#F0E68C"];
    
    fen -> placement;
    fen -> turn;
    fen -> castle;
    fen -> ep;
    fen -> halfmove;
    fen -> fullmove;
    
    label="6 space-separated fields";
}
@enddot

**Example FEN (Starting Position):**

@verbatim
rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1
|____________________________________________| | |__| | |
              Field 1                        | |  |  | |
         (Piece Placement)                   | |  |  | |
                                             | |  |  | |
                        Field 2 (Turn) ------+ |  |  | |
                                               |  |  | |
               Field 3 (Castling Rights) ------+  |  | |
                                                  |  | |
                   Field 4 (En Passant) ----------+  | |
                                                     | |
                      Field 5 (Halfmove) ------------+ |
                                                       |
                      Field 6 (Fullmove) --------------+
@endverbatim

**Field-by-Field Breakdown:**

| Field | Content | Values | Example |
|-------|---------|--------|---------|
| 1 | Piece placement (rank 8 → rank 1) | Pieces: KQRBNPkqrbnp<br/>Empty: 1-8<br/>Separator: / | rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR |
| 2 | Active color | w or b | w |
| 3 | Castling rights | KQkq or - | KQkq |
| 4 | En passant square | Square or - | - or e3 |
| 5 | Halfmove clock | 0-100 | 0 |
| 6 | Fullmove number | 1+ | 1 |

@subsubsection fen_piece_placement Parsing Piece Placement

**Encoding Rules:**

@code{.cpp}
// Piece letters:
// UPPERCASE = White pieces
// P = pawn, N = knight, B = bishop, R = rook, Q = queen, K = king
//
// lowercase = Black pieces  
// p = pawn, n = knight, b = bishop, r = rook, q = queen, k = king
//
// Numbers 1-8 = consecutive empty squares
// / = separator between ranks

// Example: "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR"
//
// Rank 8: rnbqkbnr    (black back rank)
// Rank 7: pppppppp    (black pawns)
// Rank 6: 8           (8 empty squares)
// Rank 5: 8           (8 empty squares)
// Rank 4: 8           (8 empty squares)
// Rank 3: 8           (8 empty squares)
// Rank 2: PPPPPPPP    (white pawns)
// Rank 1: RNBQKBNR    (white back rank)
@endcode

**Complete Parsing Algorithm:**

@code{.cpp}
/**
 * @brief Parse FEN string and populate Position structure
 * @param pos Position to fill
 * @param fen FEN string to parse
 * @return true if parsing successful, false if invalid FEN
 */
bool parseFEN(Position& pos, const std::string& fen) {
    // STEP 1: Clear all bitboards
    pos.WhitePawns = pos.WhiteKnights = pos.WhiteBishops = 0;
    pos.WhiteRooks = pos.WhiteQueen = pos.WhiteKing = 0;
    pos.BlackPawns = pos.BlackKnights = pos.BlackBishops = 0;
    pos.BlackRooks = pos.BlackQueen = pos.BlackKing = 0;
    
    // STEP 2: Split FEN into 6 fields
    std::istringstream iss(fen);
    std::string piece_data, turn, castling, ep, halfmove_str, fullmove_str;
    
    if (!(iss >> piece_data >> turn >> castling >> ep >> halfmove_str >> fullmove_str)) {
        return false;  // Invalid FEN (missing fields)
    }
    
    // STEP 3: Parse piece placement (rank 8 down to rank 1)
    int rank = 7;  // Start at rank 8 (index 7)
    int file = 0;  // Start at file a (index 0)
    
    for (char c : piece_data) {
        if (c == '/') {
            // Rank separator - move to next rank
            if (file != 8) return false;  // Rank must have 8 squares!
            rank--;
            file = 0;
        }
        else if (c >= '1' && c <= '8') {
            // Empty squares - skip ahead
            file += (c - '0');
        }
        else {
            // It's a piece - place it on the board
            if (file >= 8) return false;  // Too many squares in rank!
            
            int square = rank * 8 + file;
            uint64_t bit = (1ULL << square);
            
            switch (c) {
                case 'P': pos.WhitePawns   |= bit; break;
                case 'N': pos.WhiteKnights |= bit; break;
                case 'B': pos.WhiteBishops |= bit; break;
                case 'R': pos.WhiteRooks   |= bit; break;
                case 'Q': pos.WhiteQueen   |= bit; break;
                case 'K': pos.WhiteKing    |= bit; break;
                
                case 'p': pos.BlackPawns   |= bit; break;
                case 'n': pos.BlackKnights |= bit; break;
                case 'b': pos.BlackBishops |= bit; break;
                case 'r': pos.BlackRooks   |= bit; break;
                case 'q': pos.BlackQueen   |= bit; break;
                case 'k': pos.BlackKing    |= bit; break;
                
                default: return false;  // Invalid piece character
            }
            
            file++;
        }
    }
    
    // Validate we ended at rank 1, file 8
    if (rank != 0 || file != 8) return false;
    
    // STEP 4: Parse active color
    if (turn == "w") {
        pos.whiteToMove = true;
    } else if (turn == "b") {
        pos.whiteToMove = false;
    } else {
        return false;  // Invalid turn indicator
    }
    
    // STEP 5: Parse castling rights
    pos.castelingRights = 0;
    
    if (castling != "-") {
        for (char c : castling) {
            switch (c) {
                case 'K': pos.castelingRights |= 0x1; break;  // White kingside
                case 'Q': pos.castelingRights |= 0x2; break;  // White queenside
                case 'k': pos.castelingRights |= 0x4; break;  // Black kingside
                case 'q': pos.castelingRights |= 0x8; break;  // Black queenside
                default: return false;  // Invalid castling character
            }
        }
    }
    
    // STEP 6: Parse en passant square
    pos.enPassant = 0;
    
    if (ep != "-") {
        if (ep.length() != 2) return false;
        
        int ep_file = ep[0] - 'a';
        int ep_rank = ep[1] - '1';
        
        if (ep_file < 0 || ep_file > 7 || ep_rank < 0 || ep_rank > 7) {
            return false;  // Invalid square
        }
        
        int ep_square = ep_rank * 8 + ep_file;
        pos.enPassant = (1ULL << ep_square);
    }
    
    // STEP 7: Parse move counters
    try {
        pos.move50rule = std::stoi(halfmove_str);
        pos.fullMoveNumber = std::stoi(fullmove_str);
    } catch (...) {
        return false;  // Invalid number format
    }
    
    // STEP 8: Update derived data
    pos.updateOccupancy();
    pos.zobrist_key = computeZobristKey(pos);  // Compute hash
    
    // STEP 9: Validate position
    // Check exactly one king per side
    if (__builtin_popcountll(pos.WhiteKing) != 1) return false;
    if (__builtin_popcountll(pos.BlackKing) != 1) return false;
    
    // Check no pawns on rank 1 or 8
    uint64_t rank1_8 = 0xFF00000000000000ULL | 0xFFULL;
    if ((pos.WhitePawns | pos.BlackPawns) & rank1_8) return false;
    
    return true;  // Success!
}
@endcode

@startuml "FEN Parsing State Machine"
!theme cerulean

[*] --> ParsePieces
ParsePieces --> ParseTurn : All 8 ranks processed
ParseTurn --> ParseCastling : Turn parsed
ParseCastling --> ParseEnPassant : Rights parsed
ParseEnPassant --> ParseHalfmove : EP parsed
ParseHalfmove --> ParseFullmove : Halfmove parsed
ParseFullmove --> ValidatePosition : Fullmove parsed
ValidatePosition --> ComputeHash : Valid
ValidatePosition --> [*] : Invalid (error)
ComputeHash --> [*] : Success

note right of ParsePieces
  Process rank by rank
  from rank 8 to rank 1
end note

note right of ValidatePosition
  Check:
  • Exactly 1 king per side
  • No pawns on rank 1/8
  • Piece counts reasonable
end note
@enduml

**Example Walkthrough:**

@verbatim
FEN: "r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1"

Step-by-step parsing:

1. Piece Placement: "r3k2r/8/8/8/8/8/8/R3K2R"
   
   Rank 8: r3k2r
   - 'r' at a8 (square 56): BlackRooks |= (1ULL << 56)
   - '3' = 3 empty squares (b8, c8, d8)
   - 'k' at e8 (square 60): BlackKing |= (1ULL << 60)
   - '2' = 2 empty squares (f8, g8)
   - 'r' at h8 (square 63): BlackRooks |= (1ULL << 63)
   
   Ranks 7-2: "8" = all empty
   
   Rank 1: R3K2R
   - 'R' at a1 (square 0): WhiteRooks |= (1ULL << 0)
   - '3' = 3 empty squares
   - 'K' at e1 (square 4): WhiteKing |= (1ULL << 4)
   - '2' = 2 empty squares
   - 'R' at h1 (square 7): WhiteRooks |= (1ULL << 7)

2. Active Color: "w"
   whiteToMove = true

3. Castling Rights: "KQkq"
   castelingRights = 0x1 | 0x2 | 0x4 | 0x8 = 0xF (all rights)

4. En Passant: "-"
   enPassant = 0 (no en passant available)

5. Halfmove Clock: "0"
   move50rule = 0

6. Fullmove Number: "1"
   fullMoveNumber = 1

Result: Position with rooks and kings only, all castling rights
@endverbatim

*/


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


---

@section chapter_3 Chapter 3: Move Generation - Finding All Legal Moves

@subsection movegen_overview 3.1 Move Generation Overview

Move generation is the **most performance-critical** component of a chess engine. During a search to depth 10, an engine may generate moves for **millions** of positions.

@subsubsection movegen_types Types of Move Generation

@dot "Move Generation Pipeline"
digraph movegen_pipeline {
    rankdir=TB;
    node [shape=box, style="filled,rounded"];
    
    position [label="Current Position", fillcolor="#90EE90"];
    
    pseudo [label="PSEUDO-LEGAL MOVES\nGenerate all moves that\nlook legal but might\nleave king in check", fillcolor="#FFD700"];
    
    legal [label="LEGAL MOVES\nFilter out moves that\nleave king in check", fillcolor="#87CEEB"];
    
    position -> pseudo [label="Fast generation"];
    pseudo -> legal [label="Legality check\n(make/unmake)"];
    
    note_pseudo [shape=note, fillcolor="#FFFACD", label="Pseudo-legal example:\nMove rook, but own king\nis in check after move"];
    
    note_legal [shape=note, fillcolor="#FFFACD", label="Legal moves are\nactually playable\nwithout breaking rules"];
    
    pseudo -> note_pseudo [style=dashed];
    legal -> note_legal [style=dashed];
}
@enddot

**Performance Considerations:**

| Approach | Speed | Safety | When to Use |
|----------|-------|--------|-------------|
| **Pseudo-legal** | Very fast (no validation) | Must validate after | Search (generate → try → validate) |
| **Legal only** | Slower (validates all) | Always safe | UCI move parsing, endgame |
| **Hybrid** | Fast for most pieces | Safe for king moves | Practical engines |

@subsubsection attack_tables_precomputed 3.2 Pre-computed Attack Tables

For pieces that don't depend on blockers (king, knight, pawn attacks), we pre-compute all attack patterns.

**Why Pre-compute?**

@code{.cpp}
// WITHOUT pre-computation (slow):
uint64_t get_knight_attacks_slow(int square) {
    uint64_t attacks = 0;
    int rank = square / 8;
    int file = square % 8;
    
    // Try all 8 knight moves
    if (rank + 2 < 8 && file + 1 < 8) attacks |= (1ULL << (square + 17));
    if (rank + 2 < 8 && file - 1 >= 0) attacks |= (1ULL << (square + 15));
    if (rank - 2 >= 0 && file + 1 < 8) attacks |= (1ULL << (square - 15));
    if (rank - 2 >= 0 && file - 1 >= 0) attacks |= (1ULL << (square - 17));
    if (rank + 1 < 8 && file + 2 < 8) attacks |= (1ULL << (square + 10));
    if (rank + 1 < 8 && file - 2 >= 0) attacks |= (1ULL << (square + 6));
    if (rank - 1 >= 0 && file + 2 < 8) attacks |= (1ULL << (square - 6));
    if (rank - 1 >= 0 && file - 2 >= 0) attacks |= (1ULL << (square - 10));
    
    return attacks;  // ~16 operations + branches
}

// WITH pre-computation (fast):
uint64_t knightAttacks[64];  // Pre-computed at startup

uint64_t get_knight_attacks_fast(int square) {
    return knightAttacks[square];  // 1 operation!
}
@endcode

**Visual Example - Knight Attack Pattern:**

@verbatim
Knight on e4 (square 28) can attack 8 squares:

  a  b  c  d  e  f  g  h
8 .  .  .  .  .  .  .  .
7 .  .  .  .  .  .  .  .
6 .  .  .  x  .  x  .  .   <- d6, f6
5 .  .  x  .  .  .  x  .   <- c5, g5
4 .  .  .  .  N  .  .  .   <- Knight here
3 .  .  x  .  .  .  x  .   <- c3, g3
2 .  .  .  x  .  x  .  .   <- d2, f2
1 .  .  .  .  .  .  .  .

Attack bitboard for e4:
Bits set at: 12, 14, 19, 23, 33, 37, 42, 44

Hex value: 0x0000284400442800
Binary (showing only relevant bits):
  ...0010100001000100000001000100100000000000

This is stored in knightAttacks[28]
@endverbatim

**Knight Attack Table Generation:**

@code{.cpp}
/**
 * @brief Initialize knight attack table
 * 
 * For each square, compute all squares a knight can attack.
 * Knight moves in L-shape: 2 squares in one direction, 1 perpendicular.
 * 
 * Time complexity: O(64 × 8) = O(1) - runs once at startup
 */
void init_knight_attacks() {
    // Knight move offsets (relative to current square)
    // These represent: 2 up 1 right, 2 up 1 left, etc.
    const int knight_offsets[8] = {
        17,   // 2 ranks up, 1 file right
        15,   // 2 ranks up, 1 file left
        10,   // 1 rank up, 2 files right
        6,    // 1 rank up, 2 files left
        -6,   // 1 rank down, 2 files right
        -10,  // 1 rank down, 2 files left
        -15,  // 2 ranks down, 1 file right
        -17   // 2 ranks down, 1 file left
    };
    
    for (int square = 0; square < 64; square++) {
        uint64_t attacks = 0;
        
        int rank = square / 8;
        int file = square % 8;
        
        for (int offset : knight_offsets) {
            int target = square + offset;
            
            // Check if target is on board
            if (target < 0 || target >= 64) continue;
            
            int target_rank = target / 8;
            int target_file = target % 8;
            
            // Check if move wrapped around board edges
            // (knight can't jump more than 2 files or ranks)
            int rank_diff = abs(target_rank - rank);
            int file_diff = abs(target_file - file);
            
            if (rank_diff <= 2 && file_diff <= 2) {
                attacks |= (1ULL << target);
            }
        }
        
        knightAttacks[square] = attacks;
    }
}
@endcode

**King Attack Table (simpler - only 8 directions):**

@code{.cpp}
void init_king_attacks() {
    // King moves: 1 square in any direction (including diagonal)
    const int king_offsets[8] = {
        8,    // North
        -8,   // South
        1,    // East
        -1,   // West
        9,    // North-East
        7,    // North-West
        -7,   // South-East
        -9    // South-West
    };
    
    for (int square = 0; square < 64; square++) {
        uint64_t attacks = 0;
        
        int rank = square / 8;
        int file = square % 8;
        
        for (int offset : king_offsets) {
            int target = square + offset;
            
            if (target < 0 || target >= 64) continue;
            
            int target_rank = target / 8;
            int target_file = target % 8;
            
            // King moves exactly 1 square
            if (abs(target_rank - rank) <= 1 && abs(target_file - file) <= 1) {
                attacks |= (1ULL << target);
            }
        }
        
        kingAttacks[square] = attacks;
    }
}
@endcode

@verbatim
King Attack Pattern (e4):

  a  b  c  d  e  f  g  h
8 .  .  .  .  .  .  .  .
7 .  .  .  .  .  .  .  .
6 .  .  .  .  .  .  .  .
5 .  .  .  x  x  x  .  .   <- d5, e5, f5
4 .  .  .  x  K  x  .  .   <- d4, (king), f4
3 .  .  .  x  x  x  .  .   <- d3, e3, f3
2 .  .  .  .  .  .  .  .
1 .  .  .  .  .  .  .  .

8 possible moves (fewer on edges/corners)
Corner king has only 3 moves!
@endverbatim

**Pawn Attacks (Color-Dependent):**

@code{.cpp}
// Pawns are special: different attacks for white/black
uint64_t whitePawnAttacks[64];
uint64_t blackPawnAttacks[64];

void init_pawn_attacks() {
    for (int square = 0; square < 64; square++) {
        int rank = square / 8;
        int file = square % 8;
        
        // White pawns attack diagonally UP (northeast, northwest)
        whitePawnAttacks[square] = 0;
        if (rank < 7) {  // Not on 8th rank
            if (file > 0) {  // Northwest
                whitePawnAttacks[square] |= (1ULL << (square + 7));
            }
            if (file < 7) {  // Northeast
                whitePawnAttacks[square] |= (1ULL << (square + 9));
            }
        }
        
        // Black pawns attack diagonally DOWN (southeast, southwest)
        blackPawnAttacks[square] = 0;
        if (rank > 0) {  // Not on 1st rank
            if (file > 0) {  // Southwest
                blackPawnAttacks[square] |= (1ULL << (square - 9));
            }
            if (file < 7) {  // Southeast
                blackPawnAttacks[square] |= (1ULL << (square - 7));
            }
        }
    }
}
@endcode

@verbatim
Pawn Attack Patterns:

White Pawn on e4:              Black Pawn on e5:

  a  b  c  d  e  f  g  h          a  b  c  d  e  f  g  h
8 .  .  .  .  .  .  .  .        8 .  .  .  .  .  .  .  .
7 .  .  .  .  .  .  .  .        7 .  .  .  .  .  .  .  .
6 .  .  .  .  .  .  .  .        6 .  .  .  .  .  .  .  .
5 .  .  .  x  .  x  .  .        5 .  .  .  .  p  .  .  .  <- Black pawn
4 .  .  .  .  P  .  .  .        4 .  .  .  x  .  x  .  .  <- Attacks down
3 .  .  .  .  .  .  .  .        3 .  .  .  .  .  .  .  .
2 .  .  .  .  .  .  .  .        2 .  .  .  .  .  .  .  .
1 .  .  .  .  .  .  .  .        1 .  .  .  .  .  .  .  .
       ^        ^                        ^        ^
    Attack    Attack                 Attack    Attack
    d5        f5                     d4        f4
@endverbatim

@subsection magic_bitboards 3.3 Magic Bitboards - Sliding Piece Attacks

@subsubsection why_magic_needed Why We Need Magic Bitboards

Sliding pieces (rook, bishop, queen) are **much harder** than leapers (knight, king):

@dot "Sliding Piece Problem"
digraph sliding_problem {
    rankdir=TB;
    node [shape=box, style="filled,rounded"];
    
    problem [label="PROBLEM:\nRook attacks depend\non blocking pieces", fillcolor="#FF6347", fontcolor=white];
    
    example1 [label="Empty Board:\nRook on e4 attacks\n14 squares", fillcolor="#FFD700"];
    
    example2 [label="Blocked Board:\nRook on e4 attacks\nonly 6 squares!", fillcolor="#FFA500"];
    
    solution [label="SOLUTION:\nMagic Bitboards\nFast lookup based\non blockers", fillcolor="#90EE90"];
    
    problem -> example1;
    problem -> example2;
    example1 -> solution;
    example2 -> solution;
}
@enddot

**Visual Example - Rook Attacks Depend on Blockers:**

@verbatim
Case 1: Empty board               Case 2: With blockers

  a  b  c  d  e  f  g  h             a  b  c  d  e  f  g  h
8 .  .  .  .  x  .  .  .           8 .  .  .  .  x  .  .  .
7 .  .  .  .  x  .  .  .           7 .  .  .  .  X  .  .  .  <- Blocker!
6 .  .  .  .  x  .  .  .           6 .  .  .  .  x  .  .  .
5 .  .  .  .  x  .  .  .           5 .  .  .  .  x  .  .  .
4 x  x  x  x  R  x  x  x           4 x  X  .  .  R  x  x  X  <- Blockers!
3 .  .  .  .  x  .  .  .           3 .  .  .  .  .  .  .  .
2 .  .  .  .  x  .  .  .           2 .  .  .  .  .  .  .  .
1 .  .  .  .  x  .  .  .           1 .  .  .  .  .  .  .  .

Attacks: 14 squares                Attacks: 6 squares
(entire rank + file)               (stops at blockers)

We need DIFFERENT attack bitboards for different blocker patterns!
@endverbatim

@subsubsection magic_concept The Magic Bitboard Concept

**Main Idea:** Use a **perfect hash function** to map blocker configurations to attack bitboards.

@dot "Magic Bitboard Lookup Process"
digraph magic_lookup {
    rankdir=LR;
    node [shape=box, style="filled,rounded"];
    
    occupied [label="Occupied\nSquares\n(all pieces)", fillcolor="#E8F4F8"];
    
    mask [label="Mask Relevant\nBlockers\n(only squares\nthat matter)", fillcolor="#FFD700"];
    
    blockers [label="Blocker\nConfiguration", fillcolor="#FFA500"];
    
    magic [label="Multiply by\nMAGIC NUMBER", fillcolor="#FF6347", fontcolor=white];
    
    shift [label="Shift Right\n(extract bits)", fillcolor="#DDA0DD"];
    
    index [label="Index into\nAttack Table", fillcolor="#87CEEB"];
    
    result [label="Attack\nBitboard", fillcolor="#90EE90"];
    
    occupied -> mask;
    mask -> blockers [label="AND"];
    blockers -> magic [label="×"];
    magic -> shift [label=">>"];
    shift -> index;
    index -> result [label="table[index]"];
}
@enddot

**Step-by-Step Example:**

@code{.cpp}
/**
 * @brief Get rook attacks using magic bitboards
 * 
 * @param square Rook position (0-63)
 * @param occupied Bitboard of all pieces on board
 * @return Bitboard of squares rook can attack
 */
uint64_t get_rook_attacks(int square, uint64_t occupied) {
    // STEP 1: Mask relevant blockers
    // Only blockers on rook's rank/file matter (edges don't block)
    uint64_t blockers = occupied & rook_masks[square];
    
    // Example for rook on e4:
    // rook_masks[28] = rank 4 + file e (minus edges)
    // = 0x0000001010106E00
    
    // STEP 2: Apply magic multiplication
    // Magic number chosen to create unique hash for each blocker config
    uint64_t hash = blockers * rook_magics[square];
    
    // STEP 3: Shift to get table index
    // Shift right by (64 - bits_needed)
    // For e4: need 12 bits (4096 possibilities)
    int index = hash >> (64 - rook_shift[square]);
    
    // STEP 4: Lookup pre-computed attack bitboard
    return rook_attacks[square][index];
}

// Pre-computed tables (initialized at startup):
uint64_t rook_masks[64];        // Relevant blocker masks
uint64_t rook_magics[64];       // Magic numbers (found by search)
int rook_shift[64];             // How many bits to shift
uint64_t* rook_attacks[64];     // Attack tables (one per square)

// Example magic number for e4:
// rook_magics[28] = 0x0080001020400080ULL
// This was found by trial-and-error to create perfect hash!
@endcode

**Why This Works - The Magic Property:**

@verbatim
For rook on e4, there are 2^10 = 1024 possible blocker configurations.

Magic multiplication spreads these 1024 patterns across the upper bits
of a 64-bit number such that after shifting, each pattern has a UNIQUE
index into the attack table.

Example (simplified with 8-bit numbers):

Blocker Pattern 1: 00101000
× Magic:           10110011
= Product:         ???10110...
>> Shift:          000010110  (index 22)

Blocker Pattern 2: 01001000  (different pattern!)
× Magic:           10110011
= Product:         ???11101...
>> Shift:          000011101  (index 29, different!)

The magic number ensures NO COLLISIONS!
@endverbatim

**Finding Magic Numbers (done once, offline):**

@code{.cpp}
/**
 * @brief Find a magic number for a square (brute force search)
 * 
 * A magic number is valid if it creates a perfect hash (no collisions)
 * for all possible blocker configurations on this square.
 * 
 * @param square Square index
 * @param bits Number of bits in mask
 * @param is_bishop true for bishop, false for rook
 * @return Magic number if found, 0 if failed
 */
uint64_t find_magic(int square, int bits, bool is_bishop) {
    uint64_t mask = is_bishop ? get_bishop_mask(square) : get_rook_mask(square);
    int n = __builtin_popcountll(mask);
    int permutations = 1 << n;  // 2^n possible blocker configs
    
    // Generate all possible blocker configurations
    uint64_t blockers[4096];
    uint64_t attacks[4096];
    
    for (int i = 0; i < permutations; i++) {
        blockers[i] = index_to_blockers(i, mask);
        attacks[i] = is_bishop ? 
            compute_bishop_attacks_slow(square, blockers[i]) :
            compute_rook_attacks_slow(square, blockers[i]);
    }
    
    // Try random magic numbers until we find one that works
    for (int attempt = 0; attempt < 100000000; attempt++) {
        uint64_t magic = random_uint64_few_bits();  // Sparse random number
        
        // Test if this magic creates perfect hash
        uint64_t used[4096] = {0};
        bool collision = false;
        
        for (int i = 0; i < permutations; i++) {
            int index = (blockers[i] * magic) >> (64 - bits);
            
            if (used[index] == 0) {
                used[index] = attacks[i];
            } else if (used[index] != attacks[i]) {
                collision = true;  // Collision detected!
                break;
            }
        }
        
        if (!collision) {
            return magic;  // Found a valid magic number!
        }
    }
    
    return 0;  // Failed (very rare with good RNG)
}
@endcode

**Performance Comparison:**

| Method | Operations | Speed |
|--------|-----------|-------|
| **Classical (ray-tracing)** | ~20-40 per lookup | Baseline |
| **Kindergarten bitboards** | ~10 | 2-3x faster |
| **Magic bitboards** | 3 (AND, multiply, shift, array access) | **10-15x faster** |
| **PEXT (BMI2)** | 2 (PEXT instruction, array access) | **20x faster** (if CPU supports) |


@subsection complete_movegen 3.4 Complete Move Generation Implementation

@subsubsection pawn_moves Generating Pawn Moves - The Complex Case

Pawns are the most complex piece for move generation due to:
1. Different movement and capture directions
2. Double-push on first move
3. En passant capture
4. Promotion to 4 different pieces

@dot "Pawn Move Types"
digraph pawn_moves {
    rankdir=TB;
    node [shape=box, style="filled,rounded"];
    
    pawn [label="Pawn Move\nGeneration", fillcolor="#90EE90"];
    
    single [label="Single Push\nForward 1 square", fillcolor="#FFD700"];
    double [label="Double Push\nForward 2 squares\n(from start rank)", fillcolor="#FFA500"];
    capture [label="Diagonal Captures\n(2 directions)", fillcolor="#FF6347", fontcolor=white];
    ep [label="En Passant\nSpecial capture", fillcolor="#DDA0DD"];
    promo [label="Promotions\n(4 pieces × moves)", fillcolor="#87CEEB"];
    
    pawn -> single;
    pawn -> double;
    pawn -> capture;
    pawn -> ep;
    
    single -> promo [label="if rank 7/2"];
    capture -> promo [label="if rank 7/2"];
}
@enddot

**Complete Implementation:**

@code{.cpp}
/**
 * @brief Generate all pawn moves (pushes, captures, en passant, promotions)
 * 
 * Handles all pawn move types including special cases.
 * Generates 4 moves for each promotion.
 * 
 * @param pos Current position
 * @param moves MoveList to append moves to
 */
void generate_pawn_moves(const Position& pos, MoveList& moves) {
    bool white = pos.whiteToMove;
    uint64_t pawns = white ? pos.WhitePawns : pos.BlackPawns;
    uint64_t empty = pos.emptySquares;
    uint64_t enemies = white ? pos.BlackoccupiedSquares : pos.WhiteoccupiedSquares;
    
    int forward = white ? 8 : -8;
    int start_rank = white ? 1 : 6;
    int promo_rank = white ? 7 : 0;
    
    // ========================================
    // SINGLE PUSHES
    // ========================================
    uint64_t push_targets = white ?
        (pawns << 8) & empty :   // White pawns push north
        (pawns >> 8) & empty;    // Black pawns push south
    
    while (push_targets) {
        int to = __builtin_ctzll(push_targets);
        int from = to - forward;
        
        if (to / 8 == promo_rank) {
            // Promotion - generate 4 moves
            moves.add(Move(from, to, white ? W_QUEEN : B_QUEEN));
            moves.add(Move(from, to, white ? W_ROOK : B_ROOK));
            moves.add(Move(from, to, white ? W_BISHOP : B_BISHOP));
            moves.add(Move(from, to, white ? W_KNIGHT : B_KNIGHT));
        } else {
            // Normal push
            moves.add(Move(from, to));
        }
        
        push_targets &= push_targets - 1;
    }
    
    // ========================================
    // DOUBLE PUSHES
    // ========================================
    uint64_t start_pawns = pawns & (white ? 0x000000000000FF00ULL : 0x00FF000000000000ULL);
    uint64_t single_push = white ? (start_pawns << 8) & empty : (start_pawns >> 8) & empty;
    uint64_t double_push = white ? (single_push << 8) & empty : (single_push >> 8) & empty;
    
    while (double_push) {
        int to = __builtin_ctzll(double_push);
        int from = to - (2 * forward);
        
        moves.add(Move(from, to));
        double_push &= double_push - 1;
    }
    
    // ========================================
    // CAPTURES (LEFT/RIGHT)
    // ========================================
    
    // Left captures (from pawn's perspective)
    uint64_t left_captures = white ?
        ((pawns & 0xFEFEFEFEFEFEFEFEULL) << 7) & enemies :  // Not A-file
        ((pawns & 0xFEFEFEFEFEFEFEFEULL) >> 9) & enemies;
    
    while (left_captures) {
        int to = __builtin_ctzll(left_captures);
        int from = to - (white ? 7 : -9);
        
        if (to / 8 == promo_rank) {
            moves.add(Move(from, to, white ? W_QUEEN : B_QUEEN));
            moves.add(Move(from, to, white ? W_ROOK : B_ROOK));
            moves.add(Move(from, to, white ? W_BISHOP : B_BISHOP));
            moves.add(Move(from, to, white ? W_KNIGHT : B_KNIGHT));
        } else {
            moves.add(Move(from, to));
        }
        
        left_captures &= left_captures - 1;
    }
    
    // Right captures
    uint64_t right_captures = white ?
        ((pawns & 0x7F7F7F7F7F7F7F7FULL) << 9) & enemies :  // Not H-file
        ((pawns & 0x7F7F7F7F7F7F7F7FULL) >> 7) & enemies;
    
    while (right_captures) {
        int to = __builtin_ctzll(right_captures);
        int from = to - (white ? 9 : -7);
        
        if (to / 8 == promo_rank) {
            moves.add(Move(from, to, white ? W_QUEEN : B_QUEEN));
            moves.add(Move(from, to, white ? W_ROOK : B_ROOK));
            moves.add(Move(from, to, white ? W_BISHOP : B_BISHOP));
            moves.add(Move(from, to, white ? W_KNIGHT : B_KNIGHT));
        } else {
            moves.add(Move(from, to));
        }
        
        right_captures &= right_captures - 1;
    }
    
    // ========================================
    // EN PASSANT
    // ========================================
    if (pos.enPassant) {
        int ep_square = __builtin_ctzll(pos.enPassant);
        
        // Find pawns that can capture en passant
        uint64_t ep_attackers = white ?
            (whitePawnAttacks[ep_square - 8]) & pawns :  // EP target is where pawn moved TO
            (blackPawnAttacks[ep_square + 8]) & pawns;
        
        while (ep_attackers) {
            int from = __builtin_ctzll(ep_attackers);
            moves.add(Move(from, ep_square, MOVE_FLAG_EP));
            ep_attackers &= ep_attackers - 1;
        }
    }
}
@endcode

**Visual Example - Pawn Move Generation:**

@verbatim
Position: White pawn on e2, black pawn on d7 (just moved d7-d5)

  a  b  c  d  e  f  g  h
8 .  .  .  .  .  .  .  .
7 .  .  .  .  .  .  .  .
6 .  .  .  .  .  .  .  .
5 .  .  .  p  .  .  .  .  <- Black pawn (just moved 2 squares)
4 .  .  .  .  .  .  .  .
3 .  .  .  .  .  .  .  .
2 .  .  .  .  P  .  .  .  <- White pawn
1 .  .  .  .  .  .  .  .

Generated Moves for e2 pawn:
1. e2-e3 (single push)
2. e2-e4 (double push)

If white plays e2-e4, then black d5 pawn can capture e4 en passant!

Next position:
  a  b  c  d  e  f  g  h
5 .  .  .  p  .  .  .  .
4 .  .  .  .  P  .  .  .  <- Just moved here
3 .  .  .  X  .  .  .  .  <- En passant target square (d3)

Black's d5 pawn can now:
- Normal: d5-d4
- En passant: d5xd4 (captures on d4, removes pawn from e4!)
@endverbatim

@subsubsection knight_moves Knight Move Generation - Simple and Fast

@code{.cpp}
/**
 * @brief Generate knight moves
 * 
 * Knights are simple: just use pre-computed attack table
 * and filter out squares occupied by own pieces.
 */
void generate_knight_moves(const Position& pos, MoveList& moves) {
    bool white = pos.whiteToMove;
    uint64_t knights = white ? pos.WhiteKnights : pos.BlackKnights;
    uint64_t own_pieces = white ? pos.WhiteoccupiedSquares : pos.BlackoccupiedSquares;
    
    while (knights) {
        int from = __builtin_ctzll(knights);
        
        // Get attacks and remove own pieces
        uint64_t attacks = knightAttacks[from] & ~own_pieces;
        
        while (attacks) {
            int to = __builtin_ctzll(attacks);
            moves.add(Move(from, to));
            attacks &= attacks - 1;
        }
        
        knights &= knights - 1;
    }
}
@endcode

@subsubsection sliding_moves Sliding Piece Move Generation

@code{.cpp}
/**
 * @brief Generate rook moves using magic bitboards
 */
void generate_rook_moves(const Position& pos, MoveList& moves) {
    bool white = pos.whiteToMove;
    uint64_t rooks = white ? pos.WhiteRooks : pos.BlackRooks;
    uint64_t own_pieces = white ? pos.WhiteoccupiedSquares : pos.BlackoccupiedSquares;
    
    while (rooks) {
        int from = __builtin_ctzll(rooks);
        
        // Use magic bitboards to get attacks
        uint64_t attacks = get_rook_attacks(from, pos.occupiedSquares) & ~own_pieces;
        
        while (attacks) {
            int to = __builtin_ctzll(attacks);
            moves.add(Move(from, to));
            attacks &= attacks - 1;
        }
        
        rooks &= rooks - 1;
    }
}

/**
 * @brief Generate bishop moves using magic bitboards
 */
void generate_bishop_moves(const Position& pos, MoveList& moves) {
    bool white = pos.whiteToMove;
    uint64_t bishops = white ? pos.WhiteBishops : pos.BlackBishops;
    uint64_t own_pieces = white ? pos.WhiteoccupiedSquares : pos.BlackoccupiedSquares;
    
    while (bishops) {
        int from = __builtin_ctzll(bishops);
        
        uint64_t attacks = get_bishop_attacks(from, pos.occupiedSquares) & ~own_pieces;
        
        while (attacks) {
            int to = __builtin_ctzll(attacks);
            moves.add(Move(from, to));
            attacks &= attacks - 1;
        }
        
        bishops &= bishops - 1;
    }
}

/**
 * @brief Generate queen moves (combines rook + bishop)
 */
void generate_queen_moves(const Position& pos, MoveList& moves) {
    bool white = pos.whiteToMove;
    uint64_t queens = white ? pos.WhiteQueen : pos.BlackQueen;
    uint64_t own_pieces = white ? pos.WhiteoccupiedSquares : pos.BlackoccupiedSquares;
    
    while (queens) {
        int from = __builtin_ctzll(queens);
        
        // Queen = Rook + Bishop attacks
        uint64_t attacks = (get_rook_attacks(from, pos.occupiedSquares) |
                           get_bishop_attacks(from, pos.occupiedSquares)) & ~own_pieces;
        
        while (attacks) {
            int to = __builtin_ctzll(attacks);
            moves.add(Move(from, to));
            attacks &= attacks - 1;
        }
        
        queens &= queens - 1;
    }
}
@endcode

@subsubsection castling_generation Castling Move Generation - Complex Rules

Castling has many conditions that must ALL be met:

@dot "Castling Validation Flow"
digraph castling {
    rankdir=TB;
    node [shape=diamond, style=filled, fillcolor="#FFE4B5"];
    
    start [label="Check Castling?", shape=box, fillcolor="#90EE90"];
    
    rights [label="Has castling\nrights?"];
    empty [label="Squares\nbetween empty?"];
    check [label="King in\ncheck?"];
    through [label="King passes\nthrough check?"];
    into [label="King moves\ninto check?"];
    
    legal [label="LEGAL\nAdd castling move", shape=box, fillcolor="#87CEEB"];
    illegal [label="ILLEGAL\nSkip", shape=box, fillcolor="#FF6347", fontcolor=white];
    
    start -> rights;
    rights -> empty [label="YES"];
    rights -> illegal [label="NO"];
    
    empty -> check [label="YES"];
    empty -> illegal [label="NO"];
    
    check -> through [label="NO"];
    check -> illegal [label="YES"];
    
    through -> into [label="NO"];
    through -> illegal [label="YES"];
    
    into -> legal [label="NO"];
    into -> illegal [label="YES"];
}
@enddot

@code{.cpp}
/**
 * @brief Generate castling moves
 * 
 * Castling is legal if:
 * 1. King and rook haven't moved (castling rights)
 * 2. Squares between are empty
 * 3. King not in check
 * 4. King doesn't pass through check
 * 5. King doesn't land in check
 */
void generate_castling_moves(const Position& pos, MoveList& moves) {
    if (pos.whiteToMove) {
        // ========== WHITE KINGSIDE (e1-g1) ==========
        if ((pos.castelingRights & 0x1) &&              // Has KS rights
            !(pos.occupiedSquares & 0x60ULL) &&         // f1, g1 empty
            !is_square_attacked(pos, 4, false) &&       // e1 not attacked
            !is_square_attacked(pos, 5, false) &&       // f1 not attacked
            !is_square_attacked(pos, 6, false)) {       // g1 not attacked
            
            moves.add(Move(4, 6, MOVE_FLAG_CASTLING));
        }
        
        // ========== WHITE QUEENSIDE (e1-c1) ==========
        if ((pos.castelingRights & 0x2) &&              // Has QS rights
            !(pos.occupiedSquares & 0xEULL) &&          // b1, c1, d1 empty
            !is_square_attacked(pos, 4, false) &&       // e1 not attacked
            !is_square_attacked(pos, 3, false) &&       // d1 not attacked
            !is_square_attacked(pos, 2, false)) {       // c1 not attacked
            
            // Note: b1 can be attacked (rook passes through)
            moves.add(Move(4, 2, MOVE_FLAG_CASTLING));
        }
    } else {
        // ========== BLACK KINGSIDE (e8-g8) ==========
        if ((pos.castelingRights & 0x4) &&
            !(pos.occupiedSquares & 0x6000000000000000ULL) &&
            !is_square_attacked(pos, 60, true) &&
            !is_square_attacked(pos, 61, true) &&
            !is_square_attacked(pos, 62, true)) {
            
            moves.add(Move(60, 62, MOVE_FLAG_CASTLING));
        }
        
        // ========== BLACK QUEENSIDE (e8-c8) ==========
        if ((pos.castelingRights & 0x8) &&
            !(pos.occupiedSquares & 0xE00000000000000ULL) &&
            !is_square_attacked(pos, 60, true) &&
            !is_square_attacked(pos, 59, true) &&
            !is_square_attacked(pos, 58, true)) {
            
            moves.add(Move(60, 58, MOVE_FLAG_CASTLING));
        }
    }
}
@endcode

**Common Castling Mistakes:**

| Mistake | Wrong Behavior | Correct Behavior |
|---------|----------------|------------------|
| Not checking b1/b8 for queenside | Allows castling through attacked b-file | b1/b8 can be attacked (only c,d,e matter) |
| Not moving rook | King moves but rook stays | Must move both pieces! |
| Allowing castling in check | King in check, still castles | Must not be in check before castling |
| Not updating castling rights | Can castle multiple times | Rights removed after king/rook moves |

*/


---

@section chapter_4 Chapter 4: Search Algorithms - Finding the Best Move

@subsection search_overview 4.1 Search Algorithm Overview

The search algorithm is the "brain" of the engine - it looks ahead at possible futures and chooses the best move.

@dot "Search Tree Example - Real Position"
digraph search_tree {
    rankdir=TB;
    node [shape=box, style="filled,rounded", fontsize=10];
    
    root [label="Current Position\ne4 e5 Nf3\nEval: +0.2", fillcolor="#90EE90"];
    
    nc6 [label="After ...Nc6\nEval: +0.1", fillcolor="#FFD700"];
    nf6 [label="After ...Nf6\nEval: +0.3", fillcolor="#FFD700"];
    d6 [label="After ...d6\nEval: +0.4", fillcolor="#FFD700"];
    
    bc4_nc6 [label="After Bc4\nEval: +0.3", fillcolor="#FFA500"];
    bb5_nc6 [label="After Bb5\nEval: +0.2", fillcolor="#FFA500"];
    nc3_nc6 [label="After Nc3\nEval: +0.1", fillcolor="#FFA500"];
    
    root -> nc6 [label="Nc6"];
    root -> nf6 [label="Nf6"];
    root -> d6 [label="d6"];
    
    nc6 -> bc4_nc6 [label="Bc4"];
    nc6 -> bb5_nc6 [label="Bb5"];
    nc6 -> nc3_nc6 [label="Nc3"];
    
    label="Search explores multiple futures\nChooses path with best eval";
}
@enddot

**Key Metrics:**

| Metric | Typical Value | Impact |
|--------|---------------|--------|
| **Branching Factor** | ~35 | Each ply multiplies positions by 35 |
| **Search Depth** | 8-20 plies | Depth 10 = ~10^15 positions without pruning! |
| **Nodes per Second** | 1M - 10M | Modern engines on single thread |
| **Effective Branching** | ~6 | With alpha-beta pruning |

@subsection alphabeta_detailed 4.2 Alpha-Beta Pruning - The Workhorse

@subsubsection alphabeta_concept Alpha-Beta Concept with Real Example

@verbatim
Consider this position:

Position A (White to move):
  We found move M1 that scores +2 (we're up 2 pawns)
  
  Now trying move M2:
    Black responds with R1: Black wins queen! Score = -9
    
  STOP! Don't check black's other responses!
  
  WHY: Black will definitely play R1 (winning queen)
       So M2 scores -9 for us
       But we already have M1 that scores +2
       M2 is worse, no matter what else we find!
       
This is a BETA CUTOFF - opponent won't allow this line.
@endverbatim

**Alpha-Beta Implementation with Detailed Comments:**

@code{.cpp}
/**
 * @brief Alpha-beta search with negamax framework
 * 
 * @param pos Position to search
 * @param depth Remaining depth (plies)
 * @param alpha Lower bound (best we can guarantee)
 * @param beta Upper bound (worst opponent will allow)
 * @param ply Current ply from root (for mate scoring)
 * @return Score from current player's perspective
 */
int alphabeta(Position& pos, int depth, int alpha, int beta, int ply) {
    // ========================================
    // BASE CASE: Depth limit reached
    // ========================================
    if (depth == 0) {
        return quiescence(pos, alpha, beta);  // Don't stop at tactical position
    }
    
    // ========================================
    // Check for draw conditions
    // ========================================
    
    // Fifty-move rule
    if (pos.move50rule >= 100) {
        return 0;
    }
    
    // Insufficient material (K vs K, KB vs K, etc.)
    if (is_insufficient_material(pos)) {
        return 0;
    }
    
    // Three-fold repetition (check hash history)
    if (is_repetition(pos)) {
        return 0;
    }
    
    // ========================================
    // Generate and count legal moves
    // ========================================
    MoveList moves;
    generate_legal_moves(pos, moves);
    
    // Terminal node: checkmate or stalemate
    if (moves.count == 0) {
        if (is_in_check(pos)) {
            // Checkmate - return mate score
            // Prefer shorter mates: depth 1 mate better than depth 5 mate
            return -MATE_SCORE + ply;
        } else {
            // Stalemate
            return 0;
        }
    }
    
    // ========================================
    // Transposition Table Lookup
    // ========================================
    TTEntry* entry = probe_tt(pos.zobrist_key);
    Move tt_move = NO_MOVE;
    
    if (entry && entry->key == pos.zobrist_key) {
        tt_move = entry->best_move;
        
        // Can we use cached result?
        if (entry->depth >= depth) {
            if (entry->flag == EXACT) {
                return entry->score;
            } else if (entry->flag == LOWER_BOUND && entry->score >= beta) {
                return beta;
            } else if (entry->flag == UPPER_BOUND && entry->score <= alpha) {
                return alpha;
            }
        }
    }
    
    // ========================================
    // Move Ordering (Critical for pruning!)
    // ========================================
    order_moves(moves, tt_move, ply);
    
    // ========================================
    // Main Search Loop
    // ========================================
    int best_score = -INFINITY;
    Move best_move = NO_MOVE;
    int moves_searched = 0;
    int original_alpha = alpha;
    
    for (int i = 0; i < moves.count; i++) {
        Move m = moves[i];
        
        // Make the move
        makemove(pos, m);
        
        int score;
        
        // ========================================
        // Principal Variation Search (PVS)
        // ========================================
        if (moves_searched == 0) {
            // First move - search with full window
            score = -alphabeta(pos, depth - 1, -beta, -alpha, ply + 1);
        } else {
            // Later moves - try null window first (assume they're worse)
            score = -alphabeta(pos, depth - 1, -alpha - 1, -alpha, ply + 1);
            
            // If it beat alpha, re-search with full window
            if (score > alpha && score < beta) {
                score = -alphabeta(pos, depth - 1, -beta, -alpha, ply + 1);
            }
        }
        
        // Undo the move
        undomove(pos, m);
        
        moves_searched++;
        
        // ========================================
        // Update best score
        // ========================================
        if (score > best_score) {
            best_score = score;
            best_move = m;
            
            // Beta cutoff - opponent won't allow this
            if (score >= beta) {
                // Update history heuristic
                if (!is_capture(m)) {
                    history[m.from][m.to] += depth * depth;
                    
                    // Update killer moves
                    if (killers[ply][0] != m) {
                        killers[ply][1] = killers[ply][0];
                        killers[ply][0] = m;
                    }
                }
                
                // Store in TT
                store_tt(pos.zobrist_key, depth, beta, LOWER_BOUND, best_move);
                
                return beta;  // Fail-high (beta cutoff)
            }
            
            // Improved alpha - we found a better move
            if (score > alpha) {
                alpha = score;
            }
        }
    }
    
    // ========================================
    // Store in Transposition Table
    // ========================================
    int flag = (best_score > original_alpha) ? EXACT : UPPER_BOUND;
    store_tt(pos.zobrist_key, depth, best_score, flag, best_move);
    
    return best_score;
}
@endcode

**Visualization of Alpha-Beta Pruning in Action:**

@verbatim
Search Tree (numbers are evaluations, X = pruned branches):

                    Root (α=-∞, β=+∞)
                   /      |      \
                 M1       M2       M3
                /         |         \
             +2.5        -9          ?
              /\         /|\         
             /  \       / | \       
           +2  +2.5   -9  X  X    <- Pruned! We know M2 ≤ -9
                      
          α=+2.5 after M1
          
          M2: First response is -9
              Since -9 < α (+2.5), and it's opponent's turn,
              they'll choose -9 or worse (for us).
              So M2 ≤ -9 < α, don't search other responses!
              
Result: Only searched 3 positions instead of 6+
@endverbatim

@dot "Alpha-Beta Pruning Visualization"
digraph alphabeta_prune {
    rankdir=TB;
    node [shape=box, style="filled,rounded"];
    
    root [label="Root Node\nα=-∞, β=+∞\nBest so far: none", fillcolor="#90EE90"];
    
    m1 [label="Move 1\nScore: +2.5\nα=+2.5", fillcolor="#FFD700"];
    m2 [label="Move 2\nScore: ≤-9\n(pruned)", fillcolor="#FF6347", fontcolor=white];
    m3 [label="Move 3\nScore: +3.0\nNEW BEST", fillcolor="#87CEEB"];
    
    m2_r1 [label="Response 1\nScore: -9", fillcolor="#FFA500"];
    m2_pruned [label="Other responses\nPRUNED!", fillcolor="#DC143C", fontcolor=white, style="filled,dashed"];
    
    root -> m1 [label="Search fully"];
    root -> m2 [label="Start search"];
    root -> m3 [label="Search fully"];
    
    m2 -> m2_r1 [label="Found bad response"];
    m2 -> m2_pruned [label="Skip these!", style=dashed];
    
    label="Alpha-beta skips provably bad branches";
}
@enddot

@subsection quiescence 4.3 Quiescence Search - Avoiding the Horizon Effect

**The Horizon Effect Problem:**

@verbatim
Position at depth 0:
  White just moved Qxf7+ (takes pawn with check)
  Static eval: +1 (up a pawn)
  
Position at depth 1 (black responds):
  Black plays Kxf7 (king takes queen!)
  Static eval: -8 (down a queen!)
  
Problem: Depth-0 eval said "+1" but move loses queen!
         We stopped searching at an unstable (tactical) position.
         
Solution: Quiescence search - keep searching captures until quiet.
@endverbatim

@dot "Quiescence vs No Quiescence"
digraph quiescence_comparison {
    rankdir=LR;
    node [shape=box, style="filled,rounded"];
    
    subgraph cluster_no_q {
        label="Without Quiescence";
        style=filled;
        fillcolor="#FFE4E4";
        
        no_q_d0 [label="Depth 0\nQxf7+\nEval: +1", fillcolor="#FFD700"];
        no_q_stop [label="STOP HERE\nReturn +1\n(WRONG!)", fillcolor="#FF6347", fontcolor=white];
        
        no_q_d0 -> no_q_stop;
    }
    
    subgraph cluster_q {
        label="With Quiescence";
        style=filled;
        fillcolor="#E4FFE4";
        
        q_d0 [label="Depth 0\nQxf7+\nCapture!", fillcolor="#FFD700"];
        q_q1 [label="Q-depth 1\nKxf7\nCapture!", fillcolor="#FFA500"];
        q_q2 [label="Q-depth 2\nNo captures\nEval: -8", fillcolor="#87CEEB"];
        q_stop [label="STOP HERE\nReturn -8\n(CORRECT!)", fillcolor="#90EE90"];
        
        q_d0 -> q_q1 [label="Continue"];
        q_q1 -> q_q2 [label="Continue"];
        q_q2 -> q_stop;
    }
}
@enddot

**Quiescence Search Implementation:**

@code{.cpp}
/**
 * @brief Quiescence search - search only captures until position is quiet
 * 
 * Prevents horizon effect by continuing to search tactical sequences
 * (captures, checks) until position stabilizes.
 * 
 * @param pos Position to evaluate
 * @param alpha Lower bound
 * @param beta Upper bound
 * @return Static evaluation or continuation score
 */
int quiescence(Position& pos, int alpha, int beta) {
    // ========================================
    // Standing Pat - Can we stand pat (not capture)?
    // ========================================
    int stand_pat = evaluate(pos);
    
    // Beta cutoff - position is already too good
    if (stand_pat >= beta) {
        return beta;
    }
    
    // Update alpha if standing pat is better
    if (stand_pat > alpha) {
        alpha = stand_pat;
    }
    
    // ========================================
    // Delta Pruning - Skip hopeless captures
    // ========================================
    const int QUEEN_VALUE = 900;
    if (stand_pat + QUEEN_VALUE + 200 < alpha) {
        // Even capturing a queen won't save us
        return alpha;
    }
    
    // ========================================
    // Generate only captures (and checks in some engines)
    // ========================================
    MoveList captures;
    generate_captures(pos, captures);
    
    // Order captures by MVV-LVA (Most Valuable Victim - Least Valuable Attacker)
    order_captures(captures);
    
    // ========================================
    // Search captures
    // ========================================
    for (int i = 0; i < captures.count; i++) {
        Move m = captures[i];
        
        // SEE Pruning - Skip losing captures
        if (SEE(pos, m) < 0) {
            continue;  // Don't search QxP if pawn is defended by pawn
        }
        
        makemove(pos, m);
        
        // Recursive quiescence search
        int score = -quiescence(pos, -beta, -alpha);
        
        undomove(pos, m);
        
        if (score >= beta) {
            return beta;
        }
        
        if (score > alpha) {
            alpha = score;
        }
    }
    
    return alpha;
}
@endcode

**Static Exchange Evaluation (SEE):**

@code{.cpp}
/**
 * @brief Static Exchange Evaluation - predict outcome of capture sequence
 * 
 * Example: White plays QxP on square where pawn is defended by pawn
 * SEE calculates: Queen takes pawn (+1), pawn takes queen (-9) = -8
 * Result: Losing capture, don't search in quiescence
 * 
 * @param pos Position
 * @param move Capture move to evaluate
 * @return Material gain/loss from capture sequence
 */
int SEE(const Position& pos, Move move) {
    int from = move.from;
    int to = move.to;
    
    // Value of captured piece
    int gain[32];  // Max exchange depth
    int depth = 0;
    
    gain[0] = piece_value[pos.pieceAt(to)];
    
    // Simulate capture sequence
    uint64_t attackers = get_attackers(pos, to);
    uint64_t occupied = pos.occupiedSquares;
    uint64_t from_bb = (1ULL << from);
    
    occupied ^= from_bb;  // Remove initial attacker
    attackers ^= from_bb;
    
    Pieces attacker = pos.pieceAt(from);
    bool side = pos.whiteToMove;
    
    // Simulate exchanges
    while (attackers) {
        depth++;
        gain[depth] = piece_value[attacker] - gain[depth - 1];
        
        if (max(-gain[depth - 1], gain[depth]) < 0) {
            break;  // Losing trade, stop
        }
        
        // Find next smallest attacker
        // (implementation details omitted for brevity)
        attacker = get_smallest_attacker(attackers, side);
        if (attacker == NO_PIECE) break;
        
        side = !side;
    }
    
    // Minimax to find actual value
    while (--depth) {
        gain[depth - 1] = -max(-gain[depth - 1], gain[depth]);
    }
    
    return gain[0];
}
@endcode


---

@section chapter_5 Chapter 5: Position Evaluation

@subsection eval_overview 5.1 Evaluation Function Overview

The evaluation function assigns a numerical score to a position. A good eval is critical - if your eval is wrong, your search finds the "best" bad moves!

@dot "Evaluation Components"
digraph eval {
    rankdir=TB;
    node [shape=box, style="filled,rounded"];
    
    eval [label="Total\nEvaluation", fillcolor="#90EE90", shape=ellipse];
    
    material [label="Material\n~70% of score", fillcolor="#FFD700"];
    position [label="Piece Position\n(PST)\n~20% of score", fillcolor="#FFA500"];
    pawn_struct [label="Pawn Structure\n~5% of score", fillcolor="#87CEEB"];
    king_safety [label="King Safety\n~5% of score", fillcolor="#DDA0DD"];
    
    material -> eval [label="weight × value"];
    position -> eval [label="weight × value"];
    pawn_struct -> eval [label="weight × value"];
    king_safety -> eval [label="weight × value"];
    
    label="Combine weighted features";
}
@enddot

@subsection piece_square_tables 5.2 Piece-Square Tables - Position Value

**Concept:** A knight on e4 is worth MORE than a knight on a1!

@verbatim
Knight PST (White's perspective, centipawns):

  a    b    c    d    e    f    g    h
8 -50  -40  -30  -30  -30  -30  -40  -50   <- Rim
7 -40  -20    0    0    0    0  -20  -40
6 -30    0  +10  +15  +15  +10    0  -30
5 -30   +5  +15  +20  +20  +15   +5  -30   <- CENTER!
4 -30    0  +15  +20  +20  +15    0  -30   <- CENTER!
3 -30   +5  +10  +15  +15  +10   +5  -30
2 -40  -20    0   +5   +5    0  -20  -40
1 -50  -40  -30  -30  -30  -30  -40  -50   <- Rim

Logic:
- Center squares (d4, e4, d5, e5): BEST (+20)
- Near center (c3-f6 area): GOOD (+10 to +15)
- Edges: BAD (-40 to -50)

Saying: "Knights on the rim are dim!"

Total value of knight = BASE_VALUE (320) + PST_BONUS
Knight on e4 = 320 + 20 = 340 cp
Knight on a1 = 320 + (-50) = 270 cp
Difference = 70 cp advantage for centralized knight!
@endverbatim

**Pawn PST - Encourage Advancement:**

@verbatim
White Pawn PST:

  a    b    c    d    e    f    g    h
8  0    0    0    0    0    0    0    0    <- Promotion (handled separately)
7 +50  +50  +50  +50  +50  +50  +50  +50   <- Encourage advance
6 +10  +10  +20  +30  +30  +20  +10  +10
5  +5   +5  +10  +25  +25  +10   +5   +5
4  0    0    0  +20  +20    0    0    0    <- Central pawns
3  +5  -5  -10    0    0  -10   -5   +5
2 +10  +10  +10  -20  -20  +10  +10  +10   <- Discourage premature h2/g2 moves
1  0    0    0    0    0    0    0    0

Logic:
- Rank 7: Big bonus (+50) - pawn close to promoting
- Rank 4-5 center: Bonus (+20-25) - space advantage
- Rank 2 wings (a2, b2, g2, h2): Bonus (+10) - don't move yet
- Rank 2 center (d2, e2): Penalty (-20) - move these first
@endverbatim

**King PST - Different for Middlegame vs Endgame:**

@code{.cpp}
// Middlegame: King should hide in corner (castle)
const int king_mg_pst[64] = {
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -20, -30, -30, -40, -40, -30, -30, -20,
    -10, -20, -20, -20, -20, -20, -20, -10,
     20,  20,   0,   0,   0,   0,  20,  20,  // Encourage castling
     20,  30,  10,   0,   0,  10,  30,  20   // Safest on g1/b1
};

// Endgame: King should fight in center
const int king_eg_pst[64] = {
    -50, -40, -30, -20, -20, -30, -40, -50,
    -30, -20, -10,   0,   0, -10, -20, -30,
    -30, -10,  20,  30,  30,  20, -10, -30,  // CENTER!
    -30, -10,  30,  40,  40,  30, -10, -30,  // CENTER!
    -30, -10,  30,  40,  40,  30, -10, -30,
    -30, -10,  20,  30,  30,  20, -10, -30,
    -30, -30,   0,   0,   0,   0, -30, -30,
    -50, -30, -30, -30, -30, -30, -30, -50
};

// Tapered evaluation based on game phase
int game_phase = calculate_phase(pos);  // 0=endgame, 256=opening
int mg_score = evaluate_mg(pos);
int eg_score = evaluate_eg(pos);
int final_score = (mg_score * game_phase + eg_score * (256 - game_phase)) / 256;
@endcode

@subsection advanced_eval 5.3 Advanced Evaluation Terms

**Passed Pawns - Very Valuable in Endgame:**

@verbatim
Example: White pawn on e5, no black pawns can stop it

  a  b  c  d  e  f  g  h
8 .  .  .  .  .  .  .  .
7 .  .  .  .  .  .  .  .
6 .  .  .  .  .  .  .  .
5 .  .  .  .  P  .  .  .  <- Passed pawn!
4 .  .  .  .  .  .  .  .
3 .  p  .  .  .  .  .  .  <- Black pawn on b3 (irrelevant)
2 .  .  .  .  .  .  .  .
1 .  .  .  .  .  .  .  .

Check if passed:
- No black pawns on files d, e, or f (adjacent+same files)
- No black pawns ahead on those files (ranks 6-8)
= Pawn is passed!

Bonus calculation:
distance_to_8th_rank = 7 - 5 = 2
bonus = 50 + (5 - distance) × 10
      = 50 + (5 - 2) × 10
      = 50 + 30 = 80 centipawns!
@endverbatim

**Doubled Pawns - Weakness:**

@code{.cpp}
int evaluate_doubled_pawns(const Position& pos) {
    int penalty = 0;
    
    for (int file = 0; file < 8; file++) {
        uint64_t file_mask = 0x0101010101010101ULL << file;
        int white_count = __builtin_popcountll(pos.WhitePawns & file_mask);
        int black_count = __builtin_popcountll(pos.BlackPawns & file_mask);
        
        // Penalty for each doubled pawn
        if (white_count > 1) penalty -= (white_count - 1) * 20;
        if (black_count > 1) penalty += (black_count - 1) * 20;
    }
    
    return penalty;
}
@endcode

---

@section chapter_6 Chapter 6: Advanced Techniques

@subsection transposition_table 6.1 Transposition Tables

**Why TT is Critical:**

@verbatim
Same position reached by different move orders:

Path A: 1.e4 e5 2.Nf3 Nc6 3.Bc4
Path B: 1.e4 e5 2.Bc4 Nc6 3.Nf3

Same position! Without TT, we search it twice = wasted work.
With TT, we store result first time, reuse it second time.

Speedup: Typically 3-10x faster with good TT!
@endverbatim

@subsection move_ordering 6.2 Move Ordering - Critical for Alpha-Beta

**Move Ordering Quality Impact:**

| Move Order | Nodes Searched | Effective Branching Factor |
|------------|----------------|----------------------------|
| **Perfect** (best first) | Minimum | ~6 |
| **Good** (TT + MVV-LVA + killers) | ~2x minimum | ~8 |
| **Poor** (worst first) | Maximum (~10^15) | ~35 |
| **Random** | Very high | ~30 |

**Ordering Scheme (priority order):**

@code{.cpp}
void order_moves(MoveList& moves, Move tt_move, int ply) {
    for (Move& m : moves) {
        if (m == tt_move) {
            m.score = 10000000;  // HIGHEST priority
        }
        else if (is_capture(m)) {
            int victim = piece_value[captured_piece(m)];
            int attacker = piece_value[moving_piece(m)];
            
            // MVV-LVA: Prefer QxP over PxP, prefer PxQ over QxQ
            m.score = 1000000 + victim * 10 - attacker;
            
            // SEE: Bonus for winning captures
            if (SEE(pos, m) >= 0) {
                m.score += 100000;
            }
        }
        else if (m == killers[ply][0]) {
            m.score = 90000;
        }
        else if (m == killers[ply][1]) {
            m.score = 80000;
        }
        else {
            // History heuristic
            m.score = history[m.from][m.to];
        }
    }
    
    // Sort by score (highest first)
    std::sort(moves.begin(), moves.end(), [](Move a, Move b) {
        return a.score > b.score;
    });
}
@endcode

---

@section conclusion Conclusion and Next Steps

@subsection what_learned What You've Learned

Congratulations! You now understand:

✅ **Board Representation**
- Bitboards and why they're superior to arrays
- Complete position structure with all state
- FEN parsing and position setup

✅ **Move Generation**
- Pre-computed attack tables (king, knight, pawn)
- Magic bitboards for sliding pieces
- Complete move generation for all pieces
- Special moves (castling, en passant, promotions)

✅ **Search Algorithms**
- Alpha-beta pruning (1000x speedup over minimax!)
- Quiescence search (avoiding horizon effect)
- Principal Variation Search
- Iterative deepening

✅ **Position Evaluation**
- Material counting
- Piece-square tables (position matters!)
- Advanced features (passed pawns, pawn structure, king safety)
- Tapered evaluation (middlegame vs endgame)

✅ **Advanced Techniques**
- Transposition tables (3-10x speedup)
- Move ordering (critical for pruning)
- Null move pruning
- Late move reductions

@subsection implementation_roadmap Implementation Roadmap

**Phase 1: Basic Engine (1-2 weeks)**
1. Implement bitboard position structure
2. Write FEN parser
3. Generate moves for all pieces
4. Implement make/unmake
5. Write simple material-only evaluation
6. Implement basic minimax search
7. Add UCI protocol basics

**Result:** ~1000 Elo engine that can play legal chess

**Phase 2: Alpha-Beta (1 week)**
1. Convert minimax to alpha-beta
2. Add quiescence search
3. Implement iterative deepening
4. Add basic time management

**Result:** ~1600 Elo engine, significant strength gain

**Phase 3: Transposition Table (1 week)**
1. Implement Zobrist hashing
2. Create TT structure
3. Add TT probing and storing
4. Handle hash collisions

**Result:** ~1900 Elo engine, much faster search

**Phase 4: Move Ordering (1 week)**
1. Add MVV-LVA for captures
2. Implement killer moves
3. Add history heuristic
4. Implement PVS

**Result:** ~2100 Elo engine, strong amateur level

**Phase 5: Advanced Evaluation (2 weeks)**
1. Create piece-square tables
2. Add pawn structure evaluation
3. Implement king safety
4. Add mobility evaluation
5. Implement tapered eval

**Result:** ~2300 Elo engine, expert level

**Phase 6: Advanced Pruning (1 week)**
1. Null move pruning
2. Late move reductions
3. Futility pruning
4. Delta pruning in quiescence

**Result:** ~2400-2500 Elo engine, master level

**Phase 7: Polish (ongoing)**
1. Fix bugs found through testing
2. Tune evaluation parameters
3. Optimize performance
4. Add opening book support

**Result:** ~2500-2700 Elo engine, depending on tuning

@subsection testing_resources Testing and Resources

**Essential Testing:**

| Test Type | Purpose | Tool |
|-----------|---------|------|
| **Perft** | Verify move generation correctness | Built-in perft command |
| **Tactical Tests** | Verify search finds tactics | WAC, Bratko-Kopec, Eigenmann |
| **Engine Matches** | Measure strength | CuteChess, BanksiaGUI |
| **Opening Suite** | Test various positions | Silver, 40H40, Swiss |

**Resources:**

- **Chess Programming Wiki**: https://www.chessprogramming.org/
- **TalkChess Forum**: http://talkchess.com/
- **Stockfish Source**: https://github.com/official-stockfish/Stockfish
- **Engine Rating Lists**: https://ccrl.chessdom.com/

**Example Perft Test:**

@code{.cpp}
// Test move generation by counting positions
void perft_test() {
    Position pos;
    pos.setFromFEN("startpos");
    
    // Correct perft values for starting position
    assert(perft(pos, 1) == 20);
    assert(perft(pos, 2) == 400);
    assert(perft(pos, 3) == 8902);
    assert(perft(pos, 4) == 197281);
    assert(perft(pos, 5) == 4865609);
    
    std::cout << "All perft tests passed!" << std::endl;
}
@endcode

@subsection final_words Final Words

Writing a chess engine is a challenging but rewarding journey. You'll learn about:
- Algorithm optimization
- Data structures
- Performance tuning
- Search algorithms
- Artificial intelligence

Most importantly, you'll have created something that can play chess at a level that would have been impossible for humans to achieve without computers!

**Good luck, and happy coding!** ♟️🚀

---

@note This guide is maintained at: https://github.com/yourusername/Chess-Engine

@author Your Name
@date 2024
@version 1.0

*/


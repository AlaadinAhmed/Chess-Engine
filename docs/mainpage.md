/**
\mainpage OctoKnight: A Technical Deep Dive

\section intro_sec Introduction
This document provides a detailed technical explanation of the internal architecture and algorithms of the OctoKnight chess engine. It explains *how* core components are implemented, not just *what* they are.

---

\section board_rep_sec 1. Board Representation
We use a **bitboard** representation. A bitboard is a `uint64_t` where each bit corresponds to a square. This is very efficient for setwise operations.

\subsection bitboards_manip_subsec 1.1 Bitboard Manipulation
We store 12 bitboards for each piece type and color (e.g., `WhitePawns`, `BlackKnights`). A piece is added to a bitboard using a bitwise OR, and removed using a bitwise AND with a NOT.

```cpp
// To add a white pawn to square e4 (index 28)
pos.WhitePawns |= (1ULL << 28);

// To remove it
pos.WhitePawns &= ~(1ULL << 28);
```
We also maintain combined occupancy bitboards (`WhiteoccupiedSquares`, `occupiedSquares`, etc.) which are critical for move generation.

\subsection fen_parsing_subsec 1.2 FEN Parsing Algorithm
FEN parsing populates the bitboards and game state. The `fen::parseFEN` function implements the following algorithm:
1.  Initialize a `Position` object to be empty.
2.  Set internal rank and file counters to the starting position (rank 7, file 0).
3.  Loop through the FEN string's piece placement section character by character:
    - If the char is a letter (e.g., 'r'), look up the corresponding `Pieces` enum. Set the bit on the correct piece bitboard at the current rank/file index. Decrement the file counter.
    - If the char is a digit '1' through '8', decrement the file counter by its integer value.
    - If the char is '/', it signifies the end of a rank. Decrement the rank counter and reset the file counter to 0.
4.  After the piece placement section, parse the remaining fields:
    - **Side to Move**: If the character is 'w', set `pos.whiteToMove = true`.
    - **Castling**: For each character in the castling string ('K', 'Q', 'k', 'q'), set the corresponding bit in the `pos.castelingRights` bitmask.
    - **En Passant**: If the en passant square is not '-', calculate its square index and set the corresponding bit on `pos.enPassant`.
5.  Finally, calculate the combined occupancy bitboards from the individual piece bitboards and compute the initial Zobrist hash key for the position.

---

\section movegen_sec 2. Move Generation Algorithm
Move generation finds all legal moves in a position. It works by first generating pseudo-legal moves (all moves a piece can make ignoring check) and then validating them.

The top-level `generate_moves` function implements this algorithm:
1.  Create an empty `MoveList` to store the results.
2.  Get a bitboard of all pieces belonging to the current player (e.g., `pos.WhiteoccupiedSquares` if it's white's turn).
3.  Loop through each piece on that bitboard. A common and fast way to do this is:
    ```cpp
    while (my_pieces) {
      int square = get_lsb_index(my_pieces); // Get index of one piece
      // ... generate moves for the piece on this square ...
      my_pieces &= my_pieces - 1; // Remove this piece from the set and continue
    }
    ```
4.  Inside the loop, use a `switch` on the piece type at `square` to call a specific move generation function (e.g., for pawns, knights, etc.).
5.  **Legality Check**: For each pseudo-legal move generated, a temporary copy of the position is used to make the move. Then, we check if the king is attacked using `is_square_attacked`. If the king is *not* attacked, the move is legal and is added to the `MoveList`.

\subsection pawn_moves_subsec 2.1 Example: Pawn Move Generation
The logic for generating pawn moves is a good example:
1.  **Single Push**: Calculate the square one step forward. If this square is found in the `pos.emptySquares` bitboard, it's a valid push. Add the move.
2.  **Double Push**: If the pawn is on its starting rank and the single push square was empty, check the square two steps forward. If it is also empty, add the move.
3.  **Captures**: Get the pawn's attack bitboard. Use a bitwise AND with the opponent's occupancy bitboard (`pos.BlackoccupiedSquares` for a white pawn) to find actual captures. For each resulting square, add a capture move.
4.  **Promotions**: For any push or capture that lands on the opponent's back rank, create four separate moves: one for each promotion to Queen, Rook, Bishop, and Knight.
5.  **En Passant**: Check if the `pos.enPassant` bitboard is set. If a friendly pawn is attacking that square, a special en passant capture move is generated.

---

\section search_sec 3. Search Algorithm: Finding the Best Move
The search function explores the game tree to find the optimal move. It is implemented in `alpha_beta_search` as a Negamax variant of the Alpha-Beta algorithm.

**The Alpha-Beta Algorithm (`alpha_beta_search`):**
1.  **Termination & Transposition Table**: Upon entering, first check if the search should stop due to time constraints. Then, probe the Transposition Table with the position's Zobrist key. If a valid entry is found with a sufficient search depth, its score is returned immediately, saving re-computation.
2.  **Base Case**: If the current search depth (`current_depth`) reaches the maximum depth (`max_depth`), the search stops at this branch and calls `quiescence()` to get a stable evaluation before returning.
3.  **Move Generation**: Call `generate_moves` to get a list of all legal moves from the current position.
4.  **Checkmate/Stalemate**: If the move list is empty, the game is over. If the king is in check, it's checkmate (return a very low score). Otherwise, it's stalemate (return a score of 0).
5.  **Recursive Exploration**: Loop through each generated move:
    a. Make the move on the board using `makemove()`. This updates the `Position` struct and its Zobrist key.
    b. Recursively call `alpha_beta_search` for the new position. The key steps here are incrementing the depth (`current_depth + 1`) and inverting the alpha-beta window (`-beta`, `-alpha`). This inversion is the Negamax principle, which allows the same evaluation function to be used for both players.
    c. Undo the move using `undomove()` to restore the board to its original state for the next iteration of the loop.
    d. **Pruning**: Compare the `score` returned from the recursive call. If `score >= beta`, it means the opponent has found a better response earlier in the tree, so this entire branch is worse for them than what they already have. We can immediately stop searching moves at this node and return `beta` (a "beta-cutoff").
    e. If the score is better than our current best (`score > alpha`), we update `alpha = score` and store this move as the new best move for this position.
6.  **Return & Store**: After checking all moves, the best score found (`alpha`) is stored in the Transposition Table along with the best move before being returned.

*/

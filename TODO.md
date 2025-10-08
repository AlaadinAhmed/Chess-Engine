## Engine Architecture
### [x]Fen Parsing 
A FEN (Forsyth–Edwards Notation) is ==a standard text-based method for describing a single position on a chessboard, containing all the necessary information to restart a game from that point==.

A FEN string consists of six parts, separated by spaces: 

1) Piece Placement: Describes the pieces on the board, starting from rank 8 down to rank 1, with pieces represented by letters (uppercase for white, lowercase for black). 
- A number indicates consecutive empty squares. 

- Each rank is separated by a forward slash (/). 

- Example: rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR represents the starting position. 

2) Active Color:
w for white or b for black, indicating whose turn it is. 
3) Castling Rights:
A string of letters (K, Q, k, q) indicating which castling moves are still possible. 

    K: White can castle kingside.
    Q: White can castle queenside.
    k: Black can castle kingside.
    q: Black can castle queenside.
    A hyphen (-) indicates no castling rights. 

4) En Passant Target Square:
The square where an en passant capture can occur, or - if none is possible. 
5) Halfmove Clock:
The number of halfmoves (plies) made since the last pawn move or capture, used for the 50-move rule. 
6) Fullmove Number:
The total number of full moves in the game, starting at 1 and incrementing after each of Black's moves. 

**Example FEN:**
The FEN for the starting position of a chess game is: `rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1`.
### UCI (Universal Chess Interface)


### [Half Done] Move Generation
[x] Rooks.
[x] Bishops.
[x] Knights.
[x] Kings.
[x] Pawns.
[] Queens.

### Checks and Checkmates Validation


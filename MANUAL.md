# OctoKnight UCI Manual

This document provides a detailed explanation of the UCI (Universal Chess Interface) commands that OctoKnight supports. For a full description of the UCI protocol, see the [official specification](http://wbec-ridderkerk.nl/html/UCIProtocol.html).

## Commands

### `uci`

This is the first command that the GUI sends to the engine. It is used to identify the engine and its capabilities.

**Example:**

```
uci
```

**Response:**

```
id name OctoKnight
id author Aladdin
uciok
```

### `isready`

This command is used to synchronize the engine with the GUI. When the GUI sends this command, the engine should finish any pending calculations and then send `readyok`.

**Example:**

```
isready
```

**Response:**

```
readyok
```

### `ucinewgame`

This command tells the engine that a new game is about to start. The engine should reset its internal state (e.g., clear the transposition table).

**Example:**

```
ucinewgame
```

### `position`

This command sets the position on the board. It can be followed by `startpos` to set the starting position, or `fen` to set a position from a FEN string. It can also be followed by a series of moves in UCI format.

**Examples:**

*   Set the starting position:

    ```
    position startpos
    ```

*   Set a position from a FEN string:

    ```
    position fen rnbqkbnr/pppp1ppp/8/4p3/4P3/8/PPPP1PPP/RNBQKBNR w KQkq - 0 2
    ```

*   Set the starting position and then make some moves:

    ```
    position startpos moves e2e4 e7e5
    ```

### `go`

This command tells the engine to start thinking about the current position. The engine will then search for the best move and send it to the GUI.

**Example:**

```
go
```

**Response:**

The engine will send a series of `info` messages with information about the search, and then finally a `bestmove` message with the best move it has found.

```
info depth 1 score cp 13 pv e2e4
info depth 2 score cp 13 pv e2e4
info depth 3 score cp 13 pv e2e4
info depth 4 score cp 13 pv e2e4
bestmove e2e4
```

### `stop`

This command tells the engine to stop thinking about the current position and send the best move it has found so far.

**Example:**

```
stop
```

### `quit`

This command tells the engine to quit.

**Example:**

```
quit
```

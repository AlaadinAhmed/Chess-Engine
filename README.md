# OctoKnight

<p align="center">
  <img src="https://i.imgur.com/gC4gA6c.png" alt="OctoKnight Logo" width="200"/>
</p>

<p align="center">
  A UCI-compliant chess engine written in C++.
</p>

---

OctoKnight is a passion project of mine, a chess engine built from the ground up. The name comes from the knight's eight possible moves, a symbol of the engine's tactical and aggressive playstyle. I'm constantly working on it, trying to make it smarter and stronger with every commit.

## Features

*   **UCI Protocol:** Fully compliant with the Universal Chess Interface, so you can use it with your favorite chess GUI.
*   **Bitboard Representation:** A fast and efficient way to represent the chessboard.
*   **Zobrist Hashing:** For quick and reliable transposition table lookups.
*   **Transposition Table:** To store and retrieve previously calculated positions.
*   **Alpha-Beta Search:** With iterative deepening to find the best move.
*   **Piece-Square Tables:** To evaluate the positional value of each piece.

## Building

I've made it easy to build OctoKnight. You'll need a C++17 compiler and CMake.

```bash
git clone https://github.com/AlaadinAhmed/octoknight.git
cd octoknight
mkdir build
cd build
cmake ..
make
```

This will create the `OctoKnight` executable in the `build` directory.

## Running

Once you've built the engine, you can run it from the command line:

```bash
./OctoKnight
```

The engine will then listen for UCI commands. I recommend using a GUI like [Arena](http://www.playwitharena.de/) or [CuteChess](https://github.com/cutechess/cutechess) to play against it. For more detailed instructions on how to use the engine with a GUI, see [USAGE.md](USAGE.md).

## Testing

I've set up some basic tests to ensure everything is working as expected. To run them:

```bash
make OctoKnight_test
./OctoKnight_test
```

## Documentation

I'm using Doxygen to generate detailed documentation for the project. You can find the latest version in the `docs/html` directory. Just open `index.html` in your browser.

To generate the documentation yourself, you'll need to have Doxygen installed. Then, from the root of the project, run:

```bash
doxygen Doxyfile
```

For a detailed guide to the UCI commands that OctoKnight supports, see [MANUAL.md](MANUAL.md).

## Future Goals

I have a lot of ideas for OctoKnight. Here's what I'm currently working on:

*   **Opening Book:** To give the engine a solid start to the game.
*   **More Advanced Evaluation:** Incorporating more sophisticated evaluation techniques.
*   **Improved Search:** Implementing more advanced search algorithms like Principal Variation Search (PVS).
*   **Endgame Tablebases:** To play the endgame perfectly.

## Contributing

I'm always open to feedback and contributions. If you have any ideas or suggestions, feel free to open an issue or submit a pull request.

## License

OctoKnight is released under the [MIT License](LICENSE).
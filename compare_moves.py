engine_moves = ["a2a3", "a2a4", "b2b3", "g2g3", "g2h3", "g2g4", "d5d6", "d5e6", "c3b1", "c3d1", "c3a4", "c3b5", "e5d3", "e5c4", "e5g4", "e5c6", "e5g6", "e5d7", "e5f7", "d2c1", "d2e3", "d2f4", "d2g5", "d2h6", "e2d1", "e2f1", "e2d3", "e2c4", "e2b5", "e2a6", "a1b1", "a1c1", "a1d1", "h1f1", "h1g1", "f3d3", "f3e3", "f3g3", "f3h3", "f3f4", "f3g4", "f3f5", "f3h5", "f3f6", "e1g1", "e1c1"]
python_chess_moves = ["e5f7", "e5d7", "e5g6", "e5c6", "e5g4", "e5c4", "e5d3", "f3f6", "f3h5", "f3f5", "f3g4", "f3f4", "f3h3", "f3g3", "f3e3", "f3d3", "c3b5", "c3a4", "c3d1", "c3b1", "e2a6", "e2b5", "e2c4", "e2d3", "e2f1", "e2d1", "d2h6", "d2g5", "d2f4", "d2e3", "d2c1", "h1g1", "h1f1", "e1f1", "e1d1", "a1d1", "a1c1", "a1b1", "e1g1", "e1c1", "d5e6", "g2h3", "d5d6", "g2g3", "b2b3", "a2a3", "g2g4", "a2a4"]

engine_moves.sort()
python_chess_moves.sort()

missing_moves = []
for move in python_chess_moves:
    if move not in engine_moves:
        missing_moves.append(move)

print("Missing moves:", missing_moves)

extra_moves = []
for move in engine_moves:
    if move not in python_chess_moves:
        extra_moves.append(move)

print("Extra moves:", extra_moves)
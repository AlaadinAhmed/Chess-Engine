import chess

fen_string = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"
board = chess.Board(fen_string)
legal_moves = list(board.legal_moves)

for move in legal_moves:
    print(move.uci())
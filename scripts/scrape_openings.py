import requests
import chess
import chess.pgn
import json
import time

def scrape_lichess_openings(fen, moves_to_play=5):
    url = "https://explorer.lichess.ovh/masters"
    params = {
        "fen": fen,
        "moves": 10,
        "topGames": 0
    }
    
    try:
        response = requests.get(url, params=params)
        if response.status_code == 200:
            return response.json()
        else:
            print(f"Error: {response.status_code}")
            return None
    except Exception as e:
        print(f"Exception: {e}")
        return None

def save_opening_data(fen, data, filename="openings.json"):
    try:
        with open(filename, 'a') as f:
            entry = {"fen": fen, "data": data}
            f.write(json.dumps(entry) + "\n")
    except Exception as e:
        print(f"Error saving data: {e}")

def traverse_openings(board, depth, max_depth, visited_fens):
    fen = board.fen()
    if fen in visited_fens or depth > max_depth:
        return

    visited_fens.add(fen)
    print(f"Scraping depth {depth}: {fen}")
    
    data = scrape_lichess_openings(fen)
    if not data:
        return

    save_opening_data(fen, data)

    for move_data in data.get('moves', [])[:3]: # Top 3 moves
        san = move_data['san']
        try:
            move = board.parse_san(san)
            board.push(move)
            traverse_openings(board, depth + 1, max_depth, visited_fens)
            board.pop()
        except ValueError:
            print(f"Invalid move: {san}")

def main():
    board = chess.Board()
    visited_fens = set()
    # Clear file
    with open("openings.json", 'w') as f:
        pass
        
    traverse_openings(board, 0, 5, visited_fens) # Depth 5
    print("Scraping complete. Data saved to openings.json")

if __name__ == "__main__":
    main()

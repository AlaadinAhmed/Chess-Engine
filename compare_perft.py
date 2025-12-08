
expected = {
    "a2a3": 2812, "a2a4": 3116, "b2b3": 2984, "c2c3": 3067, "c2c4": 3197,
    "d2d3": 3286, "d2d4": 3344, "g2g3": 2938, "g2g4": 2999, "h2h3": 2942,
    "h2h4": 3042, "d5d6": 2276, "e5d3": 2883, "e5f3": 2635, "e5c4": 2898,
    "e5g4": 2838, "e5c6": 2697, "e5d7": 2642, "e5f7": 2652, "e5g6": 2589,
    "c3b1": 2773, "c3d1": 2800, "c3a4": 2743, "c3b5": 2688, "c3e2": 2778,
    "d5e6": 3266, "e2d1": 3277, "e2f1": 3224, "e2d3": 3309, "e2c4": 3329,
    "e2g4": 3334, "e2b5": 3326, "e2h5": 3334, "e2a6": 3233, "f3e3": 3339,
    "f3d3": 3339, "f3g3": 3339, "f3h3": 3339, "f3f4": 3255, "f3f5": 3175,
    "f3f6": 2110, "f3g4": 3281, "f3h5": 3277, "e1d1": 3285, "e1f1": 3234
}

with open("build/kiwipete_divide_fixed.txt", "r") as f:
    for line in f:
        if ": " in line:
            parts = line.strip().split(": ")
            if len(parts) == 2:
                move = parts[0]
                try:
                    count = int(parts[1])
                    if move in expected:
                        if count != expected[move]:
                            print(f"Mismatch for {move}: Expected {expected[move]}, Got {count}, Diff {expected[move] - count}")
                    else:
                        # print(f"Unknown move {move}: {count}")
                        pass
                except ValueError:
                    pass

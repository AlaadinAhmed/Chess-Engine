#include "magics.hpp"
#include "magics_consts.hpp"
#include "globals.hpp"
#include "utils.hpp"

void init_magics() {
    // This function is intentionally left empty because the magic numbers are pre-calculated.
}

uint64_t get_rook_attacks(int square, uint64_t occupancy) {
    occupancy &= rook_masks[square];
    occupancy *= rook_magics[square];
    occupancy >>= 64 - rook_rellevant_bits[square];
    return rook_attacks[square][occupancy];
}

uint64_t get_bishop_attacks(int square, uint64_t occupancy) {
    occupancy &= bishop_masks[square];
    occupancy *= bishop_magics[square];
    occupancy >>= 64 - bishop_rellevant_bits[square];
    return bishop_attacks[square][occupancy];
}
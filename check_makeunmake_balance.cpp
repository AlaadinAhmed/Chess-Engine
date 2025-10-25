// Add counters to track make/unmake balance
#include <iostream>

int make_count = 0;
int unmake_count = 0;

extern "C" {
    void count_make() { make_count++; }
    void count_unmake() { unmake_count++; }
    void print_balance() {
        std::cerr << "Make: " << make_count << ", Unmake: " << unmake_count << std::endl;
    }
}

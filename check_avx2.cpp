#include <iostream>
int main() {
#ifdef __AVX2__
    std::cout << "AVX2 Defined" << std::endl;
    return 0;
#else
    std::cout << "AVX2 NOT Defined" << std::endl;
    return 1;
#endif
}

#include "cuckoo.h"

#include <iostream>

const std::size_t hash(int input) {
    return input ^ (input << 2);
}

int main() {
    cuckoo<int, int> c(hash, 17);
    c.insert(0, 23);
    c.insert(0, 23);
    std::cout << c.get(0);

    return 0;
}
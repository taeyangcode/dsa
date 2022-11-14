#include "cuckoo.h"

#include <iostream>
#include <map>
#include <utility>

template <typename T>
const std::size_t hashOne(T&& a) {
    return a;
}

template <typename T>
const std::size_t hashTwo(T&& a) {
    return a;
}

int main() {
    cuckoo<int, int> c(hashOne, hashTwo);
    c.insert(0, 1);
    c.insert(0, 20);
    std::cout << c[0];

    return 0;
}
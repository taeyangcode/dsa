#include "cuckoo.h"

#include <iostream>
#include <map>

const std::size_t hashOne(int&& a) {
    return a;
}

const std::size_t hashTwo(int&& a) {
    return a;
}

int main() {
    cuckoo<int, int> c(hashOne, hashTwo);

    return 0;
}
#include "cuckoo.h"

#include <iostream>
#include <map>

const bool hashOne(int&& a) {
    return true;
}

const bool hashTwo(int&& a) {
    return true;
}

int main() {
    cuckoo<int, int> c(hashOne, hashTwo);

    return 0;
}
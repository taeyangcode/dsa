#include "cuckoo.h"

#include <iostream>
#include <map>

const bool hash(int&& a) {
    return true;
}

int main() {
    cuckoo<int, int> c(hash);

    return 0;
}
#include "cuckoo.h"

#include <iostream>

int main() {
    table<int> t;
    t.insert(1, 5);
    std::cout << t.get(0);

    return 0;
}
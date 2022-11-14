#include "cuckoo.h"

#include <cassert>
#include <iostream>

template <typename T>
const std::size_t hashOne(T&& a) {
    return a;
}

template <typename T>
const std::size_t hashTwo(T&& a) {
    return a;
}

void testOne() {
    cuckoo<int, int> c(hashOne<int>, hashTwo<int>);

    assert(c.size() == 0);  // initial size is zero

    c.insert(5, 10);        // insert value 10 at key 5
    assert(c.size() == 1);  // size should be 1
    assert(c[5] == 10);     // value should be 10

    c.insert(5, 20);        // key 5's value should be overwritten with 20
    assert(c.size() == 1);  // size should not change since overwrite
    assert(c[5] == 20);

    assert(c.maxSize() == 11);  // make sure no resizing has occurred

    c.insert(16, 30);       // insert value with same hash (5)
    assert(c.size() == 2);  // should insert into second table/array
    assert(c[16] == 30);

    c.insert(27, 40);       // collision and resize should occur as hash of 27 is also 5
    assert(c.size() == 3);  // successful insertion after resize should result in size of 3
    assert(c[5] == 20);
    assert(c[16] == 30);
    assert(c[27] == 40);

    c.insert(38, 50);  // another collision (hash 5)
    assert(c.size() == 4);
    assert(c[5] == 20);
    assert(c[16] == 30);
    assert(c[27] == 40);
    assert(c[38] == 50);
}
#ifndef CUCKOO_H_
#define CUCKOO_H_

#include <cmath>
#include <optional>
#include <stdexcept>
#include <utility>

// is_prime credit: https://stackoverflow.com/a/5694432 - implementation 4
const bool is_prime(std::size_t target) {
    for (std::size_t num = 3;; num += 2) {
        std::size_t quotient = target / num;
        if (quotient < num) {
            return true;
        }
        if (target == quotient * num) {
            return false;
        }
    }
    return true;
}

// next_prime credit: https://stackoverflow.com/a/5694432 - implementation 4
const std::size_t next_prime(const std::size_t num) {
    if (num <= 2) {
        return 2;
    }

    std::size_t n = num;
    if (!(n & 1)) {
        ++n;
    }
    for (; !is_prime(n); n += 2) {
    }
    return n;
}

template <typename T, typename U>
class node {
   public:
    T m_key;
    U m_value;

    node() {
    }

    node(T&& key, U&& value) : m_key(std::forward<T>(key)), m_value(std::forward<U>(value)) {
    }

    T& key() {
        return this->m_key;
    }

    U& value() {
        return this->m_value;
    }

    void setValue(U&& value) {
        this->m_value = value;
    }
};

template <typename T, typename U>
class cuckoo {
   private:
    std::size_t m_maxSize;
    std::size_t m_currentSize;

    const float m_loadFactor;

    std::optional<node<T, U> >* m_arrayOne;
    std::optional<node<T, U> >* m_arrayTwo;

    const std::size_t (*m_hashOne)(T&&);
    const std::size_t (*m_hashTwo)(T&&);

    std::optional<node<T, U> >& insertOne(T&& key, U&& value) {
        const std::size_t index = this->m_hashOne(std::forward<T>(key)) % this->m_maxSize;
        std::optional<node<T, U> >& oldValue = this->m_arrayOne[index];
        this->m_arrayOne[index]->setValue(std::forward<U>(value));
        return oldValue;
    }

    std::optional<node<T, U> >& insertTwo(T&& key, U&& value) {
        const std::size_t index = this->m_hashTwo(std::forward<T>(key)) % this->m_maxSize;
        std::optional<node<T, U> >& oldValue = this->m_arrayTwo[index];
        this->m_arrayTwo[index]->setValue(std::forward<U>(value));
        return oldValue;
    }

    const std::size_t cycleLength() {
        return std::log10(this->m_currentSize);
    }

   public:
    cuckoo(const std::size_t (*hashOne)(T&&), const std::size_t (*hashTwo)(T&&)) : m_hashOne(hashOne), m_hashTwo(hashTwo), m_maxSize(11), m_loadFactor(0.5f), m_currentSize(0) {
        this->m_arrayOne = new std::optional<node<T, U> >[11];
        this->m_arrayTwo = new std::optional<node<T, U> >[11];
    }

    cuckoo(const std::size_t (*hashOne)(T&&), const std::size_t (*hashTwo)(T&&), const std::size_t maxSize) : m_hashOne(hashOne), m_hashTwo(hashTwo), m_maxSize(maxSize), m_loadFactor(0.5f), m_currentSize(0) {
        this->m_arrayOne = new std::optional<node<T, U> >[this->m_maxSize];
        this->m_arrayTwo = new std::optional<node<T, U> >[this->m_maxSize];
    }

    cuckoo(const std::size_t (*hashOne)(T&&), const std::size_t (*hashTwo)(T&&), const float loadFactor) : m_hashOne(hashOne), m_hashTwo(hashTwo), m_maxSize(11), m_loadFactor(loadFactor), m_currentSize(0) {
        this->m_arrayOne = new std::optional<node<T, U> >[11];
        this->m_arrayTwo = new std::optional<node<T, U> >[11];
    }

    cuckoo(const std::size_t (*hashOne)(T&&), const std::size_t (*hashTwo)(T&&), const std::size_t maxSize, const float loadFactor) : m_hashOne(hashOne), m_hashTwo(hashTwo), m_maxSize(maxSize), m_loadFactor(loadFactor), m_currentSize(0) {
        this->m_arrayOne = new std::optional<node<T, U> >[this->m_maxSize];
        this->m_arrayTwo = new std::optional<node<T, U> >[this->m_maxSize];
    }

    ~cuckoo() {
        delete[] this->m_arrayOne;
        delete[] this->m_arrayTwo;
    }

    U& operator[](T&& key) {
        std::size_t index = this->m_hashOne(std::forward<T>(key)) % this->m_maxSize;
        if (this->m_arrayOne[index].has_value()) {
            return this->m_arrayOne[index]->value();
        }
        index = this->m_hashTwo(std::forward<T>(key)) % this->m_maxSize;
        if (this->m_arrayTwo[index].has_value()) {
            return this->m_arrayTwo[index]->value();
        }
        throw std::out_of_range("node does not exist");
    }

    void resize() {
        const std::size_t oldSize = this->m_maxSize;
        std::optional<node<T, U> >* oldArrayOne = this->m_arrayOne;
        std::optional<node<T, U> >* oldArrayTwo = this->m_arrayTwo;

        this->m_maxSize = next_prime(this->m_maxSize);
        this->m_arrayOne = new std::optional<node<T, U> >[this->m_maxSize];
        this->m_arrayTwo = new std::optional<node<T, U> >[this->m_maxSize];

        for (std::size_t index = 0, found = 0; index < oldSize && found < this->m_currentSize; ++index) {
            std::optional<node<T, U> >& nodeOne = oldArrayOne[index];
            if (nodeOne.has_value()) {
                this->insert(std::forward<T>(nodeOne->key()), std::forward<U>(nodeOne->value()));
                ++found;
            }

            std::optional<node<T, U> >& nodeTwo = oldArrayTwo[index];
            if (nodeTwo.has_value()) {
                this->insert(std::forward<T>(nodeTwo->key()), std::forward<U>(nodeTwo->value()));
                ++found;
            }
        }
        delete[] oldArrayOne;
        delete[] oldArrayTwo;
    }

    void insert(T&& key, U&& value) {
        if (this->m_currentSize >= this->m_maxSize / this->m_loadFactor) {
            this->resize();
        }

        std::optional<node<T, U> >& nodeOne = this->m_arrayOne[this->m_hashOne(std::forward<T>(key)) % this->m_maxSize];
        if (!nodeOne.has_value()) {
            nodeOne = node<T, U>(std::forward<T>(key), std::forward<U>(value));
            return;
        }
        if (nodeOne->key() == key) {
            nodeOne->setValue(std::forward<U>(value));
            return;
        }

        std::optional<node<T, U> >& nodeTwo = this->m_arrayTwo[this->m_hashTwo(std::forward<T>(key)) % this->m_maxSize];
        if (!nodeTwo.has_value()) {
            nodeTwo = node<T, U>(std::forward<T>(key), std::forward<U>(value));
            return;
        }
        if (nodeTwo->key() == key) {
            nodeTwo->setValue(std::forward<U>(value));
            return;
        }

        const std::size_t cycles = this->cycleLength();
        std::optional<node<T, U> > currentNode = node<T, U>(std::forward<T>(key), std::forward<U>(value));
        for (std::size_t evictions = 0; evictions <= cycles;) {
            if (evictions == cycles) {
                this->resize();
                this->insert(std::forward<T>(currentNode->key()), std::forward<U>(currentNode->value()));
                return;
            }
            if (currentNode.has_value()) {
                currentNode = this->insertOne(std::forward<T>(currentNode->key()), std::forward<U>(currentNode->value()));
                ++evictions;
            } else {
                return;
            }

            if (evictions == cycles) {
                this->resize();
                this->insert(std::forward<T>(currentNode->key()), std::forward<U>(currentNode->value()));
                return;
            }
            if (currentNode.has_value()) {
                currentNode = this->insertTwo(std::forward<T>(currentNode->key()), std::forward<U>(currentNode->value()));
                ++evictions;
            } else {
                return;
            }
        }
    }
};

#endif
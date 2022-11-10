#ifndef CUCKOO_H_
#define CUCKOO_H_

#include <optional>
#include <stdexcept>

class invalid_hash : public std::runtime_error {
   public:
    invalid_hash(const char* what_arg = "hash function must be supplied for non-primitive types") : runtime_error(what_arg) {
    }
};

// is_prime credit: https://stackoverflow.com/a/5694432 - Implementation 4
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

// next_prime credit: https://stackoverflow.com/a/5694432 - Implementation 4
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
class tables {
   private:
    std::optional<U>* m_tableOne;
    std::optional<U>* m_tableTwo;

    std::size_t m_currentSize;
    std::size_t m_maxSize;

    const std::size_t (*m_hash)(const T);

    void insertOne(T key, U value) {
        const std::size_t hash = this->m_hash(key) % this->m_maxSize;
        const std::optional<U> oldValue = this->m_tableOne[hash];
        if (value == *oldValue) {
            return;
        }
        this->m_tableOne[hash] = value;
        if (oldValue.has_value()) {
            this->insertTwo(hash, *oldValue);
        }
    }

    void insertTwo(T key, U value) {
        const std::size_t hash = this->m_hash(key) % this->m_maxSize;
        const std::optional<U> oldValue = this->m_tableTwo[hash];
        if (value == *oldValue) {
            return;
        }
        this->m_tableTwo[hash] = value;
        if (oldValue.has_value()) {
            this->insertOne(hash, *oldValue);
        }
    }

   public:
    tables(const std::size_t (*hash)(const T), std::size_t size) : m_hash(hash), m_size(size) {
    }

    void insert(T key, U value) {
        this->insertOne(key, value);
    }
};

template <typename T, typename U>
class cuckoo {
   private:
    std::optional<U>* m_tableOne;
    std::optional<U>* m_tableTwo;

    std::size_t m_currentSize;
    std::size_t m_maxSize;

    const std::size_t (*m_hash)(const T);

    void insertOne(T key, U value) {
        const std::size_t hash = this->m_hash(key) % this->m_maxSize;
        const std::optional<U> oldValue = this->m_tableOne[hash];
        if (value == *oldValue) {
            return;
        }
        this->m_tableOne[hash] = value;
        if (oldValue.has_value()) {
            this->insertTwo(hash, *oldValue);
        }
    }

    void insertTwo(T key, U value) {
        const std::size_t hash = this->m_hash(key) % this->m_maxSize;
        const std::optional<U> oldValue = this->m_tableTwo[hash];
        if (value == *oldValue) {
            return;
        }
        this->m_tableTwo[hash] = value;
        if (oldValue.has_value()) {
            this->insertOne(hash, *oldValue);
        }
    }

   public:
    cuckoo(const std::size_t (*hash)(const T)) : m_hash(hash), m_currentSize(0), m_maxSize(11) {
        this->m_tableOne = new std::optional<U>[11];
        this->m_tableTwo = new std::optional<U>[11];
    }

    cuckoo(const std::size_t (*hash)(const T), std::size_t size) : m_hash(hash), m_currentSize(0), m_maxSize(size) {
        this->m_tableOne = new std::optional<U>[this->m_maxSize];
        this->m_tableTwo = new std::optional<U>[this->m_maxSize];
    }

    ~cuckoo() {
        delete[] this->m_tableOne;
        delete[] this->m_tableTwo;
    }

    void insert(T key, U value) {
        this->insertOne(key, value);
    }

    const U& get(T key) {
        const std::size_t hash = this->m_hash(key) % this->m_maxSize;
        if (this->m_tableOne[hash].has_value()) {
            return *this->m_tableOne[hash];
        }
        if (this->m_tableTwo[hash].has_value()) {
            return *this->m_tableTwo[hash];
        }
        throw std::out_of_range("value does not exist at key");
    }

    void resize() {
        std::size_t size = this->next_prime(this->m_maxSize * 2);
        std::optional<U>* tableOne = new std::optional<U>[size];
        std::optional<U>* tableTwo = new std::optional<U>[size];
    }
};

#endif
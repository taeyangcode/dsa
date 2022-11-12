#ifndef CUCKOO_H_
#define CUCKOO_H_

#include <optional>
#include <stdexcept>

class invalid_hash : public std::runtime_error {
   public:
    invalid_hash(const char* what_arg = "hash function must be supplied for non-primitive types") : runtime_error(what_arg) {
    }
};

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
class cuckoo {
   private:
    std::size_t m_maxSize;
    std::size_t m_currentSize;

    const float m_loadFactor;

    std::optional<U>* m_arrayOne;
    std::optional<U>* m_arrayTwo;

    const bool (*m_hashOne)(T&&);
    const bool (*m_hashTwo)(T&&);

    const std::optional<U>& insertOne(T&& key, U&& value) {
        const std::size_t index = this->m_hashOne(key) % this->m_maxSize;
        const std::optional<U>& oldValue = this->m_arrayOne[index];
        this->m_arrayOne[index] = value;
        return oldValue;
    }

    const std::optional<U>& insertTwo(T&& key, U&& value) {
        const std::size_t index = this->m_hashTwo(key) % this->m_maxSize;
        const std::optional<U>& oldValue = this->m_arrayTwo[index];
        this->m_arrayTwo[index] = value;
        return oldValue;
    }

   public:
    cuckoo(const bool (*hashOne)(T&&), const bool (*hashTwo)(T&&)) : m_hashOne(hashOne), m_hashTwo(hashTwo), m_maxSize(11), m_loadFactor(0.5f), m_currentSize(0) {
        this->m_arrayOne = new std::optional<U>[11];
        this->m_arrayTwo = new std::optional<U>[11];
    }

    cuckoo(const bool (*hashOne)(T&&), const bool (*hashTwo)(T&&), std::size_t maxSize) : m_hashOne(hashOne), m_hashTwo(hashTwo), m_maxSize(maxSize), m_currentSize(0) {
        this->m_arrayOne = new std::optional<U>[this->m_maxSize];
        this->m_arrayTwo = new std::optional<U>[this->m_maxSize];
    }

    cuckoo(const bool (*hashOne)(T&&), const bool (*hashTwo)(T&&), const float loadFactor) : m_hashOne(hashOne), m_hashTwo(hashTwo), m_maxSize(11), m_loadFactor(loadFactor), m_currentSize(0) {
        this->m_arrayOne = new std::optional<U>[11];
        this->m_arrayTwo = new std::optional<U>[11];
    }

    cuckoo(const bool (*hashOne)(T&&), const bool (*hashTwo)(T&&), std::size_t maxSize, const float loadFactor) : m_hashOne(hashOne), m_hashTwo(hashTwo), m_maxSize(maxSize), m_loadFactor(loadFactor), m_currentSize(0) {
        this->m_arrayOne = new std::optional<U>[this->m_maxSize];
        this->m_arrayTwo = new std::optional<U>[this->m_maxSize];
    }

    ~cuckoo() {
        delete[] this->m_arrayOne;
        delete[] this->m_arrayTwo;
    }

    U& operator[](T&& key) {
        std::size_t index = this->m_hashOne(key) % this->m_maxSize;
        if (this->m_arrayOne[index].has_value()) {
            return *this->m_arrayOne[index];
        }
        index = this->hashTwo(key) % this->m_maxSize;
        if (this->m_arrayTwo[index].has_value()) {
            return *this->m_arrayTwo[index];
        }
        throw std::out_of_range();
    }

    void resize() {
        const std::size_t newSize = next_prime(this->m_maxSize);
        const std::optional<U>*& oldArrayOne = this->m_arrayOne;
        const std::optional<U>*& oldArrayTwo = this->m_arrayTwo;

        this->m_arrayOne = new std::optional<U>[newSize];
        this->m_arrayTwo = new std::optional<U>[newSize];
    }

    void insert(T&& key, U&& value) {
        std::size_t index = this->m_hashOne(key) % this->m_maxSize;
        if (!this->m_arrayOne[index].has_value()) {
            this->m_arrayOne[index] = value;
            return;
        }
        index = this->m_hashTwo(key) % this->m_maxSize;
        if (!this->m_arrayTwo[index].has_value()) {
            this->m_arrayTwo[index] = value;
            return;
        }
    }
};

#endif
#ifndef CUCKOO_H_
#define CUCKOO_H_

#include <optional>
#include <stdexcept>
#include <type_traits>

class invalid_hash : public std::runtime_error {
   public:
    invalid_hash(const char* what_arg = "hash function must be supplied for non-primitive types") : runtime_error(what_arg) {
    }
};

template <typename T>
const bool isPrimitive() {
    return std::is_fundamental<T>::value;
}

template <typename T>
class table {
   private:
    std::optional<T>* m_array;
    std::size_t m_size;
    const std::size_t (*m_hash)(const T);

    // isPrime credit: https://stackoverflow.com/a/5694432 - Implementation 4
    const bool isPrime(std::size_t target) {
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

    // nextPrime credit: https://stackoverflow.com/a/5694432 - Implementation 4
    const std::size_t nextPrime(const std::size_t num) {
        if (num <= 2) {
            return 2;
        }

        std::size_t n = num;
        if (!(n & 1)) {
            ++n;
        }
        for (; !this->isPrime(n); n += 2) {
        }
        return n;
    }

   public:
    table() : m_size(17) {
        this->m_array = new std::optional<T>[17];
    }

    table(std::size_t size) : m_size(size) {
        this->m_array = new std::optional<T>[size];
    }

    ~table() {
        delete[] this->m_array;
    }

    const std::size_t& size() {
        return this->m_size;
    }

    void insert(const std::size_t index, const T& value) {
        this->m_array[index] = value;
    }

    T& get(const std::size_t index) {
        if (index >= this->m_size || !this->m_array[index]) {
            throw std::out_of_range("value does not exist");
        }
        return *this->m_array[index];
    }

    void expand() {
        const std::size_t newSize = this->nextPrime(this->m_size * 2);
        std::optional<T>* newArray = new std::optional<T>[newSize];
    }
};

template <typename T, typename U>
class cuckoo {
};

#endif
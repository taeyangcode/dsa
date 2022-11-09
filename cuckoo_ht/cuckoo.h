#ifndef CUCKOO_H_
#define CUCKOO_H_

#include <optional>
#include <stdexcept>

template <typename T>
class table {
   private:
    std::optional<T>* m_array;
    std::size_t m_size;

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

    void insert(const std::size_t index, const T& value) {
        this->m_array[index] = value;
    }

    T& get(const std::size_t index) const {
        if (index >= this->m_size || !this->m_array[index].has_value()) {
            throw std::out_of_range("value does not exist");
        }
        return *this->m_array[index];
    }
};

template <typename T, typename U>
class cuckoo {
};

#endif
#pragma once

#include "Iterator.h"
#include "Types.h"
#include <cstring.h>

namespace Lib {
template<typename T>
class Vector {
public:
    Vector() : m_capacity(0), m_items(0) {}

    explicit Vector(size_t capacity) : m_capacity(capacity) {
        m_storage = new T[capacity];
    }

    ~Vector() {
        delete[] m_storage;
    }

    void append(T item) {
        if (m_items == m_capacity) {
            auto new_capacity = m_capacity == 0 ? 1 : m_capacity * 2;
            grow(new_capacity);
        }
        m_storage[m_items++] = item;
    }

    void grow(size_t new_capacity) {
        T *new_storage = new T[new_capacity];
        if (m_storage != nullptr) {
            memcpy(new_storage, m_storage, m_items * sizeof(T));
            delete[] m_storage;
        }
        m_capacity = new_capacity;
        m_storage = new_storage;
    }

    T &operator[](size_t index) {
        return m_storage[index];
    }

    [[nodiscard]] Iterator<T> begin() const { return {&m_storage[0]}; }

    [[nodiscard]] Iterator<T> end() const { return {&m_storage[m_items]}; }

    [[nodiscard]] size_t length() const { return m_items; }

private:
    size_t m_capacity{0};
    size_t m_items{0};
    T *m_storage{nullptr};
};
}// namespace Lib

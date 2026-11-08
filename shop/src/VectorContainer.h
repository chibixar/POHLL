#pragma once
#include <cstddef>
#include <stdexcept>
#include <utility>

// Собственный динамический массив с итератором (только перемещение)
template <typename T>
class VectorContainer
{
public:
    template <typename U>
    class BasicIterator
    {
        U* p_;

    public:
        explicit BasicIterator(U* p = nullptr) : p_(p) {}
        U& operator*() const { return *p_; }
        U* operator->() const { return p_; }
        BasicIterator& operator++() { ++p_; return *this; }
        BasicIterator operator++(int) { BasicIterator t = *this; ++p_; return t; }
        bool operator==(const BasicIterator& o) const { return p_ == o.p_; }
        bool operator!=(const BasicIterator& o) const { return p_ != o.p_; }
    };

    using Iterator = BasicIterator<T>;
    using ConstIterator = BasicIterator<const T>;

private:
    T* data_ = nullptr;
    size_t size_ = 0;
    size_t cap_ = 0;

    void reserve(size_t new_cap)
    {
        if (new_cap <= cap_)
            return;
        T* fresh = new T[new_cap];
        for (size_t i = 0; i < size_; ++i)
            fresh[i] = std::move(data_[i]);
        delete[] data_;
        data_ = fresh;
        cap_ = new_cap;
    }

public:
    VectorContainer() = default;
    VectorContainer(const VectorContainer&) = delete;
    VectorContainer& operator=(const VectorContainer&) = delete;

    VectorContainer(VectorContainer&& o) noexcept
        : data_(std::exchange(o.data_, nullptr)),
          size_(std::exchange(o.size_, 0)),
          cap_(std::exchange(o.cap_, 0)) {}

    VectorContainer& operator=(VectorContainer&& o) noexcept
    {
        if (this != &o)
        {
            delete[] data_;
            data_ = std::exchange(o.data_, nullptr);
            size_ = std::exchange(o.size_, 0);
            cap_ = std::exchange(o.cap_, 0);
        }
        return *this;
    }

    ~VectorContainer() { delete[] data_; }

    void push_back(T value)
    {
        if (size_ == cap_)
            reserve(cap_ == 0 ? 8 : cap_ * 2);
        data_[size_++] = std::move(value);
    }

    void insert(size_t index, T value)
    {
        if (index > size_)
            throw std::out_of_range("VectorContainer::insert: индекс вне диапазона");
        if (size_ == cap_)
            reserve(cap_ == 0 ? 8 : cap_ * 2);
        for (size_t i = size_; i > index; --i)
            data_[i] = std::move(data_[i - 1]);
        data_[index] = std::move(value);
        ++size_;
    }

    // Возвращает удалённый элемент (нужен для Undo)
    T erase(size_t index)
    {
        if (index >= size_)
            throw std::out_of_range("VectorContainer::erase: индекс вне диапазона");
        T removed = std::move(data_[index]);
        for (size_t i = index; i + 1 < size_; ++i)
            data_[i] = std::move(data_[i + 1]);
        data_[size_ - 1] = T();
        --size_;
        return removed;
    }

    T& operator[](size_t i)
    {
        if (i >= size_) throw std::out_of_range("VectorContainer: индекс вне диапазона");
        return data_[i];
    }
    const T& operator[](size_t i) const
    {
        if (i >= size_) throw std::out_of_range("VectorContainer: индекс вне диапазона");
        return data_[i];
    }

    size_t size() const { return size_; }
    bool empty() const { return size_ == 0; }

    T* data() { return data_; }
    const T* data() const { return data_; }

    Iterator begin() { return Iterator(data_); }
    Iterator end() { return Iterator(data_ + size_); }
    ConstIterator begin() const { return ConstIterator(data_); }
    ConstIterator end() const { return ConstIterator(data_ + size_); }
};

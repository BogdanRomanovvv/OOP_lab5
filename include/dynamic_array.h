#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H

#include <memory_resource>
#include <iterator>
#include <stdexcept>
#include <algorithm>

template <typename T>
class DynamicArray
{
public:
    using allocator_type = std::pmr::polymorphic_allocator<T>;
    using value_type = T;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using reference = T &;
    using const_reference = const T &;
    using pointer = T *;
    using const_pointer = const T *;

    class Iterator
    {
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = T;
        using difference_type = ptrdiff_t;
        using pointer = T *;
        using reference = T &;

        Iterator() : ptr_(nullptr) {}
        explicit Iterator(pointer ptr) : ptr_(ptr) {}

        reference operator*() const { return *ptr_; }
        pointer operator->() const { return ptr_; }
        reference operator[](difference_type n) const { return ptr_[n]; }

        // Префиксный инкремент
        Iterator &operator++()
        {
            ++ptr_;
            return *this;
        }

        // Постфиксный инкремент
        Iterator operator++(int)
        {
            Iterator temp = *this;
            ++ptr_;
            return temp;
        }

        // Префиксный декремент
        Iterator &operator--()
        {
            --ptr_;
            return *this;
        }

        // Постфиксный декремент
        Iterator operator--(int)
        {
            Iterator temp = *this;
            --ptr_;
            return temp;
        }

        // Арифметика указателей
        Iterator &operator+=(difference_type n)
        {
            ptr_ += n;
            return *this;
        }

        Iterator &operator-=(difference_type n)
        {
            ptr_ -= n;
            return *this;
        }

        Iterator operator+(difference_type n) const
        {
            return Iterator(ptr_ + n);
        }

        Iterator operator-(difference_type n) const
        {
            return Iterator(ptr_ - n);
        }

        difference_type operator-(const Iterator &other) const
        {
            return ptr_ - other.ptr_;
        }

        // Операторы сравнения
        bool operator==(const Iterator &other) const { return ptr_ == other.ptr_; }
        bool operator!=(const Iterator &other) const { return ptr_ != other.ptr_; }
        bool operator<(const Iterator &other) const { return ptr_ < other.ptr_; }
        bool operator>(const Iterator &other) const { return ptr_ > other.ptr_; }
        bool operator<=(const Iterator &other) const { return ptr_ <= other.ptr_; }
        bool operator>=(const Iterator &other) const { return ptr_ >= other.ptr_; }

    private:
        pointer ptr_;
    };

    class ConstIterator
    {
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = T;
        using difference_type = ptrdiff_t;
        using pointer = const T *;
        using reference = const T &;

        ConstIterator() : ptr_(nullptr) {}
        explicit ConstIterator(pointer ptr) : ptr_(ptr) {}
        ConstIterator(const Iterator &it) : ptr_(&(*it)) {}

        reference operator*() const { return *ptr_; }
        pointer operator->() const { return ptr_; }
        reference operator[](difference_type n) const { return ptr_[n]; }

        // Префиксный инкремент
        ConstIterator &operator++()
        {
            ++ptr_;
            return *this;
        }

        // Постфиксный инкремент
        ConstIterator operator++(int)
        {
            ConstIterator temp = *this;
            ++ptr_;
            return temp;
        }

        // Префиксный декремент
        ConstIterator &operator--()
        {
            --ptr_;
            return *this;
        }

        // Постфиксный декремент
        ConstIterator operator--(int)
        {
            ConstIterator temp = *this;
            --ptr_;
            return temp;
        }

        // Арифметика указателей
        ConstIterator &operator+=(difference_type n)
        {
            ptr_ += n;
            return *this;
        }

        ConstIterator &operator-=(difference_type n)
        {
            ptr_ -= n;
            return *this;
        }

        ConstIterator operator+(difference_type n) const
        {
            return ConstIterator(ptr_ + n);
        }

        ConstIterator operator-(difference_type n) const
        {
            return ConstIterator(ptr_ - n);
        }

        difference_type operator-(const ConstIterator &other) const
        {
            return ptr_ - other.ptr_;
        }

        // Операторы сравнения
        bool operator==(const ConstIterator &other) const { return ptr_ == other.ptr_; }
        bool operator!=(const ConstIterator &other) const { return ptr_ != other.ptr_; }
        bool operator<(const ConstIterator &other) const { return ptr_ < other.ptr_; }
        bool operator>(const ConstIterator &other) const { return ptr_ > other.ptr_; }
        bool operator<=(const ConstIterator &other) const { return ptr_ <= other.ptr_; }
        bool operator>=(const ConstIterator &other) const { return ptr_ >= other.ptr_; }

    private:
        pointer ptr_;
    };

    using iterator = Iterator;
    using const_iterator = ConstIterator;

    // Конструкторы
    explicit DynamicArray(std::pmr::memory_resource *mr = std::pmr::get_default_resource())
        : allocator_(mr), data_(nullptr), size_(0), capacity_(0) {}

    explicit DynamicArray(size_type count, std::pmr::memory_resource *mr = std::pmr::get_default_resource())
        : allocator_(mr), data_(nullptr), size_(0), capacity_(0)
    {
        resize(count);
    }

    DynamicArray(size_type count, const T &value, std::pmr::memory_resource *mr = std::pmr::get_default_resource())
        : allocator_(mr), data_(nullptr), size_(0), capacity_(0)
    {
        resize(count, value);
    }

    // Конструктор копирования
    DynamicArray(const DynamicArray &other)
        : allocator_(other.allocator_), data_(nullptr), size_(0), capacity_(0)
    {
        reserve(other.size_);
        for (size_type i = 0; i < other.size_; ++i)
        {
            push_back(other.data_[i]);
        }
    }

    // Конструктор перемещения
    DynamicArray(DynamicArray &&other) noexcept
        : allocator_(std::move(other.allocator_)),
          data_(other.data_),
          size_(other.size_),
          capacity_(other.capacity_)
    {
        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }

    // Деструктор
    ~DynamicArray()
    {
        clear();
        if (data_)
        {
            allocator_.deallocate(data_, capacity_);
        }
    }

    // Оператор присваивания
    DynamicArray &operator=(const DynamicArray &other)
    {
        if (this != &other)
        {
            clear();
            reserve(other.size_);
            for (size_type i = 0; i < other.size_; ++i)
            {
                push_back(other.data_[i]);
            }
        }
        return *this;
    }

    // Оператор перемещающего присваивания
    DynamicArray &operator=(DynamicArray &&other) noexcept
    {
        if (this != &other)
        {
            clear();
            if (data_)
            {
                allocator_.deallocate(data_, capacity_);
            }

            allocator_ = std::move(other.allocator_);
            data_ = other.data_;
            size_ = other.size_;
            capacity_ = other.capacity_;

            other.data_ = nullptr;
            other.size_ = 0;
            other.capacity_ = 0;
        }
        return *this;
    }

    // Итераторы
    iterator begin() { return iterator(data_); }
    iterator end() { return iterator(data_ + size_); }
    const_iterator begin() const { return const_iterator(data_); }
    const_iterator end() const { return const_iterator(data_ + size_); }
    const_iterator cbegin() const { return const_iterator(data_); }
    const_iterator cend() const { return const_iterator(data_ + size_); }

    // Доступ к элементам
    reference operator[](size_type index)
    {
        return data_[index];
    }

    const_reference operator[](size_type index) const
    {
        return data_[index];
    }

    reference at(size_type index)
    {
        if (index >= size_)
        {
            throw std::out_of_range("DynamicArray::at: индекс вне диапазона");
        }
        return data_[index];
    }

    const_reference at(size_type index) const
    {
        if (index >= size_)
        {
            throw std::out_of_range("DynamicArray::at: индекс вне диапазона");
        }
        return data_[index];
    }

    reference front() { return data_[0]; }
    const_reference front() const { return data_[0]; }
    reference back() { return data_[size_ - 1]; }
    const_reference back() const { return data_[size_ - 1]; }

    // Размер и емкость
    size_type size() const { return size_; }
    size_type capacity() const { return capacity_; }
    bool empty() const { return size_ == 0; }

    // Модификаторы
    void push_back(const T &value)
    {
        if (size_ == capacity_)
        {
            reserve(capacity_ == 0 ? 1 : capacity_ * 2);
        }
        std::allocator_traits<allocator_type>::construct(allocator_, data_ + size_, value);
        ++size_;
    }

    void push_back(T &&value)
    {
        if (size_ == capacity_)
        {
            reserve(capacity_ == 0 ? 1 : capacity_ * 2);
        }
        std::allocator_traits<allocator_type>::construct(allocator_, data_ + size_, std::move(value));
        ++size_;
    }

    template <typename... Args>
    reference emplace_back(Args &&...args)
    {
        if (size_ == capacity_)
        {
            reserve(capacity_ == 0 ? 1 : capacity_ * 2);
        }
        std::allocator_traits<allocator_type>::construct(
            allocator_, data_ + size_, std::forward<Args>(args)...);
        ++size_;
        return data_[size_ - 1];
    }

    void pop_back()
    {
        if (size_ > 0)
        {
            --size_;
            std::allocator_traits<allocator_type>::destroy(allocator_, data_ + size_);
        }
    }

    void clear()
    {
        for (size_type i = 0; i < size_; ++i)
        {
            std::allocator_traits<allocator_type>::destroy(allocator_, data_ + i);
        }
        size_ = 0;
    }

    void reserve(size_type new_capacity)
    {
        if (new_capacity <= capacity_)
        {
            return;
        }

        pointer new_data = allocator_.allocate(new_capacity);

        // Перемещаем существующие элементы
        for (size_type i = 0; i < size_; ++i)
        {
            std::allocator_traits<allocator_type>::construct(
                allocator_, new_data + i, std::move(data_[i]));
            std::allocator_traits<allocator_type>::destroy(allocator_, data_ + i);
        }

        if (data_)
        {
            allocator_.deallocate(data_, capacity_);
        }

        data_ = new_data;
        capacity_ = new_capacity;
    }

    void resize(size_type new_size)
    {
        if (new_size > capacity_)
        {
            reserve(new_size);
        }

        if (new_size > size_)
        {
            for (size_type i = size_; i < new_size; ++i)
            {
                std::allocator_traits<allocator_type>::construct(allocator_, data_ + i);
            }
        }
        else if (new_size < size_)
        {
            for (size_type i = new_size; i < size_; ++i)
            {
                std::allocator_traits<allocator_type>::destroy(allocator_, data_ + i);
            }
        }

        size_ = new_size;
    }

    void resize(size_type new_size, const T &value)
    {
        if (new_size > capacity_)
        {
            reserve(new_size);
        }

        if (new_size > size_)
        {
            for (size_type i = size_; i < new_size; ++i)
            {
                std::allocator_traits<allocator_type>::construct(allocator_, data_ + i, value);
            }
        }
        else if (new_size < size_)
        {
            for (size_type i = new_size; i < size_; ++i)
            {
                std::allocator_traits<allocator_type>::destroy(allocator_, data_ + i);
            }
        }

        size_ = new_size;
    }

    allocator_type get_allocator() const { return allocator_; }

private:
    allocator_type allocator_;
    pointer data_;
    size_type size_;
    size_type capacity_;
};

#endif // DYNAMIC_ARRAY_H

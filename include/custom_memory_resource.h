#ifndef CUSTOM_MEMORY_RESOURCE_H
#define CUSTOM_MEMORY_RESOURCE_H

#include <memory_resource>
#include <list>
#include <algorithm>
#include <iostream>

class CustomMemoryResource : public std::pmr::memory_resource
{
private:
    struct MemoryBlock
    {
        void *ptr{nullptr};  // Адрес начала блока памяти в куче
        size_t size{0};      // Сколько байт занимает этот блок
        size_t alignment{0}; // Выравнивание памяти (нужно для правильной работы с разными типами данных)
        bool free{false};    // true = блок свободен и можно его переиспользовать, false = блок занят
    };

    // Список всех блоков памяти (и занятых, и свободных)
    std::list<MemoryBlock> allocated_blocks_;

    // Статистика: сколько всего байт мы выделили за всё время работы
    size_t total_allocated_bytes_{0};

    // Статистика: сколько всего байт мы пометили как освобожденные
    size_t total_deallocated_bytes_{0};

    // Режим отладки: если true, то будем выводить сообщения о каждой операции с памятью
    bool verbose_{false};

protected:
    void *do_allocate(size_t bytes, size_t alignment) override
    {
        // Пытаемся найти уже существующий свободный блок, который подходит по размеру и выравниванию
        auto it = std::find_if(allocated_blocks_.begin(), allocated_blocks_.end(),
                               [bytes, alignment](const MemoryBlock &block)
                               {
                                   // Блок подходит, если:
                                   // 1. Он помечен как свободный (free == true)
                                   // 2. Он достаточно большой (size >= bytes)
                                   // 3. У него правильное выравнивание (alignment совпадает)
                                   return block.free && block.size >= bytes && block.alignment == alignment;
                               });

        // Если нашли подходящий свободный блок
        if (it != allocated_blocks_.end())
        {
            // Помечаем блок как занятый (теперь он снова используется)
            it->free = false;

            // Если включен режим отладки, выводим информацию
            if (verbose_)
            {
                std::cout << "CustomMemoryResource: переиспользован блок "
                          << it->ptr << " размером " << it->size << " байт\n";
            }

            // Возвращаем адрес этого блока
            return it->ptr;
        }

        // Если не нашли подходящий блок, выделяем новую память на куче
        // ::operator new - это глобальная функция выделения памяти
        // std::align_val_t нужен для правильного выравнивания памяти
        void *ptr = ::operator new(bytes, std::align_val_t(alignment));

        // Добавляем информацию о новом блоке в наш список
        // {ptr, bytes, alignment, false} - создаём структуру MemoryBlock
        // false означает, что блок занят (не свободен)
        allocated_blocks_.push_back({ptr, bytes, alignment, false});

        // Обновляем статистику: увеличиваем счётчик выделенных байт
        total_allocated_bytes_ += bytes;

        // Если включен режим отладки, выводим информацию о новом блоке
        if (verbose_)
        {
            std::cout << "CustomMemoryResource: выделен новый блок "
                      << ptr << " размером " << bytes << " байт\n";
        }

        // Возвращаем адрес нового блока
        return ptr;
    }

    void do_deallocate(void *ptr, size_t bytes, size_t alignment) override
    {
        // Ищем блок с указанным адресом в нашем списке
        auto it = std::find_if(allocated_blocks_.begin(), allocated_blocks_.end(),
                               [ptr](const MemoryBlock &block)
                               {
                                   // Сравниваем адреса: если совпадают, значит нашли нужный блок
                                   return block.ptr == ptr;
                               });

        // Если нашли блок с таким адресом
        if (it != allocated_blocks_.end())
        {
            // Просто помечаем блок как свободный, но НЕ удаляем его из списка!
            // Это ключевой момент: блок остаётся в памяти и может быть переиспользован
            it->free = true;

            // Обновляем статистику: увеличиваем счётчик освобождённых байт
            total_deallocated_bytes_ += bytes;

            // Если включен режим отладки, выводим информацию
            if (verbose_)
            {
                std::cout << "CustomMemoryResource: освобожден блок "
                          << ptr << " размером " << bytes << " байт\n";
            }
        }
        // Если блок не найден - ничего не делаем (это нормально, может быть вызов с nullptr)
    }

    bool do_is_equal(const std::pmr::memory_resource &other) const noexcept override
    {
        return this == &other;
    }

public:
    /**
     * Конструктор по умолчанию.
     * Создаёт пустой менеджер памяти без выделенных блоков.
     */
    CustomMemoryResource() = default;

    ~CustomMemoryResource() override
    {
        // Проходим по всем блокам в списке
        for (auto &block : allocated_blocks_)
        {
            // Физически удаляем блок памяти с помощью глобального оператора delete
            // Важно передать alignment, чтобы память удалилась корректно
            ::operator delete(block.ptr, std::align_val_t(block.alignment));
        }

        // Если включен режим отладки, выводим итоговую статистику
        if (verbose_)
        {
            std::cout << "CustomMemoryResource: очищено "
                      << allocated_blocks_.size() << " блоков\n";
        }
    }

    CustomMemoryResource(const CustomMemoryResource &) = delete;
    CustomMemoryResource &operator=(const CustomMemoryResource &) = delete;

    void set_verbose(bool verbose) { verbose_ = verbose; }

    void print_allocated_blocks() const
    {
        std::cout << "=== Информация о блоках памяти ===\n";
        int index = 0;

        // Проходим по всем блокам и выводим информацию о каждом
        for (const auto &block : allocated_blocks_)
        {
            std::cout << "Блок " << index++ << ": "
                      << "ptr=" << block.ptr
                      << ", size=" << block.size
                      << ", alignment=" << block.alignment
                      << ", status=" << (block.free ? "FREE" : "USED")
                      << "\n";
        }

        // Выводим итоговую статистику
        std::cout << "Всего блоков: " << allocated_blocks_.size() << "\n";
        std::cout << "Активных: " << get_allocated_blocks_count() << "\n";
        std::cout << "Свободных: " << get_free_blocks_count() << "\n";
    }

    size_t get_allocated_blocks_count() const
    {
        return std::count_if(allocated_blocks_.begin(), allocated_blocks_.end(),
                             [](const MemoryBlock &block)
                             {
                                 // Блок активен, если он НЕ свободен (!free)
                                 return !block.free;
                             });
    }

    size_t get_free_blocks_count() const
    {
        return std::count_if(allocated_blocks_.begin(), allocated_blocks_.end(),
                             [](const MemoryBlock &block)
                             {
                                 // Блок свободен, если у него free == true
                                 return block.free;
                             });
    }

    size_t get_total_allocated_bytes() const { return total_allocated_bytes_; }

    size_t get_total_deallocated_bytes() const { return total_deallocated_bytes_; }
};

#endif

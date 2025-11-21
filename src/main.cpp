#include <iostream>
#include <string>
#include "custom_memory_resource.h"
#include "dynamic_array.h"

struct Person
{
    std::string name;
    int age;

    Person() : name(""), age(0) {}
    Person(std::string n, int a) : name(std::move(n)), age(a) {}

    friend std::ostream &operator<<(std::ostream &os, const Person &p)
    {
        os << "{" << p.name << ", " << p.age << "}";
        return os;
    }
};

int main()
{

    // Создаём свой memory_resource
    CustomMemoryResource mr;

    // Пример 1: Работа с int
    std::cout << "1. Динамический массив с int:\n";
    DynamicArray<int> numbers(&mr);

    numbers.push_back(10);
    numbers.push_back(20);
    numbers.push_back(30);

    std::cout << "   Элементы: ";
    for (const auto &num : numbers)
    {
        std::cout << num << " ";
    }
    std::cout << "\n   Размер: " << numbers.size() << "\n\n";

    // Пример 2: Работа со структурой Person
    std::cout << "2. Динамический массив с Person:\n";
    DynamicArray<Person> people(&mr);

    people.push_back(Person("Иван", 25));
    people.push_back(Person("Мария", 30));
    people.emplace_back("Петр", 35);

    std::cout << "   Люди: ";
    for (const auto &person : people)
    {
        std::cout << person << " ";
    }
    std::cout << "\n   Размер: " << people.size() << "\n\n";

    // Пример 3: Использование итератора
    std::cout << "3. Изменение через итератор:\n";
    for (auto it = numbers.begin(); it != numbers.end(); ++it)
    {
        *it += 5;
    }

    std::cout << "   Элементы после изменения: ";
    for (const auto &num : numbers)
    {
        std::cout << num << " ";
    }
    std::cout << "\n\n";

    std::cout << "=== Программа завершена успешно ===\n";

    return 0;
}

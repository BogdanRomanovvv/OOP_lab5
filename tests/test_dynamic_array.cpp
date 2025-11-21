#include <gtest/gtest.h>
#include "custom_memory_resource.h"
#include "dynamic_array.h"
#include <string>
#include <algorithm>

// Тесты для DynamicArray
class DynamicArrayTest : public ::testing::Test
{
protected:
    CustomMemoryResource *mr;

    void SetUp() override
    {
        mr = new CustomMemoryResource();
    }

    void TearDown() override
    {
        delete mr;
    }
};

// Тесты с простым типом (int)
TEST_F(DynamicArrayTest, DefaultConstructor)
{
    DynamicArray<int> arr(mr);
    EXPECT_EQ(arr.size(), 0);
    EXPECT_TRUE(arr.empty());
}

TEST_F(DynamicArrayTest, PushBack)
{
    DynamicArray<int> arr(mr);
    arr.push_back(10);
    arr.push_back(20);
    arr.push_back(30);

    EXPECT_EQ(arr.size(), 3);
    EXPECT_EQ(arr[0], 10);
    EXPECT_EQ(arr[1], 20);
    EXPECT_EQ(arr[2], 30);
}

TEST_F(DynamicArrayTest, PopBack)
{
    DynamicArray<int> arr(mr);
    arr.push_back(10);
    arr.push_back(20);
    arr.pop_back();

    EXPECT_EQ(arr.size(), 1);
    EXPECT_EQ(arr[0], 10);
}

TEST_F(DynamicArrayTest, AccessOperator)
{
    DynamicArray<int> arr(mr);
    arr.push_back(100);
    arr.push_back(200);

    EXPECT_EQ(arr[0], 100);
    EXPECT_EQ(arr[1], 200);

    arr[0] = 150;
    EXPECT_EQ(arr[0], 150);
}

TEST_F(DynamicArrayTest, AtMethod)
{
    DynamicArray<int> arr(mr);
    arr.push_back(10);
    arr.push_back(20);

    EXPECT_EQ(arr.at(0), 10);
    EXPECT_EQ(arr.at(1), 20);
    EXPECT_THROW(arr.at(2), std::out_of_range);
}

TEST_F(DynamicArrayTest, FrontAndBack)
{
    DynamicArray<int> arr(mr);
    arr.push_back(10);
    arr.push_back(20);
    arr.push_back(30);

    EXPECT_EQ(arr.front(), 10);
    EXPECT_EQ(arr.back(), 30);
}

TEST_F(DynamicArrayTest, Clear)
{
    DynamicArray<int> arr(mr);
    arr.push_back(10);
    arr.push_back(20);
    arr.clear();

    EXPECT_EQ(arr.size(), 0);
    EXPECT_TRUE(arr.empty());
}

TEST_F(DynamicArrayTest, Reserve)
{
    DynamicArray<int> arr(mr);
    arr.reserve(10);

    EXPECT_GE(arr.capacity(), 10);
    EXPECT_EQ(arr.size(), 0);
}

TEST_F(DynamicArrayTest, Resize)
{
    DynamicArray<int> arr(mr);
    arr.resize(5);

    EXPECT_EQ(arr.size(), 5);
    for (size_t i = 0; i < arr.size(); ++i)
    {
        EXPECT_EQ(arr[i], 0);
    }
}

TEST_F(DynamicArrayTest, ResizeWithValue)
{
    DynamicArray<int> arr(mr);
    arr.resize(5, 42);

    EXPECT_EQ(arr.size(), 5);
    for (size_t i = 0; i < arr.size(); ++i)
    {
        EXPECT_EQ(arr[i], 42);
    }
}

TEST_F(DynamicArrayTest, CopyConstructor)
{
    DynamicArray<int> arr1(mr);
    arr1.push_back(10);
    arr1.push_back(20);

    DynamicArray<int> arr2 = arr1;

    EXPECT_EQ(arr2.size(), 2);
    EXPECT_EQ(arr2[0], 10);
    EXPECT_EQ(arr2[1], 20);

    // Проверяем независимость
    arr2[0] = 100;
    EXPECT_EQ(arr1[0], 10);
    EXPECT_EQ(arr2[0], 100);
}

TEST_F(DynamicArrayTest, MoveConstructor)
{
    DynamicArray<int> arr1(mr);
    arr1.push_back(10);
    arr1.push_back(20);

    DynamicArray<int> arr2 = std::move(arr1);

    EXPECT_EQ(arr2.size(), 2);
    EXPECT_EQ(arr2[0], 10);
    EXPECT_EQ(arr2[1], 20);
    EXPECT_EQ(arr1.size(), 0);
}

TEST_F(DynamicArrayTest, AssignmentOperator)
{
    DynamicArray<int> arr1(mr);
    arr1.push_back(10);
    arr1.push_back(20);

    DynamicArray<int> arr2(mr);
    arr2 = arr1;

    EXPECT_EQ(arr2.size(), 2);
    EXPECT_EQ(arr2[0], 10);
    EXPECT_EQ(arr2[1], 20);
}

TEST_F(DynamicArrayTest, EmplaceBack)
{
    DynamicArray<int> arr(mr);
    arr.emplace_back(10);
    arr.emplace_back(20);

    EXPECT_EQ(arr.size(), 2);
    EXPECT_EQ(arr[0], 10);
    EXPECT_EQ(arr[1], 20);
}

// Тесты итератора
TEST_F(DynamicArrayTest, IteratorBeginEnd)
{
    DynamicArray<int> arr(mr);
    arr.push_back(10);
    arr.push_back(20);
    arr.push_back(30);

    auto it = arr.begin();
    EXPECT_EQ(*it, 10);
    ++it;
    EXPECT_EQ(*it, 20);
    ++it;
    EXPECT_EQ(*it, 30);
    ++it;
    EXPECT_EQ(it, arr.end());
}

TEST_F(DynamicArrayTest, IteratorPostIncrement)
{
    DynamicArray<int> arr(mr);
    arr.push_back(10);
    arr.push_back(20);

    auto it = arr.begin();
    auto old_it = it++;

    EXPECT_EQ(*old_it, 10);
    EXPECT_EQ(*it, 20);
}

TEST_F(DynamicArrayTest, IteratorComparison)
{
    DynamicArray<int> arr(mr);
    arr.push_back(10);
    arr.push_back(20);

    auto it1 = arr.begin();
    auto it2 = arr.begin();
    auto it3 = arr.end();

    EXPECT_TRUE(it1 == it2);
    EXPECT_FALSE(it1 == it3);
    EXPECT_TRUE(it1 != it3);
}

TEST_F(DynamicArrayTest, IteratorModification)
{
    DynamicArray<int> arr(mr);
    arr.push_back(10);
    arr.push_back(20);
    arr.push_back(30);

    for (auto it = arr.begin(); it != arr.end(); ++it)
    {
        *it *= 2;
    }

    EXPECT_EQ(arr[0], 20);
    EXPECT_EQ(arr[1], 40);
    EXPECT_EQ(arr[2], 60);
}

TEST_F(DynamicArrayTest, RangeBasedFor)
{
    DynamicArray<int> arr(mr);
    arr.push_back(10);
    arr.push_back(20);
    arr.push_back(30);

    int sum = 0;
    for (const auto &val : arr)
    {
        sum += val;
    }

    EXPECT_EQ(sum, 60);
}

TEST_F(DynamicArrayTest, ConstIterator)
{
    DynamicArray<int> arr(mr);
    arr.push_back(10);
    arr.push_back(20);

    const auto &const_arr = arr;

    int count = 0;
    for (auto it = const_arr.begin(); it != const_arr.end(); ++it)
    {
        count++;
    }

    EXPECT_EQ(count, 2);
}

TEST_F(DynamicArrayTest, STLAlgorithms)
{
    DynamicArray<int> arr(mr);
    arr.push_back(30);
    arr.push_back(10);
    arr.push_back(20);

    std::sort(arr.begin(), arr.end());

    EXPECT_EQ(arr[0], 10);
    EXPECT_EQ(arr[1], 20);
    EXPECT_EQ(arr[2], 30);
}

// Тесты со сложной структурой
struct TestStruct
{
    std::string name;
    int value;

    TestStruct() : name(""), value(0) {}
    TestStruct(std::string n, int v) : name(std::move(n)), value(v) {}

    bool operator==(const TestStruct &other) const
    {
        return name == other.name && value == other.value;
    }
};

TEST_F(DynamicArrayTest, ComplexType)
{
    DynamicArray<TestStruct> arr(mr);

    arr.push_back(TestStruct("First", 1));
    arr.push_back(TestStruct("Second", 2));
    arr.emplace_back("Third", 3);

    EXPECT_EQ(arr.size(), 3);
    EXPECT_EQ(arr[0].name, "First");
    EXPECT_EQ(arr[1].value, 2);
    EXPECT_EQ(arr[2].name, "Third");
}

TEST_F(DynamicArrayTest, ComplexTypeIteration)
{
    DynamicArray<TestStruct> arr(mr);
    arr.emplace_back("A", 1);
    arr.emplace_back("B", 2);
    arr.emplace_back("C", 3);

    for (auto &item : arr)
    {
        item.value *= 10;
    }

    EXPECT_EQ(arr[0].value, 10);
    EXPECT_EQ(arr[1].value, 20);
    EXPECT_EQ(arr[2].value, 30);
}

TEST_F(DynamicArrayTest, MemoryResourceUsage)
{
    DynamicArray<int> arr(mr);

    size_t initial_blocks = mr->get_allocated_blocks_count();

    for (int i = 0; i < 10; ++i)
    {
        arr.push_back(i);
    }

    // Должны быть выделены блоки памяти
    EXPECT_GT(mr->get_allocated_blocks_count(), initial_blocks);
}

TEST_F(DynamicArrayTest, AutomaticReallocation)
{
    DynamicArray<int> arr(mr);

    // Добавляем много элементов, чтобы вызвать реаллокацию
    for (int i = 0; i < 100; ++i)
    {
        arr.push_back(i);
    }

    EXPECT_EQ(arr.size(), 100);
    EXPECT_GE(arr.capacity(), 100);

    // Проверяем корректность данных
    for (int i = 0; i < 100; ++i)
    {
        EXPECT_EQ(arr[i], i);
    }
}

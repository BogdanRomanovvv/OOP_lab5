#include <gtest/gtest.h>
#include "custom_memory_resource.h"

// Тесты для CustomMemoryResource
class CustomMemoryResourceTest : public ::testing::Test
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

TEST_F(CustomMemoryResourceTest, BasicAllocation)
{
    void *ptr = mr->allocate(100);
    ASSERT_NE(ptr, nullptr);
    EXPECT_EQ(mr->get_allocated_blocks_count(), 1);
    EXPECT_EQ(mr->get_free_blocks_count(), 0);
    mr->deallocate(ptr, 100);
}

TEST_F(CustomMemoryResourceTest, Deallocation)
{
    void *ptr = mr->allocate(100);
    mr->deallocate(ptr, 100);

    EXPECT_EQ(mr->get_allocated_blocks_count(), 0);
    EXPECT_EQ(mr->get_free_blocks_count(), 1);
}

TEST_F(CustomMemoryResourceTest, MemoryReuse)
{
    // Выделяем и освобождаем память
    void *ptr1 = mr->allocate(100);
    mr->deallocate(ptr1, 100);

    EXPECT_EQ(mr->get_free_blocks_count(), 1);

    // Выделяем снова - должна переиспользоваться память
    void *ptr2 = mr->allocate(100);

    EXPECT_EQ(mr->get_allocated_blocks_count(), 1);
    EXPECT_EQ(mr->get_free_blocks_count(), 0);

    // Память может быть переиспользована
    EXPECT_EQ(ptr1, ptr2);

    mr->deallocate(ptr2, 100);
}

TEST_F(CustomMemoryResourceTest, MultipleAllocations)
{
    void *ptr1 = mr->allocate(50);
    void *ptr2 = mr->allocate(100);
    void *ptr3 = mr->allocate(150);

    EXPECT_EQ(mr->get_allocated_blocks_count(), 3);
    EXPECT_NE(ptr1, ptr2);
    EXPECT_NE(ptr2, ptr3);
    EXPECT_NE(ptr1, ptr3);

    mr->deallocate(ptr1, 50);
    mr->deallocate(ptr2, 100);
    mr->deallocate(ptr3, 150);
}

TEST_F(CustomMemoryResourceTest, Statistics)
{
    void *ptr1 = mr->allocate(100);
    void *ptr2 = mr->allocate(200);

    EXPECT_GE(mr->get_total_allocated_bytes(), 300);

    mr->deallocate(ptr1, 100);
    EXPECT_GE(mr->get_total_deallocated_bytes(), 100);

    mr->deallocate(ptr2, 200);
    EXPECT_GE(mr->get_total_deallocated_bytes(), 300);
}

TEST_F(CustomMemoryResourceTest, DifferentSizes)
{
    void *ptr1 = mr->allocate(10);
    mr->deallocate(ptr1, 10);

    // Пытаемся выделить блок большего размера
    void *ptr2 = mr->allocate(100);

    // Должен быть выделен новый блок (старый мал)
    EXPECT_EQ(mr->get_allocated_blocks_count(), 1);
    EXPECT_EQ(mr->get_free_blocks_count(), 1);

    mr->deallocate(ptr2, 100);
}

TEST_F(CustomMemoryResourceTest, Cleanup)
{
    // Выделяем память, но не освобождаем
    mr->allocate(100);
    mr->allocate(200);
    mr->allocate(300);

    EXPECT_EQ(mr->get_allocated_blocks_count(), 3);

    // Деструктор должен очистить всю неосвобожденную память
    // Это проверяется при уничтожении mr в TearDown
}

TEST_F(CustomMemoryResourceTest, IsEqual)
{
    CustomMemoryResource mr2;

    EXPECT_TRUE(mr->is_equal(*mr));
    EXPECT_FALSE(mr->is_equal(mr2));
}

TEST_F(CustomMemoryResourceTest, AlignedAllocation)
{
    void *ptr1 = mr->allocate(100, 16);
    void *ptr2 = mr->allocate(100, 32);

    EXPECT_NE(ptr1, nullptr);
    EXPECT_NE(ptr2, nullptr);
    EXPECT_EQ(mr->get_allocated_blocks_count(), 2);

    mr->deallocate(ptr1, 100, 16);
    mr->deallocate(ptr2, 100, 32);
}

TEST_F(CustomMemoryResourceTest, ReuseWithSameAlignment)
{
    void *ptr1 = mr->allocate(100, 16);
    mr->deallocate(ptr1, 100, 16);

    // Выделяем с тем же выравниванием - должна переиспользоваться память
    void *ptr2 = mr->allocate(100, 16);

    EXPECT_EQ(ptr1, ptr2);
    EXPECT_EQ(mr->get_free_blocks_count(), 0);

    mr->deallocate(ptr2, 100, 16);
}

TEST_F(CustomMemoryResourceTest, NoReuseWithDifferentAlignment)
{
    void *ptr1 = mr->allocate(100, 16);
    mr->deallocate(ptr1, 100, 16);

    // Выделяем с другим выравниванием - не должна переиспользоваться память
    void *ptr2 = mr->allocate(100, 32);

    EXPECT_NE(ptr1, ptr2);
    EXPECT_EQ(mr->get_allocated_blocks_count(), 1);
    EXPECT_EQ(mr->get_free_blocks_count(), 1);

    mr->deallocate(ptr2, 100, 32);
}

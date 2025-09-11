#include <algorithm>
#include <random>

#include "../MemoryPool.hpp"
#include <gtest/gtest.h>

class alignas(uintptr_t) TestClass {
 public:
  // Counter to check if constructor and destructor are called
  static inline int mInstanceCount = 0;

  TestClass() {
    ++mInstanceCount;
  }

  ~TestClass() {
    --mInstanceCount;
  }
};

// Test fixture for MemoryPool
class MemoryPoolTest : public ::testing::Test {
 protected:
  void SetUp() override {
    TestClass::mInstanceCount = 0;
  }
};

// Test case for allocation
TEST_F(MemoryPoolTest, Allocation) {
  constexpr size_t kPoolSize = 10;
  MemoryPool<TestClass> pool(kPoolSize);

  for (size_t i = 0; i < kPoolSize; ++i) {
    auto ptr = pool.Allocate();
    ASSERT_NE(ptr, nullptr);
  }
}

// Test case for allocation when pool is full
TEST_F(MemoryPoolTest, AllocationFull) {
  constexpr size_t kPoolSize = 5;
  MemoryPool<TestClass> pool(kPoolSize);

  for (size_t i = 0; i < kPoolSize; ++i) {
    (void)pool.Allocate();
  }

  // After filling the pool, the next allocation should fail
  ASSERT_EQ(pool.Allocate(), nullptr);
}

// Test case for destruction
TEST_F(MemoryPoolTest, Destruction) {
  constexpr size_t kPoolSize = 20;
  MemoryPool<TestClass> pool(kPoolSize);

  ASSERT_EQ(TestClass::mInstanceCount, 0);

  std::vector<TestClass*> allocatedPtrs;

  // Allocate 5 objects and check if constructors are called
  for (size_t i = 0; i < 5; ++i) {
    auto ptr = pool.Allocate();
    ASSERT_NE(ptr, nullptr);
    allocatedPtrs.push_back(ptr);
  }
  ASSERT_EQ(TestClass::mInstanceCount, 5);

  // Free the allocated objects and check if destructors are called
  for (auto ptr : allocatedPtrs) {
    pool.Free(ptr);
  }

  ASSERT_EQ(TestClass::mInstanceCount, 0);
}

// Test case for shuffled destruction
TEST_F(MemoryPoolTest, SuffledDestruction) {
  constexpr size_t kPoolSize = 20;
  MemoryPool<TestClass> pool(kPoolSize);

  ASSERT_EQ(TestClass::mInstanceCount, 0);

  std::vector<TestClass*> allocatedPtrs;

  // Allocate 5 objects and check if constructors are called
  for (size_t i = 0; i < 5; ++i) {
    auto ptr = pool.Allocate();
    ASSERT_NE(ptr, nullptr);
    allocatedPtrs.push_back(ptr);
  }
  ASSERT_EQ(TestClass::mInstanceCount, 5);

  std::ranges::shuffle(allocatedPtrs, std::mt19937{std::random_device{}()});
  // Free the allocated objects and check if destructors are called
  for (auto ptr : allocatedPtrs) {
    pool.Free(ptr);
  }

  ASSERT_EQ(TestClass::mInstanceCount, 0);
}

// Test case for freeing memory
TEST_F(MemoryPoolTest, Freeing) {
  constexpr size_t kPoolSize = 1;
  MemoryPool<TestClass> pool(kPoolSize);

  auto ptr = pool.Allocate();
  ASSERT_NE(ptr, nullptr);

  pool.Free(ptr);
  // After freeing, we should be able to allocate again
  auto newPtr = pool.Allocate();
  ASSERT_NE(newPtr, nullptr);
}

// Test case for smart pointer allocation
TEST_F(MemoryPoolTest, SmartAllocation) {
  constexpr size_t kPoolSize = 10;
  MemoryPool<TestClass> pool(kPoolSize);

  auto smartPtr = pool.AllocateSmart();
  ASSERT_NE(smartPtr, nullptr);

  // Test if deleter is correctly associated
  auto& deleter = smartPtr.get_deleter();
  // We can't directly check the internal state, but its existence is a good
  // sign
}

// Test case for smart allocation when pool is full
TEST_F(MemoryPoolTest, FullSmartAllocation) {
  constexpr size_t kPoolSize = 3;
  MemoryPool<TestClass> pool(kPoolSize);

  for (size_t i = 0; i < kPoolSize; ++i) {
    (void)pool.Allocate();
  }

  // After filling the pool, the next smart allocation should return a null
  // unique_ptr
  auto smartPtr = pool.AllocateSmart();
  ASSERT_EQ(smartPtr, nullptr);
}

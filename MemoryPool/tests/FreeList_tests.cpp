#include <algorithm>
#include <atomic>
#include <memory>
#include <random>
#include <thread>
#include <vector>

#include "../FreeList.hpp"
#include "gtest/gtest.h"

struct alignas(alignof(uintptr_t)) TestNode {
  int value;
};

TEST(FreeListTest, EmptyListReturnsNullptr) {
  constexpr size_t kNumBlocks = 1;
  std::vector<std::byte> buffer(kNumBlocks * sizeof(TestNode));
  FreeList<TestNode> freeList(buffer.data(), kNumBlocks);

  TestNode* node = freeList.Pop();
  ASSERT_NE(node, nullptr);

  ASSERT_EQ(freeList.Pop(), nullptr);

  freeList.Push(node);

  TestNode* nextNode = freeList.Pop();
  ASSERT_EQ(nextNode, node);
}

TEST(FreeListTest, StressTest) {
  constexpr size_t kNumBlocks = 10000;
  std::vector<std::byte> buffer(kNumBlocks * sizeof(TestNode));
  FreeList<TestNode> freeList(buffer.data(), kNumBlocks);

  std::vector<TestNode*> allocatedNodes;
  for (size_t i = 0; i < kNumBlocks; ++i) {
    TestNode* node = freeList.Pop();
    ASSERT_NE(node, nullptr);
    allocatedNodes.push_back(node);
  }

  ASSERT_EQ(freeList.Pop(), nullptr);

  for (size_t i = 0; i < kNumBlocks; ++i) {
    freeList.Push(allocatedNodes[i]);
  }

  for (size_t i = 0; i < kNumBlocks; ++i) {
    ASSERT_NE(freeList.Pop(), nullptr);
  }
}

TEST(FreeListTest, PopAndPushSingleThread) {
  constexpr size_t kNumBlocks = 10;
  std::vector<std::byte> buffer(kNumBlocks * sizeof(TestNode));
  FreeList<TestNode> freeList(buffer.data(), kNumBlocks);

  std::vector<TestNode*> poppedNodes;
  for (size_t i = 0; i < kNumBlocks; ++i) {
    TestNode* node = freeList.Pop();
    ASSERT_NE(node, nullptr);
    poppedNodes.push_back(node);
  }

  ASSERT_EQ(freeList.Pop(), nullptr);

  for (size_t i = 0; i < kNumBlocks; ++i) {
    freeList.Push(poppedNodes[i]);
  }

  std::vector<TestNode*> rePoppedNodes;
  for (size_t i = 0; i < kNumBlocks; ++i) {
    TestNode* node = freeList.Pop();
    ASSERT_NE(node, nullptr);
    rePoppedNodes.push_back(node);
  }

  for (size_t i = 0; i < kNumBlocks; ++i) {
    ASSERT_EQ(rePoppedNodes[i], poppedNodes[kNumBlocks - 1 - i]);
  }
}

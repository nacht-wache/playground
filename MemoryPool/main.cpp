#include <array>
#include <iostream>

#include "MemoryPool.hpp"

namespace {
constexpr int kSize = 100;
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) {
  try {
    using Type = int64_t;
    MemoryPool<Type> pool{kSize};
    std::array<Type*, kSize> ptrs{nullptr};
    for (int i = 0; i < kSize; ++i) {
      ptrs[i] = pool.Allocate(i);
    }

    for (size_t i = 0; i < kSize; ++i) {
      std::cout << ptrs[i] << ' ' << *ptrs[i] << std::endl;
    }

    for (size_t i = 0; i < kSize; ++i) {
      pool.Free(ptrs[i]);
    }
  } catch (std::exception& e) {
    std::cout << e.what() << std::endl;
  }
}
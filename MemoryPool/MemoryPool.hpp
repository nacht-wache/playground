#pragma once
#include <cstddef>
#include <iostream>
#include <memory>
#include <vector>

#include "FreeList.hpp"

template <typename T, typename FreeList = FreeList<T>>
// :(
// todo fix
  requires(alignof(T) >= alignof(uintptr_t))
class MemoryPool {
  inline constinit static auto kAlignment = std::alignment_of_v<T>;
  inline constinit static auto kSize = sizeof(T);

 public:
  struct FreeBlockDeleter {
    MemoryPool& pool;

    void operator()(T* ptr) noexcept(false) {
      pool.Free(ptr);
    }
  };

 public:
  using Unique = std::unique_ptr<T, FreeBlockDeleter>;

 public:
  explicit MemoryPool(std::size_t size) noexcept
      : m_data(static_cast<std::byte*>(operator new[](
            size* kSize, std::align_val_t{kAlignment}))),
        m_freeList(m_data, size) {
  }

  ~MemoryPool() noexcept {
    // it's up to user to return all ptrs for destruction
    operator delete[](m_data, std::align_val_t{kAlignment});
  }

  // Non-copyable
  MemoryPool(MemoryPool const& other) = delete;

  MemoryPool& operator=(MemoryPool const& other) = delete;

  // Non-movable
  // Deleter holds reference to initial Pool
  MemoryPool(MemoryPool&& other) noexcept = delete;

  MemoryPool& operator=(MemoryPool&& other) noexcept = delete;

 public:
  // since C++17 we can overload based on noexcept specifier
  // strong exception-safety guarantee
  template <typename... U>
  [[nodiscard]] T* Allocate(U&&... args) noexcept(
      std::is_nothrow_constructible_v<T, U...>) {
    auto freeBlock = m_freeList.Pop();
    if (freeBlock == nullptr) {
      return nullptr;
    }

    // strong exception-safety guarantee
    try {
      new (freeBlock) T(std::forward<U>(args)...);
    } catch (std::exception& e) {
      m_freeList.Push(freeBlock);
      std::cout << e.what() << std::endl;
      return nullptr;
    } catch (...) {
      m_freeList.Push(freeBlock);
      return nullptr;
    }

    return freeBlock;
  }

  // since C++17 we can overload based on noexcept specifier
  template <typename... U>
    requires std::is_nothrow_constructible_v<T, U...>
  [[nodiscard]] T* Allocate(U&&... args) noexcept {
    auto freeBlock = m_freeList.Pop();
    if (freeBlock == nullptr) {
      return nullptr;
    }

    new (freeBlock) T(std::forward<U>(args)...);
    return freeBlock;
  }

  template <typename... U>
  [[nodiscard]] Unique AllocateSmart(U&&... args) noexcept(
      noexcept(Allocate(std::forward<U>(args)...))) {
    return CreateUnique(Allocate(std::forward<U>(args)...));
  }

  void Free(T* ptr) noexcept {
    std::destroy_at(ptr);
    m_freeList.Push(ptr);
  }

 private:
  [[nodiscard]] FreeBlockDeleter CreateDeleter() noexcept {
    return {.pool = *this};
  }

  [[nodiscard]] Unique CreateUnique(T* ptr) noexcept {
    return Unique{ptr, CreateDeleter()};
  }

 private:
  std::byte* m_data;
  FreeList m_freeList;
};

template <typename T, template <typename> typename FreeList>
using UniqueV = std::vector<typename MemoryPool<T, FreeList<T>>::Unique>;

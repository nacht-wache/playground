#pragma once
#include <cstddef>
#include <memory>
#include <stdexcept>
#include <vector>

template <typename T>
class MemoryPool {
  inline constinit static auto kAlignment = std::alignment_of_v<T>;

public:
  struct FreeBlockDeleter {
    MemoryPool& pool;

    void operator()(T* ptr) noexcept(false) {
      if (pool.m_freeBlock == 0)
        throw std::logic_error{"Call of FreeBlockDeleter to full Pool!"};

      std::destroy_at(ptr);
      --pool.m_freeBlock;
    }
  };

public:
  using Unique = std::unique_ptr<T, FreeBlockDeleter>;

public:
  explicit MemoryPool(std::size_t size) noexcept
    : m_data(static_cast<std::byte*>(operator new[](
          size * sizeof(T), std::align_val_t{kAlignment})))
      , m_size(size)
      , m_freeBlock(0) {
  }

  ~MemoryPool() noexcept {
    operator delete [](m_data, std::align_val_t{kAlignment});
  }

  // Non-copyable
  MemoryPool(MemoryPool const& other) = delete;

  MemoryPool& operator=(MemoryPool const& other) = delete;

  // Non-movable
  // Deleter holds reference to initial Pool
  MemoryPool(MemoryPool&& other) noexcept = delete;

  MemoryPool& operator=(MemoryPool&& other) noexcept = delete;

public:
  template <typename... U>
  [[nodiscard]] T* Allocate(U&&... args) {
    if (m_freeBlock == m_size)
      return nullptr;

    auto ptr = reinterpret_cast<T*>(m_data) + m_freeBlock;
    new(ptr) T(std::forward<U>(args)...);
    ++m_freeBlock;
    return ptr;
  }

  template <typename... U>
  [[nodiscard]] Unique allocateSmart(U&&... args) {
    if (m_freeBlock == m_size)
      return CreateUnique(nullptr);

    auto ptr = reinterpret_cast<T*>(m_data) + m_freeBlock;
    new(ptr) T(std::forward<U>(args)...);
    ++m_freeBlock;

    return this->CreateUnique(ptr);
  }

  void Free(T* ptr) noexcept(false) {
    if (!ptr)
      return;

    if (m_freeBlock == 0)
      throw std::logic_error{"Call of free to full Pool!"};

    std::destroy_at(ptr);
    --m_freeBlock;
  }

private:
  [[nodiscard]] FreeBlockDeleter CreateDeleter() noexcept {
    return {.pool = *this};
  }

  [[nodiscard]] Unique CreateUnique(T* ptr) {
    return Unique{ptr, CreateDeleter()};
  }

private:
  std::byte* m_data;
  size_t m_size;
  size_t m_freeBlock;
};

template <typename T>
using unique_v = std::vector<typename MemoryPool<T>::Unique>;

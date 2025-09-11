#pragma once
#include <cassert>
#include <new>

/// powered by https://github.com/google/filament/blob/main/libs/utils/include/utils/Allocator.h
template<typename Ptr>
static Ptr* AddPtr(Ptr* ptr, std::size_t b) noexcept {
  return reinterpret_cast<Ptr*>(reinterpret_cast<uintptr_t>(ptr) + static_cast<uintptr_t>(b));
}

template<typename Ptr>
static Ptr* Align(Ptr* ptr, size_t alignment) noexcept {
  assert((alignment & (alignment - 1)) == 0 && "Alignment must be a power of two");
  return reinterpret_cast<Ptr*>((reinterpret_cast<uintptr_t>(ptr) + alignment - 1) & ~(alignment - 1));
}

template<typename Ptr>
static Ptr* Align(Ptr* ptr, size_t alignment, size_t offset) noexcept {
  return Align(AddPtr(ptr, offset), alignment);
}

class FreeList {
public:
  FreeList() = delete;

  explicit FreeList(std::byte* begin, std::align_val_t alignment, std::size_t size, std::size_t space) noexcept {
    auto align = static_cast<size_t>(alignment);
    auto beg = Align(begin, align);
    // begins lifetime of Node
    m_head = new (beg) Node;
    auto cur = m_head;
    for (std::size_t i = 1; i < space; ++i) {
      // begins lifetime of Node
      auto next = new (Align(AddPtr(cur, size), align)) Node;
    // well-defined
      cur->next = next;
      cur = next;
    }
    cur->next = nullptr;
  }

  ~FreeList() = default;

  FreeList(FreeList const& other) = delete;
  FreeList& operator=(FreeList const& other) = delete;
  FreeList(FreeList&& other) noexcept = default;
  FreeList& operator=(FreeList&& other) noexcept = default;

public:
  template <typename T>
  T * Pop() noexcept {
    auto head = m_head;
    m_head = head ? head->next : nullptr;
    // let the user beware of lifetime
    return reinterpret_cast<T *>(head);
  }

  template <typename T>
  void Push(T * ptr) noexcept {
    // begins lifetime of Node
    auto head = new (ptr) Node;
    // well-defined
    head->next = m_head;
    m_head = head;
  }

public:
  struct Node {
    Node* next = nullptr;
  };

private:
  Node* m_head{nullptr};
};

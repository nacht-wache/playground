#pragma once
#include <cassert>
#include <memory>
#include <new>

template<typename Ptr>
static Ptr* AddPtr(Ptr* ptr, std::size_t b) noexcept {
  return reinterpret_cast<Ptr *>(reinterpret_cast<uintptr_t>(ptr) + static_cast<uintptr_t>(b));
}

template<typename Ptr>
static Ptr* Align(Ptr* ptr, size_t alignment) noexcept {
  return reinterpret_cast<Ptr *>((reinterpret_cast<uintptr_t>(ptr) + alignment - 1) & ~(alignment - 1));
}

template<typename Ptr>
static Ptr* Align(Ptr* ptr, size_t alignment, size_t offset) noexcept {
  return Align(AddPtr(ptr, offset), alignment);
}

class FreeList {
public:
  FreeList() = delete;

  explicit FreeList(std::byte* begin, std::align_val_t alignment, std::size_t size, std::size_t space) noexcept {
    assert(static_cast<std::size_t>(alignment) >= alignof(uintptr_t));
    auto align = static_cast<size_t>(alignment);
    auto beg = Align(begin, align);
    m_head = reinterpret_cast<Node *>(beg);
    auto cur = m_head;
    for (std::size_t i = 1; i < space; ++i) {
      auto next = Align(AddPtr(cur, size), align);
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
  T * Pop() {
    auto head = m_head;
    m_head = head ? head->next : nullptr;
    return reinterpret_cast<T *>(head);
  }

  template <typename T>
  void Push(T * ptr) {
    auto head = reinterpret_cast<Node *>(ptr);
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

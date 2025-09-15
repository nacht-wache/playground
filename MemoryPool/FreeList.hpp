#pragma once
#include "AlignUtils.hpp"

// LIFO
template <typename T>
requires (alignof(T) >= alignof(uintptr_t))
class FreeList {
  inline static constinit auto kAlignment = alignof(T);
  inline static constinit auto kSize = sizeof(T);

public:
  FreeList() = delete;

  explicit FreeList(std::byte* begin, std::size_t space) noexcept {
    auto beg = Align(begin, kAlignment);
    // begins lifetime of Node
    m_head = new (beg) Node;
    auto cur = m_head;
    for (std::size_t i = 1; i < space; ++i) {
      // begins lifetime of Node
      m_head = std::construct_at(Align(AddPtr(cur, kSize), kAlignment));
      // well-defined
      m_head->next = cur;
      cur = m_head;
    }
  }

  ~FreeList() = default;

  FreeList(FreeList const& other) = delete;
  FreeList& operator=(FreeList const& other) = delete;
  FreeList(FreeList&& other) noexcept = default;
  FreeList& operator=(FreeList&& other) noexcept = default;

public:
  T * Pop() noexcept {
    auto head = m_head;
    m_head = head ? head->next : nullptr;
    // let the user beware of lifetime
    return reinterpret_cast<T *>(head);
  }

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

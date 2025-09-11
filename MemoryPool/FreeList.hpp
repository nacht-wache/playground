#pragma once
#include <memory>
#include <new>

template <typename Ptr>
Ptr * AddPtr(Ptr * ptr, std::size_t value) {
  return reinterpret_cast<Ptr*>(reinterpret_cast<uintptr_t>(ptr) + static_cast<uintptr_t>(value));
}

template <typename Ptr>
Ptr * Align(Ptr * ptr, std::size_t align, std::size_t size, std::size_t & space) {
  auto v = static_cast<void *>(ptr);
  std::align(align, size, v, space);
  return static_cast<Ptr *>(v);
}

class FreeList {
public:
  FreeList() = delete;

  explicit FreeList(std::byte* begin, std::align_val_t alignment, std::size_t size, std::size_t space) noexcept {
    auto align = static_cast<size_t>(alignment);
    auto beg = reinterpret_cast<Node *>(Align(begin, align, space, space));
    m_head = static_cast<Node *>(beg);
    auto cur = m_head;
    while (space != 0) {
      auto next = Align(AddPtr(cur, size), align, size, space);
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

#include <cstdio>
#include <new>

#include "AlignedAlloc.hpp"

// no inline, required by [replacement.functions]/3
void* operator new(std::size_t sz) {
  std::printf("new(size_t), size = %zu\n", sz);
  if (sz == 0) {
    ++sz; // avoid std::malloc(0) which may return nullptr on success
  }

  if (void* ptr = std::malloc(sz)) {
    return ptr;
  }

  throw std::bad_alloc{}; // required by [new.delete.single]/3
}

void* operator new[](std::size_t sz) {
  std::printf("new[](size_t), size = %zu\n", sz);
  if (sz == 0) {
    ++sz;
  }

  if (void* ptr = std::malloc(sz)) {
    return ptr;
}

  throw std::bad_alloc{};
}

void* operator new[](std::size_t sz, std::align_val_t align) {
  std::printf("new[](size_t, std::align_val_t), size = %zu\n", sz);
  if (sz == 0) {
    ++sz;
}

  if (void* ptr = ALIGNED_ALLOC(static_cast<size_t>(align), sz)) {
    return ptr;
}

  throw std::bad_alloc{};
}

void operator delete[](void* ptr,
                       [[maybe_unused]] std::align_val_t align) noexcept {
  std::puts("delete[](void*, std::align_val_t)");
  ALIGNED_FREE(ptr);
}


void operator delete[](void* ptr) noexcept {
  std::puts("delete[](void*)");
  std::free(ptr);
}

void operator delete(void* ptr) noexcept {
  std::puts("delete(void*)");
  std::free(ptr);
}
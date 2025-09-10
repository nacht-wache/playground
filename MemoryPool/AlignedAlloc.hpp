#pragma once
#include <cstdlib>

#ifdef _MSC_VER
#include <malloc.h>

inline void* AlignedAllocWrapper(std::align_val_t alignment, size_t size) {
  return _aligned_malloc(size, alignment);
}

inline void AlignedFreeWrapper(void* ptr) {
  _aligned_free(ptr);
}

#define ALIGNED_ALLOC(alignment, size) AlignedAllocWrapper(alignment, size)
#define ALIGNED_FREE(ptr) AlignedFreeWrapper(ptr)

#elif __cplusplus >= 201703L
#define ALIGNED_ALLOC(alignment, size) std::aligned_alloc(alignment, size)
#define ALIGNED_FREE(ptr) std::free(ptr)

#else
#include <stdlib.h>
#define ALIGNED_ALLOC(alignment, size) aligned_alloc(alignment, size)
#define ALIGNED_FREE(ptr) free(ptr)

#endif // _MSC_VER
#pragma once
#include <cassert>
#include <cstdint>

/// powered by https://github.com/google/filament/blob/main/libs/utils/include/utils/Allocator.h
template<typename Ptr>
static Ptr* AddPtr(Ptr* ptr, std::size_t b) noexcept {
    return reinterpret_cast<Ptr*>(reinterpret_cast<uintptr_t>(ptr) + static_cast<uintptr_t>(b));
}

template<typename Ptr>
static Ptr* Align(Ptr* ptr, std::size_t alignment) noexcept {
    assert((alignment & (alignment - 1)) == 0 && "Alignment must be a power of two");
    return reinterpret_cast<Ptr*>((reinterpret_cast<uintptr_t>(ptr) + alignment - 1) & ~(alignment - 1));
}

template<typename Ptr>
static Ptr* Align(Ptr* ptr, std::size_t alignment, std::size_t offset) noexcept {
    return Align(AddPtr(ptr, offset), alignment);
}
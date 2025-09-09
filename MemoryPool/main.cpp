#include <array>

#include "MemoryPool.hpp"

#include <cstdlib>
#include <iostream>

// no inline, required by [replacement.functions]/3
void * operator new(std::size_t sz)
{
    std::printf("new(size_t), size = %zu\n", sz);
    if (sz == 0)
        ++sz; // avoid std::malloc(0) which may return nullptr on success
 
    if (void *ptr = std::malloc(sz))
        return ptr;
 
    throw std::bad_alloc{}; // required by [new.delete.single]/3
}

void * operator new[](std::size_t sz)
{
    std::printf("new[](size_t), size = %zu\n", sz);
    if (sz == 0)
        ++sz; // avoid std::malloc(0) which may return nullptr on success
 
    if (void *ptr = std::malloc(sz))
        return ptr;
 
    throw std::bad_alloc{}; // required by [new.delete.single]/3
}

void * operator new[](std::size_t sz, std::align_val_t align)
{
    std::printf("new[](size_t, std::align_val_t), size = %zu\n", sz);
    if (sz == 0)
        ++sz; // avoid std::malloc(0) which may return nullptr on success
 
    if (void *ptr = _aligned_malloc(sz,  static_cast<size_t>(align)))
        return ptr;
 
    throw std::bad_alloc{}; // required by [new.delete.single]/3
}

void operator delete[](void * ptr, std::align_val_t align) noexcept
{
	std::puts("delete[](void*, std::align_val_t)");
    _aligned_free(ptr);
}

void operator delete[](void * ptr) noexcept
{
	std::puts("delete[](void*)");
    std::free(ptr);
}

void operator delete(void * ptr) noexcept
{
    std::puts("delete(void*)");
    std::free(ptr);
}

namespace
{
	constexpr int kSize = 100;
}

int main(int argc, char * argv[])
{
    try
    {
	    MemoryPool<int> pool{kSize};
	    std::array<int *, kSize> ptrs;
	    for (int i = 0; i < kSize; ++i)
	        ptrs[i] = pool.Allocate(i);

	    for (size_t i = 0; i < kSize; ++i)
	        std::cout << ptrs[i] << ' ' << *ptrs[i] << std::endl;

	    for (size_t i = 0; i < kSize; ++i)
	        pool.Free(ptrs[i]);
	    
    } catch (std::exception & e)
    {
	    std::cout << e.what() << std::endl;
    }
}

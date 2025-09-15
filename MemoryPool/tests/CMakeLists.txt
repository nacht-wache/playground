add_executable(free_list_tests "../FreeList.hpp" FreeList_tests.cpp)
target_link_libraries(free_list_tests PRIVATE gtest_main)

add_executable(memory_pool_tests "../MemoryPool.hpp" MemoryPool_tests.cpp)
target_link_libraries(memory_pool_tests PRIVATE gtest_main)


include(GoogleTest)
gtest_discover_tests(
        free_list_tests
        memory_pool_tests
)
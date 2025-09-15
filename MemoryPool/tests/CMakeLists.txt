function(add_test name)
    target_link_libraries(${name} PRIVATE gtest_main)
    gtest_discover_tests(${name})
endfunction()

include(GoogleTest)
add_executable(free_list_tests "../FreeList.hpp" FreeList_tests.cpp)
add_test(free_list_tests)

add_executable(memory_pool_tests "../MemoryPool.hpp" MemoryPool_tests.cpp)
add_test(memory_pool_tests)

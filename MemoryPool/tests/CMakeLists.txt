file(GLOB test_sources "*.cpp")
add_executable(run_tests ${test_sources} "../MemoryPool.hpp")

target_link_libraries(run_tests
        PRIVATE
        gtest_main
)

include(GoogleTest)
gtest_discover_tests(
        run_tests
)
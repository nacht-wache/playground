set(target_name "MemoryPool")

add_executable(${target_name}
        "main.cpp"
        "MemoryPool.hpp"
        "AlignedAlloc.hpp"
        "OverrideNewDelete.cpp"
)

if (MSVC)
    target_compile_options(${target_name} PRIVATE /fsanitize=address)
    target_link_options(${target_name} PRIVATE /fsanitize=address)
else ()
    target_compile_options(${target_name} PRIVATE -fsanitize=address)
    target_link_options(${target_name} PRIVATE -fsanitize=address)
endif ()

include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.17.0
)

FetchContent_MakeAvailable(googletest)

file(GLOB test_sources "tests/*.cpp")
add_executable(run_tests ${test_sources} "MemoryPool.hpp")

target_link_libraries(run_tests
        PRIVATE
        gtest_main
)

include(GoogleTest)
gtest_discover_tests(
        run_tests
)

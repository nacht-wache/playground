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

add_subdirectory(tests)

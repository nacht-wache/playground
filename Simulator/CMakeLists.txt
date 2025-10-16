set(target_name "Simulator")

add_executable(${target_name}
        "main.cpp"
        "Simulator.cpp"
        "Simulator.hpp"
)

if (${CMAKE_BUILD_TYPE} STREQUAL Debug)
    target_compile_options(${target_name} PRIVATE -fsanitize=address)
    target_link_options(${target_name} PRIVATE -fsanitize=address)
endif ()

add_subdirectory(tests)

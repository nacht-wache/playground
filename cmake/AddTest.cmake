function(add_test name)
    target_link_libraries(${name} PRIVATE gtest_main)
    gtest_discover_tests(${name})

    if (${CMAKE_BUILD_TYPE} STREQUAL Debug)
        target_compile_options(${name} PRIVATE -fsanitize=address)
        target_link_options(${name} PRIVATE -fsanitize=address)
    endif ()
endfunction()
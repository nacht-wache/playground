include(GoogleTest)
add_executable(simulator_tests "../Simulator.hpp" "../Simulator.cpp" Simulator_tests.cpp)
add_test(simulator_tests)

# Enable testing
enable_testing()

# Add Boost.Test dependency
find_package(Boost REQUIRED COMPONENTS unit_test_framework)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Add the unit test executable
add_executable(FuzzySearcherTest FuzzySearcherTest.cpp)

# Link the unit test executable with Boost.Test
target_link_libraries(FuzzySearcherTest fzf Boost::unit_test_framework)

# Add the test to CTest
add_test(NAME FuzzySearcherTest COMMAND FuzzySearcherTest)

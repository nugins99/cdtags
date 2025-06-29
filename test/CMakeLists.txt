# Enable testing
include(GoogleTest)

find_package(GTest REQUIRED)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)


add_executable(ControllerTest ControllerTest.cpp FuzzySearcherTest.cpp)
target_link_libraries(ControllerTest GTest::gtest GTest::gtest_main fzf)
gtest_discover_tests(ControllerTest)   

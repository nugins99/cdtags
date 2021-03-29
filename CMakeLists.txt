cmake_minimum_required(VERSION 2.8)
project(cdtags)

set(Boost_USE_STATIC_LIBS ON)
set(CMAKE_CXX_STANDARD 11)
find_package(Boost REQUIRED COMPONENTS filesystem program_options )
add_subdirectory(src)

cmake_minimum_required(VERSION 3.16)
project(cdtags)

set(CMAKE_CXX_STANDARD 14)
find_package(Boost REQUIRED COMPONENTS filesystem program_options )

add_subdirectory(src)

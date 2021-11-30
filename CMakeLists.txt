cmake_minimum_required(VERSION 3.0..3.20)
project(cdtags)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set the package version.
# This applies to both the RPM package and # a "configure_file" command to replace values in
# src/Version.h.in"
SET(CPACK_PACKAGE_VERSION_MAJOR "1")
SET(CPACK_PACKAGE_VERSION_MINOR "1")
SET(CPACK_PACKAGE_VERSION_PATCH "1")

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_CXX_STANDARD 17)
endif()

# Boost
#if (STATIC)
    set(Boost_USE_STATIC_LIBS ON)
#endif()
set(Boost_INSTALL_DIR ${CONAN_BOOST_ROOT})
set(Boost_DEBUG ON)
message(STATUS "BOOST DIR: ${Boost_INSTALL_DIR}")
#find_package(Boost 1.77.0 REQUIRED COMPONENTS filesystem program_options)
find_package(Boost 1.77.0 REQUIRED COMPONENTS program_options)

# Source Dirs
add_subdirectory(src)

# Packaging
set(CPACK_SOURCE_GENERATOR "TGZ")
set(CPACK_SOURCE_IGNORE_FILES
  \\.git/
  build/
  ".*~$"
)
set(CPACK_VERBATIM_VARIABLES YES)
set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_SOURCE_DIR}/LICENSE.txt)
include(CPack)

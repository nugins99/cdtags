cmake_minimum_required(VERSION 3.0..3.20)
project(cdtags)
SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set the package version.
# This applies to both the RPM package and # a "configure_file" command to replace values in
# src/Version.h.in"
SET(CPACK_PACKAGE_VERSION_MAJOR "1")
SET(CPACK_PACKAGE_VERSION_MINOR "1")
SET(CPACK_PACKAGE_VERSION_PATCH "1")

#set(Boost_USE_STATIC_LIBS ON)
set(CMAKE_CXX_STANDARD 11)
find_package(Boost REQUIRED COMPONENTS filesystem program_options )
add_subdirectory(src)

set(CPACK_SOURCE_GENERATOR "TGZ")
set(CPACK_SOURCE_IGNORE_FILES
  \\.git/
  build/
  ".*~$"
)
set(CPACK_VERBATIM_VARIABLES YES)
set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_SOURCE_DIR}/LICENSE.txt)
include(CPack)

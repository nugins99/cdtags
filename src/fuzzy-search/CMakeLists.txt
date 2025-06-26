add_library(fzf FuzzySearcher.cpp FuzzySearcher.h TTY.cpp Application.h Application.cpp)
target_link_libraries(fzf Boost::program_options Boost::iostreams)
target_include_directories(fzf PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(fuzzy-search FuzzySearchApp.cpp )
target_link_libraries(fuzzy-search fzf Boost::program_options Boost::iostreams)
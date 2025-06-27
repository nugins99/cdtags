add_library(fzf
	Application.cpp
	FileReader.cpp
	StdinReader.cpp
	FuzzySearcher.cpp
	Reader.h
	TTY.h
	Application.h
	FileReader.h
	AsyncReader.h
	FuzzySearcher.h
	InputReaderFactory.h
	TTY.cpp)

target_link_libraries(fzf Boost::program_options Boost::iostreams)
target_include_directories(fzf PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(fuzzy-search main.cpp)
target_link_libraries(fuzzy-search fzf Boost::program_options Boost::iostreams)

install(TARGETS fuzzy-search)
install(FILES fuzzy-search-activate.sh DESTINATION share/cdtags)
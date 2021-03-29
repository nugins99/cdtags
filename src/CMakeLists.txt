
set(CMAKE_EXE_LINKER_FLAGS "-static-libstdc++")
add_executable(cdtags main.cpp Config.h Config.cpp Commands.h ChangeDirectory.cpp ChangeDirectory.h ListTags.cpp ListTags.h AddTag.cpp AddTag.h Complete.cpp Complete.h Remove.cpp Remove.h Debug.h Debug.cpp)
target_link_libraries(cdtags Boost::filesystem Boost::program_options)
install(TARGETS cdtags)
install(FILES cdtags-activate.sh DESTINATION share/cdtags)
configure_file("Version.h.in" "Version.h")
add_executable(cdtags
        main.cpp
        Config.h
        Config.cpp
        Commands.h
        ChangeDirectory.cpp
        ChangeDirectory.h
        ListTags.cpp
        ListTags.h
        AddTag.cpp
        AddTag.h
        Complete.cpp
        Complete.h
        Remove.cpp
        Remove.h
        Debug.h
        Debug.cpp)
target_link_libraries(cdtags Boost::program_options)

target_include_directories(cdtags PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

install(TARGETS cdtags)
install(FILES cdtags-activate.sh DESTINATION share/cdtags)

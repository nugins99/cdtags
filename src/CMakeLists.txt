if (STATIC)
    set(CMAKE_EXE_LINKER_FLAGS "-static-libstdc++")
endif()

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

if (CMAKE_COMPILER_IS_GNUCXX MATCHES 1)
    target_link_libraries(cdtags stdc++fs)
endif()

target_include_directories(cdtags PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_compile_definitions(cdtags PUBLIC _GLIBCXX_USE_CXX11_ABI=0)

install(TARGETS cdtags)
install(FILES cdtags-activate.sh DESTINATION share/cdtags)

/// @file TTY.h
/// @brief Provides a wrapper for /dev/tty input and output using Boost streams.

#ifndef TTY_H
#define TTY_H

#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/stream.hpp>
#include <memory>
#include <stdexcept>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h> // For terminal settings

namespace io = boost::iostreams;

class TTY {
public:
    TTY();
    ~TTY();

    TTY(const TTY&) = delete;  // Disable copy constructor
    TTY& operator=(const TTY&) = delete;  // Disable copy assignment    
    TTY(TTY&&) = delete;  // Disable move constructor
    TTY& operator=(TTY&&) = delete;  // Disable move assignment

    std::ostream& out();
    std::istream& in();

    constexpr static std::string_view reverse = "\033[7m";  // Enter alternate screen buffer
    constexpr static std::string_view normal = "\033[0m";  // Exit alternate screen buffer
    constexpr static std::string_view clearScreen = "\033[2J\033[H";  // Clear screen and move cursor to home position
    constexpr static std::string_view red = "\033[31m";  // Red text
    constexpr static std::string_view green = "\033[32m";  // Green text

    void clear()
    {
        out() << TTY::clearScreen;  // Clear screen
    }

private:
    int m_fd;
    std::unique_ptr<io::file_descriptor_source> m_tty_in;
    std::unique_ptr<io::file_descriptor_sink> m_tty_out;
    io::stream<io::file_descriptor_source> m_in;
    io::stream<io::file_descriptor_sink> m_out;

    termios original;
};

#endif // TTY_H
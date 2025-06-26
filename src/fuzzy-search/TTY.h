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
    //std::istream& in();
    char getch()
    {
        char c;
        if (read(m_fd, &c, 1) != 1) {
            throw std::runtime_error("Failed to read from TTY");
        }
        return c;
    }

    constexpr static std::string_view reverse = "\033[7m";  // Enter alternate screen buffer
    constexpr static std::string_view normal = "\033[0m";  // Exit alternate screen buffer
    constexpr static std::string_view clearScreen = "\033[2J\033[H";  // Clear screen and move cursor to home position
    constexpr static std::string_view red = "\033[31m";  // Red text
    constexpr static std::string_view green = "\033[32m";  // Green text
    constexpr static std::string_view yellow = "\033[33m";  // Yellow text
    constexpr static std::string_view blue = "\033[34m";  // Blue text
    constexpr static std::string_view magenta = "\033[35m";  // Magenta text
    constexpr static std::string_view cyan = "\033[36m";  // Cyan text
    constexpr static std::string_view white = "\033[37m";  // White text    
    constexpr static std::string_view gray = "\033[90m";  // Gray text
    constexpr static std::string_view bold = "\033[1m";  // Bold text       
    constexpr static std::string_view underline = "\033[4m";  // Underlined text            
    constexpr static std::string_view reset = "\033[0m";  // Reset all attributes

    void clear()
    {
        out() << TTY::clearScreen;  // Clear screen
    }

private:
    int m_fd;
    //std::unique_ptr<io::file_descriptor_source> m_tty_in;
    std::unique_ptr<io::file_descriptor_sink> m_tty_out;
    //io::stream<io::file_descriptor_source> m_in;
    io::stream<io::file_descriptor_sink> m_out;

    termios original;
};

#endif // TTY_H
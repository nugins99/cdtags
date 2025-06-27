/// @file TTY.h
/// @brief Provides a wrapper for /dev/tty input and output using Boost streams.

#ifndef TTY_H
#define TTY_H

#include <fcntl.h>
#include <termios.h>  // For terminal settings
#include <unistd.h>

#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/stream.hpp>
#include <memory>
#include <stdexcept>
#include <unordered_set>

namespace io = boost::iostreams;

/// @brief A class that provides a simple interface for reading from and writing
/// to the terminal (TTY).  It opens /dev/tty for reading and writing, allowing
/// for character input and output with ANSI escape codes for formatting.  This
/// class is designed to be used in terminal applications where direct
/// interaction with the terminal is required.  It provides methods for reading
/// single characters, writing formatted output, and clearing the terminal
/// screen.  The class also includes ANSI escape codes for text formatting, such
/// as colors and styles.  The TTY class manages terminal settings and restores
/// them upon destruction to ensure the terminal remains in a usable state.
class TTY
{
   public:
    /// @brief Construct a new TTY object and open /dev/tty for I/O.
    TTY();
    /// @brief Destructor. Restores terminal settings and closes file descriptors.
    ~TTY();

    TTY(const TTY&) = delete;             ///< Disable copy constructor
    TTY& operator=(const TTY&) = delete;  ///< Disable copy assignment
    TTY(TTY&&) = delete;                  ///< Disable move constructor
    TTY& operator=(TTY&&) = delete;       ///< Disable move assignment

    /// @brief Get the output stream for writing to the terminal.
    /// @return std::ostream& Reference to the output stream.
    std::ostream& out();
    // std::istream& in();

    /// @brief Read a single character from the terminal (blocking).
    /// @throws std::runtime_error if reading fails.
    /// @return char The character read.
    char getch()
    {
        char c;
        if (read(m_fd, &c, 1) != 1)
        {
            throw std::runtime_error("Failed to read from TTY");
        }
        return c;
    }

    /// @name ANSI Escape Codes
    ///@{
    constexpr static std::string_view reverse = "\033[7m";  ///< Reverse video (highlight)
    constexpr static std::string_view normal = "\033[0m";   ///< Reset all attributes
    constexpr static std::string_view clearScreen =
        "\033[2J\033[H";  ///< Clear screen and move cursor to home position
    constexpr static std::string_view red = "\033[31m";       ///< Red text
    constexpr static std::string_view green = "\033[32m";     ///< Green text
    constexpr static std::string_view yellow = "\033[33m";    ///< Yellow text
    constexpr static std::string_view blue = "\033[34m";      ///< Blue text
    constexpr static std::string_view magenta = "\033[35m";   ///< Magenta text
    constexpr static std::string_view cyan = "\033[36m";      ///< Cyan text
    constexpr static std::string_view white = "\033[37m";     ///< White text
    constexpr static std::string_view gray = "\033[90m";      ///< Gray text
    constexpr static std::string_view bold = "\033[1m";       ///< Bold text
    constexpr static std::string_view underline = "\033[4m";  ///< Underlined text
    constexpr static std::string_view reset = "\033[0m";      ///< Reset all attributes
    ///@}

    /// @brief Clear the terminal screen.
    void clear()
    {
        out() << TTY::clearScreen;  // Clear screen
    }

    /// @brief Return a copy of text with any character in chars_to_bold wrapped in ANSI bold codes.
    /// @param text The string to process.
    /// @param chars_to_bold The set of characters to bold.
    /// @return std::string The processed string with bolded characters.
    static std::string boldMatching(const std::string& text, const std::string& chars_to_bold)
    {
        std::unordered_set<char> bold_chars(chars_to_bold.begin(), chars_to_bold.end());
        std::string result;
        for (char c : text)
        {
            if (bold_chars.count(c))
            {
                result += TTY::bold;
                result += c;
                result += TTY::normal;
            }
            else
            {
                result += c;
            }
        }
        return result;
    }

   private:
    int m_fd;  ///< File descriptor for /dev/tty
    // std::unique_ptr<io::file_descriptor_source> m_tty_in;
    std::unique_ptr<io::file_descriptor_sink> m_tty_out;
    // io::stream<io::file_descriptor_source> m_in;
    io::stream<io::file_descriptor_sink> m_out;

    termios original;  ///< Original terminal settings
};

#endif  // TTY_H
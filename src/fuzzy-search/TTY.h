/// @file TTY.h
/// @brief Provides a wrapper for /dev/tty input and output using Boost streams.

#ifndef TTY_H
#define TTY_H

#include <common/AnsiCodes.h>
#include <fcntl.h>
#include <termios.h>  // For terminal settings
#include <unistd.h>

#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/stream.hpp>
#include <memory>
#include <stdexcept>
#include <unordered_set>
#include "InputInterface.h"

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
class TTY : public fzf::InputInterface
{
   public:
    /// @brief Construct a new TTY object and open /dev/tty for I/O.
    TTY();
    /// @brief Destructor. Restores terminal settings and closes file descriptors.
    ~TTY() override;

    TTY(const TTY&) = delete;             ///< Disable copy constructor
    TTY& operator=(const TTY&) = delete;  ///< Disable copy assignment
    TTY(TTY&&) = delete;                  ///< Disable move constructor
    TTY& operator=(TTY&&) = delete;       ///< Disable move assignment

    /// @brief Get the output stream for writing to the terminal.
    /// @return std::ostream& Reference to the output stream.
    std::ostream& out();

    

    /// @brief Get next event
    virtual fzf::InputEvent getNextEvent() override; 

    /// @brief Clear the terminal screen.
    void clear()
    {
        out() << ansi::cursor::clearScreen;  // Clear screen
    }

    /// @brief Return a copy of text with any character in chars_to_bold wrapped in ANSI bold codes.
    /// @param text The string to process.
    /// @param chars_to_bold The set of characters to bold.
    /// @return std::string The processed string with bolded characters.
    static std::string boldMatching(const std::string& text, const std::string& chars_to_bold);

    /// Write results to the output.
    /// @param results The results to write.
    void writeResults(const fzf::Results& results) override;

    virtual void updateProgress(size_t count)  override;

   private:

    /// @brief Read a single character from the terminal (blocking).
    /// @throws std::runtime_error if reading fails.
    /// @return char The character read.
    char getch();

    int m_fd;  ///< File descriptor for /dev/tty
    // std::unique_ptr<io::file_descriptor_source> m_tty_in;
    std::unique_ptr<io::file_descriptor_sink> m_tty_out;
    // io::stream<io::file_descriptor_source> m_in;
    io::stream<io::file_descriptor_sink> m_out;

    termios original;  ///< Original terminal settings
    std::string m_searchString;
};

#endif  // TTY_H
/// @file TTY.cpp
/// @brief Implementation of the TTY class.

#include "TTY.h"

// Set terminal to raw mode to read keypresses
void set_raw_mode(int fd, termios& original)
{
    termios raw;
    tcgetattr(fd, &original);
    raw = original;
    raw.c_lflag &= ~(ECHO | ICANON);  // Disable echo and canonical mode
    tcsetattr(fd, TCSAFLUSH, &raw);
}

// Restore original terminal settings
void restore_terminal(int fd, const termios& original) { tcsetattr(fd, TCSAFLUSH, &original); }

/// @brief Saves the current console contents and switches to the alternate
/// screen buffer.
void saveConsoleContents(TTY& tty)
{
    tty.out() << "\033[?1049h";  // Switch to alternate screen buffer
    tty.out().flush();
}

/// @brief Restores the original console contents and switches back to the main
/// screen buffer.
void restoreConsoleContents(TTY& tty)
{
    tty.out() << "\033[?1049l";  // Switch back to main screen buffer
    tty.out().flush();
}

TTY::TTY() : m_fd(open("/dev/tty", O_RDWR))
{
    if (m_fd < 0)
    {
        throw std::runtime_error("Failed to open /dev/tty");
    }

    // m_tty_in = std::make_unique<io::file_descriptor_source>(m_fd, io::never_close_handle);
    m_tty_out = std::make_unique<io::file_descriptor_sink>(m_fd, io::never_close_handle);

    // m_in.open(*m_tty_in);
    m_out.open(*m_tty_out);

    saveConsoleContents(*this);
    set_raw_mode(m_fd, original);
}

TTY::~TTY()
{
    restoreConsoleContents(*this);

    // Restore terminal settings
    restore_terminal(m_fd, original);
}

std::ostream& TTY::out() { return m_out; }

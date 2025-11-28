/// @file TTY.cpp
/// @brief Implementation of the TTY class.

#include "TTY.h"

#include <unordered_set>

#include "common/AnsiCodes.h"

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

std::string TTY::boldMatching(const std::string& text, const std::string& chars_to_bold)
{
    std::unordered_set<char> bold_chars(chars_to_bold.begin(), chars_to_bold.end());
    std::string result;
    for (char c : text)
    {
        if (bold_chars.count(c))
        {
            result += ansi::text::bold;
            result += ansi::fg::yellow;
            result += c;
            result += ansi::reset::fg;
            result += ansi::reset::bold;
        }
        else
        {
            result += c;
        }
    }
    return result;
}

void TTY::writeResults(const fzf::Results& results)
{
    clear();
    out() << std::format("{} - {} of {} results\n", results.resultRange.first,
                         results.resultRange.second, results.results.size());
    out() << "Search String: " << results.searchString << "\n";
    out() << "Total Results: " << results.totalResults << "\n";
    for (const auto& result : results.results)
    {
        // Prefix each line with its index in green
        out() << ansi::fg::green << result.index << ansi::text::normal;
        if (result.selected)
        {
            // Highlight selected option with reverse video
            out() << ansi::text::inverse;
            out() << "> " << TTY::boldMatching(result.line, results.searchString);
        }
        else
        {
            out() << "  " << TTY::boldMatching(result.line, results.searchString);
        }
        // Append score in gray
        out() << std::format(" {}({}){}\n", ansi::fg::gray, result.score, ansi::text::normal);
    }
    updateProgress(results.results.size());
    // Display current search string
    out() << ansi::fg::red << "> " << ansi::text::normal << results.searchString;
    out().flush();
}

void TTY::updateProgress(size_t count)
{
    static constexpr std::string_view spinner = "|/-\\";
    const auto spinnerValue = spinner[count % spinner.size()];
    out() << ansi::fg::yellow << std::format("{} Updating...", spinnerValue) << ansi::text::normal
          << "\n";
}

char TTY::getch() {
    char c;
    if (read(m_fd, &c, 1) != 1)
    {
        throw std::runtime_error("Failed to read from TTY");
    }
    return c;
}

fzf::InputEvent TTY::getNextEvent()
{
    fzf::InputEvent event;
    // Read user input and process it
    char c = getch();

    if (c == '\033')  // Escape sequence
    {
        // Handle escape sequences for special keys
        c = getch();
        if (c == '[')
        {
            c = getch();
            switch (c)
            {
                case 'A':
                    event.type = fzf::InputType::UpArrow;
                    break;  // Up arrow
                case 'B':
                    event.type = fzf::InputType::DownArrow;
                    break;  // Down arrow
                default:    // All other escape sequences
                    event.type = fzf::InputType::Unknown;
            }
        }
    }
    else if (c == '\n' || c == '\r')
    {
        event.type = fzf::InputType::Newline;
    }
    else if (c == 127)  // Backspace
    {
        m_searchString.pop_back();
        event.type = fzf::InputType::Backspace;
        event.searchString = m_searchString;
    }
    else if (isprint(c))
    {
        event.type = fzf::InputType::PrintableChar;
        event.character = c;
    }
    return event;
}

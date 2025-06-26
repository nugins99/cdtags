#include "Application.h"

#include <algorithm>
#include <cassert>
#include <format>
#include <ranges>

#include "FuzzySearcher.h"

Application::Application(std::string& searchString, fzf::Reader::Ptr& inputReader,
                         std::size_t numResults)
    : m_searchString(searchString),
      m_inputReader(inputReader),
      m_numResults(numResults),
      log("fzf_search.log")
{
    m_connection = m_inputReader->onUpdate.connect(std::bind_front(&Application::onUpdate, this));
}

void Application::run()
{
    m_inputReader->start();  // Start the input reader

    while (processInput() == FuzzySearchResult::Continue)
    {
        updateDisplay();
    }
}

std::string Application::result() const
{
    if (m_selectedIndex == -1)
    {
        return m_results[0].first;  // Return first result if no selection
    }
    return m_selectedLine;  // Return empty string if no selection
}

void Application::updateSpinner(size_t count)
{
    static const std::string spinner = "|/-\\";
    static size_t index = 0;
    m_tty.out() << TTY::green
                << std::format("({}) {} Updating... {}", count, spinner[count % spinner.size()],
                               m_inputReader->data().back())
                << TTY::normal << "\n";
}

void Application::onUpdate(fzf::Reader::ReadStatus status, const std::string& line)
{
    log << "Input Reader Updated: "
        << (status == fzf::Reader::ReadStatus::EndOfFile ? "End of File" : "Continue") << std::endl;
    if (!line.empty())
    {
        performIncrementalSearch(line);
    }
    updateDisplay();
}

void Application::onUpArrow()
{
    log << "UP ARROW PRESSED" << std::endl;
    if (m_selectedIndex == -1)
    {
        m_selectedIndex = 0;  // Initialize selected index if not set
        return;
    }

    if (m_selectedIndex > 0)
    {
        --m_selectedIndex;  // Move up in the results
    }
}

void Application::onDownArrow()
{
    log << "DOWN ARROW PRESSED" << std::endl;
    if (m_selectedIndex == -1)
    {
        m_selectedIndex = 0;  // Initialize selected index if not set
        return;
    }
    if (m_selectedIndex < m_results.size() - 1)
    {
        ++m_selectedIndex;
    }
}

void Application::onBackspace()
{
    log << "BACKSPACE PRESSED" << std::endl;
    if (!m_searchString.empty())
    {
        m_searchString.pop_back();  // Remove last character
        performFuzzySearch();       // Update options
    }
}

void Application::onPrintableChar(char c)
{
    log << "PRINTABLE CHARACTER PRESSED: " << c << std::endl;
    if (isprint(c))
    {
        m_searchString += c;   // Append character to search string
        performFuzzySearch();  // Update options
    }
}

Application::FuzzySearchResult Application::processInput()
{
    // Read user input
    char c;
    c = m_tty.getch();

    if (c == '\033')
    {                       // Escape sequence for arrow keys
        c = m_tty.getch();  // Skip '['
        c = m_tty.getch();  // Get actual arrow key
        if (c == 'A')
        {  // Up arrow
            onUpArrow();
        }
        else if (c == 'B')
        {  // Down arrow
            onDownArrow();
        }
    }
    else if (c == '\n' || c == '\r')
    {  // Enter key (CR or LF)
        log << "ENTER PRESSED" << std::endl;
        return FuzzySearchResult::Select;  // Return selected option
    }
    else if (c == '\b' || c == 127)
    {  // Backspace key (127 for macOS)
        onBackspace();
    }
    else if (isprint(c))
    {                        // Printable characters
        onPrintableChar(c);  // Append character to search string
    }
    else
    {
        log << "UNSUPPORTED CHARACTER PRESSED: " << int(c) << std::endl;
    }
    return FuzzySearchResult::Continue;  // Continue searching
}

void Application::updateDisplay()
{
    std::scoped_lock lock(m_mutex);
    m_tty.out() << "PID: " << getpid() << "\n";
    log << "PID: " << getpid() << "\n";
    log << "Updating display with search string: " << m_searchString << std::endl;
    log << "Number of options: " << m_results.size() << " Selected: " << m_selectedIndex
        << std::endl;
    // Clear screen and display options
    m_tty.clear();

    assert(m_tty.out().good());
    m_tty.out() << std::format("{}{}/{}{} --------------------------------\n", TTY::green,
                               m_results.size(), m_numResults, TTY::normal);
    int localSelectedIndex = m_selectedIndex;  // Local copy for thread safety
    if (m_selectedIndex == -1)
    {
        localSelectedIndex = 0;  // Initialize selected index if not set
    }

    // Find last index with non-zero score
    for (size_t i = m_results.size(); i > 0; --i)
    {
        if (m_results[i - 1].second > 0)
        {
            localSelectedIndex = std::min(localSelectedIndex, int(i - 1));
            break;  // Found a valid index
        }
    }

    auto firstZeroEntry = std::find_if(m_results.begin(), m_results.end(),
                                       [](const auto& result) { return result.second <= 0; });
    int lastEntryIndex = std::distance(m_results.begin(), firstZeroEntry);

    size_t start = std::max(0, localSelectedIndex - (m_numResults / 2));
    size_t stop = std::min(lastEntryIndex, int(start + m_numResults));
    m_tty.out() << std::format("{} - {} of {} ({}) results\n", start, stop, m_results.size(),
                               lastEntryIndex);

    for (size_t i = start; i < stop; ++i)
    {
        if (m_results[i].second <= 0)
        {
            log << "Skipping result with non-positive score: " << m_results[i].first << std::endl;
            break;
        }
        m_tty.out() << TTY::green << i << TTY::normal;
        if (i == localSelectedIndex)
        {
            // Highlight selected option with reverse video
            m_tty.out() << TTY::reverse;
            m_tty.out() << "> " << m_results[i].first;
            m_tty.out() << std::format(" {}({}){}\n", TTY::gray, m_results[i].second, TTY::normal);
            m_selectedLine = m_results[i].first;  // Update selected line
        }
        else
        {
            m_tty.out() << "  " << m_results[i].first;
            m_tty.out() << std::format(" {}({}){}\n", TTY::gray, m_results[i].second, TTY::normal);
        }
    }
    if (m_inputReader->status() == fzf::Reader::ReadStatus::Continue)
    {
        updateSpinner(m_inputReader->data().size());
    }
    // Display current search string
    m_tty.out() << TTY::red << "> " << TTY::green << m_searchString << TTY::normal;
    m_tty.out().flush();
}

void Application::performFuzzySearch()
{
    // Get lines from input reader;
    std::vector<std::string> lines = m_inputReader->data();
    log << "Performing fuzzy search with search string: " << m_searchString << std::endl;
    log << "Number of lines: " << lines.size() << std::endl;

    m_results.clear();    // Clear previous results
    m_seenLines.clear();  // Clear seen lines
    for (const auto& line : lines)
    {
        if (m_seenLines.contains(line))
        {
            log << "Skipping seen line: " << line << std::endl;
            continue;  // Skip already seen lines
        }
        int distance = fzf::smithWaterman(m_searchString,
                                          line);  // Using Smith-Waterman for similarity

        log << "Score: " << distance << " Line: " << line << std::endl;
        m_results.emplace_back(line, distance);
        m_seenLines.insert(line);  // Mark line as seen
    }

    // Sort results based on the distance
    std::sort(m_results.begin(), m_results.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    updateSelectedLineIndex();  // Update selected line index
}

void Application::performIncrementalSearch(const std::string& line)
{
    assert(!line.empty());
    log << "Performing incremental search with line: " << line << std::endl;
    if (m_seenLines.contains(line))
    {
        return;  // Skip already seen lines
    }
    int distance = fzf::smithWaterman(m_searchString,
                                      line);  // Using Smith-Waterman for similarity
    m_results.emplace_back(line, distance);
    m_seenLines.insert(line);  // Mark line as seen

    // Sort results based on the distance
    std::sort(m_results.begin(), m_results.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });

    updateSelectedLineIndex();  // Update selected line index
}

void Application::updateSelectedLineIndex()
{
    if (m_selectedIndex == -1)
    {
        return;  // No selection yet
    }
    auto it = std::find_if(m_results.begin(), m_results.end(),
                           [this](const auto& result) { return result.first == m_selectedLine; });

    m_selectedIndex = (it != m_results.end()) ? std::distance(m_results.begin(), it) : 0;
    log << "Updating selected line index: " << m_selectedIndex << std::endl;
}

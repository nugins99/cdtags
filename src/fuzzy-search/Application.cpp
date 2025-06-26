#include "Application.h"
#include <cassert>
#include <algorithm>
#include <format>
#include <ranges>
#include <unordered_set>
#include "FuzzySearcher.h"

Application::Application(std::string& searchString, fzf::Reader::Ptr& inputReader, std::size_t numResults)
    : m_searchString(searchString),
      m_inputReader(inputReader),
      m_numResults(numResults),
      log("fzf_search.log")
{
    m_connection =
        m_inputReader->onUpdate.connect(std::bind_front(&Application::onUpdate, this));
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
    if (m_selectedIndex >= 0 && m_selectedIndex < m_options.size())
    {
        return m_options[m_selectedIndex];  // Return the selected option
    }
    return "";  // Return empty string if no selection
}

void Application::updateSpinner(size_t count)
{
    static const std::string spinner = "|/-\\";
    static size_t index = 0;
    m_tty.out() << TTY::green
                << std::format("({}) {} Updating...", count,
                               spinner[count / 100 % spinner.size()])
                << TTY::normal << "\n";
}

void Application::onUpdate(fzf::Reader::ReadStatus status)
{
    log << "Input Reader Updated: "
        << (status == fzf::Reader::ReadStatus::EndOfFile ? "End of File" : "Continue")
        << std::endl;
    log << "New string: " << m_inputReader->data().back() << std::endl;
    updateDisplay();
}

void Application::onUpArrow()
{
    log << "UP ARROW PRESSED" << std::endl;
    if (m_selectedIndex > 0)
    {
        --m_selectedIndex;
    }
}

void Application::onDownArrow()
{
    log << "DOWN ARROW PRESSED" << std::endl;
    if (m_selectedIndex < m_options.size() - 1)
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
    assert(m_tty.in().good());
    // Read user input
    char c;
    m_tty.in().get(c);

    if (c == '\033')
    {                       // Escape sequence for arrow keys
        m_tty.in().get(c);  // Skip '['
        m_tty.in().get(c);  // Get actual arrow key
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
    log << "Number of options: " << m_options.size() << " Selected: " << m_selectedIndex
        << std::endl;
    // Clear screen and display options
    m_tty.clear();
    m_tty.out() << "> " << m_inputReader->data().back() << "\n";
    updateSpinner(m_inputReader->data().size());
    assert(m_tty.out().good());
    m_tty.out() << std::format("{}{}{} --------------------------------\n", TTY::green,
                               m_options.size(), TTY::normal);

    for (size_t i = 0; i < std::min(m_options.size(), m_numResults); ++i)
    {
        m_tty.out() << TTY::green << i <<  TTY::normal;
        if (i == m_selectedIndex)
        {
            // Highlight selected option with reverse video
            m_tty.out() << TTY::reverse;
            m_tty.out() << "> " << m_options[i]; 
            m_tty.out() << TTY::normal << "\n";  
        }
        else
        {
            m_tty.out() << "  " << m_options[i] << "\n";
        }
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

    // Vector to store pairs of line and its Levenshtein distance
    std::vector<std::pair<std::string, int>> results;

    // Set to track unique lines
    std::unordered_set<std::string> seenLines;

    for (const auto& line : lines)
    {
        if (seenLines.find(line) == seenLines.end())
        {  // Check if line is unique
            int distance = fzf::smithWaterman(m_searchString,
                                              line);  // Using Smith-Waterman for similarity
            results.emplace_back(line, distance);
            seenLines.insert(line);  // Mark line as seen
        }
    }

    // Sort results based on the distance
    std::sort(results.begin(), results.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });

    // Return the top results as a vector of strings
    m_selectedIndex = 0;  // Reset selection when options are updated
    m_options.clear();
    auto nonzeroResults = [](const auto& pair) { return pair.second > 0; };
    for (auto& [line, i] : std::views::filter(results, nonzeroResults))
    {
        m_options.push_back(line);
    }
}
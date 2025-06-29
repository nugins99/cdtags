#include "Application.h"

#include <common/AnsiCodes.h>

#include <algorithm>
#include <cassert>
#include <format>
#include <iostream>
#include <ranges>

#include "FuzzySearcher.h"

Application::Application(std::string& searchString, fzf::Reader::Ptr& inputReader, TTY & tty,
                         std::size_t numResults)
    : m_tty(tty), m_searchString(searchString), m_inputReader(inputReader), m_numResults(numResults)
{
    m_connection = m_inputReader->onUpdate.connect(std::bind_front(&Application::onUpdate, this));
}
Application::~Application()
{
    m_inputReader->stop();  // Ensure input reader is stopped
}

std::string_view Application::result() const 
{
    if (m_selectedIndex == -1)
    {
        return m_results[0].first;  // Return first result if no selection
    }
    return m_selectedLine;  // Return empty string if no selection
}

void Application::updateSpinner(size_t count, std::string_view value)
{
    static constexpr std::string_view spinner = "|/-\\";
    auto spinnerValue = spinner[count % spinner.size()];
    std::scoped_lock lock(m_linesMutex);
    if (m_lines.empty())
    {
        m_tty.out() << ansi::fg::yellow << std::format("{} No results found", spinnerValue)
                    << ansi::text::normal << "\n";
        return;
    }
    m_tty.out() << ansi::fg::yellow << std::format("{} Updating... {}", spinnerValue, value)
                << ansi::text::normal << "\n";
}

void Application::onUpdate(fzf::Reader::ReadStatus status, const std::string& line)
{
    if (status != fzf::Reader::ReadStatus::EndOfFile)
    {
        std::scoped_lock lock(m_linesMutex);
        if (line.empty() || m_lines.contains(line))
        {
            // Skip empty lines or lines already seen, do not add
            return;
        }
        m_lines.insert(line);  // Store the line read from input
        performIncrementalSearch(line);
    }
    updateDisplay();
}

void Application::updateDisplay()
{
    std::scoped_lock lock(m_displayMutex);
    // Clear screen and display options
    m_tty.clear();
    std::cerr << "Updating display with search string: " << m_searchString << std::endl;

    int localSelectedIndex = m_selectedIndex;  // Local copy for thread safety
    if (m_selectedIndex == -1)
    {
        localSelectedIndex = 0;  // Initialize selected index if not set
    }
    std::cerr << "\tLocal selected index: " << localSelectedIndex << std::endl;

    // Find last index with non-zero score
    for (size_t i = m_results.size(); i > 0; --i)
    {
        if (m_results[i - 1].second > 0)
        {
            localSelectedIndex = std::min(localSelectedIndex, int(i - 1));
            break;  // Found a valid index
        }
    }
    std::cerr << "\tAdjusted local selected index: " << localSelectedIndex << std::endl;

    // Determine the range of results to display
    // Find the first entry with a score of zero or less
    // This is the last possible entry that should be displayed
    auto firstZeroEntry = std::find_if(m_results.begin(), m_results.end(),
                                       [](const auto& result) { return result.second <= 0; });
    int lastEntryIndex = std::distance(m_results.begin(), firstZeroEntry);
    std::cerr << "\tLast entry index: " << lastEntryIndex << std::endl;

    // Start index is the middle of the results list, adjusted for the number of results to display
    size_t start = std::max(0, localSelectedIndex - (m_numResults / 2));
    // Stop index is the minimum of the last entry index and the start index plus the number of
    // results
    size_t stop = std::min(lastEntryIndex, int(start + m_numResults));
    m_tty.out() << "Showing num lines: " << m_numResults << "\n";
    m_tty.out() << std::format("{} - {} of {} results\n", start, stop, lastEntryIndex);

    for (size_t i = start; i < stop; ++i)
    {
        std::cerr << "\tProcessing result: " << i << " - " << m_results[i].first
                  << " with score: " << m_results[i].second << std::endl;
        // Check if the score is zero or less
        if (m_results[i].second <= 0)
        {
            // Skip entries with zero or negative score
            // Since this list is sorted, we can break early
            // break;
        }
        m_tty.out() << ansi::fg::green << i << ansi::text::normal;
        if (static_cast<int>(i) == localSelectedIndex)
        {
            // Highlight selected option with reverse video
            m_tty.out() << ansi::text::inverse;
            m_tty.out() << "> " << TTY::boldMatching(m_results[i].first, m_searchString);
            m_tty.out() << std::format(" {}({}){}\n", ansi::fg::gray, m_results[i].second,
                                       ansi::text::normal);
            m_selectedLine = m_results[i].first;  // Update selected line
        }
        else
        {
            m_tty.out() << "  " << TTY::boldMatching(m_results[i].first, m_searchString);
            m_tty.out() << std::format(" {}({}){}\n", ansi::fg::gray, m_results[i].second,
                                       ansi::text::normal);
        }
    }
    if (m_inputReader->status() == fzf::Reader::ReadStatus::Continue)
    {
        updateSpinner(m_lines.size(), "");
    }
    // Display current search string
    m_tty.out() << ansi::fg::red << "> " << ansi::text::normal << m_searchString;
    m_tty.out().flush();
}

bool resultCompare(const std::pair<std::string, int>& a, const std::pair<std::string, int>& b)
{
    if (a.second == b.second)
    {
        return b.first.size() >
               a.first.size();  // Compare based on the first element (line) if scores are equal
    }
    return b.second < a.second;  // Compare based on the second element (score)
}

void Application::performFuzzySearch()
{
    std::scoped_lock lock(m_searchMutex);
    // Get lines from input reader;
    {
        std::scoped_lock lock(m_linesMutex);
        m_lines = m_inputReader->data();

        m_results.clear();  // Clear previous results
        for (const auto& line : m_lines)
        {
            int distance = fzf::score(m_searchString, line);
            m_results.emplace_back(line, distance);
        }
    }

    // Sort results based on the distance
    std::ranges::sort(m_results, resultCompare);
    updateSelectedLineIndex();  // Update selected line index
}

void Application::performIncrementalSearch(const std::string& line)
{
    std::scoped_lock lock(m_searchMutex);
    assert(!line.empty());
    int distance = fzf::score(m_searchString, line);
    // Insert & Sort results based on the distance
    // TODO - this is not efficient, we should insert in the right place as
    // results are already sorted.
    m_results.push_back({line, distance});
    std::ranges::sort(m_results, resultCompare);
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
    std::cerr << "Updating selected line index: " << m_selectedIndex << std::endl;
}

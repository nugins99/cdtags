#include "Application.h"

#include <algorithm>
#include <cassert>
#include <ranges>

#include "FuzzySearcher.h"

Application::Application(std::string& searchString, fzf::Reader::Ptr& inputReader, fzf::InputInterface& tty,
                         std::size_t numResults)
    : m_tty(tty), m_searchString(searchString), m_inputReader(inputReader), m_numResults(numResults)
{
    m_inputReader->onUpdate = std::bind_front(&Application::onUpdate, this);
}

Application::~Application()
{
    m_inputReader->disconnect();
    m_inputReader->stop();  // Ensure input reader is stopped
}

const std::string & Application::result() const
{
    if (m_selectedIndex == -1)
    {
        return m_results[0].first;  // Return first result if no selection
    }
    return m_selectedLine;  // Return empty string if no selection
}

void Application::updateSpinner(size_t count) { m_tty.updateProgress(count); }

void Application::onUpdate(fzf::Reader::ReadStatus status, const std::string& line)
{
    if (status != fzf::Reader::ReadStatus::EndOfFile)
    {
        performIncrementalSearch(line);
    }
    updateDisplay();
}

void Application::updateDisplay()
{
    std::scoped_lock lock(m_searchMutex);

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

    // Determine the range of results to display
    // Find the first entry with a score of zero or less
    // This is the last possible entry that should be displayed
    auto firstZeroEntry = std::find_if(m_results.begin(), m_results.end(),
                                       [](const auto& result) { return result.second <= 0; });
    int lastEntryIndex = std::distance(m_results.begin(), firstZeroEntry);

    // Start index is the middle of the results list, adjusted for the number of results to display
    size_t start = std::max(0, localSelectedIndex - (m_numResults / 2));
    // Stop index is the minimum of the last entry index and the start index plus the number of
    // results
    size_t stop = std::min(lastEntryIndex, int(start + m_numResults));

    fzf::Results displayResults;
    displayResults.searchString = m_searchString;
    displayResults.totalResults = lastEntryIndex;
    displayResults.resultRange = {start, stop};
    for (size_t i = start; i < stop; ++i)
    {
        // Check if the score is zero or less
        if (m_results[i].second <= 0)
        {
            // Skip entries with zero or negative score
            // Since this list is sorted, we can break early
            // break;
        }

        displayResults.results.push_back(fzf::Result(i, m_results[i].first,
                                                     (static_cast<int>(i) == localSelectedIndex),
                                                     m_results[i].second));
        if (static_cast<int>(i) == localSelectedIndex)
        {
            m_selectedLine = m_results[i].first;  // Update selected line
        }
    }
    m_tty.writeResults(displayResults);
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
        for (auto& line : m_results)
        {
            line.second = fzf::score(m_searchString, line.first);
        }
    }

    // Sort results based on the score
    std::ranges::sort(m_results, resultCompare);
    updateSelectedLineIndex();  // Update selected line index
}

void Application::performIncrementalSearch(const std::string& line)
{
    std::scoped_lock lock(m_searchMutex);
    assert(!line.empty());
    int score = fzf::score(m_searchString, line);

    // Insert into sorted list
    auto insertPos = std::ranges::lower_bound(m_results, std::make_pair(line, score));
    m_results.insert(insertPos, {line, score});
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
}

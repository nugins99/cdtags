#ifndef FUZZY_APPLICATION_H
#define FUZZY_APPLICATION_H

#include <boost/signals2.hpp>
#include <functional>
#include <mutex>
#include <string>
#include <vector>

#include "Reader.h"
#include "TTY.h"

/// @class Application
/// @brief Interactive fuzzy search application for terminal use.
///
/// Handles user input, manages search state, and displays results in the terminal.
/// Uses a TTY for terminal I/O and a Reader for input data. Thread-safe for concurrent updates.
class Application
{
   public:
    /// @brief Result of a fuzzy search input processing step.
    enum class FuzzySearchResult
    {
        Continue,  ///< Continue processing input
        Exit,      ///< Exit the application
        Select     ///< Select the current result
    };

    /// @brief Construct a new Application object.
    /// @param searchString Reference to the search string to use for fuzzy searching.
    /// @param inputReader Reference to the input reader function object.
    /// @param numResults The number of results to return/display.
    Application(std::string& searchString, fzf::Reader::Ptr& inputReader, std::size_t numResults);
    /// @brief Destructor. Ensures input reader is stopped.
    ~Application();

    /// @brief Run the main application loop.
    void run();
    /// @brief Get the currently selected result string.
    /// @return std::string The selected line, or the first result if none selected.
    std::string result() const;

   private:
    /// @brief Update the spinner/progress indicator in the terminal.
    /// @param count Number of lines processed or spinner step.
    void updateSpinner(size_t count);
    /// @brief Handle updates from the input reader.
    /// @param status The read status.
    /// @param line The new line read.
    void onUpdate(fzf::Reader::ReadStatus status, const std::string& line);
    /// @brief Handle up arrow key input.
    void onUpArrow();
    /// @brief Handle down arrow key input.
    void onDownArrow();
    /// @brief Handle backspace key input.
    void onBackspace();
    /// @brief Handle printable character input.
    /// @param c The character pressed.
    void onPrintableChar(char c);
    /// @brief Process a single user input event.
    /// @return FuzzySearchResult The result of processing input.
    FuzzySearchResult processInput();
    /// @brief Update the terminal display with current results and state.
    void updateDisplay();
    /// @brief Perform an incremental search as new lines are read.
    /// @param line The new line to consider.
    void performIncrementalSearch(const std::string& line);
    /// @brief Perform a full fuzzy search on all input lines.
    void performFuzzySearch();
    /// @brief Update the selected line index based on current results.
    void updateSelectedLineIndex();

    TTY m_tty;                         ///< TTY object for terminal interaction.
    std::mutex m_displayMutex;         ///< Mutex for thread safety.
    std::mutex m_searchMutex;          ///< Mutex for search operations.
    std::mutex m_linesMutex;           ///< Mutex for lines read from input.
    std::string& m_searchString;       ///< The search string to use for fuzzy searching.
    fzf::Reader::Ptr& m_inputReader;   ///< The input reader function object.
    std::vector<std::string> m_lines;  ///< Vector of lines read from input.
    int m_numResults;                  ///< The number of results to return.
    int m_selectedIndex{-1};           ///< The index of the currently selected option.
    std::string m_selectedLine{};      ///< The currently selected line.
    std::vector<std::pair<std::string, int>> m_results;  ///< Vector of scored lines.
    boost::signals2::scoped_connection m_connection;     ///< Connection for input reader updates.
};

#endif  // APPLICATION_H

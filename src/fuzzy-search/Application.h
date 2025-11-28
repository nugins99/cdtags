#ifndef FUZZY_APPLICATION_H
#define FUZZY_APPLICATION_H

#include <mutex>
#include <string>
#include <vector>

#include "InputInterface.h"
#include "ModelInterface.h"
#include "Reader.h"

/// @class Application
/// @brief Interactive fuzzy search application for terminal use.
///
/// Handles user input, manages search state, and displays results in the terminal.
/// Uses a TTY for terminal I/O and a Reader for input data. Thread-safe for concurrent updates.
class Application : public fzf::ModelInterface
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
    /// @param tty Reference to the TTY object for terminal interaction.
    /// @param numResults The number of results to return/display.
    Application(std::string& searchString, fzf::Reader::Ptr& inputReader, fzf::InputInterface& tty,
                std::size_t numResults);
    /// @brief Destructor. Ensures input reader is stopped.
    ~Application();

    /// @brief Get the currently selected result string.
    /// @return std::string The selected line, or the first result if none selected.
    const std::string & result() const override;

    /// @brief Get the current search string.
    std::string searchString() const override { return m_searchString; }

    /// @brief  Set the search string and update the model.
    /// @param searchString
    void setSearchString(std::string searchString) override
    {
        m_searchString = std::move(searchString);
        performFuzzySearch();
        updateDisplay();
    }
    /// @brief Finish the search and clean up resources.
    void finishSearch()
    {
        m_inputReader->setEndOfFile();  // Signal end of input
        m_inputReader->disconnect();
    }

    /// @brief Get the currently selected result index.
    int getSelectedIndex() const override { return m_selectedIndex; }

    /// @brief  Set the currently selected result index.
    /// @param index
    void setSelectedIndex(int index) override
    {
        if (index < 0 || index >= static_cast<int>(m_results.size()))
        {
            m_selectedIndex = -1;  // Reset selection if index is out of bounds
            m_selectedLine.clear();
            return;
        }

        if (index < static_cast<int>(m_results.size()))
        {
            m_selectedIndex = index;
            m_selectedLine = m_results[index].first;
        }
        updateDisplay();
    }

    /// @brief Get the number of results in the model.
    /// @return The number of results.
    std::size_t size() const override { return m_results.size(); }

   private:
    /// @brief Update the spinner/progress indicator in the terminal.
    /// @param count Number of lines processed or spinner step.
    void updateSpinner(size_t count);
    /// @brief Handle updates from the input reader.
    /// @param status The read status.
    /// @param line The new line read.
    void onUpdate(fzf::Reader::ReadStatus status, const std::string& line);
    /// @brief Update the terminal display with current results and state.
    void updateDisplay();
    /// @brief Perform an incremental search as new lines are read.
    /// @param line The new line to consider.
    void performIncrementalSearch(const std::string& line);
    /// @brief Perform a full fuzzy search on all input lines.
    void performFuzzySearch();
    /// @brief Update the selected line index based on current results.
    void updateSelectedLineIndex();

    fzf::InputInterface& m_tty;       ///< TTY object for terminal interaction.
    std::mutex m_searchMutex;         ///< Mutex for search operations.
    std::string& m_searchString;      ///< The search string to use for fuzzy searching.
    fzf::Reader::Ptr& m_inputReader;  ///< The input reader function object.
    int m_numResults;                 ///< The number of results to return.
    int m_selectedIndex{-1};          ///< The index of the currently selected option.
    std::string m_selectedLine{};     ///< The currently selected line.
    std::vector<std::pair<std::string, int>> m_results;  ///< Vector of scored lines.
};

#endif  // APPLICATION_H

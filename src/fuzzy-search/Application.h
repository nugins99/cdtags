#ifndef FUZZY_APPLICATION_H
#define FUZZY_APPLICATION_H

#include <boost/signals2.hpp>
#include <fstream>
#include <functional>
#include <mutex>
#include <string>
#include <unordered_set>
#include <vector>

#include "Reader.h"
#include "TTY.h"

class Application
{
   public:
    enum class FuzzySearchResult
    {
        Continue,
        Exit,
        Select
    };

    Application(std::string& searchString, fzf::Reader::Ptr& inputReader, std::size_t numResults);

    void run();
    std::string result() const;

   private:
    void updateSpinner(size_t count);
    void onUpdate(fzf::Reader::ReadStatus status, const std::string& line);
    void onUpArrow();
    void onDownArrow();
    void onBackspace();
    void onPrintableChar(char c);
    FuzzySearchResult processInput();
    void updateDisplay();
    void performIncrementalSearch(const std::string& line);
    void performFuzzySearch();
    void updateSelectedLineIndex();

    TTY m_tty;                         ///< TTY object for terminal interaction.
    std::mutex m_displayMutex;                ///< Mutex for thread safety.
    std::mutex m_searchMutex;          ///< Mutex for search operations.
    std::string& m_searchString;       ///< The search string to use for fuzzy searching.
    fzf::Reader::Ptr& m_inputReader;   ///< The input reader function object.
    int m_numResults;                  ///< The number of results to return.
    std::size_t m_selectedIndex = -1;  ///< The index of the currently selected option.
    std::string m_selectedLine{};      ///< The currently selected line.
    std::vector<std::pair<std::string, int>> m_results;  ///< Vector of scored lines.
    std::unordered_set<std::string> m_seenLines;         ///< Set to track seen lines.
    boost::signals2::scoped_connection m_connection;     ///< Connection for input reader updates.
    std::ofstream log;
};

#endif  // APPLICATION_H

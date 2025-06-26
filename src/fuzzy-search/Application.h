#ifndef FUZZY_APPLICATION_H
#define FUZZY_APPLICATION_H

#include <boost/signals2.hpp>
#include <fstream>
#include <functional>
#include <mutex>
#include <string>
#include <vector>

#include "InputReader.h"
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
    void onUpdate(fzf::Reader::ReadStatus status);
    void onUpArrow();
    void onDownArrow();
    void onBackspace();
    void onPrintableChar(char c);
    FuzzySearchResult processInput();
    void updateDisplay();
    void performFuzzySearch();

    TTY m_tty;                           ///< TTY object for terminal interaction.
    std::mutex m_mutex;                  ///< Mutex for thread safety.
    std::string& m_searchString;         ///< The search string to use for fuzzy searching.
    fzf::Reader::Ptr& m_inputReader;     ///< The input reader function object.
    std::size_t m_numResults;            ///< The number of results to return.
    std::size_t m_selectedIndex = 0;     ///< The index of the currently selected option.
    std::vector<std::string> m_options;  ///< The list of options to display.
    boost::signals2::scoped_connection m_connection;  ///< Connection for input reader updates.
    std::ofstream log;
};

#endif // APPLICATION_H

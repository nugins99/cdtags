#pragma once

#include <istream>

#include "InputInterface.h"
#include "ModelInterface.h"

namespace fzf
{

/// @class Controller
class Controller
{
   public:
    /// @brief Construct a new Controller object.
    /// @param searchString Reference to the search string to use for fuzzy searching.
    /// @param inputReader Reference to the input reader function object.
    /// @param numResults The number of results to return/display.
    Controller(InputInterface& inputStream, ModelInterface& model)
        : m_tty(inputStream), m_model(model)
    {}

    /// @brief Destructor. Ensures input reader is stopped.
    ~Controller() = default;

    /// @brief Run the main application loop.
    void run()
    {
        while (!m_stop)
        {
            processInput();
        }
    }

    void stop() { m_stop = true; }
    bool stopped() const { return m_stop; }

   private:
    void processInput()
    {
        // Read user input and process it
        char c;
        c = m_tty.getch();

        if (c == '\033')  // Escape sequence
        {
            // Handle escape sequences for special keys
            c = m_tty.getch();
            if (c == '[')
            {
                c = m_tty.getch();
                switch (c)
                {
                    case 'A':
                        onUpArrow();
                        break;  // Up arrow
                    case 'B':
                        onDownArrow();
                        break;  // Down arrow
                    case 'C':   /* Right arrow */
                        break;
                    case 'D': /* Left arrow */
                        break;
                }
            }
        }
        else if (c == '\n' || c == '\r')
        {
            m_stop = true;  // Stop the loop
        }
        else if (c == 127)  // Backspace
        {
            std::string searchString = m_model.searchString();
            if (!searchString.empty())
            {
                searchString.pop_back();
                m_model.setSearchString(searchString);
            }
        }
        else if (isprint(c))
        {
            onPrintableChar(c);  // Handle printable characters
        }
    }

    void onUpArrow()
    {
        // Handle up arrow key input
        auto selectedIndex = m_model.getSelectedIndex();
        if (selectedIndex > 0)
        {
            m_model.setSelectedIndex(selectedIndex - 1);
        }
    }

    void onDownArrow()
    {
        // Handle down arrow key input
        auto selectedIndex = m_model.getSelectedIndex();
        if (selectedIndex < static_cast<int>(m_model.size() - 1))
        {
            m_model.setSelectedIndex(selectedIndex + 1);
        }
    }

    void onBackspace()
    {
        std::string searchString = m_model.searchString();
        if (!searchString.empty())
        {
            searchString.pop_back();
            m_model.setSearchString(searchString);
        }
    }

    void onPrintableChar(char c)
    {
        std::string searchString = m_model.searchString();
        searchString += c;
        m_model.setSearchString(searchString);
    }

    InputInterface& m_tty;    ///< Input stream for reading data
    ModelInterface& m_model;  ///< Reference to the application managing the UI and state
    bool m_stop{false};       ///< Flag to control the main loop
};
}  // namespace fzf
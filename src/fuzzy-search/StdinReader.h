#pragma once
#include <iosfwd>
#include <iostream>
#include <istream>
#include <stdexcept>
#include <thread>

#include "Reader.h"

namespace fzf
{

/// @class fzf::StdinReader
/// @brief Reads lines asynchronously from standard input for fuzzy search.
///
/// Inherits from fzf::Reader. Launches a background thread to read lines from stdin,
/// adding each unique, non-empty line to the input set and notifying listeners.
/// Thread-safe and supports start/stop control.
class StdinReader : public Reader
{
   public:
    /// @brief Constructs a new StdinReader and prepares for input.
    StdinReader();
    /// @brief Destructor. Stops reading and joins the background thread.
    ~StdinReader();

    // Disable copy and move semantics
    StdinReader(const StdinReader&) = delete;             ///< Copy constructor deleted
    StdinReader& operator=(const StdinReader&) = delete;  ///< Copy assignment deleted
    StdinReader(StdinReader&&) = delete;                  ///< Move constructor deleted

    /// @brief Starts the background thread to read from stdin.
    void start() override;
    /// @brief Signals the background thread to stop and waits for it to finish.
    void stop() override;

   private:
    /// @brief Internal method run in a background thread to read lines from stdin.
    void read();

    std::thread m_thread;             ///< Thread for reading input asynchronously
    std::atomic<bool> m_stop{false};  ///< Flag to stop reading
};

}  // namespace fzf

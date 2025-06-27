#pragma once
#include <boost/signals2.hpp>
#include <mutex>
#include <string>
#include <unordered_set>
#include <vector>

namespace fzf
{

/// @class fzf::Reader
/// @brief Abstract base class for reading and tracking unique lines of input for fuzzy search.
///
/// Provides thread-safe storage of unique lines, notifies listeners via Boost.Signals2 when new
/// lines are added, and tracks reading status. Intended for use in fuzzy search applications where
/// input may come from files, streams, or other sources.
class Reader
{
   public:
    /// @brief Smart pointer alias for managing Reader instances.
    using Ptr = std::unique_ptr<Reader>;

    /// @brief Virtual destructor.
    virtual ~Reader() = default;

    /// @brief Returns a copy of all unique lines read so far.
    /// @return std::vector<std::string> The lines read.
    std::vector<std::string> data() const
    {
        std::scoped_lock lock(m_mutex);
        return m_lines;
    }

    /// @brief Starts the reading process. Must be implemented by derived classes.
    virtual void start() = 0;

    /// @brief Stops the reading process. Optional to override.
    virtual void stop() {}

    /// @enum ReadStatus
    /// @brief Indicates whether input is ongoing or finished.
    enum class ReadStatus
    {
        Continue,  ///< Reading input.
        EndOfFile  ///< End of file or input stream.
    };

    /// @brief Returns the current reading status.
    /// @return ReadStatus The current status.
    ReadStatus status() const { return m_status; }

    /// @brief Adds a line if it is non-empty and not already seen. Notifies listeners if added.
    /// @param line The line to add.
    /// @return true if the line was added, false otherwise.
    bool addLine(std::string line)
    {
        {
            if (line.empty())
            {
                // Skip empty lines
                return false;
            }
            std::scoped_lock lock(m_mutex);
            if (!m_seenLines.insert(line).second)
            {
                // Line already seen, do not add
                return false;
            }
            m_lines.emplace_back(std::move(line));
        }
        // Notify subscribers about the update
        onUpdate(ReadStatus::Continue, m_lines.back());
        return true;
    }

    /// @brief Signal emitted when a new line is added, passing the current status and the new line.
    boost::signals2::signal<void(ReadStatus, std::string)> onUpdate;

   protected:
    ReadStatus m_status = ReadStatus::Continue;  ///< Current read status.

   private:
    mutable std::mutex m_mutex;                   ///< Mutex to protect access to m_lines.
    std::vector<std::string> m_lines;             ///< Stores the lines read from input.
    std::unordered_set<std::string> m_seenLines;  ///< Set to track seen lines.
};

}  // namespace fzf

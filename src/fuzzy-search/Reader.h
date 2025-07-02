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
    std::unordered_set<std::size_t> getSeenValues() const
    {
        std::scoped_lock lock(m_mutex);
        return m_seenLines;
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
        auto hashValue =  m_hash(line);
        if (line.empty() || m_seenLines.contains(hashValue))
        {
            // Skip empty lines or line already seen, do not add
            return false;
        }
        std::scoped_lock lock(m_mutex);
        m_seenLines.insert(hashValue);  // Insert line into seen set
        // Notify subscribers about the update
        onUpdate(ReadStatus::Continue, line);
        return true;
    }

    void setEndOfFile()
    {
        std::scoped_lock lock(m_mutex);
        m_status = ReadStatus::EndOfFile;  // Set status to End of File
        onUpdate(m_status, "");            // Notify subscribers about the end of file
    }

    /// @brief Signal emitted when a new line is added, passing the current status and the new line.
    boost::signals2::signal<void(ReadStatus, std::string)> onUpdate;

   private:
    mutable std::mutex m_mutex;                   ///< Mutex to protect access to m_seenLines.
    ReadStatus m_status = ReadStatus::Continue;   ///< Current read status.
    //std::unordered_set<std::string> m_seenLines;  ///< Set to track seen lines.
    std::hash<std::string> m_hash{};
    std::unordered_set<std::size_t> m_seenLines;  ///< Set to track seen lines.
};

}  // namespace fzf

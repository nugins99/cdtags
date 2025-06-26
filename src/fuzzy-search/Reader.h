#pragma once
#include <boost/signals2.hpp>
#include <mutex>
#include <string>
#include <vector>

namespace fzf
{

class Reader
{
   public:
    using Ptr = std::unique_ptr<Reader>;
    virtual ~Reader() = default;

    std::vector<std::string> data() const
    {
        std::scoped_lock lock(m_mutex);
        return m_lines;
    }

    /// @brief Starts the reading process.
    virtual void start() = 0;

    enum class ReadStatus
    {
        Continue,  ///< Reading input.
        EndOfFile  ///< End of file or input stream.
    };

    ReadStatus status() const { return m_status; }

    void addLine(std::string line)
    {
        {
            std::scoped_lock lock(m_mutex);
            m_lines.emplace_back(std::move(line));
        }
        onUpdate(ReadStatus::Continue, m_lines.back());  // Notify subscribers about the update
    }

    boost::signals2::signal<void(ReadStatus, std::string)> onUpdate;

   protected:
    ReadStatus m_status = ReadStatus::Continue;  ///< Current read status.

   private:
    mutable std::mutex m_mutex;        ///< Mutex to protect access to m_lines.
    std::vector<std::string> m_lines;  ///< Stores the lines read from input.
};

}  // namespace fzf

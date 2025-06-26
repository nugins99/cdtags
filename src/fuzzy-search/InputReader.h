#pragma once
#include <boost/program_options.hpp>
#include <boost/signals2.hpp>
#include <filesystem>  // For file path validation
#include <fstream>
#include <functional>
#include <iostream>
#include <mutex>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

namespace fzf
{

namespace po = boost::program_options;

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

    void addLine(std::string line)
    {
        {
            std::scoped_lock lock(m_mutex);
            m_lines.push_back(std::move(line));
        }
        onUpdate(ReadStatus::Continue);  // Notify subscribers about the update
    }

    boost::signals2::signal<void(ReadStatus)> onUpdate;

   private:
    mutable std::mutex m_mutex;        ///< Mutex to protect access to m_lines.
    std::vector<std::string> m_lines;  ///< Stores the lines read from stdin.
};

class AsyncReader : public Reader
{
   public:
    AsyncReader(std::istream* in = &std::cin) : m_in(in) {}
    ~AsyncReader() { m_thread.join(); };

    // Disable copy and move semantics
    AsyncReader(const AsyncReader&) = delete;
    AsyncReader& operator=(const AsyncReader&) = delete;
    AsyncReader(AsyncReader&&) = delete;

    virtual void start()
    {
        if (m_in && !m_in->good())
        {
            throw std::runtime_error("Input is not available.");
        }
        m_thread = std::thread([this] { read(); });
    }

   private:
    void read()
    {
        std::string line;
        while (std::getline(*m_in, line))
        {
            addLine(line);
        }
        // Notify subscribers about EOF
        onUpdate(ReadStatus::EndOfFile);
    }

    std::istream* m_in;  ///< Input stream to read from.
    std::thread m_thread;
};

class AsyncFileReader : public AsyncReader
{
   public:
    AsyncFileReader(const std::string& filePath) : AsyncReader(&m_fileStream), m_filePath(filePath)
    {
        if (!validateFilePath(filePath))
        {
            throw std::runtime_error("Invalid file path: " + filePath);
        }
        m_fileStream.open(filePath);
        if (!m_fileStream.is_open())
        {
            throw std::runtime_error("Failed to open file: " + filePath);
        }
    }

    ~AsyncFileReader() { m_fileStream.close(); }

   private:
    /// @brief Validates the file path.
    /// @param filePath The file path to validate.
    /// @return True if the file path is valid, false otherwise.
    bool validateFilePath(const std::string& filePath)
    {
        if (!std::filesystem::exists(filePath))
        {
            return false;
        }
        return true;
    }
    std::ifstream m_fileStream;  ///< File stream to read from.
    std::string m_filePath;      ///< Path to the file to read from.
};

/// @brief Creates an input reader function object based on command-line arguments.
/// @param vm The parsed variables map.
/// @return A function object for reading input.
inline Reader::Ptr createInputReader(const po::variables_map& vm)
{
    if (vm.count("stdin"))
    {
        std::cin.tie(0);  // Untie cin from cout for better performance
        std::cin.sync_with_stdio(false);  // Disable synchronization with C I/O
        return std::make_unique<fzf::AsyncReader>(&std::cin);
    }
    else
    {
        std::string filePath = vm["file"].as<std::string>();
        return std::make_unique<fzf::AsyncFileReader>(filePath);
    }
}

}  // namespace fzf
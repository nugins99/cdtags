#include "StdinReader.h"

#include <boost/asio/read_until.hpp>
#include <string>

namespace fzf
{

StdinReader::StdinReader() {}

StdinReader::~StdinReader()
{
    m_thread.join();  // Ensure the thread is joined before destruction
}

void StdinReader::start() { m_thread = std::thread(std::bind_front(&StdinReader::read, this)); }

void StdinReader::stop()
{
    m_stop = true;
    m_thread.join();                   // Wait for the reading thread to finish
    m_status = ReadStatus::EndOfFile;  // Set status to End of File
}

void StdinReader::read()
{
    std::string line;
    while (std::getline(std::cin, line) && !m_stop)
    {
        if (line.empty())  // Skip empty lines
            continue;
        addLine(line);  // Add the line to the internal storage
    }
    if (m_stop)
    {
        // Exit if stop was requested
        return;
    }
    // If we reach here, it means we hit EOF or an error occurred
    m_status = ReadStatus::EndOfFile;  // Set status to End of File
    onUpdate(m_status, "");            // Notify subscribers about the end of file
}

}  // namespace fzf

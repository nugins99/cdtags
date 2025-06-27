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
}

void StdinReader::read()
{
    std::string line;
    while (std::getline(std::cin, line) && !m_stop)
    {
        addLine(line);  // Add the line to the internal storage
    }
    if (m_stop)
    {
        // Exit if stop was requested
        return;
    }
    setEndOfFile();  // Set end of file status now that reading is done
}

}  // namespace fzf

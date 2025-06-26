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

void StdinReader::read()
{
    std::string line;
    while (std::getline(std::cin, line))
    {
        addLine(line);
    }
    m_status = ReadStatus::EndOfFile;  // Set status to End of File
    onUpdate(m_status, "");            // Notify subscribers about the end of file
}

}  // namespace fzf

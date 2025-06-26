#include "AsyncReader.h"

#include <boost/asio/read_until.hpp>
#include <string>

namespace fzf
{

AsyncReader::AsyncReader()
{}

AsyncReader::~AsyncReader() {
    m_thread.join();  // Ensure the thread is joined before destruction
}

void AsyncReader::start() {
    m_thread = std::thread(std::bind_front(&AsyncReader::read, this));
}

void AsyncReader::read()
{
    std::string line;
    while (std::getline(std::cin, line))
    {
        addLine(line);
    }
    onUpdate(ReadStatus::EndOfFile);  // Notify subscribers about EOF
}

}  // namespace fzf

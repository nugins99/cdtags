#include "AsyncFileReader.h"

namespace fzf
{

AsyncFileReader::AsyncFileReader(const std::string& filePath)
    : Reader(), m_filePath(filePath), m_fileStream()
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

void AsyncFileReader::start()
{
    if (!m_fileStream.is_open())
    {
        throw std::runtime_error("File stream is not open.");
    }
    std::string line;
    while (std::getline(m_fileStream, line))
    {
        addLine(line);
    }
    onUpdate(ReadStatus::EndOfFile);  // Notify subscribers about EOF
}

AsyncFileReader::~AsyncFileReader() { m_fileStream.close(); }

bool AsyncFileReader::validateFilePath(const std::string& filePath)
{
    return std::filesystem::exists(filePath);
}

}  // namespace fzf

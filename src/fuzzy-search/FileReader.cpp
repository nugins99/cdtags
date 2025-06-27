#include "FileReader.h"

namespace fzf
{

FileReader::FileReader(const std::string& filePath)
    : Reader(), m_filePath(filePath) 
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

void FileReader::start()
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
    setEndOfFile();  // Set end of file status now that reading is done
}

bool FileReader::validateFilePath(const std::string& filePath)
{
    return std::filesystem::exists(filePath);
}

}  // namespace fzf

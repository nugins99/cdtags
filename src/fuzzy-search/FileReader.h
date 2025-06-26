#pragma once
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>

#include "AsyncReader.h"

namespace fzf
{

class FileReader : public Reader
{
   public:
    FileReader(const std::string& filePath);
    ~FileReader();

   private:
    void start() override;

    bool validateFilePath(const std::string& filePath);

    std::ifstream m_fileStream;  ///< File stream to read from.
    std::string m_filePath;      ///< Path to the file to read from.
};

}  // namespace fzf

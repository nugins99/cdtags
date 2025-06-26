#pragma once
#include "AsyncReader.h"
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>

namespace fzf
{

class AsyncFileReader : public Reader 
{
   public:
    AsyncFileReader(const std::string& filePath);
    ~AsyncFileReader();

   private:

   void start() override;

    bool validateFilePath(const std::string& filePath);

    std::ifstream m_fileStream;  ///< File stream to read from.
    std::string m_filePath;      ///< Path to the file to read from.
};

}  // namespace fzf

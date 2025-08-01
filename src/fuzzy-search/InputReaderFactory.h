#pragma once
#include <boost/asio/io_context.hpp>
#include <boost/program_options.hpp>
#include <string>

#include "AsyncFileReader.h"
#include "StdinReader.h"
#include "FileReader.h"
#include "FileListReader.h"

namespace fzf
{

namespace po = boost::program_options;

/// @brief Creates an input reader based on command-line arguments.
/// @param vm The parsed variables map.
/// @return A unique pointer to the input reader.
inline Reader::Ptr createInputReader(const po::variables_map& vm,
                                     [[maybe_unused]] boost::asio::io_context& ioContext)
{
    if (vm.count("stdin"))
    {
        return std::make_unique<StdinReader>();
    }
    else if (vm.count("directories"))
    {
        return std::make_unique<FileListReader>(std::filesystem::current_path(), FileListReader::SearchType::Directories);
    }
    else if (vm.count("files"))
    {
        return std::make_unique<FileListReader>(std::filesystem::current_path(), FileListReader::SearchType::Files);
    }
    else
    {
        std::string filePath = vm["file"].as<std::string>();
        return std::make_unique<FileReader>(filePath);
    }
}

}  // namespace fzf

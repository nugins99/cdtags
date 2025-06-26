#pragma once
#include "AsyncFileReader.h"
#include "AsyncReader.h"
#include <boost/program_options.hpp>
#include <string>
#include <boost/asio/io_context.hpp>

namespace fzf
{

namespace po = boost::program_options;

/// @brief Creates an input reader based on command-line arguments.
/// @param vm The parsed variables map.
/// @return A unique pointer to the input reader.
inline Reader::Ptr createInputReader(const po::variables_map& vm, boost::asio::io_context& ioContext)
{
    if (vm.count("stdin"))
    {
        return std::make_unique<AsyncReader>();
    }
    else
    {
        std::string filePath = vm["file"].as<std::string>();
        return std::make_unique<AsyncFileReader>(filePath);
    }
}

}  // namespace fzf

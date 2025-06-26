/// @file FuzzySearchApp.cpp
/// @brief Application to perform fuzzy search using Levenshtein distance.

#include "Application.h"
#include "InputReaderFactory.h"
#include "TTY.h"
#include <boost/program_options.hpp>
#include <iostream>
#include <string>
#include <boost/asio/io_context.hpp>

namespace po = boost::program_options;

/// @brief Parses command-line options.
/// @param argc The argument count.
/// @param argv The argument vector.
/// @return A variables map containing parsed options.
po::variables_map parseCommandLineOptions(int argc, char* argv[])
{
    po::options_description desc("Allowed options");
    desc.add_options()("help,h", "Display help message")("search,s", po::value<std::string>(),
                                                         "Search string")(
        "file,f", po::value<std::string>(), "File path")("stdin", "Read input from standard input")(
        "results,r", po::value<int>()->default_value(10), "Number of results to return");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help"))
    {
        std::cout << desc << std::endl;
        exit(0);
    }

    if (!vm.count("search") || (!vm.count("file") && !vm.count("stdin")))
    {
        std::cerr << "Error: Missing required options --search and either --file or --stdin"
                  << std::endl;
        std::cout << desc << std::endl;
        exit(1);
    }

    return vm;
}

int main(int argc, char* argv[])
{
    try
    {
        boost::asio::io_context ioContext;  // Create an IO context for asynchronous operations
        
        po::variables_map vm = parseCommandLineOptions(argc, argv);
        std::string searchString = vm["search"].as<std::string>();
        int numResults = vm["results"].as<int>();

        fzf::Reader::Ptr inputReader = fzf::createInputReader(vm, ioContext);
        Application app(searchString, inputReader, numResults);
        app.run();

        // Output the selected result
        std::cout << app.result() << std::flush;
        return EXIT_SUCCESS;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}

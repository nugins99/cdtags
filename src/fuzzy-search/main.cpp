/// @file FuzzySearchApp.cpp
/// @brief Application to perform fuzzy search using Levenshtein distance.

#include <boost/algorithm/string.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/program_options.hpp>
#include <iostream>
#include <string>

#include "Application.h"
#include "Controller.h"
#include "InputReaderFactory.h"
#include "JSONRPCInterface.h"
#include "TTY.h"

namespace po = boost::program_options;

/// @brief Parses command-line options.
/// @param argc The argument count.
/// @param argv The argument vector.
/// @return A variables map containing parsed options.
po::variables_map parseCommandLineOptions(int argc, char* argv[])
{
    po::options_description desc("Allowed options");
    // clang-format off
    desc.add_options()("help,h", "Display help message")
        ("search,s", po::value<std::string>()->default_value(""), "Search string")
        ("readline_line", po::value<std::string>(), "")
        ("file,f", po::value<std::string>(), "File path")("stdin", "Read input from standard input")
        ("files,F", "File listing, recursive from current directory")
        ("directories,D", "Directory listing, recursive from current directory")
        ("results,r", po::value<int>()->default_value(10), "Number of possible results")
        ("jsonrpc,j", "Use JSON-RPC for input/output");
    // clang-format on

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help"))
    {
        std::cout << desc << std::endl;
        exit(0);
    }

    if (!vm.count("search") ||
        (!vm.count("file") && !vm.count("stdin") && !vm.count("files") && !vm.count("directories")))
    {
        std::cerr << "Error: Missing required options --search and either --file or --stdin"
                  << std::endl;
        std::cout << desc << std::endl;
        exit(1);
    }

    return vm;
}

std::unique_ptr<fzf::InputInterface> createInputInterface(const po::variables_map& vm)
{
    if (vm.count("jsonrpc"))
    {
        return std::make_unique<fzf::JSONRPCInterface>(std::cin, std::cout);
    }
    else
    {
        return std::make_unique<TTY>();
    }
}

int main(int argc, char* argv[])
{
    try
    {
        boost::asio::io_context ioContext;  // Create an IO context for asynchronous operations

        po::variables_map vm = parseCommandLineOptions(argc, argv);
        std::string searchString = vm["search"].as<std::string>();
        int numResults = vm["results"].as<int>();
        std::string resultBase{};

        auto tty = createInputInterface(vm);
        fzf::Reader::Ptr inputReader = fzf::createInputReader(vm, ioContext);
        Application app(searchString, inputReader, *tty, numResults);
        fzf::Controller controller(*tty, app);
        inputReader->start();
        controller.run();
        tty->writeFinalResult(app.result());
        return EXIT_SUCCESS;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}

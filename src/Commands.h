#ifndef CDTAGS_COMMANDS_H
#define CDTAGS_COMMANDS_H

#include "Debug.h"
#include "Version.h"
#include <boost/program_options.hpp>
#include <functional>
#include <iostream>
#include <set>
#include <unordered_map>

namespace cdtags {

namespace po = boost::program_options;

class CommandHandler
{
public:
  virtual int process(const std::vector<std::string>& args) = 0;
  virtual int help(std::ostream& out) = 0;
};

class CommandParser
{
public:

  CommandParser() = default;

  void doHelp(std::ostream& out,
              const po::options_description& opt,
              const std::string& cmd)
  {
    // Print global options.
    out << opt << std::endl;

    auto subCmd = subCommands.find(cmd);
    if (subCmd == subCommands.end()) {
      out << "Available commands:" << std::endl;
      for (const auto & c : commands) {
        out << "\t" << c << std::endl;
      }
    } else {
      subCmd->second->help(out);
    }
  }

  int process(int argc, const char** argv)
  {

    po::options_description global("Global Options");
    po::positional_options_description pos;

    // clang-format off
            global.add_options()
                    ("help", "Print this help message")
                    ("verbose", "Generate debug output")
                    ("version", "Print version and return")
                    ("command", po::value<std::string>(), "Command")
                    ("subargs", po::value<std::vector<std::string>>(), "Subarg commands");

            pos.add("command", 1).add("subargs", -1);


            po::variables_map vm;

            po::parsed_options parsed =
                    po::command_line_parser(argc, argv)
                        .options(global)
                        .positional(pos)
                        .allow_unregistered()
                        .run();
            //clang-format on

            po::store(parsed, vm);

            std::string cmd;

            if (vm.count("command"))
            {
                cmd = vm["command"].as<std::string>();
            }

            if (vm.count("version"))
            {
                std::cout << "Version: "<< cdtags::version << std::endl;
                return 0;
            }

            if (vm.count("help"))
            {
                doHelp(std::cout, global, cmd);
                return 0;
            }

            if (vm.count("verbose"))
            {
                cdtags::set_verbose();
            }

            if (vm.count("command"))
            {
                // Collect all the unrecognized options from the first pass. This will include the
                // (positional) command name, so we need to erase that.
                std::vector<std::string> opts = po::collect_unrecognized(parsed.options, po::include_positional);
                opts.erase(opts.begin());

                auto command = subCommands.find(cmd);
                if (command != subCommands.end())
                {
                    return command->second->process(opts);
                }
                else
                {
                    std::cerr << "Unknown command: " << cmd << std::endl;
                    doHelp(std::cerr, global, "");
                    return 1;
                }
            }
            doHelp(std::cerr, global, "");
            return 0;
        }

        void addSubCommand(const std::string &cmd, CommandHandler * handler)
        {
            commands.insert(cmd);
            subCommands[cmd] = handler;
        }

    private:
        std::unordered_map<std::string, CommandHandler*> subCommands;
        std::set<std::string> commands;
    };

}// namespace cdtags


#endif//CDTAGS_COMMANDS_H

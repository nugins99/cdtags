#include "ChangeDirectory.h"
#include <cstdlib>

namespace cdtags {

int
ChangeDirectory::process(const std::vector<std::string>& args)
{
  auto cfg = parseConfig();
  if (args.size() >= 1) {
    // Reverting to previous directory.
    if (args[0] == "-") {
      std::cout << getenv("OLDPWD") << std::endl;
      return 0;
    }

    // Search through the list of tags to find the path.
    auto it = cfg.paths.find(args[0]);
    if (it != cfg.paths.end()) {
      // Found a match - return the full path.
      std::cout << it->second.string() << std::endl;
    } else {
      // No match, just return what was sent to us.
      std::cout << args[0] << std::endl;
    }
  }
  return 0;
}

int
ChangeDirectory::help(std::ostream& out)
{
  out << "Look up directory given tag name." << std::endl;
  out << "Positional arguments: " << std::endl;
  out << "\t <tag> - Alias for a directory." << std::endl;
  return 0;
}
}

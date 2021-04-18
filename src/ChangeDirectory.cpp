#include "ChangeDirectory.h"
#include <cstdlib>
#include "Debug.h"

namespace cdtags {

int
ChangeDirectory::process(const std::vector<std::string>& args)
{
  auto cfg = parseConfig();
  if (!args.empty()) {
    // Reverting to previous directory.
    if (args[0] == "-") {
      DEBUG("Returning to old PWD");
      std::cout << getenv("OLDPWD") << std::endl;
      return 0;
    }

    // Absolute path or path leading with '.' -> no replacement
    // Examples:
    // /home/joe
    // ../joe
    // ./Documents/...
    // or some other relative directory that exists.
    if (args[0][0] == '/' || args[0][0] == '.' || fs::is_directory(args[0]))
    {
      DEBUG("Not replacing path on abs, rel, or existing path");
      std::cout << args[0] << std::endl;
      return 0;
    }


    // Ok.. now we have:
    // a
    // a/b
    // but neither will resolve to a directory, so we attempt to replace it.

    auto possibleTag = fs::path(args[0]).begin()->string();
    auto resolvedTagIt = cfg.paths.find(possibleTag);

    // Nope - not a tag.
    if (resolvedTagIt == cfg.paths.end())
    {
      DEBUG("Not a tag: " << possibleTag);
      std::cout << args[0] << std::endl;
      return 0;
    }

    // First element matched a tag, replace the tag with the new path.
    DEBUG("Is a tag: " << possibleTag << " -> " << resolvedTagIt->second);

    // Probably not the most efficient way - but it works.
    // Maybe we can use ranges?
    auto path = resolvedTagIt->second;
    auto taggedPath = fs::path(args[0]);
    auto taggedPathIt = taggedPath.begin();
    for(++taggedPathIt; taggedPathIt != taggedPath.end(); ++taggedPathIt)
    {
      DEBUG("Appending: " << *taggedPathIt);
      path /= *taggedPathIt;
    }
    std::cout << path.string() << std::endl;
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

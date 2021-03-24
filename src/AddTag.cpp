#include "AddTag.h"
#include "Config.h"
int
cdtags::AddTag::process(const std::vector<std::string>& args)
{

  if (args.size() != 2) {
    std::cerr << "add: Invalid arguments" << std::endl;
    return -1;
  }

  if (!fs::is_directory(args[1])) {
    std::cerr << "add: Invalid arguments" << std::endl;
    std::cerr << "\t" << args[1] << ": is not a directory.";
    return -1;
  }

  // TODO check for duplicates?
  auto cfg = parseConfig();
  cfg.aliases[args[1]].insert(args[0]);
  saveConfig(cfg);
  return 0;
}

int
cdtags::AddTag::help(std::ostream& out)
{
  out << "Add a tag:"
      << "\n\n";
  out << "\t<tag> <path>" << std::endl;
  return 0;
}

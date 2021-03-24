#include "Remove.h"
#include "Config.h"
#include "Debug.h"

int
cdtags::Remove::process(const std::vector<std::string>& args)
{
  auto cfg = parseConfig();
  DEBUG("Removing tag: " << args[0]);
  for (auto& p : cfg.aliases) {
    p.second.erase(args[0]);
  }
  saveConfig(cfg);
  return 0;
}

int
cdtags::Remove::help(std::ostream& out)
{
  out << "Remove a tag:"
      << "\n\n";
  out << "\t<tag>" << std::endl;
  return 0;
}

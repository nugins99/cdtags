#include <string>

#include "ChangeDirectory.h"
#include "ListTags.h"
int
cdtags::ListTags::process([[maybe_unused]] const std::vector<std::string>& args)
{
  auto cfg = parseConfig();
  for (const auto& p : cfg.paths) {
    std::cout << p.first << "\t" << p.second << std::endl;
  }
  return 0;
}
int
cdtags::ListTags::help([[maybe_unused]] std::ostream& out)
{
  return 0;
}

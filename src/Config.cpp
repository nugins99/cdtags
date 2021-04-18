#include "Config.h"
#include "Debug.h"
#include <boost/algorithm/string.hpp>
#include <iostream>

namespace cdtags {

fs::path
configFile()
{
  static const fs::path file = ".config/cdtags/config";
  // Read configuration
  fs::path home(getenv("HOME"));
  return home / file;
}

Config
parseConfig()
{
  namespace alg = boost::algorithm;
  // Check to see if the file exits...
  if (!fs::is_regular_file(configFile())) {
    return Config();
  }

  Config cfg;

  std::ifstream in(configFile().c_str());
  std::string line;
  size_t lineNo = 0;
  while (std::getline(in, line)) {
    lineNo++;
    alg::trim(line);
    // ignore blank lines or comments.
    if (line.empty() || line[0] == '#') {
      continue;
    }
    std::vector<std::string> fields;
    alg::split(fields, line, alg::is_any_of(","));

    if (fields.size() < 2) {
      std::cerr << "Warning : (" << lineNo << ") Insufficient fields" << line
                << std::endl;
      continue;
    }

    for (int i = 1; i < fields.size(); ++i) {
      if (fields[i].empty())
        continue; // ignore empty tags
      cfg.paths[fields[i]] = fields[0];
      cfg.aliases[fields[0]].insert(fields[i]);
    }
  }
  return cfg;
}

void
saveConfig(const Config& m)
{

  if (!fs::is_directory(configFile().branch_path())) {
    std::cout << "Creating: " << configFile().branch_path() << std::endl;
    fs::create_directories(configFile().branch_path());
  }

  std::ofstream out(configFile().c_str());
  for (const auto& path : m.aliases) {
    DEBUG("Path: " << path.first);
    out << path.first.string() << ",";
    for (const auto& p : path.second) {
      DEBUG(p);
      out << p << ",";
    }
    out << '\n';
  }
}
}

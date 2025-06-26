#ifndef CDTAGS_CONFIG_H
#define CDTAGS_CONFIG_H

//#include <boost/filesystem.hpp>
#include <filesystem>
#include <map>
#include <set>

namespace cdtags {
namespace fs = std::filesystem;

// Readability
typedef std::string Alias;

struct Config
{
  // Maps an alias to a path
  typedef std::map<Alias, fs::path> PathMap;

  // Maps a path to a list of aliases
  typedef std::map<fs::path, std::set<Alias>> AliasMap;

  PathMap paths;
  AliasMap aliases;
};

Config
parseConfig();

void
saveConfig(const Config& m);
}
#endif // CDTAGS_CONFIG_H

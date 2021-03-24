#include "Complete.h"
#include "Config.h"
#include "Debug.h"

namespace cdtags {

std::vector<fs::path>
list_subdirs(const fs::path& p)
{
  std::vector<fs::path> v;
  for (auto it = fs::directory_iterator(p); it != fs::directory_iterator();
       ++it) {
    if (fs::is_directory(*it)) {
      v.push_back(it->path());
    }
  }
  return v;
}

void
complete_abs_dir(const fs::path& curr,
                 const std::string& prefix = "",
                 const std::string& replacement = "")
{
  DEBUG("abs path complete: " << curr)
  if (fs::is_directory(curr)) {
    DEBUG(curr.has_leaf() << ": " << curr);

    if (fs::path(curr).filename_is_dot() || (curr == "/")) {
      DEBUG(curr << ": filename is dot");
      for (auto d : list_subdirs(curr)) {
        DEBUG(d << ": " << prefix);
        if (prefix.empty()) {
          std::cout << d.string() << std::endl;
        } else {
          auto& pathString = d.string();
          DEBUG("r: " << replacement << ": ps: " << pathString
                      << ": prefix: " << prefix)
          std::cout << replacement + pathString.substr(curr.string().size() + 1)
                    << std::endl;
        }
      }
      return;
    } else {
      // Do we even need this?
      // DEBUG("");
      // std::cout << curr << std::endl;
    }
  } else {

    std::vector<fs::path> matches;
    auto parent = fs::path(curr).parent_path();
    auto leaf = fs::path(curr).leaf().string();
    if (!fs::is_directory(parent)) {
      return;
    }

    for (auto d : list_subdirs(parent)) {
      if (d.leaf().string().find(leaf) == 0) {
        std::cout << d.string() << std::endl;
      }
    }
  }
}

int
complete_tags(const std::string& curr, const Config& cfg)
{
  for (auto v : cfg.paths) {
    if (v.first.find(curr) == 0) {
      DEBUG("Tag completer: " << curr << " : " << v.first);
      std::cout << v.first << std::endl;
    }
  }
  return 0;
}

int
complete_relative_path(fs::path& p)
{
  DEBUG("Relative path completer: " << p);
  auto parent = p.parent_path();
  if (parent.empty()) {
    DEBUG("Empty parent - no completions: " << p);
    return 0;
  }
  complete_abs_dir(parent);
  return 0;
}

int
complete_tag_based_relative_path(const fs::path& p, const Config& cfg)
{
  // TODO

  auto currPath = fs::path(p);
  auto first = currPath.begin()->string();

  auto tagPathIt = cfg.paths.find(first);
  if (tagPathIt == cfg.paths.end()) {
    return 0;
  }
  auto tagPath = tagPathIt->second;

  if (tagPath.empty()) {
    // Handled as a relative path
    return 0;
  }

  auto searchPath = tagPath;
  for (auto it = ++(currPath.begin()); it != currPath.end(); ++it) {
    searchPath = searchPath / it->string();
  }

  DEBUG(tagPath);
  DEBUG(currPath);
  complete_abs_dir(searchPath, tagPath.string(), p.string());
  // TODO - we are completing <tag>/<partiial-path>

  return 0;
}

} // namespace cdtags

int
cdtags::Complete::process(const std::vector<std::string>& args)
{
  auto cfg = parseConfig();

  // Only a single argument - can't do anything.
  // Maybe - list cdtags?
  if (args.size() != 1) {
    return 0;
  }

  auto curr = args[0];

  // Handle absolute directory
  if (curr[0] == '/') {
    complete_abs_dir(curr);
    return 0;
  }

  // Is first element of path a tag?
  auto currPath = fs::path(curr);
  auto first = currPath.begin()->string();

  if (std::distance(currPath.begin(), currPath.end()) == 1) {
    //  Input is just "something"
    complete_tags(curr, cfg);
    complete_relative_path(currPath);
  } else {
    //  Input is "something/a/"
    //  or "something/a/b/"
    complete_tags(curr, cfg);
    complete_relative_path(currPath);
    complete_tag_based_relative_path(currPath, cfg);
  }

  return 0;
}
int
cdtags::Complete::help(std::ostream& out)
{
  out << "Returns possible directory completions given argument." << std::endl;
  out << "Positional arguments: " << std::endl;
  out << "\t <path> - Absolute, Relative, or tagged partial path" << std::endl;
  return 0;
}

#ifndef CDTAGS_DEBUG_H
#define CDTAGS_DEBUG_H

#define DEBUG(x)                                                               \
  do {                                                                         \
    if (cdtags::verbose()) {                                                   \
      std::cerr << colors::yellow << __FILE__ << ":" << __LINE__ << ": "       \
                << colors::red << x << colors::reset << std::endl;             \
    }                                                                          \
  } while (false);

namespace cdtags {
void
set_verbose();
bool
verbose();
}

namespace colors {
constexpr const char* txtblk = "\e[0;30m"; // Black - Regular
constexpr const char* red = "\e[0;31m";    // Red
constexpr const char* txtgrn = "\e[0;32m"; // Green
constexpr const char* yellow = "\e[0;33m"; // Yellow
constexpr const char* txtblu = "\e[0;34m"; // Blue
constexpr const char* txtpur = "\e[0;35m"; // Purple
constexpr const char* txtcyn = "\e[0;36m"; // Cyan
constexpr const char* txtwht = "\e[0;37m"; // White
constexpr const char* bldblk = "\e[1;30m"; // Black - Bold
constexpr const char* bldred = "\e[1;31m"; // Red
constexpr const char* bldgrn = "\e[1;32m"; // Green
constexpr const char* bldylw = "\e[1;33m"; // Yellow
constexpr const char* bldblu = "\e[1;34m"; // Blue
constexpr const char* bldpur = "\e[1;35m"; // Purple
constexpr const char* bldcyn = "\e[1;36m"; // Cyan
constexpr const char* bldwht = "\e[1;37m"; // White
constexpr const char* unkblk = "\e[4;30m"; // Black - Underline
constexpr const char* undred = "\e[4;31m"; // Red
constexpr const char* undgrn = "\e[4;32m"; // Green
constexpr const char* undylw = "\e[4;33m"; // Yellow
constexpr const char* undblu = "\e[4;34m"; // Blue
constexpr const char* undpur = "\e[4;35m"; // Purple
constexpr const char* undcyn = "\e[4;36m"; // Cyan
constexpr const char* undwht = "\e[4;37m"; // White
constexpr const char* bakblk = "\e[40m";   // Black - Background
constexpr const char* bakred = "\e[41m";   // Red
constexpr const char* bakgrn = "\e[42m";   // Green
constexpr const char* bakylw = "\e[43m";   // Yellow
constexpr const char* bakblu = "\e[44m";   // Blue
constexpr const char* bakpur = "\e[45m";   // Purple
constexpr const char* bakcyn = "\e[46m";   // Cyan
constexpr const char* bakwht = "\e[47m";   // White
constexpr const char* reset = "\e[0m";     // Text Reset
}

#endif

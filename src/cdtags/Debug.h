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
constexpr const char* txtblk = "\033[0;30m"; // Black - Regular
constexpr const char* red = "\033[0;31m";    // Red
constexpr const char* txtgrn = "\033[0;32m"; // Green
constexpr const char* yellow = "\033[0;33m"; // Yellow
constexpr const char* txtblu = "\033[0;34m"; // Blue
constexpr const char* txtpur = "\033[0;35m"; // Purple
constexpr const char* txtcyn = "\033[0;36m"; // Cyan
constexpr const char* txtwht = "\033[0;37m"; // White
constexpr const char* bldblk = "\033[1;30m"; // Black - Bold
constexpr const char* bldred = "\033[1;31m"; // Red
constexpr const char* bldgrn = "\033[1;32m"; // Green
constexpr const char* bldylw = "\033[1;33m"; // Yellow
constexpr const char* bldblu = "\033[1;34m"; // Blue
constexpr const char* bldpur = "\033[1;35m"; // Purple
constexpr const char* bldcyn = "\033[1;36m"; // Cyan
constexpr const char* bldwht = "\033[1;37m"; // White
constexpr const char* unkblk = "\033[4;30m"; // Black - Underline
constexpr const char* undred = "\033[4;31m"; // Red
constexpr const char* undgrn = "\033[4;32m"; // Green
constexpr const char* undylw = "\033[4;33m"; // Yellow
constexpr const char* undblu = "\033[4;34m"; // Blue
constexpr const char* undpur = "\033[4;35m"; // Purple
constexpr const char* undcyn = "\033[4;36m"; // Cyan
constexpr const char* undwht = "\033[4;37m"; // White
constexpr const char* bakblk = "\033[40m";   // Black - Background
constexpr const char* bakred = "\033[41m";   // Red
constexpr const char* bakgrn = "\033[42m";   // Green
constexpr const char* bakylw = "\033[43m";   // Yellow
constexpr const char* bakblu = "\033[44m";   // Blue
constexpr const char* bakpur = "\033[45m";   // Purple
constexpr const char* bakcyn = "\033[46m";   // Cyan
constexpr const char* bakwht = "\033[47m";   // White
constexpr const char* reset = "\033[0m";     // Text Reset
}

#endif

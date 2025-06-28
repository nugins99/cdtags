#ifndef CDTAGS_DEBUG_H
#define CDTAGS_DEBUG_H
#include <common/AnsiCodes.h>

#define DEBUG(x)                                                               \
  do {                                                                         \
    if (cdtags::verbose()) {                                                   \
      std::cerr << ansi::fg::yellow << __FILE__ << ":" << __LINE__ << ": "       \
                << ansi::fg::red << x << ansi::reset::fg << std::endl;             \
    }                                                                          \
  } while (false);

namespace cdtags {
void
set_verbose();
bool
verbose();
}



#endif

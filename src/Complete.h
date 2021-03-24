#ifndef CDTAGS_COMPLETE_H
#define CDTAGS_COMPLETE_H

#include "Commands.h"
#include <boost/filesystem/path.hpp>

namespace cdtags {

class Complete : public CommandHandler
{
  int process(const std::vector<std::string>& args) override;
  int help(std::ostream& out) override;
};
}

#endif // CDTAGS_COMPLETE_H

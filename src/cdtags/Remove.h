#ifndef CDTAGS_REMOVE_H
#define CDTAGS_REMOVE_H

#include "Commands.h"

namespace cdtags {
class Remove : public CommandHandler
{

  int process(const std::vector<std::string>& args) override;
  int help(std::ostream& out) override;
};
}

#endif // CDTAGS_REMOVE_H

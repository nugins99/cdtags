#ifndef CDTAGS_LISTTAGS_H
#define CDTAGS_LISTTAGS_H

#include "Commands.h"

namespace cdtags {

class ListTags : public CommandHandler
{
  int process(const std::vector<std::string>& args) override;
  int help(std::ostream& out) override;
};

}

#endif // CDTAGS_LISTTAGS_H

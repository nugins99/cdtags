#ifndef CDTAGS_ADDTAG_H
#define CDTAGS_ADDTAG_H

#include "Commands.h"
namespace cdtags {

class AddTag : public CommandHandler
{
  int process(const std::vector<std::string>& args) override;
  int help(std::ostream& out) override;
};

}

#endif // CDTAGS_ADDTAG_H

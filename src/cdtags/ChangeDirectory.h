#ifndef CDTAGS_CHANGEDIRECTORY_H
#define CDTAGS_CHANGEDIRECTORY_H

#include "Commands.h"
#include "Config.h"

namespace cdtags {
class ChangeDirectory : public CommandHandler
{
public:
  int process(const std::vector<std::string>& args) override;
  int help(std::ostream& out) override;

private:
};
} // namespace cdtags
#endif // CDTAGS_CHANGEDIRECTORY_H

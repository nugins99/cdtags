#include "AddTag.h"
#include "ChangeDirectory.h"
#include "Commands.h"
#include "Complete.h"
#include "ListTags.h"
#include "Remove.h"

using namespace cdtags;
namespace fs = boost::filesystem;

int
main(int argc, const char** argv)
{

  // Commands
  ChangeDirectory cd;
  ListTags lt;
  AddTag at;
  Remove remove;
  Complete complete;

  cdtags::CommandParser parser;
  parser.addSubCommand("cd", &cd);
  parser.addSubCommand("list", &lt);
  parser.addSubCommand("add", &at);
  parser.addSubCommand("remove", &remove);
  parser.addSubCommand("complete", &complete);
  parser.process(argc, argv);

  return 0;
}

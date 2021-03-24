namespace {
bool verboseEnabled = false;
}

namespace cdtags {
void
set_verbose()
{
  verboseEnabled = true;
}

bool
verbose()
{
  return verboseEnabled;
}

}

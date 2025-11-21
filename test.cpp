#include "OpenLogging.hpp"

int main (int argc, char *argv[])
{
  OpenLogging::log ("HELlO {}");
  // COMPILE TIME ERROR
  // OpenLogging::log ("HELlO }");
  OpenLogging::log ("hello {}", true);
  return 0;
}

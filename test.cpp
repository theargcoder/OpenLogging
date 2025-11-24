#include "OpenLogging.hpp"

int main (int argc, char *argv[])
{
  OpenLogging::log ("HELlO {}");
  // COMPILE TIME ERROR
  // OpenLogging::log ("HELlO }");
  int b = 9;
  auto a = &b;

  auto fn = [] () {};

  OpenLogging::log ("hello {}", fn);
  OpenLogging::log ("hello {}", a);
  OpenLogging::log ("hello {}", nullptr);
  OpenLogging::log ("hello {}", true);
  OpenLogging::log ("hello {}", 1);
  OpenLogging::log ("hello {}", 1.0);
  OpenLogging::log ("hello {}", 1.0f);
  OpenLogging::log ("hello {}", "const char *");
  OpenLogging::log ("hello {}", const_cast<char *> ("const char *"));
  OpenLogging::log ("hello {}", std::string ("std::string"));

  return 0;
}

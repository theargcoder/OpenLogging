#include "include/OpenLogging.h"

#define RETURN_GOOD 0
#define NON_MAGIC_NUM 0
#define CHAR_PTR_SIZE 12

int main()
{
  OpenLogging logger;
  logger.info("HELlO");

  // COMPILE TIME ERROR

  int byx = NON_MAGIC_NUM;
  auto *abc = &byx;

  auto fnct = []() -> int { return RETURN_GOOD; };
  std::array<char, CHAR_PTR_SIZE> char_ptr = { 'c', 'h', 'a', 'r', ' ', 'p', 'o', 'i', 'n', 't', 'e', 'r' };

  // Shouldn't complile
  // logger.info("hello {}{}", fnct);
  // logger.info("hello {}}", fnct);
  // logger.info("hello {{}", fnct);
  // logger.info("hello {}", fnct);
  // logger.info("Hell yeah \\\\{{}\\\\}", 2);
  // logger.info("Hell yeah \\\\{{}\\}\\", 2);

  logger.info("hello {}", fnct);
  logger.info("hello {}", abc);
  logger.info("hello {}def", "abc");
  logger.info("hello {} def", "abc");
  logger.info("hello {} hello", 1);
  logger.info("Hell yeah \\{{}\\}", 2);
  logger.info("Hell yeah \\\\{}\\\\", 2);
  logger.info("hello {}, {}, {} ", "hi", "hihi", "bye");

  logger.debug("hello {}", 1.0F);
  logger.debug("hello {}", "const char *");
  logger.warn("hello {}", true);
  logger.warn("hello {}", std::string("std::string"));
  logger.error("hello {}", 1);
  logger.error("hello {}", 1.0);
  logger.fatal("hello {}", char_ptr);
  logger.fatal("hello {}", nullptr);

  return RETURN_GOOD;
};

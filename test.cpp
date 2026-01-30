#include "include/OpenLogging.h"
#include <cfloat>
#include <cstdint>

#define RETURN_GOOD 0
#define NON_MAGIC_NUM 0
#define CHAR_PTR_SIZE 12

int main()
{
  OpenLogging logger;

  int byx = NON_MAGIC_NUM;
  auto *abc = &byx;

  auto fnct = []() -> int { return RETURN_GOOD; };
  std::array<char, CHAR_PTR_SIZE> char_ptr = { 'c', 'h', 'a', 'r', ' ', 'p', 'o', 'i', 'n', 't', 'e', 'r' };

  // COMPILE TIME ERROR
  // Shouldn't complile
  // logger.info("hello {}{}", fnct);
  // logger.info("hello {}}", fnct);
  // logger.info("hello {{}", fnct);
  // logger.info("hello {}", fnct);
  // logger.info("Hell yeah \\\\{{}\\\\}", 2);
  // logger.info("Hell yeah \\\\{{}\\}\\", 2);

  logger.info("HELlO");
  logger.info("hello {}", fnct);
  logger.info("hello {}", abc);
  logger.info("hello {}def", "abc");
  logger.info("hello {} def", "abc");
  logger.info("hello {} hello", 1);
  logger.info("Hell yeah \\{{}\\}", 2);
  logger.info(R"(Hell yeah \\{}\\)", 2);
  logger.info("hello {}, {}, {} ", "hi", "hihi", "bye");

  logger.info("positive int {}", 1'234'567);
  logger.info("positive uint {}", 1'234'567U);
  logger.info("negative int {}", -1'234'567);

  logger.info("positive MAX_UINT64 {}", UINT64_MAX);
  logger.info("nevative SMALLEST_INT64 {}", INT64_MIN);

  logger.info("positive DBL_MAX {}", DBL_MAX);
  logger.info("nevative DBL_MIN {}", DBL_MIN);

  logger.info("positive DBL {}", 0.0000123456);
  logger.info("nevative DBL {}", -0.0000123456);

  logger.debug("hello {}", 1.0F);
  logger.debug("hello {}", "we passed a const char *");
  logger.warn("hello {}", true);
  logger.warn("hello {}", std::string("we passed a std::string"));
  logger.error("hello {}", 1);
  logger.error("hello {}", 1.0);
  logger.fatal("hello {}", char_ptr);
  logger.fatal("hello {}", nullptr);

  return RETURN_GOOD;
};

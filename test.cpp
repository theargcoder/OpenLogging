#include <limits>
#define BOOST_TEST_MODULE UnitTests
#include <boost/test/included/unit_test.hpp>

#include <cfloat>
#include <cstdint>

#include <climits>

#include "include/OpenLogging.h"

BOOST_AUTO_TEST_CASE(just_some_logging_no_exceptions_should_happen)
{

  OpenLogging logger;

  const constexpr auto CHAR_PTR_SIZE = 12;
  int byx = 1;
  auto *abc = &byx;

  auto fnct = []() -> int { return 1; };
  std::array<char, CHAR_PTR_SIZE> char_ptr = { 'c', 'h', 'a', 'r', ' ', 'p', 'o', 'i', 'n', 't', 'e', 'r' };

  // COMPILE TIME ERROR
  // Shouldn't complile
  // logger.info("hello {}{}", fnct);
  // logger.info("hello {}}", fnct);
  // logger.info("hello {{}", fnct);
  // logger.info("hello {}", fnct);
  // logger.info("Hell yeah \\\\{{}\\\\}", 2);
  // logger.info("Hell yeah \\\\{{}\\}\\", 2);

  BOOST_CHECK_NO_THROW(logger.info("HELlO"));
  BOOST_CHECK_NO_THROW(logger.info("hello {}", fnct));
  BOOST_CHECK_NO_THROW(logger.info("hello {}", abc));
  BOOST_CHECK_NO_THROW(logger.info("hello {}def", "abc"));
  BOOST_CHECK_NO_THROW(logger.info("hello {} def", "abc"));
  BOOST_CHECK_NO_THROW(logger.info("hello {} hello", 1));
  BOOST_CHECK_NO_THROW(logger.info("Hell yeah \\{{}\\}", 2));
  BOOST_CHECK_NO_THROW(logger.info(R"(Hell yeah \\{}\\)", 2));
  BOOST_CHECK_NO_THROW(logger.info("hello {}, {}, {} ", "hi", "hihi", "bye"));

  BOOST_CHECK_NO_THROW(logger.info("positive int {}", 1'234'567));
  BOOST_CHECK_NO_THROW(logger.info("positive uint {}", 1'234'567U));
  BOOST_CHECK_NO_THROW(logger.info("negative int {}", -1'234'567));

  BOOST_CHECK_NO_THROW(logger.info("positive MAX_UINT64 {}", UINT64_MAX));
  BOOST_CHECK_NO_THROW(logger.info("nevative SMALLEST_INT64 {}", INT64_MIN));

  BOOST_CHECK_NO_THROW(logger.info("positive DBL_MAX {}", DBL_MAX));
  BOOST_CHECK_NO_THROW(logger.info("nevative DBL_MIN {}", DBL_MIN));

  BOOST_CHECK_NO_THROW(logger.info("positive DBL {}", 0.0000123456));
  BOOST_CHECK_NO_THROW(logger.info("nevative DBL {}", -0.0000123456));

  BOOST_CHECK_NO_THROW(logger.debug("hello {}", 1.0F));
  BOOST_CHECK_NO_THROW(logger.debug("hello {}", "we passed a const char *"));
  BOOST_CHECK_NO_THROW(logger.warn("hello {}", true));
  BOOST_CHECK_NO_THROW(logger.warn("hello {}", std::string("we passed a std::string")));
  BOOST_CHECK_NO_THROW(logger.error("hello {}", 1));
  BOOST_CHECK_NO_THROW(logger.error("hello {}", 1.0));
  BOOST_CHECK_NO_THROW(logger.fatal("hello {}", char_ptr));
  BOOST_CHECK_NO_THROW(logger.fatal("hello {}", nullptr));
}

BOOST_AUTO_TEST_CASE(int32_t_test_all_numbers)
{
  OpenLogging logger;

  for(int32_t i = -10'000, lim = 0; i < 10'000 && lim < 10; i++)
  {
    const auto log = logger.format<false>("{}", i);
    const auto num_to_str = std::string(std::to_string(i));
    if(log != num_to_str)
    {
      std::cout << "log == '" << log << "' AND to_str == '" << num_to_str << "'" << std::endl;
      std::cout << "log        = ";
      for(unsigned char c : log)
      {
        std::cout << std::hex << (int)c << " ";
      }
      std::cout << std::endl;
      std::cout << "num_to_str = ";
      for(unsigned char c : num_to_str)
      {
        std::cout << std::hex << (int)c << " ";
      }
      std::cout << std::endl;
      BOOST_CHECK_EQUAL(log, num_to_str);
      lim++;
    }
  }

  {
    const auto log = logger.format<false>("{}", INT32_MIN);
    const auto num_to_str = std::string(std::to_string(INT32_MIN));
    BOOST_CHECK_EQUAL(log, num_to_str);
  }

  {
    const auto log = logger.format<false>("{}", INT32_MAX);
    const auto num_to_str = std::string(std::to_string(INT32_MAX));
    BOOST_CHECK_EQUAL(log, num_to_str);
  }
}

BOOST_AUTO_TEST_CASE(test_all_integers)
{
  const auto tester = []<typename T>
    requires std::is_integral_v<T>
  (const T &val)
  {
    const auto lopper = []<typename Type>(const bool &PLUS, const Type &DELIM) -> void
    {
      OpenLogging logger;
      for(Type i = DELIM, lim = 0; i < DELIM + ((PLUS) ? +10'000 : -10'000) && lim < 10; i++)
      {
        const auto log = logger.format<false>("{}", i);
        const auto num_to_str = std::string(std::to_string(i));
        if(log != num_to_str)
        {
          std::cout << "log == '" << log << "' AND to_str == '" << num_to_str << "'" << std::endl;
          std::cout << "log        = ";
          for(unsigned char c : log)
          {
            std::cout << std::hex << (int)c << " ";
          }
          std::cout << std::endl;
          std::cout << "num_to_str = ";
          for(unsigned char c : num_to_str)
          {
            std::cout << std::hex << (int)c << " ";
          }
          std::cout << std::endl;
          BOOST_CHECK_EQUAL(log, num_to_str);
          lim++;
        }
      }
    };

    const constexpr auto MIN = std::numeric_limits<T>::min();
    const constexpr auto MAX = std::numeric_limits<T>::max();
    lopper(true, MIN);
    lopper(true, 0);
    lopper(false, 0);
    lopper(false, MAX);
  };

  tester(static_cast<int8_t>(0));
  tester(static_cast<int16_t>(0));
  tester(static_cast<int32_t>(0));
  tester(static_cast<int64_t>(0));

  tester(static_cast<uint8_t>(0));
  tester(static_cast<uint16_t>(0));
  tester(static_cast<uint32_t>(0));
  tester(static_cast<uint64_t>(0));
}

BOOST_AUTO_TEST_CASE(int64_t_test_all_numbers)
{
  OpenLogging logger;

  for(int32_t i = -10'000, lim = 0; i < 10'000 && lim < 10; i++)
  {
    const auto log = logger.format<false>("{}", i);
    const auto num_to_str = std::string(std::to_string(i));
    if(log != num_to_str)
    {
      std::cout << "log == '" << log << "' AND to_str == '" << num_to_str << "'" << std::endl;
      std::cout << "log        = ";
      for(unsigned char c : log)
      {
        std::cout << std::hex << (int)c << " ";
      }
      std::cout << std::endl;
      std::cout << "num_to_str = ";
      for(unsigned char c : num_to_str)
      {
        std::cout << std::hex << (int)c << " ";
      }
      std::cout << std::endl;
      BOOST_CHECK_EQUAL(log, num_to_str);
      lim++;
    }
  }

  {
    const auto log = logger.format<false>("{}", INT32_MIN);
    const auto num_to_str = std::string(std::to_string(INT32_MIN));
    BOOST_CHECK_EQUAL(log, num_to_str);
  }

  {
    const auto log = logger.format<false>("{}", INT32_MAX);
    const auto num_to_str = std::string(std::to_string(INT32_MAX));
    BOOST_CHECK_EQUAL(log, num_to_str);
  }
}

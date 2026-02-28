#include <limits>
#include <type_traits>
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

namespace
{
  const auto log_str_into_hex = [](const std::string &in)
  {
    for(unsigned char c : in)
    {
      std::cout << std::hex << (int)c << " ";
    }
  };

  const auto lopper = []<typename Type>(const bool &PLUS, const Type &DELIM, const Type &JUMP) -> void
  {
    const constexpr auto WISHED_RANGE = 10'000;
    const constexpr auto MAX_NUM = std::numeric_limits<Type>::max();
    const constexpr Type RANGE = WISHED_RANGE < MAX_NUM ? WISHED_RANGE : MAX_NUM;
    const constexpr Type MAX_ERRORS = 10;

    OpenLogging logger;
    for(Type i = DELIM, lim = 0, max_iter = 0; ((PLUS) ? i < DELIM + RANGE : i > DELIM - RANGE) && lim < MAX_ERRORS && max_iter < RANGE; (PLUS) ? i += JUMP : i -= JUMP, max_iter++)
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

  const auto tester = []<typename T>(const T & /*unused*/)
  {
    const constexpr auto MIN = std::numeric_limits<T>::min();
    const constexpr auto MIN_JUMP = std::numeric_limits<T>::denorm_min();
    const constexpr auto MAX = std::numeric_limits<T>::max();
    lopper(true, MIN, MIN_JUMP);
    lopper(true, MIN, T{ 1 });
    lopper(true, T{ 0 }, MIN_JUMP);
    lopper(true, T{ 0 }, T{ 1 });

    lopper(false, MAX, MIN_JUMP);
    lopper(false, MAX, T{ 1 });
    lopper(false, T{ 0 }, MIN_JUMP);
    lopper(false, T{ 0 }, T{ 1 });
  };
} // namespace

BOOST_AUTO_TEST_CASE(test_all_integegral_v)
{
  tester(static_cast<int8_t>(0));
  tester(static_cast<int16_t>(0));
  tester(static_cast<int32_t>(0));
  tester(static_cast<int64_t>(0));

  tester(static_cast<uint8_t>(0));
  tester(static_cast<uint16_t>(0));
  tester(static_cast<uint32_t>(0));
  tester(static_cast<uint64_t>(0));
}

BOOST_AUTO_TEST_CASE(test_all_floating_point_v)
{
  tester(static_cast<float_t>(0));
  tester(static_cast<float_t>(0));
}

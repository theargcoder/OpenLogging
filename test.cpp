#define BOOST_TEST_MODULE UnitTests
#include <boost/test/included/unit_test.hpp>

#include <cfloat>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <limits>
#include <type_traits>

#include "include/Constants/Constants.h"
#include "include/Helpers/Helpers.h"
#include "include/Helpers/Numeric.h"
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

  // logger.info("hello {}{}", fnct);

  // const auto resters = Structures::valid_string_format<3>("{.3f} {9d} {.3fe00}");

  /*
  for(auto fmt : resters.fmts)
  {
    for(int j = 0; j < Constants::Size::MAX_FMT_SIZE; j++)
    {
      std::cout << fmt[j];
    }

    std::cout << "\n";
  }
  */

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

  const auto log_time_table = []<typename T>(T, const auto &log, const auto &std_fmt)
  {
    using namespace std::chrono;

    // ANSI Color Codes
    const std::string_view RESET = "\033[0m";
    const std::string_view GREEN = "\033[32m";
    const std::string_view RED = "\033[31m";
    const std::string_view YELLOW = "\033[33m";

    auto get_colors = [&](auto v1, auto v2) -> std::pair<std::string_view, std::string_view>
    {
      if(v1 == 0 || v2 == 0)
        return { RESET, RESET }; // Avoid division by zero

      double ratio = static_cast<double>(v1) / static_cast<double>(v2);

      // Yellow if within 3% (between 0.97 and 1.03)
      if(std::abs(1.0 - ratio) <= 0.03)
        return { YELLOW, YELLOW };

      // Green for faster (smaller value), Red for slower
      if(v1 < v2)
        return { GREEN, RED };
      return { RED, GREEN };
    };

    auto print_row = [&](std::string_view label, auto val1, auto val2)
    {
      auto [c1, c2] = get_colors(val1, val2);
      // Format strings inside the colored blocks to maintain column alignment
      std::cout << std::format("  {: <15} | {}{: >15}{} | {}{: >15}{}\n", label, c1, val1, RESET, c2, val2, RESET);
    };

    // Header Logic
    std::string title = std::is_floating_point_v<T> ? " FLOATING_TYPE_V RESULTS " : " INTEGRAL_TYPE_V RESULTS ";
    std::cout << "\n" << std::format("{:=^51}", title) << "\n";
    std::cout << std::format("  {: <15} | {: >15} | {: >15}\n", "Unit", "Logger", "std::format");
    std::cout << std::string(51, '-') << "\n";

    // Data Rows
    print_row("Seconds", duration<double>(log).count(), duration<double>(std_fmt).count());
    print_row("Milliseconds", duration_cast<milliseconds>(log).count(), duration_cast<milliseconds>(std_fmt).count());
    print_row("Microseconds", duration_cast<microseconds>(log).count(), duration_cast<microseconds>(std_fmt).count());

    std::cout << std::string(51, '=') << "\n";
  };

} // namespace

namespace
{
  const auto lopper_integer = []<typename Type>(const bool &PLUS, const Type &DELIM, const Type &JUMP, auto &log_took, auto &fmt_took) -> void
  {
    const constexpr auto WISHED_RANGE = 10'000;
    const constexpr auto MAX_NUM = std::numeric_limits<Type>::max();
    const constexpr Type RANGE = WISHED_RANGE < MAX_NUM ? static_cast<Type>(WISHED_RANGE) : MAX_NUM;
    const constexpr Type MAX_ERRORS = 10;

    OpenLogging logger;
    for(Type i = DELIM, lim = 0, max_iter = 0; ((PLUS) ? i < DELIM + RANGE : i > DELIM - RANGE) && lim < MAX_ERRORS && max_iter < RANGE; (PLUS) ? i += JUMP : i -= JUMP, max_iter++)
    {

      const auto st_log = std::chrono::high_resolution_clock::now();
      const auto log = logger.format<false>("{}", i);
      const auto en_log = std::chrono::high_resolution_clock::now();

      const auto st_fmt = std::chrono::high_resolution_clock::now();
      const auto num_to_str = std::format("{}", std::to_string(i));
      const auto en_fmt = std::chrono::high_resolution_clock::now();

      log_took += std::chrono::duration_cast<std::chrono::nanoseconds>(en_log - st_log);
      fmt_took += std::chrono::duration_cast<std::chrono::nanoseconds>(en_fmt - st_fmt);

      if(log != num_to_str)
      {
        std::cout << "log == '" << log << "' AND to_str == '" << num_to_str << "'" << std::endl;
        std::cout << "log         = ";

        log_str_into_hex(log);

        std::cout << std::endl;
        std::cout << "num_to_str = ";

        log_str_into_hex(num_to_str);

        std::cout << std::endl;
        BOOST_CHECK_EQUAL(log, num_to_str);

        lim++;
      }
    }
  };

  const auto tester_integer = []<typename T>(const T & /*unused*/) -> auto
  {
    std::chrono::nanoseconds log_took{ 0 };
    std::chrono::nanoseconds fmt_took{ 0 };

    const constexpr auto MIN = std::numeric_limits<T>::min();
    const constexpr auto MAX = std::numeric_limits<T>::max();
    const constexpr T UNIT = T{ 1 };

    // ---- Extremes and Zero Region ----
    lopper_integer(true, MIN, UNIT, log_took, fmt_took);
    lopper_integer(false, MAX, UNIT, log_took, fmt_took);
    lopper_integer(true, T{ 0 }, UNIT, log_took, fmt_took);
    lopper_integer(false, T{ 0 }, UNIT, log_took, fmt_took);

    // ---- Around powers of two (Bit boundaries) ----
    for(int e = 1; e < std::numeric_limits<T>::digits; ++e)
    {
      const T val = UNIT << e;
      lopper_integer(true, val, UNIT, log_took, fmt_took);
      lopper_integer(false, val, UNIT, log_took, fmt_took);
    }

    // ---- Around powers of ten (String length boundaries) ----
    for(T val = 10; val > 0 && val < MAX / 10; val *= 10)
    {
      lopper_integer(true, val, UNIT, log_took, fmt_took);
      lopper_integer(false, val, UNIT, log_took, fmt_took);
    }

    // ---- Large magnitude sweeps (Sparse) ----
    if constexpr(sizeof(T) >= 4)
    {
      lopper_integer(true, MIN / 2, T{ 123 }, log_took, fmt_took);
      lopper_integer(false, MAX / 2, T{ 123 }, log_took, fmt_took);
    }

    // ---- Randomish coverage ----
    lopper_integer(true, static_cast<T>(MAX * 0.1), UNIT, log_took, fmt_took);
    lopper_integer(false, static_cast<T>(MAX * 0.9), UNIT, log_took, fmt_took);

    return std::make_pair(log_took, fmt_took);
  };

  const auto test_and_benchmark_int = []<typename T>
    requires std::is_integral_v<T>
  (const T &)
  {
    const auto res = tester_integer(static_cast<T>(0));

    log_time_table(0, res.first, res.second);
  };
} // namespace

BOOST_AUTO_TEST_CASE(test_all_integegral_v)
{
  test_and_benchmark_int(static_cast<int8_t>(0));
  test_and_benchmark_int(static_cast<int16_t>(0));
  test_and_benchmark_int(static_cast<int32_t>(0));
  test_and_benchmark_int(static_cast<int64_t>(0));

  test_and_benchmark_int(static_cast<uint8_t>(0));
  test_and_benchmark_int(static_cast<uint16_t>(0));
  test_and_benchmark_int(static_cast<uint32_t>(0));
  test_and_benchmark_int(static_cast<uint64_t>(0));
}

namespace
{
  const auto test_float_table = []<typename T>
    requires std::is_floating_point_v<T>
  (T)
  {
    const constexpr auto FloatTable = Constants::FloatingDigitsTable<T>();
    const auto &table = FloatTable.table;

    constexpr int BIAS = Constants::FloatingDigitsTable<T>::BIAS;
    constexpr int MAX_DIGITS10 = Constants::FloatingDigitsTable<T>::MAX_DIGITS10;
    constexpr int ACTUAL_DIGITS10 = Constants::FloatingDigitsTable<T>::ACTUAL_DIGITS10;
    constexpr auto MIN_SIG_FIGS = Constants::FloatingDigitsTable<T>::MIN_SIG_FIGS;
    constexpr auto MAX_SIG_FIGS = Constants::FloatingDigitsTable<T>::MAX_SIG_FIGS;

    const double scale = std::pow(10.0, MAX_DIGITS10);

    for(int i = 0; i < FloatTable.SIZE; ++i)
    {
      const int exp = i - BIAS;
      const auto val = table[i];

      // digit count
      const int digits = std::to_string(val).size();

      static const auto LOG_10_2 = std::log10(2.0);

      // reconstruct approximate value
      const double result = (static_cast<double>(val) / (scale)) * std::pow(10.0L, static_cast<int32_t>(std::floor(LOG_10_2 * exp)));
      const double expected = std::pow(2.0, exp);

      const double abs_error = std::abs(result - expected);
      const double rel_error = abs_error / expected;

      // bounds
      BOOST_CHECK(val >= MIN_SIG_FIGS / 10);
      BOOST_CHECK(val < MAX_SIG_FIGS / 10);
      BOOST_CHECK_EQUAL(digits, ACTUAL_DIGITS10);
      const double max_tolerance = std::pow(10, -(MAX_DIGITS10));
      // if its double we allow up to 3 micro-units; otherwise 1 micro unit
      BOOST_CHECK_SMALL(rel_error, (std::is_same_v<T, double>) ? 3 * max_tolerance : max_tolerance);

      bool log = !(val >= MIN_SIG_FIGS / 10) || !(val < MAX_SIG_FIGS / 10) || !(digits == ACTUAL_DIGITS10)
                 || !(rel_error <= ((std::is_same_v<T, double>) ? 3 * max_tolerance : max_tolerance));

      constexpr int FP_PREC = std::numeric_limits<T>::max_digits10;

      if(log)
      {
        std::cout << std::format("table[{:+4}] = {} \t| result {:.{}e} | expected {:.{}e} | rel_err {:.{}e}\n", exp, val, result, FP_PREC, expected, FP_PREC, rel_error, FP_PREC);
      }
    }
  };
} // namespace

BOOST_AUTO_TEST_CASE(test_sig_figs_of_floating_point_v_table)
{
  test_float_table(static_cast<float>(0));
  test_float_table(static_cast<double>(0));
}

namespace
{
  const auto almost_equal = []<typename Type>(const Type &type, const long double &a, const long double &b) -> bool
  {
    const auto abs_error = std::abs(a - b);
    const auto denom = std::max(std::abs(b), static_cast<long double>(std::numeric_limits<Type>::denorm_min()));
    const auto rel_error = abs_error / denom;

    const constexpr auto REL_TOL = 1.0L / Helpers::Math::Constexpr::pow(10.0L, std::numeric_limits<Type>::digits10 - 1);
    BOOST_CHECK_SMALL(rel_error, REL_TOL);
    return rel_error <= REL_TOL;
  };

  const auto lopper_format_exponential = []<typename Type>(const bool &PLUS, const Type &DELIM, const Type &JUMP, auto &log_took, auto &fmt_took) -> void
  {
    const constexpr auto WISHED_RANGE = 10'000;
    const constexpr auto MAX_NUM = std::numeric_limits<Type>::max();
    const constexpr Type RANGE = WISHED_RANGE < MAX_NUM ? WISHED_RANGE : MAX_NUM;
    const constexpr Type MAX_ERRORS = 10;

    OpenLogging logger;

    for(Type i = DELIM, lim = 0, max_iter = 0; ((PLUS) ? i < DELIM + RANGE : i > DELIM - RANGE) && lim < MAX_ERRORS && max_iter < RANGE; (PLUS) ? i += JUMP : i -= JUMP, max_iter++)
    {
      constexpr int FP_PREC = std::numeric_limits<Type>::digits10;
      const auto st_log = std::chrono::high_resolution_clock::now();
      const auto log = logger.format<false>("{}", i);
      // const auto log = Helpers::Numeric::ToStr(i);
      const auto en_log = std::chrono::high_resolution_clock::now();

      const auto st_fmt = std::chrono::high_resolution_clock::now();
      const auto num_to_str = std::format("{:.{}e}", i, FP_PREC);
      const auto en_fmt = std::chrono::high_resolution_clock::now();

      log_took += std::chrono::duration_cast<std::chrono::nanoseconds>(en_log - st_log);
      fmt_took += std::chrono::duration_cast<std::chrono::nanoseconds>(en_fmt - st_fmt);

      if(log != num_to_str)
      {
        const auto log_val = std::strtold(log.c_str(), nullptr);
        const auto ref_val = std::strtold(num_to_str.c_str(), nullptr);

        if(!almost_equal(i, log_val, ref_val))
        {
          std::cout << "log == '" << log << "' AND to_str == '" << num_to_str << "'" << std::endl;

          std::cout << "log        = ";

          log_str_into_hex(log);

          std::cout << std::endl;

          std::cout << "num_to_str = ";

          log_str_into_hex(num_to_str);

          std::cout << std::endl;

          lim++;
        }
      }
    }
  };

  const auto tester_format_exponential = []<typename T>(const T &)
  {
    const constexpr auto MIN = std::numeric_limits<T>::min();
    const constexpr auto DENORM = std::numeric_limits<T>::denorm_min();
    const constexpr auto MAX = std::numeric_limits<T>::max();
    const constexpr auto EPS = std::numeric_limits<T>::epsilon();

    std::chrono::nanoseconds log_took{ 0 };
    std::chrono::nanoseconds fmt_took{ 0 };

    // ---- small / subnormal region ----
    lopper_format_exponential(true, T{ 0 }, DENORM, log_took, fmt_took);
    lopper_format_exponential(true, MIN, DENORM, log_took, fmt_took);

    // ---- small normal numbers ----
    lopper_format_exponential(true, MIN, EPS, log_took, fmt_took);
    lopper_format_exponential(true, MIN * T{ 10 }, EPS, log_took, fmt_took);

    // ---- around powers of two ----
    for(int e = -20; e <= 20; ++e)
    {
      const T val = std::ldexp(T{ 1 }, e); // 2^e
      lopper_format_exponential(true, val, EPS * val, log_took, fmt_took);
      lopper_format_exponential(false, val, EPS * val, log_took, fmt_took);
    }

    // ---- around powers of ten ----
    for(int e = -20; e <= 20; ++e)
    {
      const T val = std::pow(T{ 10 }, e);
      lopper_format_exponential(true, val, EPS * val, log_took, fmt_took);
      lopper_format_exponential(false, val, EPS * val, log_took, fmt_took);
    }

    // ---- medium magnitude sweeps ----
    lopper_format_exponential(true, T{ 1 }, EPS, log_took, fmt_took);
    lopper_format_exponential(true, T{ 100 }, EPS * T{ 100 }, log_took, fmt_took);
    lopper_format_exponential(true, T{ 1e6 }, EPS * T{ 1e6 }, log_took, fmt_took);

    // ---- large numbers ----
    lopper_format_exponential(false, MAX, EPS * MAX, log_took, fmt_took);
    lopper_format_exponential(false, MAX / T{ 10 }, EPS * MAX, log_took, fmt_took);
    lopper_format_exponential(false, MAX / T{ 1000 }, EPS * MAX, log_took, fmt_took);

    // ---- randomish mantissa coverage ----
    lopper_format_exponential(true, T{ 1.234 }, T{ 0.0001 }, log_took, fmt_took);
    lopper_format_exponential(true, T{ 123.456 }, T{ 0.01 }, log_took, fmt_took);
    lopper_format_exponential(false, T{ 98765.4321 }, T{ 0.1 }, log_took, fmt_took);

    return std::make_pair(log_took, fmt_took);
  };

  const auto test_and_benchmark_float = []<typename T>
    requires std::is_floating_point_v<T>
  (const T &)
  {
    const auto float_res = tester_format_exponential(static_cast<T>(0));

    log_time_table(0.0, float_res.first, float_res.second);
  };

} // namespace

BOOST_AUTO_TEST_CASE(test_all_floating_point_v)
{
  test_and_benchmark_float(static_cast<float>(0));
  test_and_benchmark_float(static_cast<double>(0));

  // tester_format_exponential(static_cast<long double>(0));
}

//

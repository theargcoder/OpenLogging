#define BOOST_TEST_MODULE UnitTests
#include <boost/test/included/unit_test.hpp>

#include <cfloat>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <limits>
#include <type_traits>

#include "include/Constants/Constants.h"
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
  struct LogHexStr
  {
    std::string_view label;
    std::string_view num_str;
    LogHexStr(const std::string &_label, const std::string &_num_str) : label(_label), num_str(_num_str) {};
  };

  const auto log_str_and_into_hex = []<typename... Args>(const Args &...logs)
  {
    std::string log = "we have:";

    ((log += std::format(" {} = '{}'", logs.label, logs.num_str)), ...);

    std::cout << log << std::endl;

    auto print_hex = [](std::string_view in)
    {
      for(unsigned char c : in)
        std::cout << std::hex << (int)c << " ";

      std::cout << std::dec << '\n';
    };

    (print_hex(logs.num_str), ...);
  };

  struct BenchResult
  {
    std::string_view label;
    std::chrono::nanoseconds time;

    BenchResult(const char *str, std::chrono::nanoseconds nano) : label(str), time(nano) {};
  };

  const auto log_time_tables = []<typename T, typename... Args>(T, const Args &...times)
  {
    using namespace std::chrono;

    // ANSI Color Codes
    const std::string_view RESET = "\033[0m";
    const std::string_view GREEN = "\033[32m";
    const std::string_view RED = "\033[31m";
    const std::string_view YELLOW = "\033[33m";

    const auto SIZE = sizeof...(times);

    // Calculate average time (using double to keep precision)
    const auto total_ns = (times.time + ...).count();
    const double average = static_cast<double>(total_ns) / SIZE;

    auto get_color = [&](nanoseconds val) -> std::string_view
    {
      if(val.count() == 0)
        return RESET;

      double ratio = static_cast<double>(val.count()) / average;

      if(std::abs(1.0 - ratio) <= 0.03)
        return YELLOW;

      return (val.count() < average) ? GREEN : RED;
    };

    auto get_label_cell = [&](const BenchResult &res) { return std::format(" | {: >15}", res.label); };

    auto get_val_cell = [&](const BenchResult &res, auto unit_type)
    {
      auto color = get_color(res.time);
      auto val = duration_cast<duration<double, typename decltype(unit_type)::period>>(res.time).count();
      return std::format(" | {}{: >15.3f}{}", color, val, RESET);
    };

    std::string header_row = std::format("{:>15}", "Unit");
    ((header_row += get_label_cell(times)), ...);

    std::string row_sec = std::format("{:>15}", "Seconds");
    ((row_sec += get_val_cell(times, seconds{})), ...);

    std::string row_milli = std::format("{:>15}", "Milliseconds");
    ((row_milli += get_val_cell(times, milliseconds{})), ...);

    std::string row_micro = std::format("{:>15}", "Microseconds");
    ((row_micro += get_val_cell(times, microseconds{})), ...);

    std::string type_name = std::is_floating_point_v<T> ? "FLOAT" : "INT";
    std::string title = std::format(" {} COMPARISON (Avg: {:.3f} millisec) ", type_name, average / 1'000'000);
    int total_width = 15 + (SIZE * 18); // 15 for label + 18 per column (| + color + 15 chars)

    std::cout << "\n" << std::format("{:=^{}}", title, total_width) << "\n";
    std::cout << header_row << "\n";
    std::cout << std::string(total_width, '-') << "\n";
    std::cout << row_sec << "\n";
    std::cout << row_milli << "\n";
    std::cout << row_micro << "\n";
    std::cout << std::string(total_width, '=') << "\n";
  };

} // namespace

namespace
{
  const auto lopper_integer = []<typename Type>(const bool &PLUS, const Type &DELIM, const Type &JUMP, auto &open_logging, auto &std_to_string, auto &std_to_char) -> void
  {
    const constexpr auto WISHED_RANGE = 10'000;
    const constexpr auto MAX_NUM = std::numeric_limits<Type>::max();
    const constexpr Type RANGE = WISHED_RANGE < MAX_NUM ? static_cast<Type>(WISHED_RANGE) : MAX_NUM;
    const constexpr Type MAX_ERRORS = 10;

    // OpenLogging logger;

    for(Type i = DELIM, lim = 0, max_iter = 0; ((PLUS) ? i < DELIM + RANGE : i > DELIM - RANGE) && lim < MAX_ERRORS && max_iter < RANGE; (PLUS) ? i += JUMP : i -= JUMP, max_iter++)
    {

      const auto st_log = std::chrono::high_resolution_clock::now();
      // const auto log = logger.format("{}", i);
      const auto log = Helpers::Numeric::OpenLogging::ToStr(i);
      const auto en_log = std::chrono::high_resolution_clock::now();

      const auto st_std_to_str = std::chrono::high_resolution_clock::now();
      // const auto num_to_str = std::format("{}", i);
      const auto num_to_str = std::to_string(i);
      const auto en_std_to_str = std::chrono::high_resolution_clock::now();

      const auto st_std_to_char = std::chrono::high_resolution_clock::now();
      const auto num_to_char = Helpers::Numeric::Std::to_string(i);
      const auto en_std_to_char = std::chrono::high_resolution_clock::now();

      open_logging += std::chrono::duration_cast<std::chrono::nanoseconds>(en_log - st_log);
      std_to_string += std::chrono::duration_cast<std::chrono::nanoseconds>(en_std_to_str - st_std_to_str);
      std_to_char += std::chrono::duration_cast<std::chrono::nanoseconds>(en_std_to_char - st_std_to_char);

      if(log != num_to_str || log != num_to_char)
      {
        log_str_and_into_hex(LogHexStr("log", log), LogHexStr("num_to_str", num_to_str), LogHexStr("num_to_char", num_to_char));

        BOOST_CHECK_EQUAL(log, num_to_str);
        BOOST_CHECK_EQUAL(log, num_to_char);

        lim++;
      }
    }
  };

  const auto tester_integer = []<typename T>(const T &) -> auto
  {
    std::chrono::nanoseconds open_loggin_took{ 0 };
    std::chrono::nanoseconds std_to_string_took{ 0 };
    std::chrono::nanoseconds std_to_chars_took{ 0 };

    const constexpr auto MIN = std::numeric_limits<T>::min();
    const constexpr auto MAX = std::numeric_limits<T>::max();
    const constexpr T UNIT = T{ 1 };

    // ---- Extremes and Zero Region ----
    lopper_integer(true, MIN, UNIT, open_loggin_took, std_to_string_took, std_to_chars_took);
    lopper_integer(false, MAX, UNIT, open_loggin_took, std_to_string_took, std_to_chars_took);
    lopper_integer(true, T{ 0 }, UNIT, open_loggin_took, std_to_string_took, std_to_chars_took);
    lopper_integer(false, T{ 0 }, UNIT, open_loggin_took, std_to_string_took, std_to_chars_took);

    // ---- Around powers of two (Bit boundaries) ----
    for(int e = 1; e < std::numeric_limits<T>::digits; ++e)
    {
      const T val = UNIT << e;
      lopper_integer(true, val, UNIT, open_loggin_took, std_to_string_took, std_to_chars_took);
      lopper_integer(false, val, UNIT, open_loggin_took, std_to_string_took, std_to_chars_took);
    }

    // ---- Around powers of ten (String length boundaries) ----
    for(T val = 10; val > 0 && val < MAX / 10; val *= 10)
    {
      lopper_integer(true, val, UNIT, open_loggin_took, std_to_string_took, std_to_chars_took);
      lopper_integer(false, val, UNIT, open_loggin_took, std_to_string_took, std_to_chars_took);
    }

    // ---- Large magnitude sweeps (Sparse) ----
    if constexpr(sizeof(T) >= 4)
    {
      lopper_integer(true, MIN / 2, T{ 123 }, open_loggin_took, std_to_string_took, std_to_chars_took);
      lopper_integer(false, MAX / 2, T{ 123 }, open_loggin_took, std_to_string_took, std_to_chars_took);
    }

    // ---- Randomish coverage ----
    lopper_integer(true, static_cast<T>(MAX * 0.1), UNIT, open_loggin_took, std_to_string_took, std_to_chars_took);
    lopper_integer(false, static_cast<T>(MAX * 0.9), UNIT, open_loggin_took, std_to_string_took, std_to_chars_took);

    return std::make_tuple(open_loggin_took, std_to_string_took, std_to_chars_took);
  };

  const auto test_and_benchmark_int = []<typename T>
    requires std::is_integral_v<T>
  (const T &)
  {
    const auto res = tester_integer(static_cast<T>(0));

    log_time_tables(0, BenchResult("OpenLogging", std::get<0>(res)), BenchResult("std::to_string", std::get<1>(res)), BenchResult("std::to_char", std::get<2>(res)));
  };
} // namespace

BOOST_AUTO_TEST_CASE(test_all_integegral_v)
{
  test_and_benchmark_int(static_cast<int8_t>(0));
  test_and_benchmark_int(static_cast<uint8_t>(0));
  test_and_benchmark_int(static_cast<int16_t>(0));
  test_and_benchmark_int(static_cast<uint16_t>(0));
  test_and_benchmark_int(static_cast<int32_t>(0));
  test_and_benchmark_int(static_cast<uint32_t>(0));
  test_and_benchmark_int(static_cast<int64_t>(0));
  test_and_benchmark_int(static_cast<uint64_t>(0));
}

namespace
{
  const auto test_float_table = []<typename T>
    requires std::is_floating_point_v<T>
  (T)
  {
    const auto FloatTable = Constants::Tables::Floating<T>();
    const auto &table = FloatTable.DIGITS;

    constexpr int BIAS = Constants::Tables::Floating<T>::BIAS;
    constexpr int MAX_DIGITS10 = Constants::Tables::Floating<T>::MAX_DIGITS10;

    const double scale = std::pow(10.0, MAX_DIGITS10);

    for(int i = 0; i < FloatTable.SIZE; ++i)
    {
      const int exp = i - BIAS;
      const auto val = table[i];

      // digit count
      const int digits = std::to_string(val).size();

      static const auto LOG_10_2 = std::log10(2.0);

      // reconstruct approximate value
      const long double result = (static_cast<double>(val) / (scale)) * std::pow(10.0L, static_cast<int32_t>(std::floor(LOG_10_2 * exp)));
      const long double expected = std::pow(2.0, exp);

      const long double abs_error = std::abs(result - expected);
      const long double rel_error = abs_error / expected;

      // bounds
      BOOST_CHECK_EQUAL(digits, FloatTable.ACTUAL_DIGITS10);
      const long double max_tolerance = std::pow(static_cast<long double>(10), static_cast<long double>(-(MAX_DIGITS10))); // 1.5 micro unit max tolerance
      BOOST_CHECK_SMALL(rel_error, max_tolerance);

      bool log = !(digits == FloatTable.ACTUAL_DIGITS10) || !(rel_error <= max_tolerance);

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

    const constexpr auto REL_TOL = 1.1 * (1.0L / Helpers::Math::Constexpr::pow(10.0L, std::numeric_limits<Type>::digits10 - 1));
    BOOST_CHECK_SMALL(rel_error, REL_TOL);
    return rel_error <= REL_TOL;
  };

  const auto lopper_format_exponential
      = []<typename Type>(const bool &PLUS, const Type &DELIM, const Type &JUMP, auto &open_logging_took, auto &std_fmt_took, auto &ryu_took) -> void
  {
    const constexpr auto WISHED_RANGE = 10'000;
    const constexpr auto MAX_NUM = std::numeric_limits<Type>::max();
    const constexpr Type RANGE = WISHED_RANGE < MAX_NUM ? WISHED_RANGE : MAX_NUM;
    const constexpr Type MAX_ERRORS = 10;

    // OpenLogging logger;

    for(Type i = DELIM, lim = 0, max_iter = 0; ((PLUS) ? i < DELIM + RANGE : i > DELIM - RANGE) && lim < MAX_ERRORS && max_iter < RANGE; (PLUS) ? i += JUMP : i -= JUMP, max_iter++)
    {
      std::string open_logging, std_format, ryu;

      const auto st_open_logging = std::chrono::high_resolution_clock::now();

      // if constexpr(std::is_same_v<Type, double>) { log = logger.format("{15}", i); } else { six in reality should be 5 log = logger.format("{6}", i); }
      open_logging = Helpers::Numeric::OpenLogging::ToStr(i);

      const auto en_open_logging = std::chrono::high_resolution_clock::now();

      const auto st_std_fmt = std::chrono::high_resolution_clock::now();

      std_format = Helpers::Numeric::Std::to_string(i);

      const auto en_std_fmt = std::chrono::high_resolution_clock::now();

      const auto st_ryu = std::chrono::high_resolution_clock::now();

      ryu = Helpers::Numeric::Ryu::ToStr(i);

      const auto en_ryu = std::chrono::high_resolution_clock::now();

      open_logging_took += std::chrono::duration_cast<std::chrono::nanoseconds>(en_open_logging - st_open_logging);
      std_fmt_took += std::chrono::duration_cast<std::chrono::nanoseconds>(en_std_fmt - st_std_fmt);
      ryu_took += std::chrono::duration_cast<std::chrono::nanoseconds>(en_ryu - st_ryu);

      if(open_logging != std_format)
      {
        const auto log_val = std::strtold(open_logging.c_str(), nullptr);
        const auto ref_val = std::strtold(std_format.c_str(), nullptr);

        if(!almost_equal(i, log_val, ref_val))
        {
          log_str_and_into_hex(LogHexStr("open_logging", open_logging), LogHexStr("std::format", std_format), LogHexStr("ryu", ryu));

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

    std::chrono::nanoseconds open_logging_time{ 0 };
    std::chrono::nanoseconds std_fmt_time{ 0 };
    std::chrono::nanoseconds ryu_time{ 0 };

    // ---- small / subnormal region ----
    lopper_format_exponential(true, T{ 0 }, DENORM, open_logging_time, std_fmt_time, ryu_time);
    lopper_format_exponential(true, MIN, DENORM, open_logging_time, std_fmt_time, ryu_time);

    // ---- small normal numbers ----
    lopper_format_exponential(true, MIN, EPS, open_logging_time, std_fmt_time, ryu_time);
    lopper_format_exponential(true, MIN * T{ 10 }, EPS, open_logging_time, std_fmt_time, ryu_time);

    // ---- around powers of two ----
    for(int e = -20; e <= 20; ++e)
    {
      const T val = std::ldexp(T{ 1 }, e); // 2^e
      lopper_format_exponential(true, val, EPS * val, open_logging_time, std_fmt_time, ryu_time);
      lopper_format_exponential(false, val, EPS * val, open_logging_time, std_fmt_time, ryu_time);
    }

    // ---- around powers of ten ----
    for(int e = -20; e <= 20; ++e)
    {
      const T val = std::pow(T{ 10 }, e);
      lopper_format_exponential(true, val, EPS * val, open_logging_time, std_fmt_time, ryu_time);
      lopper_format_exponential(false, val, EPS * val, open_logging_time, std_fmt_time, ryu_time);
    }

    // ---- medium magnitude sweeps ----
    lopper_format_exponential(true, T{ 1 }, EPS, open_logging_time, std_fmt_time, ryu_time);
    lopper_format_exponential(true, T{ 100 }, EPS * T{ 100 }, open_logging_time, std_fmt_time, ryu_time);
    lopper_format_exponential(true, T{ 1e6 }, EPS * T{ 1e6 }, open_logging_time, std_fmt_time, ryu_time);

    // ---- large numbers ----
    lopper_format_exponential(false, MAX, EPS * MAX, open_logging_time, std_fmt_time, ryu_time);
    lopper_format_exponential(false, MAX / T{ 10 }, EPS * MAX, open_logging_time, std_fmt_time, ryu_time);
    lopper_format_exponential(false, MAX / T{ 1000 }, EPS * MAX, open_logging_time, std_fmt_time, ryu_time);

    // ---- randomish mantissa coverage ----
    lopper_format_exponential(true, T{ 1.234 }, T{ 0.0001 }, open_logging_time, std_fmt_time, ryu_time);
    lopper_format_exponential(true, T{ 123.456 }, T{ 0.01 }, open_logging_time, std_fmt_time, ryu_time);
    lopper_format_exponential(false, T{ 98765.4321 }, T{ 0.1 }, open_logging_time, std_fmt_time, ryu_time);

    return std::make_tuple(open_logging_time, std_fmt_time, ryu_time);
  };

  const auto test_and_benchmark_float = []<typename T>
    requires std::is_floating_point_v<T>
  (const T &)
  {
    const auto float_res = tester_format_exponential(static_cast<T>(0));

    log_time_tables(0.0, BenchResult("OpenLogging", std::get<0>(float_res)), BenchResult("std::to_chars", std::get<1>(float_res)), BenchResult("ryu", std::get<2>(float_res)));
  };

} // namespace

BOOST_AUTO_TEST_CASE(test_all_floating_point_v)
{
  test_and_benchmark_float(static_cast<float>(0));
  test_and_benchmark_float(static_cast<double>(0));

  // tester_format_exponential(static_cast<long double>(0));
}

//

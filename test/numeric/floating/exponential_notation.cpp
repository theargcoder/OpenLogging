#define BOOST_TEST_MODULE ScientificNotatioTests
#include <boost/test/included/unit_test.hpp>

#include <chrono>
#include <cmath>
#include <limits>
#include <type_traits>

#include "include/Algos/Competition.h"
#include "include/Algos/Floating/Exponential.h"

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
  const auto almost_equal = []<typename Type>(const Type &, const long double &a, const long double &b) -> bool
  {
    const auto abs_error = std::abs(a - b);
    const auto denom = std::max(std::abs(b), static_cast<long double>(std::numeric_limits<Type>::denorm_min()));
    const auto rel_error = abs_error / denom;

    const constexpr auto REL_TOL = 1.0L / Helpers::Math::Constexpr::pow(10.0L, std::numeric_limits<Type>::digits10 - 1);
    BOOST_CHECK_SMALL(rel_error, REL_TOL);
    return rel_error <= REL_TOL;
  };

  const auto lopper_format_exponential
      = []<typename Type>(const int &PRECISION, const bool &PLUS, const Type &DELIM, const Type &JUMP, auto &open_logging_took, auto &std_fmt_took, auto &ryu_took) -> void
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
      open_logging = Helpers::Numeric::Floating::ExponentialNotation::ToStr(i, PRECISION);

      const auto en_open_logging = std::chrono::high_resolution_clock::now();

      const auto st_std_fmt = std::chrono::high_resolution_clock::now();

      std_format = Helpers::Numeric::Std::to_string<true>(i, PRECISION);

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

          open_logging = Helpers::Numeric::Floating::ExponentialNotation::ToStr(i, PRECISION);
          lim++;
        }
      }
    }
  };

  const auto tester_format_exponential = []<typename T>(const T &, const int &PRECISION)
  {
    const constexpr auto MIN = std::numeric_limits<T>::min();
    const constexpr auto DENORM = std::numeric_limits<T>::denorm_min();
    const constexpr auto MAX = std::numeric_limits<T>::max();
    const constexpr auto EPS = std::numeric_limits<T>::epsilon();

    std::chrono::nanoseconds open_logging_time{ 0 };
    std::chrono::nanoseconds std_fmt_time{ 0 };
    std::chrono::nanoseconds ryu_time{ 0 };

    // ---- small / subnormal region ----
    lopper_format_exponential(PRECISION, true, T{ 0 }, DENORM, open_logging_time, std_fmt_time, ryu_time);
    lopper_format_exponential(PRECISION, true, MIN, DENORM, open_logging_time, std_fmt_time, ryu_time);

    // ---- small normal numbers ----
    lopper_format_exponential(PRECISION, true, MIN, EPS, open_logging_time, std_fmt_time, ryu_time);
    lopper_format_exponential(PRECISION, true, MIN * T{ 10 }, EPS, open_logging_time, std_fmt_time, ryu_time);

    // ---- around powers of two ----
    for(int e = -20; e <= 20; ++e)
    {
      const T val = std::ldexp(T{ 1 }, e); // 2^e
      lopper_format_exponential(PRECISION, true, val, EPS * val, open_logging_time, std_fmt_time, ryu_time);
      lopper_format_exponential(PRECISION, false, val, EPS * val, open_logging_time, std_fmt_time, ryu_time);
    }

    // ---- around powers of ten ----
    for(int e = -20; e <= 20; ++e)
    {
      const T val = std::pow(T{ 10 }, e);
      lopper_format_exponential(PRECISION, true, val, EPS * val, open_logging_time, std_fmt_time, ryu_time);
      lopper_format_exponential(PRECISION, false, val, EPS * val, open_logging_time, std_fmt_time, ryu_time);
    }

    // ---- medium magnitude sweeps ----
    lopper_format_exponential(PRECISION, true, T{ 1 }, EPS, open_logging_time, std_fmt_time, ryu_time);
    lopper_format_exponential(PRECISION, true, T{ 100 }, EPS * T{ 100 }, open_logging_time, std_fmt_time, ryu_time);
    lopper_format_exponential(PRECISION, true, T{ 1e6 }, EPS * T{ 1e6 }, open_logging_time, std_fmt_time, ryu_time);

    // ---- large numbers ----
    lopper_format_exponential(PRECISION, false, MAX, EPS * MAX, open_logging_time, std_fmt_time, ryu_time);
    lopper_format_exponential(PRECISION, false, MAX / T{ 10 }, EPS * MAX, open_logging_time, std_fmt_time, ryu_time);
    lopper_format_exponential(PRECISION, false, MAX / T{ 1000 }, EPS * MAX, open_logging_time, std_fmt_time, ryu_time);

    // ---- randomish mantissa coverage ----
    lopper_format_exponential(PRECISION, true, T{ 1.234 }, T{ 0.0001 }, open_logging_time, std_fmt_time, ryu_time);
    lopper_format_exponential(PRECISION, true, T{ 123.456 }, T{ 0.01 }, open_logging_time, std_fmt_time, ryu_time);
    lopper_format_exponential(PRECISION, false, T{ 98765.4321 }, T{ 0.1 }, open_logging_time, std_fmt_time, ryu_time);

    return std::make_tuple(open_logging_time, std_fmt_time, ryu_time);
  };

  const auto test_and_benchmark_float = []<typename T>
    requires std::is_floating_point_v<T>
  (const T &, const int &PRECISION)
  {
    const auto float_res = tester_format_exponential(static_cast<T>(0), PRECISION);

    log_time_tables(0.0, BenchResult("OpenLogging", std::get<0>(float_res)), BenchResult("std::to_chars", std::get<1>(float_res)), BenchResult("ryu", std::get<2>(float_res)));
  };

} // namespace

BOOST_AUTO_TEST_CASE(test_all_floating_point_v)
{
  // floats
  test_and_benchmark_float(static_cast<float>(0), 1);
  test_and_benchmark_float(static_cast<float>(0), 2);
  test_and_benchmark_float(static_cast<float>(0), 3);
  test_and_benchmark_float(static_cast<float>(0), 4);
  test_and_benchmark_float(static_cast<float>(0), 5);
  test_and_benchmark_float(static_cast<float>(0), 6);
  // doubles
  test_and_benchmark_float(static_cast<double>(0), 1);
  test_and_benchmark_float(static_cast<double>(0), 2);
  test_and_benchmark_float(static_cast<double>(0), 3);
  test_and_benchmark_float(static_cast<double>(0), 4);
  test_and_benchmark_float(static_cast<double>(0), 5);
  test_and_benchmark_float(static_cast<double>(0), 6);
  test_and_benchmark_float(static_cast<double>(0), 7);
  test_and_benchmark_float(static_cast<double>(0), 8);
  test_and_benchmark_float(static_cast<double>(0), 9);
  test_and_benchmark_float(static_cast<double>(0), 10);
  test_and_benchmark_float(static_cast<double>(0), 11);
  test_and_benchmark_float(static_cast<double>(0), 12);
  test_and_benchmark_float(static_cast<double>(0), 13);
  test_and_benchmark_float(static_cast<double>(0), 14);
  test_and_benchmark_float(static_cast<double>(0), 15);
  test_and_benchmark_float(static_cast<double>(0), 16);
  test_and_benchmark_float(static_cast<double>(0), 17);

  // tester_format_exponential(static_cast<long double>(0));
}
///

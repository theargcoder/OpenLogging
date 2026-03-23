#define BOOST_TEST_MODULE IntegerTests
#include <boost/test/included/unit_test.hpp>

#include <chrono>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <limits>
#include <type_traits>

#include "include/Algos/Competition.h"
#include "include/Algos/Integer.h"

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

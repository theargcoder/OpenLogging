#include <utility>
#define BOOST_TEST_MODULE IntegerTests
#include <boost/test/included/unit_test.hpp>

#include <chrono>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <limits>
#include <string>
#include <type_traits>

#include "include/Helpers/Math.h"

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
  template <uint64_t N, typename Type>
  auto looper_magic_division(const bool &PLUS, const Type &DELIM, const Type &JUMP, auto &open_logging, auto &std_to_string) -> void
  {
    const constexpr auto WISHED_RANGE = 100'000;
    const constexpr auto MAX_NUM = std::numeric_limits<Type>::max();
    const constexpr Type RANGE = WISHED_RANGE < MAX_NUM ? static_cast<Type>(WISHED_RANGE) : MAX_NUM;
    const constexpr Type MAX_ERRORS = 10;

    // OpenLogging logger;

    Type divisor = Helpers::Math::Constexpr::ipow(Type{ 10 }, N);

    for(Type i = DELIM, lim = 0, max_iter = 0; ((PLUS) ? i < DELIM + RANGE : i > DELIM - RANGE) && lim < MAX_ERRORS && max_iter < RANGE; (PLUS) ? i += JUMP : i -= JUMP, max_iter++)
    {
      const auto st_log = std::chrono::high_resolution_clock::now();
      const auto our_div_10 = Helpers::Math::Magic::Division::div_by_10_pow_n<N>(i);
      const auto en_log = std::chrono::high_resolution_clock::now();

      const auto st_std_to_str = std::chrono::high_resolution_clock::now();
      const auto regular_div_10 = i / divisor;
      const auto en_std_to_str = std::chrono::high_resolution_clock::now();

      open_logging += std::chrono::duration_cast<std::chrono::nanoseconds>(en_log - st_log);
      std_to_string += std::chrono::duration_cast<std::chrono::nanoseconds>(en_std_to_str - st_std_to_str);

      if(our_div_10 != regular_div_10)
      {
        BOOST_CHECK_EQUAL(our_div_10, regular_div_10);
        log_str_and_into_hex(LogHexStr("Helpers::Math::Magic::div_by_10_denominator", std::to_string(our_div_10)), LogHexStr("regular IDIV got", std::to_string(regular_div_10)));

        const auto for_debug = Helpers::Math::Magic::Division::div_by_10_denominator(i, divisor);

        lim++;
      }
    }
  };

  template <uint64_t N, typename Type>
  auto looper_magic_modulus(const bool &PLUS, const Type &DELIM, const Type &JUMP, auto &open_logging, auto &std_to_string) -> void
  {
    const constexpr auto WISHED_RANGE = 100'000;
    const constexpr auto MAX_NUM = std::numeric_limits<Type>::max();
    const constexpr Type RANGE = WISHED_RANGE < MAX_NUM ? static_cast<Type>(WISHED_RANGE) : MAX_NUM;
    const constexpr Type MAX_ERRORS = 10;

    // OpenLogging logger;

    Type divisor = Helpers::Math::Constexpr::ipow(Type{ 10 }, N);

    for(Type i = DELIM, lim = 0, max_iter = 0; ((PLUS) ? i < DELIM + RANGE : i > DELIM - RANGE) && lim < MAX_ERRORS && max_iter < RANGE; (PLUS) ? i += JUMP : i -= JUMP, max_iter++)
    {
      const auto st_log = std::chrono::high_resolution_clock::now();
      const auto our_div_10 = Helpers::Math::Magic::Modulo::mod_by_10_pow_n<N>(i);
      const auto en_log = std::chrono::high_resolution_clock::now();

      const auto st_std_to_str = std::chrono::high_resolution_clock::now();
      const auto regular_div_10 = i % divisor;
      const auto en_std_to_str = std::chrono::high_resolution_clock::now();

      open_logging += std::chrono::duration_cast<std::chrono::nanoseconds>(en_log - st_log);
      std_to_string += std::chrono::duration_cast<std::chrono::nanoseconds>(en_std_to_str - st_std_to_str);

      if(our_div_10 != regular_div_10)
      {
        BOOST_CHECK_EQUAL(our_div_10, regular_div_10);
        log_str_and_into_hex(LogHexStr("Helpers::Math::Magic::div_by_10_denominator", std::to_string(our_div_10)), LogHexStr("regular IDIV got", std::to_string(regular_div_10)));

        const auto for_debug = Helpers::Math::Magic::Division::div_by_10_denominator(i, divisor);

        lim++;
      }
    }
  };

  template <uint64_t N, typename T>
  auto tester_magic_division(const T &) -> auto
  {
    std::chrono::nanoseconds helpers_math_magic_took{ 0 };
    std::chrono::nanoseconds regular_idiv_instruction_took{ 0 };

    const constexpr auto MIN = std::numeric_limits<T>::min();
    const constexpr auto MAX = std::numeric_limits<T>::max();
    const constexpr T UNIT = T{ 1 };

    // ---- Extremes and Zero Region ----
    looper_magic_division<N>(true, MIN, UNIT, helpers_math_magic_took, regular_idiv_instruction_took);
    looper_magic_division<N>(false, MAX, UNIT, helpers_math_magic_took, regular_idiv_instruction_took);
    looper_magic_division<N>(true, T{ 0 }, UNIT, helpers_math_magic_took, regular_idiv_instruction_took);
    looper_magic_division<N>(false, T{ 0 }, UNIT, helpers_math_magic_took, regular_idiv_instruction_took);

    // ---- Around powers of two (Bit boundaries) ----
    for(int e = 1; e < std::numeric_limits<T>::digits; ++e)
    {
      const T val = UNIT << e;
      looper_magic_division<N>(true, val, UNIT, helpers_math_magic_took, regular_idiv_instruction_took);
      looper_magic_division<N>(false, val, UNIT, helpers_math_magic_took, regular_idiv_instruction_took);
    }

    // ---- Around powers of ten (String length boundaries) ----
    for(T val = 10; val > 0 && val < MAX / 10; val *= 10)
    {
      looper_magic_division<N>(true, val, UNIT, helpers_math_magic_took, regular_idiv_instruction_took);
      looper_magic_division<N>(false, val, UNIT, helpers_math_magic_took, regular_idiv_instruction_took);
    }

    // ---- Large magnitude sweeps (Sparse) ----
    if constexpr(sizeof(T) >= 4)
    {
      looper_magic_division<N>(true, MIN / 2, T{ 123 }, helpers_math_magic_took, regular_idiv_instruction_took);
      looper_magic_division<N>(false, MAX / 2, T{ 123 }, helpers_math_magic_took, regular_idiv_instruction_took);
    }

    // ---- Randomish coverage ----
    looper_magic_division<N>(true, static_cast<T>(MAX * 0.1), UNIT, helpers_math_magic_took, regular_idiv_instruction_took);
    looper_magic_division<N>(false, static_cast<T>(MAX * 0.9), UNIT, helpers_math_magic_took, regular_idiv_instruction_took);

    return std::make_tuple(helpers_math_magic_took, regular_idiv_instruction_took);
  }

  template <uint64_t N, typename T>
  auto tester_magic_modulus(const T &) -> auto
  {
    std::chrono::nanoseconds helpers_math_magic_took{ 0 };
    std::chrono::nanoseconds regular_idiv_instruction_took{ 0 };

    const constexpr auto MIN = std::numeric_limits<T>::min();
    const constexpr auto MAX = std::numeric_limits<T>::max();
    const constexpr T UNIT = T{ 1 };

    // ---- Extremes and Zero Region ----
    looper_magic_modulus<N>(true, MIN, UNIT, helpers_math_magic_took, regular_idiv_instruction_took);
    looper_magic_modulus<N>(false, MAX, UNIT, helpers_math_magic_took, regular_idiv_instruction_took);
    looper_magic_modulus<N>(true, T{ 0 }, UNIT, helpers_math_magic_took, regular_idiv_instruction_took);
    looper_magic_modulus<N>(false, T{ 0 }, UNIT, helpers_math_magic_took, regular_idiv_instruction_took);

    // ---- Around powers of two (Bit boundaries) ----
    for(int e = 1; e < std::numeric_limits<T>::digits; ++e)
    {
      const T val = UNIT << e;
      looper_magic_modulus<N>(true, val, UNIT, helpers_math_magic_took, regular_idiv_instruction_took);
      looper_magic_modulus<N>(false, val, UNIT, helpers_math_magic_took, regular_idiv_instruction_took);
    }

    // ---- Around powers of ten (String length boundaries) ----
    for(T val = 10; val > 0 && val < MAX / 10; val *= 10)
    {
      looper_magic_modulus<N>(true, val, UNIT, helpers_math_magic_took, regular_idiv_instruction_took);
      looper_magic_modulus<N>(false, val, UNIT, helpers_math_magic_took, regular_idiv_instruction_took);
    }

    // ---- Large magnitude sweeps (Sparse) ----
    if constexpr(sizeof(T) >= 4)
    {
      looper_magic_modulus<N>(true, MIN / 2, T{ 123 }, helpers_math_magic_took, regular_idiv_instruction_took);
      looper_magic_modulus<N>(false, MAX / 2, T{ 123 }, helpers_math_magic_took, regular_idiv_instruction_took);
    }

    // ---- Randomish coverage ----
    looper_magic_modulus<N>(true, static_cast<T>(MAX * 0.1), UNIT, helpers_math_magic_took, regular_idiv_instruction_took);
    looper_magic_modulus<N>(false, static_cast<T>(MAX * 0.9), UNIT, helpers_math_magic_took, regular_idiv_instruction_took);

    return std::make_tuple(helpers_math_magic_took, regular_idiv_instruction_took);
  };

  template <typename T, size_t... I>
    requires std::is_integral_v<T>
  const auto test_and_benchmark_div_magic_impl(std::index_sequence<I...>)
  {
    auto res = tester_magic_division<1>(T{ 0 });
    ((res = tester_magic_division<I + 1>(T{ 0 }), log_time_tables(0, BenchResult("div_by_10_denom", std::get<0>(res)), BenchResult("IDIV instr", std::get<1>(res)))), ...);
  };

  template <typename T>
    requires std::is_integral_v<T>
  const auto test_and_benchmark_div_magic(T)
  {
    test_and_benchmark_div_magic_impl<T>(std::make_index_sequence<std::numeric_limits<T>::digits10>{});
  };

  template <typename T, size_t... I>
    requires std::is_integral_v<T>
  const auto test_and_benchmark_mod_magic_impl(std::index_sequence<I...>)
  {
    auto res = tester_magic_modulus<1>(T{ 0 });
    ((res = tester_magic_modulus<I + 1>(T{ 0 }), log_time_tables(0, BenchResult("mod_by_10_denom", std::get<0>(res)), BenchResult("IMOD instr", std::get<1>(res)))), ...);
  };

  template <typename T>
    requires std::is_integral_v<T>
  const auto test_and_benchmark_mod_magic(T)
  {
    test_and_benchmark_mod_magic_impl<T>(std::make_index_sequence<std::numeric_limits<T>::digits10>{});
  };
} // namespace

BOOST_AUTO_TEST_CASE(test_all_integegral_v)
{
  test_and_benchmark_div_magic<uint32_t>(0);
  test_and_benchmark_mod_magic<uint32_t>(0);
  test_and_benchmark_div_magic<uint64_t>(0);
  test_and_benchmark_mod_magic<uint64_t>(0);
}

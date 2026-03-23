#define BOOST_TEST_MODULE ConstantsTests
#include <boost/test/included/unit_test.hpp>

#include <cstdint>
#include <iostream>
#include <limits>
#include <type_traits>

#include "include/Constants/Constants.h"

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

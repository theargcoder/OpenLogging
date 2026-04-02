#define BOOST_TEST_MODULE ConstantsTests
#include <boost/multiprecision/cpp_bin_float.hpp> // Add this header
#include <boost/test/included/unit_test.hpp>

#include <cmath>
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
    constexpr int ACTUAL_DIGITS10 = Constants::Tables::Floating<T>::ACTUAL_DIGITS10;

    // Use Boost's 100-decimal-digit precision float for the test bounds
    using BigFloat = boost::multiprecision::cpp_bin_float_100;

    const BigFloat LOG_10_2_BF("0.301029995663981195213738894724493026768"); // Hardcoded for exactness or calculate via Boost

    for(int i = 0; i < FloatTable.SIZE; ++i)
    {
      const int exp = i - BIAS;
      const auto &val = table[i];
      const int digits = static_cast<int>(boost::multiprecision::log10((BigFloat{ val }))) + 1;
      const BigFloat scale = boost::multiprecision::pow(BigFloat(10), digits - 1);

      // Calculate power of 10 safely
      int32_t p10 = static_cast<int32_t>(std::floor(0.3010299956639812 * exp)); // Standard double is fine for the integer exponent

      // Reconstruct approximate value using BigFloat
      BigFloat result = (static_cast<BigFloat>(val) / scale) * boost::multiprecision::pow(BigFloat(10), p10);
      BigFloat expected = boost::multiprecision::pow(BigFloat(2), exp);

      BigFloat abs_error = boost::multiprecision::abs(result - expected);
      BigFloat rel_error = abs_error / expected;

      // Bounds testing
      BOOST_CHECK_EQUAL(digits, FloatTable.ACTUAL_DIGITS10);

      // Convert back to standard double for the BOOST_CHECK if needed, or just use Boost's native comparisons.
      double rel_error_dbl = static_cast<T>(rel_error);
      double max_tolerance = std::pow(T{ 10.0 }, -MAX_DIGITS10);

      BOOST_CHECK_SMALL(rel_error_dbl, max_tolerance);

      bool log = !(digits == FloatTable.ACTUAL_DIGITS10) || !(rel_error_dbl <= max_tolerance);

      if(log)
      {
        constexpr int FP_PREC = std::numeric_limits<T>::max_digits10;
        // Note: You may need to format BigFloat as a string for std::format
        std::cout << std::format("table[{:+4}] = {} \t| rel_err {:.{}e}\n", exp, val, rel_error_dbl, FP_PREC);
      }
    }
  };
} // namespace

BOOST_AUTO_TEST_CASE(test_sig_figs_of_floating_point_v_table)
{
  test_float_table(static_cast<float>(0));
  test_float_table(static_cast<double>(0));
}

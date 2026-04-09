#define BOOST_TEST_MODULE ConstantsTests

#include <boost/multiprecision/cpp_bin_float.hpp>
#include <boost/test/included/unit_test.hpp>

#include <cmath>
#include <cstdint>
#include <iostream>
#include <limits>
#include <type_traits>

#include "include/Constants/Constants.h"

namespace
{
  namespace Tests
  {
    template <typename T>
      requires(std::is_integral_v<T> || std::is_same_v<T, __uint128_t>)
    static int log10(T x)
    {
      int digits = 0;
      while(x >= 10)
      {
        x /= 10;
        ++digits;
      }
      return digits;
    }

    template <typename T>
      requires std::is_integral_v<T>
    static T pow(T base, int exp)
    {
      T result = 1;
      while(exp > 0)
      {
        if(exp & 1)
        {
          result *= base;
        }

        base *= base;
        exp >>= 1;
      }
      return result;
    }
  }

  const auto test_float_table = []<typename T>
    requires std::is_floating_point_v<T>
  (T)
  {
    using FloatTable = Constants::Tables::Floating<T>;
    auto &table = Constants::Tables::Floating<T>::DIGITS;

    constexpr int BIAS = Constants::Tables::Floating<T>::BIAS;

    // Use Boost's 100-decimal-digit precision float for the test bounds
    using BigFloat = boost::multiprecision::cpp_bin_float_100;

    const BigFloat LOG_10_2_BF("0.301029995663981195213738894724493026768"); // Hardcoded for exactness or calculate via Boost

    for(int i = 0; i < FloatTable::SIZE; ++i)
    {
      const int exp = i - BIAS;
      using hig_type = std::remove_cvref_t<decltype(table[i].hig)>;
      using low_type = std::remove_cvref_t<decltype(table[i].low)>;
      const __uint128_t val = static_cast<__uint128_t>(table[i].hig) * Tests::pow(__uint128_t{ 10 }, std::numeric_limits<low_type>::digits10 - 1) + table[i].low;
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
      BOOST_CHECK_EQUAL(digits, 22);

      // Convert back to standard double for the BOOST_CHECK if needed, or just use Boost's native comparisons.
      BigFloat max_tolerance = boost::multiprecision::pow(BigFloat{ 10.0 }, -1 * (std::numeric_limits<hig_type>::digits10 + std::numeric_limits<low_type>::digits10) + 2);

      BOOST_CHECK_SMALL(rel_error, max_tolerance);

      bool log = !(digits == 22) || !(rel_error <= max_tolerance);

      if(log)
      {
        constexpr int FP_PREC = std::numeric_limits<T>::max_digits10;
        // Note: You may need to format BigFloat as a string for std::format
        std::cout << std::format("table[{:+4}] = {} \t| result {} | expected {}| rel_err {}\n", exp, val, result.str(), expected.str(), rel_error.str(), FP_PREC);
      }
    }
  };
} // namespace

BOOST_AUTO_TEST_CASE(test_sig_figs_of_floating_point_v_table)
{
  test_float_table(static_cast<float>(0));
  test_float_table(static_cast<double>(0));
}

/**
 * Calculates the first N digits of 2^exp.
 * @tparam N The number of leading digits to extract (max 38 for __uint128_t).
 * @param exp The exponent for the power of 2.
 * @return The first N digits as a raw 128-bit integer.
 */

/*
template <int N>
__uint128_t get_leading_digits(int exp)
{
  static_assert(N > 0 && N <= 38, "N must be between 1 and 38 digits to fit in __uint128_t.");

  using namespace boost::multiprecision;

  // Use 100-decimal-digit precision to ensure N digits remain accurate
  // even for very large exponents.
  using BigFloat = cpp_bin_float_100;

  // 1. Calculate 2^exp
  BigFloat value = pow(BigFloat(2), exp);

  // 2. Determine the base-10 magnitude
  // log10(value) gives us the exponent. floor() tells us how many
  // digits exist before the decimal (minus 1).
  int magnitude = floor(log10(value)).convert_to<int>();

  // 3. Scale the value to "shift" the first N digits into the integer part
  // Formula: value * 10^((N-1) - magnitude)
  // If magnitude is larger than N, this multiplies by a negative power (divides).
  BigFloat shifted = value * pow(BigFloat(10), (N - 1) - magnitude);

  // 4. Convert to __uint128_t
  // We convert to boost's uint128_t first, then cast to the native GCC/Clang type.
  return static_cast<__uint128_t>(shifted.convert_to<uint128_t>());
}

BOOST_AUTO_TEST_CASE(bannananana)
{
  std::cout << "{";
  for(int i = Constants::Tables::Floating<double>::MIN_BIN_EXP; i <= Constants::Tables::Floating<double>::MAX_BIN_EXP; i++)
  {
    // 19 + 3 = 22
    const auto res = get_leading_digits<22>(i);
    const auto res_int_big = static_cast<uint64_t>(res / 1'000);
    const auto res_rem = static_cast<uint16_t>(res % 1'000);

    std::cout << '{' << std::format("{}ULL,{}", res_int_big, res_rem) << "},";
  }
  std::cout << "\b};";
}
*/

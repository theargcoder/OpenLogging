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
    const auto table = Constants::Tables::Floating<T>::DIGITS;
    // Use Boost's 100-decimal-digit precision float for the test bounds
    using BigFloat = boost::multiprecision::cpp_bin_float_100;

    const BigFloat LOG_10_2_BF("0.301029995663981195213738894724493026768"); // Hardcoded for exactness or calculate via Boost

    for(int i = Constants::Tables::Floating<float>::MIN_BIN_EXP; i <= Constants::Tables::Floating<float>::MAX_BIN_EXP; i++)
    {
      const auto idx = i + Constants::Tables::Floating<T>::BIAS;
      const __uint128_t val = static_cast<__uint128_t>(table[idx].hig) * Tests::pow(__uint128_t{ 10 }, 18)
                              + static_cast<__uint128_t>(table[idx].mid) * Tests::pow(__uint128_t{ 10 }, 9) + table[idx].low;
      const int digits = static_cast<int>(boost::multiprecision::log10((BigFloat{ val }))) + 1;
      const BigFloat scale = boost::multiprecision::pow(BigFloat(10), digits - 1);

      // Calculate power of 10 safely
      int32_t p10 = static_cast<int32_t>(std::floor(0.3010299956639812 * i)); // Standard double is fine for the integer exponent

      // Reconstruct approximate value using BigFloat
      BigFloat result = (static_cast<BigFloat>(val) / scale) * boost::multiprecision::pow(BigFloat(10), p10);
      BigFloat expected = boost::multiprecision::pow(BigFloat(2), i);

      BigFloat abs_error = boost::multiprecision::abs(result - expected);
      BigFloat rel_error = abs_error / expected;

      // Bounds testing
      BOOST_CHECK_EQUAL(digits, 9 + 9 + 9);

      // Convert back to standard double for the BOOST_CHECK if needed, or just use Boost's native comparisons.
      BigFloat max_tolerance = boost::multiprecision::pow(BigFloat{ 10.0 }, -1 * (27 - 1));

      BOOST_CHECK_SMALL(rel_error, max_tolerance);

      bool log = !(digits == 27) || !(rel_error <= max_tolerance);

      if(log)
      {
        constexpr int FP_PREC = std::numeric_limits<T>::max_digits10;
        // Note: You may need to format BigFloat as a string for std::format
        std::cout << std::format("table[{:+4}] = {} \t| result {} | expected {}| rel_err {}\n", i, val, result.str(), expected.str(), rel_error.str(), FP_PREC);
      }
    }
  };
} // namespace

BOOST_AUTO_TEST_CASE(test_sig_figs_of_floating_point_v_table)
{
  test_float_table(static_cast<float>(0));
  test_float_table(static_cast<double>(0));
}

/*
auto mul3_128b(const uint64_t &mantissa, const uint32_t &m_high, const uint32_t &m_mid, const uint32_t &m_low)
{
  static const constexpr __uint128_t SHIFT53 = (__uint128_t)1 << 53;
  static const constexpr __uint128_t MASK53 = SHIFT53 - 1;

  static const constexpr __uint128_t DEC7 = 10'000'000ULL;
  static const constexpr __uint128_t DEC9 = 1'000'000'000ULL;
  static const constexpr __uint128_t DEC18 = DEC9 * DEC9;

  const __uint128_t p_hi = static_cast<__uint128_t>(mantissa) * m_high;
  const __uint128_t p_mid = static_cast<__uint128_t>(mantissa) * m_mid;
  const __uint128_t p_low = static_cast<__uint128_t>(mantissa) * m_low;
  const __uint128_t carry = (((p_hi & MASK53) * DEC18) + ((p_mid & MASK53) * DEC9) + (p_low & MASK53)) >> 53;

  const uint32_t q_hi = p_hi >> 53;
  const uint32_t q_mid = p_mid >> 53;
  const uint32_t q_low = p_low >> 53;

  const __uint128_t total = static_cast<__uint128_t>(q_hi) * DEC18 + static_cast<uint64_t>(q_mid) * DEC9 + q_low + carry;

  const uint64_t top_18_digits = static_cast<uint64_t>(total / DEC7);

  return top_18_digits;
}

auto mul2_128b(const uint64_t &mantissa, const uint32_t &m_high, const uint32_t &m_low)
{
  static const constexpr __uint128_t DEC9 = 1'000'000'000ULL;
  const unsigned __int128 mul = static_cast<__uint128_t>(mantissa) * m_high * DEC9 + static_cast<__uint128_t>(mantissa) * m_low;

  return static_cast<uint64_t>(mul >> 53);
}

struct table_3_way
{
  uint32_t hig;
  uint32_t mid;
  uint32_t low;
};

BOOST_AUTO_TEST_CASE(multiplytest)
{
  const uint64_t mantissa = 6646139978835021;
  table_3_way tablevals = { 135525271, 560688054, 250931600 };

  {
    auto expected_truncated = mul2_128b(mantissa, tablevals.hig, tablevals.mid);
    BOOST_CHECK_EQUAL(expected_truncated, 99'999'999'998'652'475); //'500'019'082);
  }
  {
    const auto expected_truncated = mul3_128b(mantissa, tablevals.hig, tablevals.mid, tablevals.low);
    BOOST_CHECK_EQUAL(expected_truncated, 99999'99999'86524'7550ULL); //'500'019'082);
  }
}
*/

/*
template <typename T>
static boost::multiprecision::cpp_int pow10_int(int n)
{
  using boost::multiprecision::cpp_int;
  cpp_int r = 1;
  while(n-- > 0)
    r *= 10;
  return r;
}

template <int Begin, int End>
std::string get_digits_range(int exp)
{
  static_assert(Begin >= 0, "Begin must be >= 0");
  static_assert(End > Begin, "End must be > Begin");

  constexpr std::size_t Len = End - Begin;
  using BigInt = boost::multiprecision::cpp_int;

  // Exact integer math to prevent floating-point anomalies.
  // 2^exp for positive, 5^(|exp|) for negative exact significant digits.
  BigInt exact_val = (exp >= 0) ? boost::multiprecision::pow(BigInt(2), exp) : boost::multiprecision::pow(BigInt(5), -exp);

  std::string s = exact_val.convert_to<std::string>();

  // Pad with trailing zeros to guarantee we can slice up to 'End'
  if(s.length() < static_cast<std::size_t>(End))
  {
    s.append(static_cast<std::size_t>(End) - s.length(), '0');
  }

  // Extract the requested range.
  // If Begin == 0, there are no unnatural leading zeros.
  // If Begin > 0, the substr naturally catches any zeros padded out by smaller chunks.
  int i = Begin;
  for(; i < End - 1; i++)
  {
    if(s[i] == '0')
    {
      continue;
    }
    break;
  }
  return s.substr(i, End - i);
}

BOOST_AUTO_TEST_CASE(bannananana)
{
  std::cout << "{";
  for(int i = Constants::Tables::Floating<double>::MIN_BIN_EXP; i <= Constants::Tables::Floating<double>::MAX_BIN_EXP; i++)
  {
    std::cout << '{';
    {
      const auto res = get_digits_range<0, 9>(i);
      std::cout << std::format("{}U,", res);
    }
    {
      const auto res = get_digits_range<9, 18>(i);
      std::cout << std::format("{}U,", res);
    }
    {
      const auto res = get_digits_range<18, 27>(i);
      std::cout << std::format("{}U", res);
    }

    std::cout << "},";
  }
  std::cout << "\b};";
}
*/

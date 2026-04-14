#include <istream>
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
      requires std::is_integral_v<T> || std::is_same_v<T, __uint128_t>
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
      const __uint128_t val = static_cast<__uint128_t>(table[idx][0]) * Tests::pow(__uint128_t{ 10 }, 18)
                              + static_cast<__uint128_t>(table[idx][1]) * Tests::pow(__uint128_t{ 10 }, 9) + table[idx][2];
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

static inline auto umulh32(const uint64_t &a, const uint32_t &b)
{
  return (uint32_t)(a * b >> 32);
}

static inline auto umulh64(const uint64_t &a, const uint64_t &b)
{
  return (uint64_t)((__uint128_t)a * b >> 64);
}

template <typename Type>
  requires std::is_same_v<uint32_t, Type>
auto mul_ret_remainder(const Type &mantissa, const uint32_t &m_high, const uint32_t &m_mid, const uint32_t &m_low)
{
  static const constexpr uint32_t DEC9 = 1'000'000'000U;

  const uint64_t MANTISSA_MAX = static_cast<uint64_t>(mantissa) << (32 - 23);

  const uint32_t p_hi_bottom = MANTISSA_MAX * m_high;
  const uint32_t p_hi_bottom_1e9 = umulh32(p_hi_bottom, DEC9);
  const uint32_t p_mid_top = umulh32(MANTISSA_MAX, m_mid);
  const uint32_t p_mid_bottom = MANTISSA_MAX * m_mid;
  const uint32_t p_mid_bottom_1e9 = umulh32(p_mid_bottom, DEC9);
  const uint32_t p_low_top = umulh32(MANTISSA_MAX, m_low);

  const uint64_t first_9_digits = umulh32(MANTISSA_MAX, m_high);
  const uint32_t next_9_digits = p_mid_top + p_hi_bottom_1e9;

  return first_9_digits;
}

static inline uint32_t top_digit(uint32_t n)
{
  return (uint64_t(n) * 0x55E63B89ULL) >> 57;
}

template <typename Type>
  requires std::is_same_v<uint64_t, Type>
auto mul_ret_remainder(const Type &mantissa, const uint32_t &m_high, const uint32_t &m_mid, const uint32_t &m_low)
{
  static const constexpr uint64_t DEC10 = 10'000'000'000ULL;
  static const constexpr uint64_t DEC9 = 1'000'000'000ULL;
  static const constexpr uint64_t DEC8 = 100'000'000ULL;

  const auto MANTISSA_MAX = mantissa << (64 - 53);

  const uint64_t AABC = static_cast<uint64_t>(m_high) * DEC9 + m_mid;
  const uint64_t CCDKS = (MANTISSA_MAX)*AABC;
  const uint32_t KJDLD = static_cast<uint32_t>(umulh64(CCDKS, DEC9));
  const uint32_t KDJSLFJ = static_cast<uint32_t>(umulh64(MANTISSA_MAX, m_low));

  const uint64_t AFIRST118 = umulh64(MANTISSA_MAX, AABC);
  const uint32_t NEXT9DIGI = KDJSLFJ + KJDLD;

  {
    const uint32_t low_top_digit = top_digit(m_high);
    const uint32_t low_part = m_low - (low_top_digit * DEC9);

    const uint64_t m_high_mid = static_cast<uint64_t>(m_high) * DEC10 + static_cast<uint64_t>(m_mid) * 10 + low_top_digit;
    const uint64_t p_hi_mid_bottom = (MANTISSA_MAX)*m_high_mid;
    const uint32_t p_hi_mid_rem_times_1e9 = static_cast<uint32_t>(umulh64(p_hi_mid_bottom, DEC8));
    const uint32_t p_low_top = static_cast<uint32_t>(umulh64(MANTISSA_MAX, low_part));

    uint64_t result = umulh64(MANTISSA_MAX, m_high_mid);
    uint32_t next_9_digits = p_low_top + p_hi_mid_rem_times_1e9;

    if(next_9_digits >= DEC8)
    {
      result++;
      next_9_digits -= DEC8;
    }
  }
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

  {
    const uint64_t mantissa = 6646139978835021;
    table_3_way tablevals = { 135525271, 560688054, 250931600 };
    auto expected_truncated = mul2_128b(mantissa, tablevals.hig, tablevals.mid);
    BOOST_CHECK_EQUAL(expected_truncated, 99'999'999'998'652'475); //'500'019'082);
  }
  {
    const uint64_t mantissa = 6646139978835021;
    table_3_way tablevals = { 135525271, 560688054, 250931600 };
    mul_ret_remainder(mantissa, tablevals.hig, tablevals.mid, tablevals.low);
    BOOST_CHECK_EQUAL(1, 99999'99999'86524'7550ULL); //'500'019'082);
  }
  {
    const uint32_t mantissa = 8388608;
    table_3_way tablevals = { .hig = 140129846, .mid = 432481707, .low = 92372958 };
    mul_ret_remainder(mantissa, tablevals.hig, tablevals.mid, tablevals.low);
    BOOST_CHECK_EQUAL(1, 99999'99999'86524'7550ULL); //'500'019'082);
  }
  {
    const uint32_t mantissa = 16759306;
    table_3_way tablevals = { .hig = 152587890, .mid = 625000000, .low = 0 };
    mul_ret_remainder(mantissa, tablevals.hig, tablevals.mid, tablevals.low);
    BOOST_CHECK_EQUAL(1, 99999'99999'86524'7550ULL); //'500'019'082);
  }
}

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

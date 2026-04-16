#include "include/Helpers/Math.h"
#include <istream>
#define BOOST_TEST_MODULE ConstantsTests

#if defined(_MSC_VER) || defined(__x86_64__) || defined(__i386__)
#include <immintrin.h> // x86 SIMD
#elif defined(__ARM_NEON) || defined(__aarch64__)
#include <arm_neon.h> // ARM SIMD
#endif

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

__attribute__((always_inline)) static uint64_t umul64hi(const uint64_t &a, const uint64_t &b)
{
#if defined(__x86_64__)
  uint64_t hi;
  uint64_t lo = a;
  asm("mul %[b]" : "+a"(lo), "=d"(hi) : [b] "r"(b) : "cc");
  return hi;
#elif defined(__aarch64__)
  uint64_t hi;
  asm("umulh %0, %1, %2" : "=r"(hi) : "r"(a), "r"(b));
  return hi;
#else
  return (uint64_t)((__uint128_t)a * b >> 64);
#endif
}

template <typename Type>
  requires std::is_same_v<uint64_t, Type>
auto mul_carry(const Type &mantissa, const uint32_t &m_to_multiply, uint32_t &digits, uint64_t &remainder)
{
  static const constexpr uint32_t DEC9 = 1'000'000'000U;

  // 1. Calculate the carry coming down from the previous stage's remainder
  // (If this is the first call, 'remainder' is 0, so these cleanly become 0)
  const uint32_t carry_down = umul64hi(remainder, DEC9);
  const uint32_t local_digits = umul64hi(mantissa, m_to_multiply);
  const uint64_t rem_shifted = remainder * DEC9;
  const uint64_t local_rem = mantissa * m_to_multiply;

  digits = local_digits + carry_down;
  remainder = local_rem + rem_shifted;

  if(remainder < local_rem)
  {
    digits++;
  }
}

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

__attribute__((always_inline)) static auto umul_hi_32x4_t(const uint32x4_t &v_a, const uint32x4_t &v_b) noexcept
{
  const uint64x2_t prod_low = vmull_u32(vget_low_u32(v_a), vget_low_u32(v_b));
  const uint64x2_t prod_high = vmull_high_u32(v_a, v_b);

  // Re-combine the high 32-bits of each 64-bit result into one 128-bit vector This is essentially 'mulhi'
  return vcombine_u32(vshrn_n_u64(prod_low, 32), vshrn_n_u64(prod_high, 32));
}

__attribute__((always_inline)) static auto umul_low_32x4_t(const uint32x4_t &v_a, const uint32x4_t &v_b) noexcept
{
  return vmulq_u32(v_a, v_b);
}

static auto simdy_to_str(const uint32_t &input)
{
  static const constexpr uint32x4_t v_magics_10e1_10e4 = { 0xD1B71759ULL, 0x10624DD3ULL, 0x51EB851FULL, 0xCCCCCCCDULL };
  static const constexpr int32x4_t v_shifts = { -13, -6, -5, -3 };

  static const constexpr uint32_t magic_div_10 = 0xCCCCCCCDULL;
  static const constexpr int32_t magic_div_10_shift = -3;

  static const constexpr uint32x4_t v_10s = { 10U, 10U, 10U, 10U };

  char buff[32];

  uint32_t top_val = 12'340, bottom_val = 56780;

  // vdupq_n_u32 is the NEON equivalent of _mm_set1_epi32
  const uint32x4_t v_top = vdupq_n_u32(top_val);
  const uint32x4_t v_btm = vdupq_n_u32(bottom_val);

  const uint32x4_t v_prod_top = umul_hi_32x4_t(v_top, v_magics_10e1_10e4);
  const uint32x4_t v_prod_btm = umul_hi_32x4_t(v_btm, v_magics_10e1_10e4);

  const uint32x4_t v_div_top = vshlq_u32(v_prod_top, v_shifts);
  const uint32x4_t v_div_btm = vshlq_u32(v_prod_btm, v_shifts);

  const uint32x4_t v_div_by_10_top = vshlq_u32(umul_hi_32x4_t(v_div_top, vdupq_n_u32(magic_div_10)), vdupq_n_u32(magic_div_10_shift));
  const uint32x4_t v_div_by_10_btm = vshlq_u32(umul_hi_32x4_t(v_div_btm, vdupq_n_u32(magic_div_10)), vdupq_n_u32(magic_div_10_shift));

  const uint32x4_t v_div_by_10_mul_10_top = umul_low_32x4_t(v_div_by_10_top, v_10s);
  const uint32x4_t v_div_by_10_mul_10_btm = umul_low_32x4_t(v_div_by_10_btm, v_10s);

  const uint32x4_t res_top = vsubq_u32(v_div_top, v_div_by_10_mul_10_top);
  const uint32x4_t res_bottom = vsubq_u32(v_div_btm, v_div_by_10_mul_10_btm);

  // 1. Narrow each 32x4 to 16x4
  const uint16x4_t top_16 = vmovn_u32(res_top);       // [d0, d1, d2, d3] in 16-bit
  const uint16x4_t bottom_16 = vmovn_u32(res_bottom); // [d4, d5, d6, d7] in 16-bit

  // 2. Combine top and bottom into one 16x8 register
  const uint16x8_t combined_16 = vcombine_u16(top_16, bottom_16);

  // 3. Narrow 16x8 to 8x8
  const uint8x8_t digits_8 = vmovn_u16(combined_16); // [d0, d1, d2, d3, d4, d5, d6, d7] in 8-bit

  // 4. Convert to ASCII by adding '0' (0x30)
  const uint8x8_t ascii_8 = vadd_u8(digits_8, vdup_n_u8('0'));

  // 5. Write to char buffer in one op
  // vst1_u8 writes 8 bytes (64 bits) from an 8x8 register
  vst1_u8(reinterpret_cast<uint8_t *>(buff), ascii_8);
}

BOOST_AUTO_TEST_CASE(SimDVecorization)
{
  simdy_to_str(123'456'789);
}

// 'results' now contains [i/10, i/100, i/1000, i/10000]}

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

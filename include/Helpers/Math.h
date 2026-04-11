#pragma once

#include <bit>
#include <cstdint>
#include <limits>
#include <numbers>
#include <type_traits>

namespace Helpers::Math::Constexpr
{
  template <typename T>
  static constexpr T abs(T x)
  {
    return x < 0 ? -x : x;
  }

  template <typename BaseType, typename ExpType>
    requires std::is_integral_v<ExpType>
  static constexpr BaseType ipow(BaseType base, ExpType exp)
  {
    if(exp < 0)
    {
      if constexpr(std::is_floating_point_v<BaseType>)
      {
        base = BaseType{ 1 } / base;
        exp = -exp;
      }
      else
      {
        return 0;
      }
    }

    BaseType res = 1;
    while(exp > 0)
    {
      if(exp % 2 == 1)
      {
        res *= base;
      }

      exp /= 2;

      // FIX: Prevent squaring base on the final step to avoid constexpr overflow!
      if(exp > 0)
      {
        base *= base;
      }
    }
    return res;
  }

  // --- Natural Logarithm (ln x) ---
  template <typename T>
  static constexpr T ln(T x)
  {
    if(x <= 0)
    {
      return std::numeric_limits<T>::quiet_NaN();
    }
    if(x == 1)
    {
      return 0;
    }

    int k = 0;
    while(x > 1.5)
    {
      x /= 2;
      k++;
    }
    while(x < 0.75)
    {
      x *= 2;
      k--;
    }

    T z = x;
    const constexpr T ln2 = std::numbers::ln2_v<long double>;

    T y = (z - 1) / (z + 1);
    T y2 = y * y;
    T term = y;
    T sum = y;
    for(int i = 3; i < 70; i += 2)
    {
      term *= y2;
      sum += term / i;
    }
    return 2 * sum + (T)k * ln2;
  }

  // --- Exponential (e^x) ---
  template <typename T>
  static constexpr T exp(T x)
  {
    if(x == 0)
    {
      return 1;
    }

    const constexpr T ln2 = std::numbers::ln2_v<T>;
    int k = static_cast<int>(x / ln2);
    T f = x - static_cast<T>(k) * ln2;

    T term = 1;
    T sum = 1;
    for(int i = 1; i < 50; ++i)
    {
      term *= f / i;
      sum += term;
    }

    // FIXED: ipow now handles negatives cleanly
    return sum * ipow(T{ 2 }, k);
  }

  // --- The Full Pow (Floating Point Exponent) ---
  template <typename T>
    requires std::is_floating_point_v<T>
  static constexpr T pow(T base, T exp_val)
  {
    if(exp_val == 0)
    {
      return 1;
    }
    if(base == 0)
    {
      return 0;
    }
    if(base < 0)
    {
      auto i_exp = static_cast<long long>(exp_val);
      if(static_cast<T>(i_exp) == exp_val)
      {
        T res = exp(exp_val * ln(abs(base)));
        return (i_exp % 2 == 0) ? res : -res;
      }
      return std::numeric_limits<T>::quiet_NaN();
    }

    return exp(exp_val * ln(base));
  }

  template <typename BaseType, typename ExpType>
  static consteval BaseType pow(BaseType base, ExpType exp_val)
  {
    return ipow(base, exp_val);
  }

  template <typename T>
    requires(std::is_integral_v<T> || std::is_same_v<T, __uint128_t>)
  static consteval int log10(T x)
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
  static consteval T pow(T base, int exp)
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

  template <typename T>
    requires std::is_floating_point_v<T>
  static consteval T log10(T x)
  {
    if(x <= 0)
    {
      return std::numeric_limits<T>::quiet_NaN();
    }
    if(x == 1)
    {
      return 0;
    }

    // Multiply by 1 / ln(10)
    const constexpr T inv_ln10 = std::numbers::log10e_v<T>;

    return ln(x) * inv_ln10;
  }

} // namespace Helpers::Math::Constexpr

namespace Helpers::Math
{
  template <typename T>
    requires std::is_floating_point_v<T> && std::numeric_limits<T>::is_iec559
  struct IEEE754;

  template <>
  struct IEEE754<float>
  {
  private:
    using underlying = uint32_t;
    using signed_underlying = int32_t;
    using uint128_t = __uint128_t;

    static constexpr underlying EXPONENT_ONLY = 0x7F800000U;
    static constexpr signed_underlying EXPONENT_ST = 23;
    static constexpr signed_underlying EXPONENT_LEFT_OFFSET = sizeof(float) * 8 - EXPONENT_ST - 1;

    static constexpr signed_underlying MIN_EXPONENT = std::numeric_limits<float>::min_exponent - std::numeric_limits<float>::digits;
    static constexpr signed_underlying EXPONENT_TABLE_OFFSET = std::numeric_limits<double>::min_exponent - std::numeric_limits<double>::digits;
    static constexpr signed_underlying EXPONENT_TABLE_BIAS = -EXPONENT_TABLE_OFFSET + MIN_EXPONENT + EXPONENT_ST;

    static constexpr signed_underlying EXPONENT_ALL_BITS_ON = 255; // as defined in IEEE-754

    static constexpr underlying MANTISSA_ONLY = 0x007FFFFFU;
    static constexpr underlying MANTISSA_IMPLICIT_1 = underlying{ 1 } << EXPONENT_ST;

    static constexpr underlying SIGN_ONLY = 0x80000000U;

    static const constexpr uint128_t half = uint128_t{ 1 } << EXPONENT_ST;
    static const constexpr auto shift = EXPONENT_ST + 1;
    static const constexpr uint128_t frac_mask = ((uint128_t{ 1 } << shift) - 1);

  public:
    underlying mantissa;
    int exponent;

  public:
    explicit IEEE754(const float &input)
    {
      const underlying bits = std::bit_cast<underlying>(input);

      const underlying man = bits & MANTISSA_ONLY;

      const signed_underlying exp = ((bits & EXPONENT_ONLY) >> EXPONENT_ST);

      if(exp >= EXPONENT_ALL_BITS_ON) [[unlikely]]
      {
        const underlying SIGN = bits & SIGN_ONLY;
        exponent = std::numeric_limits<decltype(exponent)>::max();
        mantissa = (man == 0) ? (SIGN) ? 2 : 1 : 0;

        return;
      }

      if(exp > 0) [[likely]]
      {
        mantissa = man | MANTISSA_IMPLICIT_1;
        exponent = exp + EXPONENT_TABLE_BIAS;
      }
      else
      {
        const int shift_internal = std::countl_zero(man) - EXPONENT_LEFT_OFFSET;

        if(shift_internal <= EXPONENT_ST) [[likely]]
        {
          mantissa = ((man << shift_internal) & MANTISSA_ONLY) | MANTISSA_IMPLICIT_1;
          exponent = 1 - shift_internal + EXPONENT_TABLE_BIAS;
        }
        else
        {
          mantissa = std::numeric_limits<decltype(mantissa)>::max();
          exponent = std::numeric_limits<decltype(exponent)>::max();
        }
      }
    }

  public:
    static auto Multiply(const float &A, const auto &B)
    {
      const uint128_t A_bits = std::bit_cast<underlying>(A);

      const uint128_t sig = (A_bits & MANTISSA_ONLY) | MANTISSA_IMPLICIT_1;

      const uint128_t prod = uint128_t{ sig } * uint128_t{ B.hig };

      return static_cast<uint128_t>(prod >> shift);
    }

  public:
    auto mul3_128b(const auto *table, int &base_10_exponent) const noexcept
    {
      static const constexpr __uint128_t SHIFT53 = (__uint128_t)1 << 53;
      static const constexpr __uint128_t MASK53 = SHIFT53 - 1;

      static const constexpr __uint128_t DEC8 = 100'000'000ULL;
      static const constexpr __uint128_t DEC9 = 1'000'000'000ULL;
      static const constexpr __uint128_t DEC18 = DEC9 * DEC9;

      static const constexpr __uint128_t min_precision = Helpers::Math::Constexpr::ipow(__uint128_t{ 10 }, 26);

      const __uint128_t p_hi = static_cast<__uint128_t>(this->mantissa) * table->hig;
      const __uint128_t p_mid = static_cast<__uint128_t>(this->mantissa) * table->mid;
      const __uint128_t p_low = static_cast<__uint128_t>(this->mantissa) * table->low;
      const __uint128_t carry = (((p_hi & MASK53) * DEC18) + ((p_mid & MASK53) * DEC9) + (p_low & MASK53)) >> 53;

      const uint32_t q_hi = p_hi >> 53;
      const uint32_t q_mid = p_mid >> 53;
      const uint32_t q_low = p_low >> 53;

      __uint128_t total = static_cast<__uint128_t>(q_hi) * DEC18 + static_cast<uint64_t>(q_mid) * DEC9 + q_low + carry;
      if(total < min_precision)
      {
        total *= 10;
        base_10_exponent--;
      }

      return static_cast<uint64_t>(total / DEC8);
    }
  };

  template <>
  struct IEEE754<double>
  {
  private:
    using underlying = uint64_t;
    using signed_underlying = int64_t;
    using uint128_t = __uint128_t;

    static constexpr underlying EXPONENT_ONLY = 0x7FF0000000000000ULL;
    static constexpr signed_underlying EXPONENT_ST = 52;
    static constexpr signed_underlying EXPONENT_LEFT_OFFSET = sizeof(double) * 8 - EXPONENT_ST - 1;

    static constexpr signed_underlying MIN_EXPONENT = std::numeric_limits<double>::min_exponent - std::numeric_limits<double>::digits;

    static constexpr signed_underlying EXPONENT_ALL_BITS_ON = 2047; // as defined in IEEE-754

    static constexpr underlying MANTISSA_ONLY = 0x000FFFFFFFFFFFFFULL;
    static constexpr underlying MANTISSA_IMPLICIT_1 = underlying{ 1 } << EXPONENT_ST;

    static constexpr underlying SIGN_ONLY = 0x8000000000000000ULL;

    static const constexpr uint128_t half = uint128_t{ 1 } << EXPONENT_ST;
    static const constexpr auto shift = EXPONENT_ST + 1;
    static const constexpr uint128_t frac_mask = ((uint128_t{ 1 } << shift) - 1);

  public:
    underlying mantissa;
    int exponent;

  public:
    explicit IEEE754(const double &input)
    {
      const underlying bits = std::bit_cast<underlying>(input);

      const underlying man = bits & MANTISSA_ONLY;

      const signed_underlying exp = ((bits & EXPONENT_ONLY) >> EXPONENT_ST);

      if(exp >= EXPONENT_ALL_BITS_ON) [[unlikely]]
      {
        const underlying SIGN = bits & SIGN_ONLY;
        exponent = std::numeric_limits<decltype(exponent)>::max();
        mantissa = (man == 0) ? (SIGN) ? 2 : 1 : 0;
      }

      if(exp > 0) [[likely]]
      {
        mantissa = man | MANTISSA_IMPLICIT_1;
        exponent = exp + EXPONENT_ST;
      }
      else
      {
        const int shift_internal = std::countl_zero(man) - EXPONENT_LEFT_OFFSET;

        if(shift_internal <= EXPONENT_ST) [[likely]]
        {
          mantissa = ((man << shift_internal) & MANTISSA_ONLY) | MANTISSA_IMPLICIT_1;
          exponent = 1 + EXPONENT_ST - shift_internal;
        }
        else
        {
          mantissa = std::numeric_limits<decltype(mantissa)>::max();
          exponent = std::numeric_limits<decltype(exponent)>::max();
        }
      }
    }

  public:
    auto mul3_128b(const auto *table, int &base_10_exponent) const noexcept
    {
      static const constexpr __uint128_t SHIFT53 = (__uint128_t)1 << 53;
      static const constexpr __uint128_t MASK53 = SHIFT53 - 1;

      static const constexpr __uint128_t DEC8 = 100'000'000ULL;
      static const constexpr __uint128_t DEC9 = 1'000'000'000ULL;
      static const constexpr __uint128_t DEC18 = DEC9 * DEC9;

      static const constexpr __uint128_t min_precision = Helpers::Math::Constexpr::ipow(__uint128_t{ 10 }, 26);

      const __uint128_t p_hi = static_cast<__uint128_t>(this->mantissa) * table->hig;
      const __uint128_t p_mid = static_cast<__uint128_t>(this->mantissa) * table->mid;
      const __uint128_t p_low = static_cast<__uint128_t>(this->mantissa) * table->low;
      const __uint128_t carry = (((p_hi & MASK53) * DEC18) + ((p_mid & MASK53) * DEC9) + (p_low & MASK53)) >> 53;

      const uint32_t q_hi = p_hi >> 53;
      const uint32_t q_mid = p_mid >> 53;
      const uint32_t q_low = p_low >> 53;

      __uint128_t total = static_cast<__uint128_t>(q_hi) * DEC18 + static_cast<uint64_t>(q_mid) * DEC9 + q_low + carry;
      if(total < min_precision)
      {
        total *= 10;
        base_10_exponent--;
      }

      return static_cast<uint64_t>(total / DEC8);
    }

  public:
    static auto Multiply(const double &A, const auto &B)
    {
      const uint128_t A_bits = std::bit_cast<underlying>(A);

      const uint128_t sig = (A_bits & MANTISSA_ONLY) | MANTISSA_IMPLICIT_1;

      const uint128_t prod = uint128_t{ sig } * uint128_t{ B.hig };

      return static_cast<uint128_t>(prod >> shift);
    }
  };
} // namespace Helpers::Math

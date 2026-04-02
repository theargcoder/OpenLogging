#pragma once

#include <bit>
#include <cstdint>
#include <limits>
#include <numbers>
#include <type_traits>
#include <utility>

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
    requires std::is_integral_v<ExpType>
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
  struct IEEE754
  {
  private:
    using underlying = std::conditional_t<std::is_same_v<T, float>, uint32_t, uint64_t>;
    using signed_underlying = std::conditional_t<std::is_same_v<T, float>, int32_t, int64_t>;

    static constexpr bool IS_DOUBLE = std::is_same_v<T, double>;

    static constexpr underlying EXPONENT_ONLY = IS_DOUBLE ? 0x7FF0000000000000ULL : 0x7F800000U;
    static constexpr underlying EXPONENT_ST = IS_DOUBLE ? 52 : 23;
    static constexpr signed_underlying EXPONENT_LEFT_OFFSET = sizeof(T) * 8 - EXPONENT_ST - 1;
    static constexpr signed_underlying EXPONENT_BIAS = std::numeric_limits<T>::max_exponent - 1;

    static constexpr underlying BIAS_IN_EXP_POS = EXPONENT_BIAS << ((IS_DOUBLE) ? 52 : 23);

    static constexpr signed_underlying MIN_EXPONENT = std::numeric_limits<T>::min_exponent - std::numeric_limits<T>::digits;

    static constexpr signed_underlying EXPONENT_ALL_BITS_ON = IS_DOUBLE ? 2046 : 255; // as defined in IEEE-754

    static constexpr underlying MANTISSA_ONLY = IS_DOUBLE ? 0x000FFFFFFFFFFFFFULL : 0x007FFFFFU;
    static constexpr underlying MANTISSA_IMPLICIT_1 = underlying{ 1 } << EXPONENT_ST;

    static constexpr underlying SIGN_ONLY = IS_DOUBLE ? 0x8000000000000000ULL : 0x80000000U;

    static constexpr underlying HALF_EXP = (EXPONENT_BIAS - 1) << EXPONENT_ST; // half == 0.5 aka 2^-1

    using wide_underlying = std::conditional_t<IS_DOUBLE, __uint128_t, uint64_t>;

  public:
    T mantissa;
    signed_underlying exponent;

  public:
    explicit IEEE754(const T &input)
    {
      const underlying bits = std::bit_cast<underlying>(input);

      const underlying man = bits & MANTISSA_ONLY;

      const signed_underlying exp = ((bits & EXPONENT_ONLY) >> EXPONENT_ST);

      const underlying SIGN = bits & SIGN_ONLY;

      if(exp == EXPONENT_ALL_BITS_ON) [[unlikely]]
      {
        exponent = std::numeric_limits<signed_underlying>::max();
        (man == underlying(0)) ? mantissa = (SIGN) ? T{ -1 } : T{ 1 } : mantissa = T{ 0 };

        return;
      }

      if(exp > 0) [[likely]]
      {
        mantissa = std::bit_cast<T>(man | SIGN | HALF_EXP);
        exponent = exp + 1 - EXPONENT_BIAS;
      }
      else
      {
        const int shift = std::countl_zero(man) - EXPONENT_LEFT_OFFSET;

        mantissa = std::bit_cast<T>(((man << shift) & MANTISSA_ONLY) | SIGN | HALF_EXP);
        exponent = 2 - EXPONENT_BIAS - shift;

        if(exponent == MIN_EXPONENT && mantissa) [[unlikely]]
        {
          mantissa = -2;
          exponent = std::numeric_limits<signed_underlying>::max();
        }
      }
    }

  public:
    static auto Multiply(const T &A, const underlying &B)
    {
      const underlying A_bits = std::bit_cast<underlying>(A);

      underlying sig = A_bits & MANTISSA_ONLY;

      sig |= (underlying{ 1 } << EXPONENT_ST);

      const wide_underlying prod = wide_underlying{ sig } * wide_underlying{ B };

      int shift = static_cast<int>(EXPONENT_ST + 1);

      underlying digits_10;

      if(shift >= 0)
      {
        digits_10 = static_cast<underlying>(prod >> shift);
      }
      else
      {
        digits_10 = static_cast<underlying>(prod << (-shift));
      }

      return digits_10;
    }

  public:
    enum RoundingResults : uint8_t
    {
      EXACT,
      INEXACT_TRUNCATED, // Decimal is SMALLER than binary bits (True remainder > 0.5)
      INEXACT_ROUNDED_UP // when remainder > half (aka 0.5), it has leftover precision
    };

  public:
    static auto MultiplyRound(const T &A, const underlying &B)
    {
      const underlying A_bits = std::bit_cast<underlying>(A);

      underlying sig = A_bits & MANTISSA_ONLY;

      sig |= (underlying{ 1 } << EXPONENT_ST);

      const wide_underlying prod = wide_underlying{ sig } * wide_underlying{ B };

      int shift = static_cast<int>(EXPONENT_ST + 1);

      underlying digits_10;
      wide_underlying remainder;

      if(shift >= 0)
      {
        digits_10 = static_cast<underlying>(prod >> shift);
        // remainder = prod - digits_10;
        remainder = (prod & ((wide_underlying{ 1 } << shift) - 1));
      }
      else
      {
        digits_10 = static_cast<underlying>(prod << (-shift));
        remainder = wide_underlying{ 0 };
      }

      if(remainder == 0)
      {
        return std::make_pair(RoundingResults::EXACT, digits_10);
      }

      if(shift >= 0)
      {
        const wide_underlying half = wide_underlying{ 1 } << (shift - 1);
        // tie → bankers rounding
        if(remainder > half || (remainder == half && digits_10 & 1U))
        {
          digits_10++;
          // return std::make_pair(RoundingResults::ROUNDED, digits_10);
        }
      }

      return std::make_pair(RoundingResults::INEXACT_TRUNCATED, digits_10);
    }

  public:
    static auto MultiplyRoundNormalize(const T &A, const underlying &B, int &exp, const auto &selected_prescision)
    {
      const underlying A_bits = std::bit_cast<underlying>(A);

      underlying sig = (A_bits & MANTISSA_ONLY) | MANTISSA_IMPLICIT_1;

      const wide_underlying prod = wide_underlying{ sig } * wide_underlying{ B };

      static const constexpr auto shift = EXPONENT_ST + 1;
      static const constexpr auto half = wide_underlying{ 1 } << (shift - 1);

      RoundingResults rounding_result{ RoundingResults::EXACT };

      auto digits_10 = static_cast<underlying>(prod >> shift);
      auto remainder = (prod & ((half << 1) - 1));

      // digitss_10 = 12345.half.quarter.eith.etc.etc

      if(remainder > half)
      {
        digits_10++;
        // We rounded UP to get to this decimal. The true value is slightly lower.
        rounding_result = RoundingResults::INEXACT_ROUNDED_UP;
      }
      else if(remainder == half && (digits_10 & 1U))
      {
        digits_10++;
        // exact rounding makes us happy
        rounding_result = RoundingResults::EXACT;
      }
      else if(remainder == wide_underlying{ 0 })
      {
        rounding_result = RoundingResults::EXACT;
      }
      else
      {
        // We truncated to get to this decimal. The true value is slightly higher.
        rounding_result = RoundingResults::INEXACT_TRUNCATED;
      }

      if(digits_10 < selected_prescision)
      {
        digits_10 *= 10;
        exp--;
      }

      return std::make_pair(rounding_result, digits_10);
    }

    //
  };

} // namespace Helpers::Math

#pragma once

#include <bit>
#include <cstdint>
#include <limits>
#include <numbers>
#include <tuple>
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
  struct IEEE754
  {
  private:
    using underlying = std::conditional_t<std::is_same_v<T, float>, uint32_t, uint64_t>;
    using signed_underlying = std::conditional_t<std::is_same_v<T, float>, int32_t, int64_t>;

    static constexpr bool IS_DOUBLE = std::is_same_v<T, double>;

    static constexpr underlying EXPONENT_ONLY = IS_DOUBLE ? 0x7FF0000000000000ULL : 0x7F800000U;
    static constexpr underlying EXPONENT_ST = IS_DOUBLE ? 52 : 23;
    static constexpr signed_underlying EXPONENT_LEFT_OFFSET = sizeof(T) * 8 - EXPONENT_ST - 1;

    static constexpr signed_underlying MIN_EXPONENT = std::numeric_limits<T>::min_exponent - std::numeric_limits<T>::digits;

    static constexpr signed_underlying EXPONENT_ALL_BITS_ON = IS_DOUBLE ? 2047 : 255; // as defined in IEEE-754

    static constexpr underlying MANTISSA_ONLY = IS_DOUBLE ? 0x000FFFFFFFFFFFFFULL : 0x007FFFFFU;
    static constexpr underlying MANTISSA_IMPLICIT_1 = underlying{ 1 } << EXPONENT_ST;

    static constexpr underlying SIGN_ONLY = IS_DOUBLE ? 0x8000000000000000ULL : 0x80000000U;

    using wide_underlying = std::conditional_t<IS_DOUBLE, __uint128_t, uint64_t>;

    static const constexpr wide_underlying half = wide_underlying{ 1 } << EXPONENT_ST;
    static const constexpr auto shift = EXPONENT_ST + 1;
    static const constexpr __uint128_t frac_mask = ((__uint128_t{ 1 } << shift) - 1);

  public:
    underlying mantissa;
    int exponent;

  public:
    explicit IEEE754(const T &input)
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
    static auto Multiply(const T &A, const auto &B)
    {
      const wide_underlying A_bits = std::bit_cast<underlying>(A);

      const wide_underlying sig = (A_bits & MANTISSA_ONLY) | MANTISSA_IMPLICIT_1;

      const wide_underlying prod = wide_underlying{ sig } * wide_underlying{ B.hig };

      wide_underlying digits_10;

      if(shift >= 0)
      {
        digits_10 = static_cast<wide_underlying>(prod >> shift);
      }
      else
      {
        digits_10 = static_cast<wide_underlying>(prod << (-shift));
      }

      return digits_10;
    }

  public:
    static auto MultiplyRoundCompliant(const auto &A, const auto &B, int &exponent)
    {
      using hig_type = std::remove_cvref_t<decltype(B.hig)>;
      static_assert(std::is_integral_v<hig_type>, "B must be an integral type");
      static_assert(std::is_unsigned_v<hig_type>, "B should be unsigned here");

      static const constexpr __uint128_t S_MASK = ((1ULL << (shift - 2)) - 1);

      if constexpr(std::is_same_v<float, T>)
      {
        // 1. Calculate the high-precision product
        __uint128_t prod = static_cast<__uint128_t>(A) * static_cast<__uint128_t>(B.hig);

        // 2. Initial extraction check
        hig_type result = static_cast<hig_type>(prod >> shift);

        static const constexpr hig_type low = Helpers::Math::Constexpr::ipow(hig_type{ 10 }, std::numeric_limits<hig_type>::digits10 - 1);
        // 3. Normalized check (the "x10" path)
        if(result < low)
        {
          prod *= 10;
          result = static_cast<hig_type>(prod >> shift);
          --exponent;
        }

        // 4. Extract raw floor and flags
        const bool G = (prod >> (shift - 1)) & 1U;
        const bool R = (prod >> (shift - 2)) & 1U;
        const bool S = (prod & S_MASK) != 0;
        // const bool LSB = result & 1U;

        const bool round_up = G && (R || S); //|| LSB);

        if(round_up)
        {
          result++;
        }

        return std::make_tuple(result, false);
      }
      else
      {
        const auto B_pow = static_cast<__uint128_t>(B.hig) * 1'000 + B.low;

        const __uint128_t prod = static_cast<__uint128_t>(A) * B_pow;
        __uint128_t result = prod >> shift;

        static const constexpr __uint128_t low = Helpers::Math::Constexpr::ipow(__uint128_t{ 10 }, std::numeric_limits<uint64_t>::digits10 + 2);

        // 4. Normalized check (the "x10" path)
        if(result < low)
        {
          result *= 10;
          --exponent;
        }

        const bool extra = (result % 1'000) != 0;
        const hig_type to_ret = static_cast<hig_type>(result / 1'000);
        return std::make_tuple(to_ret, extra);
      }

    } //
  };
} // namespace Helpers::Math

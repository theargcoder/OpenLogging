#pragma once

#include <bit>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <limits>
#include <type_traits>

#include "include/Helpers/Helpers.h"

struct Helpers::Math
{
  struct Constexpr
  {
    template <typename T>
    static constexpr T pow(T base, int exp)
    {
      return exp == 0 ? 1 : base * Helpers::Math::Constexpr::pow(base, exp - 1);
    }
  };

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
    static constexpr signed_underlying EXP_LEFT_OFFSET = sizeof(T) * 8 - EXPONENT_ST - 1;
    static constexpr signed_underlying EXPONENT_BIAS = std::numeric_limits<T>::max_exponent - 1;

    static constexpr underlying MANTISSA_ONLY = IS_DOUBLE ? 0x000FFFFFFFFFFFFFULL : 0x007FFFFFU;
    static constexpr underlying LEADING_1 = underlying(1) << (EXPONENT_ST - 1);

    static constexpr underlying SIGN_ONLY = IS_DOUBLE ? 0x8000000000000000ULL : 0x80000000U;

    static constexpr underlying HALF_EXP = (EXPONENT_BIAS - 1) << EXPONENT_ST;
    static constexpr underlying ZERO_EXP = (EXPONENT_BIAS) << EXPONENT_ST;

  public:
    T mantissa;
    signed_underlying exponent;

    explicit IEEE754(const T &input)
    {
      const underlying bits = std::bit_cast<underlying>(input);

      const underlying man = bits & MANTISSA_ONLY;

      const signed_underlying exp = ((bits & EXPONENT_ONLY) >> EXPONENT_ST);

      const underlying SIGN = bits & SIGN_ONLY;

      if(exp > 0)
      {
        mantissa = std::bit_cast<T>(man | SIGN | HALF_EXP);
        exponent = exp - EXPONENT_BIAS + 1;
      }
      else
      {
        const int shift = std::countl_zero(man) - EXP_LEFT_OFFSET;

        mantissa = std::bit_cast<T>(((man << shift) & MANTISSA_ONLY) | SIGN | HALF_EXP);
        exponent = -EXPONENT_BIAS - shift + 2;
      }
    }
  };
};

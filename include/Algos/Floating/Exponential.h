#pragma once

#include <cstring>
#include <limits>
#include <string>
#include <type_traits>

#include "include/Algos/Floating/DigitsPrecision.h"
#include "include/Helpers/Math.h"

#include "include/Constants/Constants.h"

#include "include/Algos/Integer.h"

#include "include/Helpers/Templating.h"

namespace Helpers::Numeric::Floating::ExponentialNotation
{
  template <typename T>
    requires std::is_floating_point_v<T> && (Helpers::Templating::Assert::at_most_64_bit_double_radix_2<T>())
  static auto ToStrCharArray(const T &input, const int &PRECISION = Constants::Tables::Floating<T>::MAX_DIGITS10)
  {
    using Floating = Constants::Tables::Floating<T>;

    static const constinit auto FloatingStruct = Floating();
    static const constinit auto &exp_table = FloatingStruct.DIGITS;

    static const constexpr auto SIZE_OF_BUFF = Floating::MAX_DIGITS10 + Floating::MAX_EXP_DIGITS10 + 10;

    Helpers::Numeric::Integral::char_array<SIZE_OF_BUFF> buff;

    buff.start_idx = SIZE_OF_BUFF;

    const auto frexpp = Helpers::Math::IEEE754(input);
    const auto &exp = frexpp.exponent;
    const auto &mantissa = frexpp.mantissa;

    if(exp == std::numeric_limits<decltype(frexpp.exponent)>::max()) [[unlikely]]
    {
      buff.start_idx -= 3;
      if(mantissa == T{ 0 })
      {
        std::memcpy(&buff.array[buff.start_idx], "nan", 3);
      }
      else if(mantissa > T{ 0 })
      {
        std::memcpy(&buff.array[buff.start_idx], "inf", 3);
      }
      else if(mantissa == T{ -1 })
      {
        buff.start_idx--;
        std::memcpy(&buff.array[buff.start_idx], "-inf", 4);
      }
      else
      {
        buff.start_idx -= 3;
        std::memcpy(&buff.array[buff.start_idx], "0.0E00", 6);
      }

      return buff;
    }

    int exp_base_10_int = ((exp * 78'913) >> 18);

    const auto exp_table_val = exp_table[exp + Floating::BIAS];

    using type = decltype(exp_table_val);

    static const constexpr auto min_precision = Helpers::Math::Constexpr::ipow(type{ 10 }, std::numeric_limits<type>::digits10 - 1);
    static const constexpr auto max_precision = Helpers::Math::Constexpr::ipow(type{ 10 }, std::numeric_limits<type>::digits10);

    static const constexpr auto base_5_rounding_table = Constants::Tables::GetExponentialRoundingTable<type, 5>();
    static const constexpr auto base_10_rounding_table = Constants::Tables::GetExponentialRoundingTable<type, 10>();

    auto [round_up, extra, digits_10] = Helpers::Math::IEEE754<T>::MultiplyReturnHighLow(mantissa, exp_table_val);

    const bool shrinked = digits_10 < min_precision;

    if(shrinked)
    {
      exp_base_10_int--;
    }

    const auto &rounding_factor_10s = base_10_rounding_table[PRECISION + shrinked];
    const auto &rounding_factor_5s = base_5_rounding_table[PRECISION + shrinked];
    const auto remainder = digits_10 % rounding_factor_10s;

    if(remainder > rounding_factor_5s)
    {
      digits_10 += rounding_factor_5s;
    }
    else if(remainder == rounding_factor_5s && !round_up)
    {
      // This is the critical TIE-BREAKER
      // If there are ANY bits left over in the binary product (G, R, or S),
      // then the real value is slightly > midpoint.
      if(extra)
      {
        digits_10 += rounding_factor_5s;
      }
      else
      {
        // Apply Round-Ties-To-Even on the LAST VISIBLE DIGIT.
        const auto last_digit = (digits_10 / rounding_factor_10s) % 10;
        if(last_digit & 1U)
        {
          digits_10 += rounding_factor_5s;
        }
      }
    }

    if(shrinked)
    {
      if(digits_10 >= min_precision)
      {
        digits_10 /= 10;
        exp_base_10_int++;
      }
    }
    else
    {
      if(digits_10 >= max_precision)
      {
        digits_10 /= 10;
        exp_base_10_int++;
      }
    }

    digits_10 /= rounding_factor_10s;

    Helpers::Numeric::Integral::ToStrReverseWriteToCharArray<true>(exp_base_10_int, buff, buff.start_idx);

    buff.array[--buff.start_idx] = 'e';

    Helpers::Numeric::Integral::ToStrReverseWriteToCharArray<false>(digits_10, buff, buff.start_idx);

    buff.array[--buff.start_idx] = '.';

    std::swap(buff.array[buff.start_idx], buff.array[buff.start_idx + 1]);

    return buff;
  }

  template <typename T>
    requires std::is_floating_point_v<T> && (Helpers::Templating::Assert::at_most_64_bit_double_radix_2<T>())
  static std::string ToStr(const T &input, const int &PRECISION = Constants::Tables::Floating<T>::MAX_DIGITS10)
  {
    const auto buff = Helpers::Numeric::Floating::ExponentialNotation::ToStrCharArray(input, PRECISION);
    return std::string(&buff.array[buff.start_idx], sizeof(buff.array) - buff.start_idx);
  }
} // namespace Helpers::Numeric::Floating::ExponentialNotation

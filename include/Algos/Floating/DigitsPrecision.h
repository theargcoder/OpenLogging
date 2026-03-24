#pragma once

#include <cstdint>
#include <cstring>
#include <limits>
#include <string>
#include <type_traits>

#include "include/Helpers/Math.h"

#include "include/Constants/Constants.h"

#include "include/Algos/Integer.h"

#include "include/Helpers/Templating.h"

namespace Helpers::Numeric::Floating::DigitsPrecision
{
  template <uint32_t PRECISION, typename T>
    requires std::is_floating_point_v<T> && (Helpers::Templating::Assert::at_most_64_bit_double_radix_2<T>())
  static auto ToStrCharArray(const T &input)
  {
    using Floating = Constants::Tables::Floating<T>;

    static const auto &table = Floating().DIGITS;

    // 1 for '+/-' also 1 for '.' also 1 for whatever
    const constexpr auto SIZE_OF_BUFF = Floating::MAX_DIGITS10 + std::numeric_limits<T>::max_exponent10 + PRECISION + 3;

    Helpers::Numeric::Integral::char_array<SIZE_OF_BUFF> buff;

    buff.start_idx = SIZE_OF_BUFF;

    const auto frexpp = Helpers::Math::IEEE754(input);
    const auto &exp = frexpp.exponent;
    const auto &mantissa = frexpp.mantissa;

    if(exp == std::numeric_limits<decltype(frexpp.exponent)>::max())
    {
      buff.start_idx = SIZE_OF_BUFF - 3;
      if(mantissa == T{ 0 })
      {
        std::memcpy(&buff.array[buff.start_idx], "nan", 3);
      }
      else if(mantissa > T{ 0 })
      {
        std::memcpy(&buff.array[buff.start_idx], "inf", 3);
      }
      else
      {
        buff.start_idx--;
        std::memcpy(&buff.array[buff.start_idx], "-inf", 4);
      }

      return buff;
    }

    const auto exp_2 = table[exp + Floating::BIAS];

    const auto digits_10 = static_cast<Floating::smallest_underlying>(mantissa * exp_2);

    const constexpr auto precision = Helpers::Math::Constexpr::pow(typename Floating::smallest_underlying(10), Floating::MAX_DIGITS10);

    int exp_shft = (digits_10 < precision) ? -1 : 0;

    const auto exp_base_10_int = ((exp * 78'913) >> 18) + exp_shft;

    auto res_buff = Helpers::Numeric::Integral::ToStrCharArray<false>(digits_10);

    if(exp_base_10_int < 0)
    {
      const auto exp_10_abs = std::abs(exp_base_10_int);
      if(exp_10_abs <= Floating::MAX_DIGITS10)
      {
        buff.start_idx -= PRECISION;
        std::memcpy(&buff.array[buff.start_idx--], &res_buff.array[res_buff.start_idx], PRECISION);
      }
      else
      {
        const auto extra_zeros = exp_10_abs - PRECISION;
        buff.start_idx -= extra_zeros;
        std::memset(&buff.array[buff.start_idx], '0', extra_zeros);
        buff.start_idx -= PRECISION;
        std::memcpy(&buff.array[buff.start_idx], &res_buff.array[res_buff.start_idx], PRECISION);
        buff.start_idx--;
      }

      buff.array[buff.start_idx--] = '0';
      buff.array[buff.start_idx] = '.';
      std::swap(buff.array[buff.start_idx], buff.array[buff.start_idx + 1]);
    }
    else
    {
      if(exp_base_10_int <= Floating::MAX_DIGITS10)
      {
        buff.start_idx -= PRECISION;
        std::memcpy(&buff.array[buff.start_idx--], &res_buff.array[res_buff.start_idx], PRECISION);
      }
      else
      {
        const auto extra_zeros = exp_base_10_int - PRECISION;
        buff.start_idx -= extra_zeros;
        std::memset(&buff.array[buff.start_idx--], '0', extra_zeros);
        buff.array[buff.start_idx--] = '.';
        buff.start_idx -= PRECISION;
        std::memcpy(&buff.array[buff.start_idx--], &res_buff.array[res_buff.start_idx], PRECISION);
      }
    }

    return buff;
  }

  template <uint32_t PRECISION, typename T>
    requires std::is_floating_point_v<T> && (Helpers::Templating::Assert::at_most_64_bit_double_radix_2<T>())
  static std::string ToStr(const T &input)
  {
    const auto buff = Helpers::Numeric::Floating::DigitsPrecision::ToStrCharArray<PRECISION>(input);
    return std::string(&buff.array[buff.start_idx], sizeof(buff.array) - buff.start_idx);
  }
} // namespace Helpers::Numeric::Floating::DigitsPrecision

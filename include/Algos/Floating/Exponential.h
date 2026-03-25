#pragma once

#include <cmath>
#include <cstring>
#include <limits>
#include <string>
#include <type_traits>

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
    static const constinit auto &table = FloatingStruct.DIGITS;

    const constexpr auto SIZE_OF_BUFF = Floating::MAX_DIGITS10 + Floating::MAX_EXP_DIGITS10 + 10;

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

    const auto exp_2 = table[exp + Floating::BIAS];

    const auto digits_10 = static_cast<Floating::smallest_underlying>(mantissa * exp_2);

    const constexpr auto precision = Helpers::Math::Constexpr::pow(typename Floating::smallest_underlying(10), Floating::MAX_DIGITS10);

    const int exp_shft = (digits_10 < precision) ? -1 : 0;

    const auto exp_base_10_int = ((exp * 78'913) >> 18) + exp_shft;

    Helpers::Numeric::Integral::ToStrReverseWriteToCharArray<true>(exp_base_10_int, buff, buff.start_idx);

    buff.array[--buff.start_idx] = 'e';

    const auto res_buff = Helpers::Numeric::Integral::ToStrCharArray<false>(digits_10);

    buff.start_idx -= PRECISION;
    std::memcpy(&buff.array[buff.start_idx--], &res_buff.array[res_buff.start_idx], PRECISION);

    buff.array[buff.start_idx] = '.';

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

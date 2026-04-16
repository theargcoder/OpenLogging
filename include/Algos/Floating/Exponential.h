#pragma once

#include <cassert>
#include <cstdint>
#include <cstdlib>
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
  static auto ToStrCharArray(const T &input, int PRECISION = Constants::Tables::Floating<T>::MAX_DIGITS10)
  {
    using Floating = Constants::Tables::Floating<double>;
    using type = std::conditional_t<std::is_same_v<float, T>, uint32_t, uint64_t>;

    static const constexpr type BASE = 10U;
    static const constexpr type DEC8 = 100'000'000U;
    static const constexpr uint32_t ROUNDING_FACTOR = 5U;
    static const constexpr int32_t MAX_PRECISION = std::is_same_v<T, float> ? 8 : 17;

    static const constexpr auto SIZE_OF_BUFF = Floating::MAX_DIGITS10 + Floating::MAX_EXP_DIGITS10 + 10;

    static const constexpr uint8_t magic_number = std::is_same_v<T, float> ? 1 : 2;

    static const constexpr auto min_precision = Helpers::Math::Constexpr::ipow(type{ 10 }, std::numeric_limits<type>::digits10 - magic_number);
    static const constexpr auto max_precision = Helpers::Math::Constexpr::ipow(type{ 10 }, std::numeric_limits<type>::digits10 - magic_number + 1);

    assert(PRECISION <= MAX_PRECISION); // no point of printing more than 8 or 17 digits respectively its actually not even necesary for round tripping

    Helpers::Numeric::Integral::char_array<SIZE_OF_BUFF> buff;
    buff.start_idx = SIZE_OF_BUFF;

    uint64_t mantissa;
    int exp_base_10_int;
    if(Helpers::Math::IEEE754::GetMantissaExponent<T>(input, mantissa, exp_base_10_int)) [[unlikely]]
    {
      if(mantissa == 0)
      {
        buff.start_idx = 3;
        std::memcpy(&buff.array[3], "nan", 3);
      }
      else if(mantissa == 1)
      {
        buff.start_idx = 3;
        std::memcpy(&buff.array[3], "inf", 3);
      }
      else if(mantissa == 2)
      {
        buff.start_idx = 4;
        std::memcpy(&buff.array[3], "-inf", 4);
      }
      else
      {
        buff.start_idx = 5;
        std::memcpy(&buff.array[5], "0.0E0", 5);
      }

      return buff;
    }

    const auto *table = &Floating::DIGITS[exp_base_10_int][0];
    exp_base_10_int = (((exp_base_10_int - Floating::BIAS) * 78'913) >> 18U);

    uint32_t extra_digits;
    type digits_10, remainder;
    Helpers::Math::IEEE754::Multiply<T>(mantissa, table, digits_10, extra_digits);

    if(digits_10 < min_precision)
    {
      digits_10 *= BASE;
      remainder = Helpers::Math::Magic::Division::div_by_10_pow_n<8>(extra_digits);
      digits_10 += remainder;
      extra_digits -= remainder * DEC8;
      extra_digits *= BASE;
      exp_base_10_int--;
    }
    else if(digits_10 > max_precision)
    {
      Helpers::Math::Magic::Modulo::mod_by_10_pow_n_void<1>(digits_10, remainder);
      Helpers::Math::Magic::Division::div_by_10_pow_n_void<1>(extra_digits);
      extra_digits += remainder * DEC8;
      exp_base_10_int++;
    }

    if(PRECISION < MAX_PRECISION)
    {
      Helpers::Math::Precision::truncate_plus_1_quo_rem(digits_10, remainder, std::numeric_limits<type>::digits10 - PRECISION - magic_number - 1);
    }
    else
    {
      remainder = Helpers::Math::Magic::Division::div_by_10_pow_n<8>(extra_digits);
      extra_digits -= remainder * DEC8;
    }

    const bool extra = extra_digits != 0 || (PRECISION < 17 && remainder != 0);

    if(PRECISION < MAX_PRECISION)
    {
      Helpers::Math::Magic::Modulo::mod_by_10_pow_n_void<1>(digits_10, remainder);
    }

    if(remainder > ROUNDING_FACTOR)
    {
      digits_10++;
    }
    else if(remainder == ROUNDING_FACTOR)
    {
      if(extra)
      {
        digits_10++;
      }
      else
      {
        // Apply Round-Ties-To-Even on the LAST VISIBLE DIGIT.
        remainder = Helpers::Math::Magic::Modulo::mod_by_10_pow_n<1>(digits_10); // digits_10 % 10;
        if(remainder & 1U)
        {
          digits_10++;
        }
      }
    }

    static const constexpr auto precision_table = Constants::Tables::GetPrecistionTable<type>();
    if(digits_10 >= precision_table[PRECISION])
    {
      digits_10 /= BASE;
      exp_base_10_int++;
    }

    uint32_t exp_abs = std::abs(exp_base_10_int);

    Helpers::Numeric::Integral::ToStrReverseWriteToCharArrayResult(exp_abs, extra_digits, buff);

    if(exp_base_10_int < 0)
    {
      buff.array[--buff.start_idx] = '-';
    }
    else
    {
      buff.array[--buff.start_idx] = '+';
    }

    buff.array[--buff.start_idx] = 'e';

    Helpers::Numeric::Integral::ToStrReverseWriteToCharArrayResult(digits_10, remainder, buff);

    buff.array[--buff.start_idx] = '.';

    std::swap(buff.array[buff.start_idx], buff.array[buff.start_idx + 1]);

    if(input < 0)
    {
      buff.array[--buff.start_idx] = '-';
    }

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

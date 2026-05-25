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
  static unsigned ToStrCharArray(char *__restrict__ buff, const T &input, int PRECISION = Constants::Tables::Floating<T>::MAX_DIGITS10);

  template <>
  unsigned ToStrCharArray(char *__restrict__ buff, const float &input, int PRECISION)
  {
    using Floating = Constants::Tables::Floating<double>;

    const constexpr unsigned BASE = 10U;
    const constexpr unsigned DEC8 = 100'000'000U;
    const constexpr uint32_t ROUNDING_FACTOR = 5U;

    const constexpr auto MIN_PRECISION = Helpers::Math::Constexpr::ipow(10U, std::numeric_limits<unsigned>::digits10 - 1);
    const constexpr auto MAX_PRECISION = Helpers::Math::Constexpr::ipow(10U, std::numeric_limits<unsigned>::digits10);

    const constexpr auto PRECISION_TABLE = Constants::Tables::Exponential::GetPrecistionTable<unsigned>();

    assert(PRECISION <= 8); // no point of printing more than 8 or 17 digits respectively its actually not even necesary for round tripping

    unsigned len = 0;

    if(input < 0.0)
    {
      buff[len++] = '-';
    }

    uint64_t mantissa;
    int exp_base_10_int;
    if(Helpers::Math::IEEE754::GetMantissaExponent<float>(input, mantissa, exp_base_10_int)) [[unlikely]]
    {
      if(mantissa == 0)
      {
        len = 3;
        std::memcpy(&buff[0], "nan", 3);
      }
      else if(mantissa == 1)
      {
        len = 3;
        std::memcpy(&buff[0], "inf", 3);
      }
      else if(mantissa == 2)
      {
        len = 4;
        std::memcpy(&buff[0], "-inf", 4);
      }
      else
      {
        len = 5;
        std::memcpy(&buff[0], "0.0E0", 5);
      }

      return len;
    }

    const auto *table = &Floating::DIGITS[exp_base_10_int][0];
    exp_base_10_int = (((exp_base_10_int - Floating::BIAS) * 78'913) >> 18U);

    unsigned extra_digits;
    unsigned digits_10, remainder;
    Helpers::Math::IEEE754::Multiply<float>(mantissa, table, digits_10, extra_digits);

    if(digits_10 < MIN_PRECISION)
    {
      digits_10 *= BASE;
      remainder = Helpers::Math::Magic::Division::div_by_10_pow_n<8>(extra_digits);
      digits_10 += remainder;
      extra_digits -= remainder * DEC8;
      extra_digits *= BASE;
      exp_base_10_int--;
    }
    else if(digits_10 > MAX_PRECISION)
    {
      Helpers::Math::Magic::Modulo::mod_by_10_pow_n_void<1>(digits_10, remainder);
      Helpers::Math::Magic::Division::div_by_10_pow_n_void<1>(extra_digits);
      extra_digits += remainder * DEC8;
      exp_base_10_int++;
    }

    Helpers::Math::Precision::truncate_plus_1_quo_rem(digits_10, remainder, std::numeric_limits<unsigned>::digits10 - PRECISION - 2);

    const bool extra = extra_digits != 0 || remainder != 0;

    Helpers::Math::Magic::Modulo::mod_by_10_pow_n_void<1>(digits_10, remainder);

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

    if(digits_10 >= PRECISION_TABLE[PRECISION])
    {
      Helpers::Math::Magic::Division::div_by_10_pow_n_void<1>(digits_10);
      exp_base_10_int++;
    }

    buff[len++] = '.';

    remainder = Helpers::Numeric::Integral::ToStrFowardWriteSIMDReturnLen<unsigned>(&buff[len], digits_10);

    std::swap(buff[len - 1], buff[len]);

    len += remainder;

    buff[len++] = 'e';

    buff[len++] = (exp_base_10_int < 0) ? '-' : '+';

    const unsigned exp_abs = std::abs(exp_base_10_int);

    len += Helpers::Numeric::Integral::ToStrFowardWriteSIMDReturnLen<uint16_t>(&buff[len], static_cast<uint16_t>(exp_abs));

    return len;
  }

  template <>
  unsigned ToStrCharArray(char *__restrict__ buff, const double &input, int PRECISION)
  {
    using Floating = Constants::Tables::Floating<double>;
    using type = uint64_t;

    const constexpr type BASE = 10U;
    const constexpr type DEC8 = 100'000'000U;
    const constexpr uint32_t ROUNDING_FACTOR = 5U;
    const constexpr int32_t MAX_PRECISION = 17;

    const constexpr auto min_precision = Helpers::Math::Constexpr::ipow(type{ 10 }, std::numeric_limits<type>::digits10 - 2);
    const constexpr auto max_precision = Helpers::Math::Constexpr::ipow(type{ 10 }, std::numeric_limits<type>::digits10 - 1);

    const constexpr auto precision_table = Constants::Tables::Exponential::GetPrecistionTable<type>();

    assert(PRECISION <= MAX_PRECISION); // no point of printing more than 8 or 17 digits respectively its actually not even necesary for round tripping

    unsigned len = 0;

    if(input < 0.0)
    {
      buff[len++] = '-';
    }

    uint64_t mantissa;
    int exp_base_10_int;
    if(Helpers::Math::IEEE754::GetMantissaExponent<double>(input, mantissa, exp_base_10_int)) [[unlikely]]
    {
      if(mantissa == 0)
      {
        len = 3;
        std::memcpy(&buff[0], "nan", 3);
      }
      else if(mantissa == 1)
      {
        len = 3;
        std::memcpy(&buff[0], "inf", 3);
      }
      else if(mantissa == 2)
      {
        len = 4;
        std::memcpy(&buff[0], "-inf", 4);
      }
      else
      {
        len = 5;
        std::memcpy(&buff[0], "0.0E0", 5);
      }

      return len;
    }

    const auto *table = &Floating::DIGITS[exp_base_10_int][0];
    exp_base_10_int = (((exp_base_10_int - Floating::BIAS) * 78'913) >> 18U);

    uint32_t extra_digits;
    type digits_10, remainder;
    Helpers::Math::IEEE754::Multiply<double>(mantissa, table, digits_10, extra_digits);

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
      Helpers::Math::Precision::truncate_plus_1_quo_rem(digits_10, remainder, std::numeric_limits<type>::digits10 - PRECISION - 3);
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

    if(digits_10 >= precision_table[PRECISION])
    {
      Helpers::Math::Magic::Division::div_by_10_pow_n_void<1>(digits_10);
      exp_base_10_int++;
    }

    buff[len++] = '.';

    remainder = Helpers::Numeric::Integral::ToStrFowardWriteSIMDReturnLen<type>(&buff[len], digits_10);

    std::swap(buff[len - 1], buff[len]);

    len += remainder;

    buff[len++] = 'e';

    buff[len++] = (exp_base_10_int < 0) ? '-' : '+';

    const unsigned exp_abs = std::abs(exp_base_10_int);

    len += Helpers::Numeric::Integral::ToStrFowardWriteSIMDReturnLen<uint16_t>(&buff[len], static_cast<uint16_t>(exp_abs));

    return len;
  }

  template <typename T>
    requires std::is_floating_point_v<T> && (Helpers::Templating::Assert::at_most_64_bit_double_radix_2<T>())
  static std::string ToStr(const T &input, const int &PRECISION = Constants::Tables::Floating<T>::MAX_DIGITS10)
  {
    char buff[64];

    const uint32_t len = ToStrCharArray<T>(&buff[0], input, PRECISION);

    return std::string{ &buff[0], len };
  }
} // namespace Helpers::Numeric::Floating::ExponentialNotation

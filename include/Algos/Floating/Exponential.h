#pragma once

#include <cassert>
#include <cstdint>
#include <cstring>
#include <exception>
#include <limits>
#include <string>
#include <type_traits>

#include "include/Helpers/Math.h"

#include "include/Constants/Constants.h"

#include "include/Algos/Integer.h"

#include "include/Helpers/Templating.h"

namespace Helpers::Numeric::Floating::ExponentialNotation
{
  static inline uint32_t pow5Factor(uint64_t value)
  {
    const uint64_t m_inv_5 = 14757395258967641293u; // 5 * m_inv_5 = 1 (mod 2^64)
    const uint64_t n_div_5 = 3689348814741910323u;  // #{ n | n = 0 (mod 2^64) } = 2^64 / 5
    uint32_t count = 0;
    for(;;)
    {
      assert(value != 0);
      value *= m_inv_5;
      if(value > n_div_5)
        break;
      ++count;
    }
    return count;
  }

  // Returns true if value is divisible by 5^p.
  static inline bool multipleOfPowerOf5(const uint64_t value, const uint32_t p)
  {
    // I tried a case distinction on p, but there was no performance difference.
    return pow5Factor(value) >= p;
  }

  // Returns true if value is divisible by 2^p.
  static inline bool multipleOfPowerOf2(const uint64_t value, const uint32_t p)
  {
    assert(value != 0);
    assert(p < 64);
    // __builtin_ctzll doesn't appear to be faster here.
    return (value & ((1ULL << p) - 1)) == 0;
  }

  template <typename T>
    requires std::is_floating_point_v<T> && (Helpers::Templating::Assert::at_most_64_bit_double_radix_2<T>())
  static auto ToStrCharArray(const T &input, const int &PRECISION = Constants::Tables::Floating<T>::MAX_DIGITS10)
  {
    using Floating = Constants::Tables::Floating<T>;

    // static const constexpr auto SIZE_OF_BUFF = Floating::MAX_DIGITS10 + Floating::MAX_EXP_DIGITS10 + 10;

    Helpers::Numeric::Integral::char_array_len<100> buff;

    const auto frexpp = Helpers::Math::IEEE754<T>(input);
    const auto &exp = frexpp.exponent;
    const auto &mantissa = frexpp.mantissa;

    if(exp == std::numeric_limits<decltype(frexpp.exponent)>::max()) [[unlikely]]
    {
      if(mantissa == T{ 0 })
      {
        buff.length = 3;
        std::memcpy(&buff.array[0], "nan", buff.length);
      }
      else if(mantissa == 1)
      {
        buff.length = 3;
        std::memcpy(&buff.array[0], "inf", buff.length);
      }
      else if(mantissa == 2)
      {
        buff.length = 4;
        std::memcpy(&buff.array[0], "-inf", buff.length);
      }
      else
      {
        buff.length = 6;
        std::memcpy(&buff.array[0], "0.0E00", buff.length);
      }

      return buff;
    }

    buff.length = 0;
    if(input < 0)
    {
      buff.length++;
      buff.array[0] = '-';
    }

    buff.array[buff.length] = '.';
    buff.length++;

    int exp_base_10_int = (((exp - Floating::BIAS) * 78'913) >> 18);

    const auto *table = &Constants::Tables::Floating<double>::DIGITS[exp];
    auto digits_10 = frexpp.mul3_128b(table, exp_base_10_int);

    static const constexpr auto digits_len = std::numeric_limits<std::remove_cvref_t<decltype(digits_10)>>::digits10 + 1;

    Helpers::Numeric::Integral::ToStrFowardWriteNdigitsAlterInput(digits_10, buff, PRECISION + 2);

    std::swap(buff.array[buff.length - 1], buff.array[buff.length]);

    buff.length += PRECISION + 1;

    bool round = 0;

    const auto after_digit = buff.array[buff.length] - '0';
    if(after_digit > 5)
    {
      round = true;
    }
    else if(after_digit == 5)
    {
      if(digits_10)
      {
        round = true;
      }
      else
      {
        const auto rem_exp = PRECISION - exp_base_10_int;
        const auto required_twos = -(exp - Floating::MANTISSA_BITS - Floating::BIAS) - rem_exp;
        bool trailingZeros = required_twos <= 0 || (required_twos < 60 && multipleOfPowerOf2(mantissa, (uint32_t)required_twos));
        if(rem_exp < 0)
        {
          const int32_t requiredFives = -rem_exp;
          trailingZeros = trailingZeros && multipleOfPowerOf5(mantissa, (uint32_t)requiredFives);
        }
        if(trailingZeros)
        {
          // Apply Round-Ties-To-Even on the LAST VISIBLE DIGIT.
          const auto last_digit = buff.array[buff.length - 1] - '0';
          round = last_digit & 1U;
        }
        else
        {
          round = true;
        }
      }
    }

    if(round)
    {
      int i = buff.length;
      bool carry = true;
      while(carry)
      {
        i--;
        if(buff.array[i] == '.')
          break;

        carry = buff.array[i] == '9';
        (carry) ? buff.array[i] = '0' : buff.array[i]++;
      }
      if(buff.array[i] == '.' && carry) // fuckkk from start till now all 9's
      {
        i--;
        if(buff.array[i] == '9')
        {
          buff.array[i] = '1';
          exp_base_10_int++;
        }
        else
        {
          buff.array[i]++;
        }
      }
    }

    buff.array[buff.length++] = 'e';

    if(exp_base_10_int <= -1000)
    {
      std::terminate(); // wtf happened; we dont know but shit is invalid
    }
    else if(exp_base_10_int <= -100)
    {
      buff.length += 4; // 1 for sign 1 for exp_digit
    }
    else if(exp_base_10_int <= -10)
    {
      buff.length += 3; // 1 for sign 1 for exp_digit
    }
    else if(exp_base_10_int < 10)
    {
      buff.length += 2; // 1 for sign 1 for exp_digit
    }
    else if(exp_base_10_int < 100)
    {
      buff.length += 3; // 1 for sign 2 for exp_digit
    }
    else if(exp_base_10_int < 1000)
    {
      buff.length += 4; // 1 for sign 1 for exp_digit
    }
    else
    {
      std::terminate(); // wtf happened; we dont know but shit is invalid
    }

    Helpers::Numeric::Integral::ToStrReverseWriteToCharArray<true>(exp_base_10_int, buff, buff.length);

    return buff;
  }

  template <typename T>
    requires std::is_floating_point_v<T> && (Helpers::Templating::Assert::at_most_64_bit_double_radix_2<T>())
  static std::string ToStr(const T &input, const int &PRECISION = Constants::Tables::Floating<T>::MAX_DIGITS10)
  {
    const auto buff = Helpers::Numeric::Floating::ExponentialNotation::ToStrCharArray(input, PRECISION);
    return std::string(&buff.array[0], buff.length);
  }
} // namespace Helpers::Numeric::Floating::ExponentialNotation

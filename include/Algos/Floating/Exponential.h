#pragma once

#include <cassert>
#include <cstdint>
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

    static const constexpr auto SIZE_OF_BUFF = Floating::MAX_DIGITS10 + Floating::MAX_EXP_DIGITS10 + 10;

    Helpers::Numeric::Integral::char_array<SIZE_OF_BUFF> buff;

    buff.start_idx = SIZE_OF_BUFF;

    const auto frexpp = Helpers::Math::IEEE754<T>(input);
    const auto &exp = frexpp.exponent;
    const auto &mantissa = frexpp.mantissa;

    if(exp == std::numeric_limits<decltype(frexpp.exponent)>::max()) [[unlikely]]
    {
      buff.start_idx -= 3;
      if(mantissa == T{ 0 })
      {
        std::memcpy(&buff.array[buff.start_idx], "nan", 3);
      }
      else if(mantissa == 1)
      {
        std::memcpy(&buff.array[buff.start_idx], "inf", 3);
      }
      else if(mantissa == 2)
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

    int exp_base_10_int = (((exp - Floating::BIAS) * 78'913) >> 18);

    const auto &exp_table_val = Floating::DIGITS[exp];

    typename Helpers::Math::IEEE754<T>::result_extra result;

    Helpers::Math::IEEE754<T>::MultiplyRoundCompliant(mantissa, exp_table_val, exp_base_10_int, result);

    if(PRECISION < std::numeric_limits<std::remove_cvref_t<decltype(result.result)>>::digits10)
    {
      static const constexpr auto base_10_rounding_table = Constants::Tables::GetExponentialRoundingTable<uint64_t, 10>();
      static const constexpr auto precision_table = Constants::Tables::GetPrecistion<uint64_t>();
      const auto &rounding_factor_10s = base_10_rounding_table[PRECISION];
      result.result /= rounding_factor_10s;
      const auto remainder = result.result % 10;
      result.result /= 10;

      // 0 = don't round up; 1 = round up unconditionally; 2 = round up if odd.
      if(remainder > 5)
      {
        result.result++;
      }
      else if(remainder == 5)
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
          // This is the critical TIE-BREAKER
          // Apply Round-Ties-To-Even on the LAST VISIBLE DIGIT.
          const auto last_digit = result.result % 10;
          if(last_digit & 1U)
          {
            result.result++;
          }
        }
        else
        {
          result.result++;
        }
      }

      if(result.result >= precision_table[PRECISION])
      {
        result.result /= 10;
        exp_base_10_int++;
      }
    }

    Helpers::Numeric::Integral::ToStrReverseWriteToCharArray<true>(exp_base_10_int, buff);

    buff.array[--buff.start_idx] = 'e';

    Helpers::Numeric::Integral::ToStrReverseWriteToCharArrayResult(result.result, buff);

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

#pragma once

#include <cmath>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <limits>
#include <string>
#include <type_traits>

#include "include/Helpers/Math.h"

#include "include/Constants/Constants.h"

#include "include/Algos/Integer.h"

#include "include/Helpers/Templating.h"

namespace Helpers::Numeric::Floating::DigitsPrecision
{
  enum class RoundingBehavior : uint8_t
  {
    TRUNCATE,
    ROUND,
    CEIL,
    FLOOR
  };

  template <RoundingBehavior BEHAVE, int32_t PRECISION, typename T>
  struct CharArrayImpl;

  template <Helpers::Numeric::Floating::DigitsPrecision::RoundingBehavior BEHAVE, uint32_t PRECISION, typename T>
    requires std::is_floating_point_v<T> && (Helpers::Templating::Assert::at_most_64_bit_double_radix_2<T>())
  static auto ToStrCharArray(const T &input)
  {
    return Helpers::Numeric::Floating::DigitsPrecision::CharArrayImpl<BEHAVE, PRECISION, T>::ToStr(input);
  }

  template <int32_t PRECISION, typename T>
    requires std::is_floating_point_v<T> && (Helpers::Templating::Assert::at_most_64_bit_double_radix_2<T>())
  struct CharArrayImpl<Helpers::Numeric::Floating::DigitsPrecision::RoundingBehavior::TRUNCATE, PRECISION, T>
  {
    static auto ToStr(const T &input)
    {
      using Floating = Constants::Tables::Floating<T>;

      static const constinit auto FloatingStruct = Floating();
      static const constinit auto &table = FloatingStruct.DIGITS;

      // 1 for '+/-' also 1 for '.' also 1 for whatever
      static const constexpr auto SIZE_OF_BUFF = Floating::MAX_DIGITS10 + std::numeric_limits<T>::max_exponent10 + PRECISION + 3;

      Helpers::Numeric::Integral::char_array<SIZE_OF_BUFF> buff;

      buff.start_idx = SIZE_OF_BUFF;

      int exp;
      uint64_t mantissa;
      Helpers::Math::IEEE754::GetMantissaExponent(input, mantissa, exp);

      if(exp == std::numeric_limits<std::remove_cvref_t<decltype(exp)>>::max()) [[unlikely]]
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
          buff.start_idx -= PRECISION;
          std::memset(&buff.array[buff.start_idx], '0', PRECISION);
          buff.array[--buff.start_idx] = '.';
          buff.array[--buff.start_idx] = '0';
        }

        return buff;
      }

      const auto exp_2 = table[exp + Floating::BIAS];

      using base_type = std::conditional_t<std::is_same_v<float, T>, uint32_t, uint64_t>;

      const auto digits_10 = static_cast<base_type>(mantissa * exp_2);

      static const constexpr auto precision = Helpers::Math::Constexpr::pow(base_type{ 10 }, Floating::MAX_DIGITS10);

      int exp_shft = (digits_10 < precision) ? -1 : 0;

      const auto exp_base_10_int = ((exp * 78'913) >> 18) + exp_shft;

      auto res_buff = Helpers::Numeric::Integral::ToStrCharArray<false>(digits_10);

      if(exp_base_10_int < 0)
      {
        const auto exp_10_abs = std::abs(exp_base_10_int + 1);

        if(exp_10_abs >= PRECISION)
        {
          buff.start_idx -= PRECISION;
          std::memset(&buff.array[buff.start_idx--], '0', PRECISION);
        }
        else
        {
          const auto quantity = PRECISION - exp_10_abs;
          buff.start_idx -= quantity;
          std::memcpy(&buff.array[buff.start_idx], &res_buff.array[res_buff.start_idx], quantity);
          buff.start_idx -= exp_10_abs;
          std::memset(&buff.array[buff.start_idx--], '0', exp_10_abs);
        }

        buff.array[buff.start_idx--] = '.';
        buff.array[buff.start_idx] = '0';
      }
      else
      {
        if(exp_base_10_int <= Floating::MAX_DIGITS10)
        {
          std::cout << "WE are HEREEE \n\n";
          buff.start_idx -= PRECISION;
          std::memset(&buff.array[buff.start_idx], '0', PRECISION);
          const auto pres_after_dot = Floating::MAX_DIGITS10 - exp_base_10_int;
          std::memcpy(&buff.array[buff.start_idx--], &res_buff.array[res_buff.start_idx + exp_base_10_int], pres_after_dot);
          buff.array[buff.start_idx--] = '.';
          buff.start_idx -= exp_base_10_int;
          std::memcpy(&buff.array[buff.start_idx--], &res_buff.array[res_buff.start_idx], exp_base_10_int);
        }
        else
        {
          buff.start_idx -= PRECISION;
          std::memset(&buff.array[buff.start_idx], '0', PRECISION);
          buff.array[buff.start_idx--] = '.';
          const auto missing = Floating::MAX_DIGITS10 - exp_base_10_int;
          buff.start_idx -= missing;
          std::memset(&buff.array[buff.start_idx--], '0', missing);
          buff.start_idx -= Floating::MAX_DIGITS10;
          std::memcpy(&buff.array[buff.start_idx--], &res_buff.array[res_buff.start_idx], Floating::MAX_DIGITS10);
        }
      }

      return buff;
    }
  };

  template <int32_t PRECISION, typename T>
    requires std::is_floating_point_v<T> && (Helpers::Templating::Assert::at_most_64_bit_double_radix_2<T>())
  struct CharArrayImpl<Helpers::Numeric::Floating::DigitsPrecision::RoundingBehavior::ROUND, PRECISION, T>
  {
    static auto ToStr(const T &input)
    {
      using FloatingStruct = Constants::Tables::Floating<T>;

      // 1 for '+/-' also 1 for '.' also 1 for whatever
      static const constexpr auto SIZE_OF_BUFF = FloatingStruct::MAX_DIGITS10 + std::numeric_limits<T>::max_exponent10 + PRECISION + 3;

      Helpers::Numeric::Integral::char_array<SIZE_OF_BUFF> buff;

      buff.start_idx = SIZE_OF_BUFF;

      int exp;
      uint64_t mantissa;
      Helpers::Math::IEEE754::GetMantissaExponent(input, mantissa, exp);

      if(exp == std::numeric_limits<std::remove_cvref_t<decltype(exp)>>::max()) [[unlikely]]
      {
        if(mantissa == T{ 0 })
        {
          buff.start_idx -= 3;
          std::memcpy(&buff.array[buff.start_idx], "nan", 3);
        }
        else if(mantissa > T{ 0 })
        {
          buff.start_idx -= 3;
          std::memcpy(&buff.array[buff.start_idx], "inf", 3);
        }
        else if(mantissa == T{ -1 })
        {
          buff.start_idx -= 4;
          std::memcpy(&buff.array[buff.start_idx], "-inf", 4);
        }
        else
        {
          buff.start_idx -= PRECISION;
          std::memset(&buff.array[buff.start_idx], '0', PRECISION);
          buff.array[--buff.start_idx] = '.';
          buff.array[--buff.start_idx] = '0';
        }

        return buff;
      }

      using base_type = std::conditional_t<std::is_same_v<float, T>, uint32_t, uint64_t>;

      static const constexpr auto min_precision = Helpers::Math::Constexpr::ipow(base_type{ 10 }, std::numeric_limits<base_type>::digits10 - 1);

      const auto *exp_2 = &Constants::Tables::Floating<T>::DIGITS[exp + FloatingStruct::BIAS][0];

      uint32_t extra_digits;
      base_type digits_10;
      Helpers::Math::IEEE754::Multiply<T>(mantissa, exp_2, digits_10, extra_digits);

      static const constexpr auto base_5_rounding_table = Constants::Tables::GetRoundingTable<T, 5>();
      static const constexpr auto base_10_rounding_table = Constants::Tables::GetRoundingTable<T, 10>();

      const int exp_base_10_int = ((exp * 78'913) >> 18); //- exp_shft;

      base_type left, right;

      if(exp_base_10_int < 0)
      {
        const auto exp_10_abs = std::abs(exp_base_10_int) - 1; // since its 0.DIGITS we need to take off 1 exponent
        const auto quantity = PRECISION - exp_10_abs;

        left = 0;
        right = (exp_10_abs > PRECISION) ? 0 : digits_10;

        if(quantity >= 0 && quantity <= FloatingStruct::MAX_DIGITS10)
        {
          const auto &rounding_factor_10s = base_10_rounding_table[quantity];
          const auto &rounding_factor_5s = base_5_rounding_table[quantity];

          const auto remainder = right % rounding_factor_10s;

          if(remainder > rounding_factor_5s || (remainder == rounding_factor_5s))
          {
            right += rounding_factor_5s;
          }

          right /= rounding_factor_10s;
        }
      }
      else if(exp_base_10_int >= 0 && exp_base_10_int <= FloatingStruct::MAX_DIGITS10)
      {
        const auto &rounding_factor_10s = base_10_rounding_table[exp_base_10_int];

        left = digits_10 / rounding_factor_10s;
        right = digits_10 % rounding_factor_10s;

        // convert internal digit scale -> output precision scale
        if constexpr(std::numeric_limits<T>::digits10 > PRECISION)
        {
          right /= base_10_rounding_table[std::numeric_limits<T>::digits10 - PRECISION];
        }

        if(right >= min_precision)
        {
          left += right / min_precision;
          right %= min_precision;
        }
      }
      else
      {
        left = digits_10;
        right = 0;
      }

      Helpers::Numeric::Integral::ToStrReverseWriteToCharArrayForceAndCapLength<PRECISION>(right, buff, buff.start_idx);
      buff.array[--buff.start_idx] = '.';
      Helpers::Numeric::Integral::ToStrReverseWriteToCharArray(left, buff, buff.start_idx);

      return buff;
    }
  };

  template <RoundingBehavior BEHAVE, int32_t PRECISION, typename T>
    requires std::is_floating_point_v<T> && (Helpers::Templating::Assert::at_most_64_bit_double_radix_2<T>())
  static std::string ToStr(const T &input)
  {
    const auto buff = Helpers::Numeric::Floating::DigitsPrecision::ToStrCharArray<BEHAVE, PRECISION>(input);
    return std::string(&buff.array[buff.start_idx], sizeof(buff.array) - buff.start_idx);
  }
} // namespace Helpers::Numeric::Floating::DigitsPrecision

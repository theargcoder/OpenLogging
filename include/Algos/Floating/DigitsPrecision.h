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
          buff.start_idx -= PRECISION;
          std::memset(&buff.array[buff.start_idx], '0', PRECISION);
          buff.array[--buff.start_idx] = '.';
          buff.array[--buff.start_idx] = '0';
        }

        return buff;
      }

      const auto exp_2 = table[exp + Floating::BIAS];

      const auto digits_10 = static_cast<Floating::smallest_underlying>(mantissa * exp_2);

      static const constexpr auto precision = Helpers::Math::Constexpr::pow(typename Floating::smallest_underlying(10), Floating::MAX_DIGITS10);

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

      static const constinit auto FloatingRounding = FloatingStruct();
      static const constinit auto &table = FloatingRounding.DIGITS;

      // 1 for '+/-' also 1 for '.' also 1 for whatever
      static const constexpr auto SIZE_OF_BUFF = FloatingStruct::MAX_DIGITS10 + std::numeric_limits<T>::max_exponent10 + PRECISION + 3;

      Helpers::Numeric::Integral::char_array<SIZE_OF_BUFF> buff;

      buff.start_idx = SIZE_OF_BUFF;

      const auto frexpp = Helpers::Math::IEEE754(input);
      const auto &exp = frexpp.exponent;
      const auto &mantissa = frexpp.mantissa;

      if(exp == std::numeric_limits<decltype(frexpp.exponent)>::max()) [[unlikely]]
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

      static const constexpr auto min_precision = Helpers::Math::Constexpr::pow(typename FloatingStruct::smallest_underlying(10), std::numeric_limits<T>::digits10);
      static const constexpr auto max_precision = Helpers::Math::Constexpr::pow(typename FloatingStruct::smallest_underlying(10), std::numeric_limits<T>::digits10 + 1);

      const auto exp_2 = table[exp + FloatingStruct::BIAS];

      auto [rounding_results, digits_10] = Helpers::Math::IEEE754<T>::multiplyandround(mantissa, exp_2);

      auto exp_shft = 0;
      if(digits_10 < min_precision)
      {
        digits_10 *= 10;
        exp_shft++;
      }

      const auto exp_base_10_int = ((exp * 78'913) >> 18) - exp_shft;
      auto exp_10_abs = std::abs(exp_base_10_int) + ((exp_base_10_int < 0) ? -1 : 0);
      auto quantity = PRECISION - exp_10_abs;

      if(quantity >= 0 && quantity < FloatingStruct::MAX_DIGITS10)
      {
        static const constexpr auto base_5_rounding_table = Constants::Tables::GetRoundingTable<T, 5>();
        static const constexpr auto base_10_rounding_table = Constants::Tables::GetRoundingTable<T, 10>();

        const auto &rounding_factor_10s = base_10_rounding_table[quantity];
        const auto &rounding_factor_5s = base_5_rounding_table[quantity];

        const auto remainder = digits_10 % rounding_factor_10s;

        if(remainder > rounding_factor_5s
           || ((rounding_results == Helpers::Math::IEEE754<T>::RoundingResults::NO_ROUNDING) && (remainder == rounding_factor_5s && ((digits_10 & 1U) == false))))
        {
          digits_10 += rounding_factor_5s;
        }

        const auto overflow = digits_10 > max_precision;
        quantity += overflow;
        exp_10_abs -= overflow;

        digits_10 /= rounding_factor_10s;
      }

      const auto res_buff = Helpers::Numeric::Integral::ToStrCharArray<false>(digits_10);

      if(exp_base_10_int < 0)
      {
        if(exp_10_abs > PRECISION)
        {
          buff.start_idx -= PRECISION;
          std::memset(&buff.array[buff.start_idx--], '0', PRECISION);
        }
        else
        {
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
        if(exp_base_10_int < FloatingStruct::MAX_DIGITS10)
        {
          buff.start_idx -= PRECISION;
          std::memset(&buff.array[buff.start_idx], '0', PRECISION);
          const int after_pt_ct = FloatingStruct::MAX_DIGITS10 - exp_base_10_int;
          const int max_prescision = PRECISION < after_pt_ct ? PRECISION : after_pt_ct;
          const int qty = quantity < max_prescision ? quantity : max_prescision;
          std::memcpy(&buff.array[buff.start_idx--], &res_buff.array[res_buff.start_idx + exp_base_10_int], qty);
          buff.array[buff.start_idx--] = '.';
          if(exp_base_10_int == 0)
          {
            buff.array[buff.start_idx] = '0';
          }
          else
          {
            std::memcpy(&buff.array[buff.start_idx], &res_buff.array[res_buff.start_idx], exp_base_10_int);
          }
        }
        else
        {
          buff.start_idx -= PRECISION;
          std::memset(&buff.array[buff.start_idx], '0', PRECISION);
          buff.array[buff.start_idx--] = '.';
          const auto missing = exp_base_10_int - FloatingStruct::MAX_DIGITS10;
          buff.start_idx -= missing;
          std::memset(&buff.array[buff.start_idx--], '0', missing);
          buff.start_idx -= FloatingStruct::MAX_DIGITS10;
          std::memcpy(&buff.array[buff.start_idx--], &res_buff.array[res_buff.start_idx], sizeof(res_buff.array) - res_buff.start_idx);
        }
      }

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

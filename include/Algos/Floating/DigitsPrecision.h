#pragma once

#include <array>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <limits>
#include <string>
#include <type_traits>
#include <utility>

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

  template <typename T, size_t... I>
  static constexpr auto GetRoundingTableImpl(std::index_sequence<I...> /*unused*/)
  {
    constexpr auto N = sizeof...(I);
    return std::array<T, N>{ 5 * Helpers::Math::Constexpr::ipow(T{ 10 }, N - I)... };
  }

  template <typename T>
  static constexpr auto GetRoundingTable()
  {
    constexpr auto N = std::numeric_limits<T>::digits10;
    return GetRoundingTableImpl<T>(std::make_index_sequence<N>());
  }

  template <int32_t PRECISION, typename T>
    requires std::is_floating_point_v<T> && (Helpers::Templating::Assert::at_most_64_bit_double_radix_2<T>())
  struct CharArrayImpl<Helpers::Numeric::Floating::DigitsPrecision::RoundingBehavior::ROUND, PRECISION, T>
  {
    static auto ToStr(const T &input)
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

      auto digits_10 = static_cast<Floating::smallest_underlying>(mantissa * exp_2);

      const constexpr auto precision_min = Helpers::Math::Constexpr::pow(typename Floating::smallest_underlying(10), Floating::MAX_DIGITS10);
      const constexpr auto precision_max = Helpers::Math::Constexpr::pow(typename Floating::smallest_underlying(10), Floating::MAX_DIGITS10 + 1);

      int exp_shft = (digits_10 < precision_min) ? -1 : 0;

      const auto exp_base_10_int = ((exp * 78'913) >> 18) + exp_shft;
      auto exp_10_abs = std::abs(exp_base_10_int + 1);
      auto quantity = PRECISION - exp_10_abs;

      const constexpr auto rounding_table = GetRoundingTable<T>();

      if(quantity >= 0 && quantity < Floating::MAX_DIGITS10)
      {
        const auto &rounding_factor = rounding_table[quantity - exp_shft];
        digits_10 += rounding_factor;
        quantity -= digits_10 > precision_max;
        exp_10_abs += digits_10 > precision_max;
      }

      auto res_buff = Helpers::Numeric::Integral::ToStrCharArray<false>(digits_10);

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

  template <RoundingBehavior BEHAVE, int32_t PRECISION, typename T>
    requires std::is_floating_point_v<T> && (Helpers::Templating::Assert::at_most_64_bit_double_radix_2<T>())
  static std::string ToStr(const T &input)
  {
    const auto buff = Helpers::Numeric::Floating::DigitsPrecision::ToStrCharArray<BEHAVE, PRECISION>(input);
    return std::string(&buff.array[buff.start_idx], sizeof(buff.array) - buff.start_idx);
  }
} // namespace Helpers::Numeric::Floating::DigitsPrecision

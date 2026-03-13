#pragma once

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <limits>
#include <string>
#include <type_traits>

#include "include/Constants.h"

#include "include/Helpers/Helpers.h"
#include "include/Helpers/Math.h"
#include "include/Helpers/Templating.h"

struct Helpers::Numeric
{
  template <size_t N>
  struct char_array
  {
    char array[N];
  };

  template <bool FORCE_SIGN = false, typename T>
    requires std::is_integral_v<T>
  // 2 exra for lookuptable; 1 for '-'; 1 for null
  static std::string /* char_array<std::numeric_limits<T>::digits10 + 4 + 1> */ ToStr(const T &input)
  {
    const constexpr auto MAX_DIGITS10 = std::numeric_limits<T>::digits10 + 1;

    if(input == 0)
    {
      return "0";
    }

    char_array<std::numeric_limits<T>::digits10 + 4 + 1> buff;
    std::memset(&buff.array[0], '\0', sizeof(buff.array));

    const auto &IntegralStruct = Constants::IntegralDigitsTable();
    const auto &lookup_table = IntegralStruct.table;

    uint32_t i = MAX_DIGITS10 + 3; // not touching null terminator just in case
    if constexpr(std::is_same_v<int8_t, T> || std::is_same_v<uint8_t, T>)
    {
      if constexpr(std::is_signed_v<T>)
      {
        Helpers::Templating::Fillers::fill_decrement_char_array(buff.array, lookup_table, i, std::abs(input), 2, 1, 0);
      }
      else
      {
        Helpers::Templating::Fillers::fill_decrement_char_array(buff.array, lookup_table, i, input, 2, 1, 0);
      }
    }
    else
    {
      static constexpr T BASE = 1000;
      T rem, quot;
      rem = input % BASE, quot = input;
      while(quot || rem)
      {
        if constexpr(std::is_signed_v<T>)
        {
          Helpers::Templating::Fillers::fill_decrement_char_array(buff.array, lookup_table, i, std::abs(rem), 2, 1, 0);
        }
        else
        {
          Helpers::Templating::Fillers::fill_decrement_char_array(buff.array, lookup_table, i, rem, 2, 1, 0);
        }

        quot /= BASE;
        rem = quot % BASE;
      }
    }

    i++;

    if(buff.array[i] == '0')
    {
      buff.array[i++] = '\0';
      if(buff.array[i] == '0')
      {
        buff.array[i++] = '\0';
      }
    }

    if(input < 0)
    {
      buff.array[--i] = '-';
    }
    else if constexpr(FORCE_SIGN)
    {
      buff.array[--i] = '+';
    }

    return std::string(buff.array + i, sizeof(buff) - i);
  }

  template <typename T, int PRECISION = Constants::FloatingDigitsTable<T>::MAX_DIGITS10>
    requires std::is_floating_point_v<T> && (Helpers::Templating::Assert::at_most_64_bit_double_radix_2<T>())
  static std::string ToStr(const T &input)
  {

    constexpr T LOG_10_2 = std::log10(T{ 2 });
    using FloatingStruct = Constants::FloatingDigitsTable<T>;
    static const auto &table = FloatingStruct().table;

    if(input == 0)
    {
      return "0";
    }
    else if(std::isfinite(input) == false)
    {
      if(std::isnan(input))
      {
        return "nan";
      }
      return (input < 0) ? "-inf" : "inf";
    }

    // +1 for '+', +1 for '.' + 1 for 'e' + 1 for null
    // +4; +1; + 1
    char buff[PRECISION + 7];
    std::memset(&buff[0], '\0', sizeof(buff));

    int exp = 0;
    T mantissa = std::frexp(input, &exp);

    const auto exp_2 = table[exp + FloatingStruct::BIAS];

    const auto digits_10 = static_cast<Constants::FloatingDigitsTable<T>::smallest_underlying>(mantissa * exp_2);

    const constexpr auto DIGITS_10_PRES = FloatingStruct::MAX_DIGITS10 - PRECISION;
    const constexpr auto take_off_precision = Helpers::Math::Constexpr::pow(static_cast<Constants::FloatingDigitsTable<T>::smallest_underlying>(10), DIGITS_10_PRES);
    const constexpr auto precision = Helpers::Math::Constexpr::pow(static_cast<Constants::FloatingDigitsTable<T>::smallest_underlying>(10), PRECISION);
    typename Constants::FloatingDigitsTable<T>::smallest_underlying res = digits_10 / take_off_precision;

    int exp_shf = 0;
    if(res < precision)
    {
      exp_shf--;
    }

    size_t i = 0, idx = 0;
    const auto str = Numeric::ToStr<true>(res);

    buff[i++] = str[idx++];

    if(str.size() < PRECISION + 1)
    {
      buff[i++] = '0';
    }
    else
    {
      buff[i++] = str[idx++];
    }

    buff[i++] = '.';

    while(idx < str.size())
    {
      buff[i++] = str[idx++];
    }

    buff[i++] = 'e';

    const auto exp_base_10_int = static_cast<int>(std::floor(exp * LOG_10_2)) + exp_shf;

    // std::cout << "exp_base_10 = " << exp_base_10_int << "\n";

    const auto exp_str = Numeric::ToStr<true>(exp_base_10_int);

    std::memcpy(&buff[i], exp_str.data(), exp_str.size());

    i = 0;
    if(buff[i] == '+')
    {
      i++;
    }

    return std::string(&buff[i]);
  }
};

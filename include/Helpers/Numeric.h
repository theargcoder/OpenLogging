#pragma once

#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <format>
#include <iostream>
#include <string>
#include <type_traits>

#include "include/Constants.h"

#include "include/Helpers/Helpers.h"
#include "include/Helpers/Math.h"
#include "include/Helpers/Templating.h"

struct Helpers::Numeric
{
  template <typename T>
    requires std::is_integral_v<T>
  static std::string ToStr(const T &input)
  {
    const constexpr auto MAX_DIGITS10 = std::numeric_limits<T>::digits10 + 1;

    if(input == 0)
    {
      return "0";
    }

    // 2 exra for lookuptable
    char buff[MAX_DIGITS10 + 4]; // +2 for NULL terminaor and for case of having a '-'
    std::memset(&buff[0], '\0', sizeof(buff));

    const auto &IntegralStruct = Constants::IntegralDigitsTable();
    const auto &lookup_table = IntegralStruct.table;

    uint32_t i = MAX_DIGITS10 + 3; // not touching null terminator just in case
    if constexpr(std::is_same_v<int8_t, T> || std::is_same_v<uint8_t, T>)
    {
      if constexpr(std::is_signed_v<T>)
      {
        Helpers::Templating::Fillers::fill_decrement_char_array(buff, lookup_table, i, std::abs(input), 2, 1, 0);
      }
      else
      {
        Helpers::Templating::Fillers::fill_decrement_char_array(buff, lookup_table, i, input, 2, 1, 0);
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
          Helpers::Templating::Fillers::fill_decrement_char_array(buff, lookup_table, i, std::abs(rem), 2, 1, 0);
        }
        else
        {
          Helpers::Templating::Fillers::fill_decrement_char_array(buff, lookup_table, i, rem, 2, 1, 0);
        }

        quot /= BASE;
        rem = quot % BASE;
      }
    }

    i++;

    if(buff[i] == '0')
    {
      buff[i++] = '\0';
      if(buff[i] == '0')
      {
        buff[i++] = '\0';
      }
    }

    if(input < 0)
    {
      buff[--i] = '-';
    }

    return std::string(buff + i, sizeof(buff) - i);
  }

  template <int PRECISION = 3 /* FloatingDigitsTable::MAX_DIGITS10 */, typename T>
    requires std::is_floating_point_v<T> && (std::numeric_limits<T>::max_exponent <= Constants::FloatingDigitsTable<T>::BIAS + 1) && (std::numeric_limits<T>::radix == 2)
             && (PRECISION <= Constants::FloatingDigitsTable<T>::MAX_DIGITS10)
  static std::string ToStr(const T &input)
  {
    const constexpr T LOG_10_2 = 0.30103;
    static const auto FloatingStruct = Constants::FloatingDigitsTable<T>();
    static const auto &table = FloatingStruct.table;

    std::string buff;

    if(input == 0)
    {
      return "0";
    }

    int exp = 0;
    T mantissa = std::frexp(input, &exp);

    const auto mantissa_2 = mantissa;
    const auto exp_2 = table[exp + FloatingStruct.BIAS];

    std::cout << std::format("mantissa_2 = {:e} exp_2 = {}\n", mantissa_2, exp_2);

    const auto digits_10 = static_cast<int64_t>(mantissa_2 * exp_2);

    std::cout << std::format("digits_10 = {} \n", digits_10);
    std::cout << std::format("\nREAL EXPECTED RESULT = {:e} \n\n", input);

    const constexpr auto DIGITS_10_PRES = FloatingStruct.MAX_DIGITS10 - PRECISION;
    const constexpr auto precision = Helpers::Math::Constexpr::pow(10, DIGITS_10_PRES - 1);
    const auto res = digits_10 / precision;
    buff = Numeric::ToStr(res);

    buff.push_back('e');

    buff += Numeric::ToStr(static_cast<int>(exp * LOG_10_2) - DIGITS_10_PRES - FloatingStruct.MAX_DIGITS10);

    return buff;
  }
};

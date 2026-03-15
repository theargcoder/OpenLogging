#pragma once

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <string>
#include <type_traits>

#include "include/Constants/Constants.h"

#include "include/Helpers/Helpers.h"
#include "include/Helpers/Math.h"
#include "include/Helpers/Templating.h"

struct Helpers::Numeric
{
private:
  template <size_t N>
  struct char_array
  {
    char array[N];
  };

public:
  template <bool FORCE_SIGN = false, typename T>
    requires std::is_integral_v<T>
  static std::string ToStr(const T &input)
  {
    const auto &[st, buff] = Helpers::Numeric::ToStrCharArray<FORCE_SIGN>(input);
    return std::string(&buff.array[st], sizeof(buff.array) - st - 1);
  }

  template <bool FORCE_SIGN = false, typename T>
    requires std::is_integral_v<T>
  static std::pair<size_t, char_array<std::numeric_limits<T>::digits10 + 4 + 1>> ToStrCharArray(const T &input)
  {
    const constexpr auto MAX_DIGITS10 = std::numeric_limits<T>::digits10 + 1;

    char_array<MAX_DIGITS10 + 4> buff;
    size_t i = MAX_DIGITS10 + 3; // not touching null terminator just in case

    const constexpr auto BASE = 10;
    const bool NEGATIVE = input < 0;

    using UT = std::make_unsigned_t<T>;
    UT val = NEGATIVE ? -static_cast<UT>(input) : static_cast<UT>(input);

    if(val == 0)
    {
      buff.array[--i] = '0';
    }
    else
    {
      while(val)
      {
        const auto rem = val % BASE;
        val /= BASE;

        buff.array[--i] = '0' + rem;
      }
    }

    if(NEGATIVE)
    {
      buff.array[--i] = '-';
    }
    else if constexpr(FORCE_SIGN)
    {
      buff.array[--i] = '+';
    }

    return std::make_pair(i, buff);
  }

  template <typename T, int PRECISION = Constants::FloatingDigitsTable<T>::MAX_DIGITS10>
    requires std::is_floating_point_v<T> && (Helpers::Templating::Assert::at_most_64_bit_double_radix_2<T>())
  static std::string ToStr(const T &input)
  {
    const static constinit T LOG_10_2 = std::log10(T{ 2 });
    using FloatingStruct = Constants::FloatingDigitsTable<T>;
    static const auto &table = FloatingStruct().table;

    if(input == 0)
    {
      return std::string{ "0" };
    }
    else if(std::isfinite(input) == false)
    {
      if(std::isnan(input))
      {
        return std::string{ "nan" };
      }
      return (input < 0) ? std::string{ "-inf" } : std::string{ "inf" };
    }

    // +1 for '+', +1 for '.' + 1 for 'e' + 1 for null
    // +4; +1; + 1
    char buff[PRECISION + 7];
    std::memset(&buff[0], '\0', sizeof(buff));

    int exp = 0;
    T mantissa = std::frexp(input, &exp);

    const auto exp_2 = table[exp + FloatingStruct::BIAS];

    const auto digits_10 = static_cast<FloatingStruct::smallest_underlying>(mantissa * exp_2);

    const constexpr auto DIGITS_10_PRES = FloatingStruct::MAX_DIGITS10 - PRECISION;
    const constexpr auto take_off_precision = Helpers::Math::Constexpr::pow(static_cast<FloatingStruct::smallest_underlying>(10), DIGITS_10_PRES);
    const constexpr auto precision = Helpers::Math::Constexpr::pow(static_cast<FloatingStruct::smallest_underlying>(10), PRECISION);

    typename FloatingStruct::smallest_underlying res = digits_10 / take_off_precision;

    int exp_shf = 0;
    if(res < precision)
    {
      exp_shf--;
    }

    size_t i = 0;
    auto [idx, buff_str] = Numeric::ToStrCharArray<true>(res);
    const auto *str = &buff_str.array[0];
    const auto str_len = sizeof(buff_str.array) - idx - 1;

    buff[i++] = str[idx++];

    if(str_len < PRECISION + 1)
    {
      buff[i++] = '0';
    }
    else
    {
      buff[i++] = str[idx++];
    }

    buff[i++] = '.';

    const auto diff = sizeof(buff_str.array) - idx - 1;
    std::memcpy(&buff[i], &str[idx], diff);

    i += diff;

    buff[i++] = 'e';

    const auto exp_base_10_int = static_cast<int>(std::floor(exp * LOG_10_2)) + exp_shf;

    auto [exp_idx, exp_buff_str] = Numeric::ToStrCharArray<true>(exp_base_10_int);
    const auto *exp_str = &exp_buff_str.array[exp_idx];
    const auto exp_str_len = sizeof(exp_buff_str.array) - exp_idx - 1;

    std::memcpy(&buff[i], exp_str, exp_str_len);

    i = 0;
    if(buff[i] == '+')
    {
      i++;
    }

    return std::string(&buff[i]);
  }
};

//
///

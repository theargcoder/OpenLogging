#pragma once

#include <cmath>
#include <cstddef>
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
  static auto ToStrCharArray(const T &input)
  {
    const constexpr auto MAX_DIGITS10 = std::numeric_limits<T>::digits10 + 1;

    char_array<MAX_DIGITS10 + 4> buff;
    size_t i = MAX_DIGITS10 + 3; // starting at the end

    const constexpr auto BASE = 10;
    const bool NEGATIVE = input < 0;

    using UT = std::make_unsigned_t<T>;
    UT val = NEGATIVE ? static_cast<UT>(-(input + 1)) + 1 : static_cast<UT>(input);

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

  template <typename T>
    requires std::is_floating_point_v<T> && (Helpers::Templating::Assert::at_most_64_bit_double_radix_2<T>())
  static std::string ToStr(const T &input, const size_t &PRECISION = Constants::Tables::Floating<T>::MAX_DIGITS10)
  {
    using Floating = Constants::Tables::Floating<T>;
    using Truncate = Constants::Tables::Truncate<Floating::MAX_DIGITS10, typename Floating::smallest_underlying>;

    const static constinit T LOG_10_2 = std::log10(T{ 2 });
    static const auto &table = Floating().DIGITS;

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

    char buff[Floating::MAX_DIGITS10 + Floating::MAX_EXP_DIGITS10 + 6];

    int exp = 0;
    T mantissa = std::frexp(input, &exp);

    const auto exp_2 = table[exp + Floating::BIAS];

    const auto digits_10 = static_cast<Floating::smallest_underlying>(mantissa * exp_2);

    const auto DIGITS_10_PRES = Floating::MAX_DIGITS10 - PRECISION;

    const auto &trunc_table = Truncate().EXP_TO_RES;

    const auto &take_off_precision = trunc_table[DIGITS_10_PRES];
    const auto &precision = trunc_table[PRECISION];

    typename Floating::smallest_underlying res = digits_10 / take_off_precision;

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

    const auto &[exp_idx, exp_buff_str] = Numeric::ToStrCharArray<true>(exp_base_10_int);
    const auto *exp_str = &exp_buff_str.array[exp_idx];
    const auto exp_str_len = sizeof(exp_buff_str.array) - exp_idx - 1;

    std::memcpy(&buff[i], exp_str, exp_str_len);

    const auto ttl_size = i + exp_str_len;

    i = 0;
    if(buff[i] == '+')
    {
      i++;
    }

    return std::string(&buff[i], ttl_size);
  }
};

//
///

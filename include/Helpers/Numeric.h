#pragma once

#include <charconv>

#include <array>
#include <charconv>
#include <cstring>
#include <limits>
#include <string>
#include <type_traits>

#include "include/Helpers/Math.h"

#include "include/Constants/Constants.h"

#include "include/Helpers/Templating.h"

extern "C"
{
#include "../ryu/ryu/ryu.h"
}

namespace Helpers::Numeric::Std
{
  template <typename Type>
  static auto to_string(Type value)
  {
    std::array<char, 64> buf;

    if constexpr(std::is_floating_point_v<Type>)
    {
      int precision;
      if constexpr(std::is_same_v<Type, double>)
        precision = 14;
      else
        precision = 5;

      auto [ptr, ec] = std::to_chars(buf.data(), buf.data() + buf.size(), value, std::chars_format::scientific, precision);

      if(ec != std::errc{})
      {
        return std::string{};
      }

      return std::string(buf.data(), ptr);
    }
    else if constexpr(std::is_integral_v<Type>)
    {
      auto [ptr, ec] = std::to_chars(buf.data(), buf.data() + buf.size(), value);

      if(ec != std::errc{})
      {
        return std::string{};
      }

      return std::string(buf.data(), ptr);
    }
  }
} // namespace Helpers::Numeric::Std

namespace Helpers::Numeric::Ryu
{
  static auto ToStr(double v)
  {
    char buffer[32];
    int len = d2s_buffered_n(v, &buffer[0]);
    return std::string(&buffer[0], len);
  }

  static auto ToStr(float v)
  {
    char buffer[32];
    int len = f2s_buffered_n(v, &buffer[0]);
    return std::string(&buffer[0], len);
  }
} // namespace Helpers::Numeric::Ryu

namespace Helpers::Numeric::OpenLogging
{
  template <int N>
  struct char_array
  {
    int start_idx;
    char array[N];
  };

  template <bool FORCE_SIGN = false, typename T>
    requires std::is_integral_v<T>
  static auto ToStrCharArray(const T &input)
  {
    const constexpr auto MAX_DIGITS10 = std::numeric_limits<T>::digits10 + 2;

    char_array<MAX_DIGITS10> buff;
    char *__restrict__ it = &buff.array[MAX_DIGITS10];

    const constexpr auto BASE = 10;
    const bool NEGATIVE = input < 0;

    using UT = std::make_unsigned_t<T>;
    UT val = NEGATIVE ? static_cast<UT>(-(input + 1)) + 1 : static_cast<UT>(input);

    do
    {
      const auto rem = val % BASE;
      val /= BASE;

      *--it = '0' + rem;

    } while(val);

    if(NEGATIVE)
    {
      *--it = '-';
    }
    else if constexpr(FORCE_SIGN)
    {
      *--it = '+';
    }

    buff.start_idx = it - &buff.array[0];

    return buff;
  }

  template <bool FORCE_SIGN = false, typename T>
    requires std::is_integral_v<T>
  static std::string ToStr(const T &input)
  {
    const auto buff = Helpers::Numeric::OpenLogging::ToStrCharArray<FORCE_SIGN>(input);
    return std::string(&buff.array[buff.start_idx], sizeof(buff.array) - buff.start_idx);
  }

  template <bool FORCE_SIGN = false, int N, typename T>
    requires std::is_integral_v<T>
  static void ToStrReverseWriteToCharArray(const T &input, char_array<N> &out_char, const int &st_idx)
  {
    char *__restrict__ it = &out_char.array[st_idx];

    const constexpr auto BASE = 10;
    const bool NEGATIVE = input < 0;

    using UT = std::make_unsigned_t<T>;
    UT val = NEGATIVE ? static_cast<UT>(-(input + 1)) + 1 : static_cast<UT>(input);

    do
    {
      const auto rem = val % BASE;
      val /= BASE;

      *--it = '0' + rem;

    } while(val);

    if(NEGATIVE)
    {
      *--it = '-';
    }
    else if constexpr(FORCE_SIGN)
    {
      *--it = '+';
    }

    out_char.start_idx = it - &out_char.array[0];
  }

  template <typename T>
    requires std::is_floating_point_v<T> && (Helpers::Templating::Assert::at_most_64_bit_double_radix_2<T>())
  static auto ToStrCharArray(const T &input, const int &PRECISION = Constants::Tables::Floating<T>::MAX_DIGITS10)
  {
    using Floating = Constants::Tables::Floating<T>;

    static const auto &table = Floating().DIGITS;

    const constexpr auto SIZE_OF_BUFF = Floating::MAX_DIGITS10 + Floating::MAX_EXP_DIGITS10 + 6;

    char_array<SIZE_OF_BUFF> buff;

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

    Helpers::Numeric::OpenLogging::ToStrReverseWriteToCharArray<true>(exp_base_10_int, buff, SIZE_OF_BUFF);

    buff.array[--buff.start_idx] = 'e';

    auto res_buff = Helpers::Numeric::OpenLogging::ToStrCharArray<false>(digits_10);

    buff.start_idx -= PRECISION;
    std::memcpy(&buff.array[buff.start_idx--], &res_buff.array[res_buff.start_idx], PRECISION);

    buff.array[buff.start_idx] = '.';

    std::swap(buff.array[buff.start_idx], buff.array[buff.start_idx + 1]);

    return buff;
  }

  template <typename T>
    requires std::is_floating_point_v<T> && (Helpers::Templating::Assert::at_most_64_bit_double_radix_2<T>())
  static std::string ToStr(const T &input, const int &PRECISION = Constants::Tables::Floating<T>::MAX_DIGITS10)
  {
    const auto buff = Helpers::Numeric::OpenLogging::ToStrCharArray(input, PRECISION);
    return std::string(&buff.array[buff.start_idx], sizeof(buff.array) - buff.start_idx);
  }
} // namespace Helpers::Numeric::OpenLogging

//
///

#pragma once

#include <charconv>
#include <cmath>
#include <limits>
#include <string>
#include <type_traits>

#include "include/Helpers/Helpers.h"

#include "include/Helpers/Math.h"

#include "include/Constants/Constants.h"

#include "include/Helpers/Templating.h"

extern "C"
{
#include "../ryu/ryu/ryu.h"
}

struct Helpers::Numeric
{
public:
  struct Std
  {
    template <typename Type>
    static std::string to_string(Type value)
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
          return {};

        return std::string(buf.data(), ptr);
      }
      else if constexpr(std::is_integral_v<Type>)
      {
        auto [ptr, ec] = std::to_chars(buf.data(), buf.data() + buf.size(), value);

        if(ec != std::errc{})
          return {};

        return std::string(buf.data(), ptr);
      }
    }
  };

public:
  struct Ryu
  {
    static std::string ToStr(double v)
    {
      char buffer[32];
      int len = d2s_buffered_n(v, buffer);
      return std::string(buffer, len);
    }

    static std::string ToStr(float v)
    {
      char buffer[32];
      int len = f2s_buffered_n(v, buffer);
      return std::string(buffer, len);
    }
  };

private:
  template <int N>
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
    return std::string(&buff.array[st], sizeof(buff.array) - st);
  }

  template <bool FORCE_SIGN = false, typename T>
    requires std::is_integral_v<T>
  static auto ToStrCharArray(const T &input)
  {
    const constexpr auto MAX_DIGITS10 = std::numeric_limits<T>::digits10 + 1;

    char_array<MAX_DIGITS10 + 1> buff;
    char *__restrict__ it = &buff.array[MAX_DIGITS10 + 1];

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

    return std::make_pair(it - &buff.array[0], buff);
  }

  template <bool FORCE_SIGN = false, int N, typename T>
    requires std::is_integral_v<T>
  static int ToStrReverseWriteToCharArray(const T &input, char_array<N> &out_char, const int &st_idx)
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

    return it - &out_char.array[0];
  }
  template <typename T>
    requires std::is_floating_point_v<T> && (Helpers::Templating::Assert::at_most_64_bit_double_radix_2<T>())
  static std::string ToStr(const T &input, const int &PRECISION = Constants::Tables::Floating<T>::MAX_DIGITS10)
  {
    const auto &[st, buff] = Helpers::Numeric::ToStrCharArray(input, PRECISION);
    return std::string(&buff.array[st], sizeof(buff.array) - st);
  }

  template <typename T>
    requires std::is_floating_point_v<T> && (Helpers::Templating::Assert::at_most_64_bit_double_radix_2<T>())
  static auto ToStrCharArray(const T &input, const int &PRECISION = Constants::Tables::Floating<T>::MAX_DIGITS10)
  {
    using Floating = Constants::Tables::Floating<T>;
    using Truncate = Constants::Tables::Truncate<Floating::MAX_DIGITS10, typename Floating::smallest_underlying>;

    static const auto &table = Floating().DIGITS;

    const constexpr auto SIZE_OF_BUFF = Floating::MAX_DIGITS10 + Floating::MAX_EXP_DIGITS10 + 6;

    char_array<SIZE_OF_BUFF> buff;

    if(input == 0)
    {
      buff.array[0] = '0';
      return std::pair(0, buff);
    }
    else if(std::isfinite(input) == false)
    {
      if(std::isnan(input))
      {
        buff.array[0] = 'n';
        buff.array[1] = 'a';
        buff.array[2] = 'n';
        return std::pair(2, buff);
      }

      bool shft = input < 0;
      buff.array[0] = '-';
      buff.array[0 + shft] = 'i';
      buff.array[1 + shft] = 'n';
      buff.array[2 + shft] = 'f';
      return std::pair(2 + shft, buff);
    }

    /*
    int exp;
    const T mantissa = std::frexp(input, &exp);
    */
    const auto frexpp = Helpers::Math::IEEE754(input);
    const auto &exp = frexpp.exponent;
    const auto &mantissa = frexpp.mantissa;

    const auto exp_2 = table[exp + Floating::BIAS];

    const auto digits_10 = static_cast<Floating::smallest_underlying>(mantissa * exp_2);

    const auto DIGITS_10_PRES = Floating::MAX_DIGITS10 - PRECISION;

    const auto &trunc_table = Truncate().EXP_TO_RES;

    const auto &take_off_precision = trunc_table[DIGITS_10_PRES];
    const auto &precision = trunc_table[PRECISION];

    const typename Floating::smallest_underlying res = digits_10 / take_off_precision;

    int exp_shf = 0;
    if(res < precision)
    {
      exp_shf--;
    }

    const auto exp_base_10_int = ((exp * 78'913) >> 18) + exp_shf;

    auto exp_idx = Numeric::ToStrReverseWriteToCharArray<true>(exp_base_10_int, buff, SIZE_OF_BUFF);

    buff.array[--exp_idx] = 'e';

    auto res_idx = Numeric::ToStrReverseWriteToCharArray<false>(res, buff, exp_idx);

    const auto var_len = res_idx - exp_idx;

    if(var_len < PRECISION + 1)
    {
      buff.array[--res_idx] = '0';
    }

    buff.array[res_idx] = '.';

    std::swap(buff.array[res_idx], buff.array[res_idx + 1]);

    return std::make_pair(res_idx, buff);
  }
};

//
///

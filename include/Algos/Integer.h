#pragma once

#include "include/Helpers/Math.h"
#include "include/Helpers/Templating.h"
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <string>
#include <type_traits>

namespace Helpers::Numeric::Integral
{
  template <int N>
  struct char_array_start
  {
    int start_idx;
    char array[N];
  };

  template <int N>
  struct char_array_len
  {
    int length;
    char array[N];
  };

  template <bool FORCE_SIGN = false, typename T>
    requires std::is_integral_v<T> || std::is_same_v<T, __uint128_t>
  static auto ToStrCharArray(const T &input)
  {
    static const constexpr auto MAX_DIGITS10 = std::numeric_limits<T>::digits10 + 2;

    char_array_start<MAX_DIGITS10> buff;
    buff.start_idx = MAX_DIGITS10;

    static const constexpr auto BASE = 10;

    char *__restrict__ it = &buff.array[buff.start_idx];

    const bool NEGATIVE = input < 0;

    using UT = Helpers::Templating::Types::make_unsigned_t<T>;
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
    requires std::is_integral_v<T> || std::is_same_v<T, __uint128_t>
  static std::string ToStr(const T &input)
  {
    const auto buff = Helpers::Numeric::Integral::ToStrCharArray<FORCE_SIGN>(input);
    return std::string(&buff.array[buff.start_idx], sizeof(buff.array) - buff.start_idx);
  }

  template <bool FORCE_SIGN = false, int N, typename T>
    requires(std::is_integral_v<T> && std::is_unsigned_v<T>) || std::is_same_v<T, __uint128_t>
  static auto ToStrFowardWriteFixedLengthCharArrayReturnNextDigit(const T &input, char_array_len<N> &out_char, const int &digits)
  {
    char *__restrict__ it = &out_char.array[out_char.length];
    static constexpr auto BASE = 10;

    static const constexpr auto min_precision = Helpers::Math::Constexpr::ipow(T{ 10 }, std::numeric_limits<T>::digits10 - 1);
    auto divisor = min_precision;
    auto val = input;
    auto digits_count = digits + out_char.length;

    // Write exactly DIGITS characters
    for(; out_char.length < digits_count; ++out_char.length)
    {
      const auto digit = val / divisor;
      val %= divisor;

      *it++ = '0' + static_cast<char>(digit);

      divisor /= BASE;
    }

    return divisor ? static_cast<uint8_t>(val / divisor) : static_cast<uint8_t>(0);
  }

  template <bool FORCE_SIGN = false, int N, typename T>
    requires std::is_integral_v<T> || std::is_same_v<T, __uint128_t>
  static void ToStrReverseWriteToCharArray(const T &input, char_array_len<N> &out_char, const int &st_idx)
  {
    char *__restrict__ it = &out_char.array[st_idx];

    static const constexpr auto BASE = 10;
    const bool NEGATIVE = input < 0;

    using UT = Helpers::Templating::Types::make_unsigned_t<T>;
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
  }

  template <bool FORCE_SIGN = false, int N, typename T>
    requires std::is_integral_v<T> || std::is_same_v<T, __uint128_t>
  static void ToStrReverseWriteToCharArray(const T &input, char_array_start<N> &out_char, const int &st_idx)
  {
    char *__restrict__ it = &out_char.array[st_idx];

    static const constexpr auto BASE = 10;
    const bool NEGATIVE = input < 0;

    using UT = Helpers::Templating::Types::make_unsigned_t<T>;
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

  template <uint32_t CAP_FORCE_LENGTH, int N, typename T>
    requires std::is_integral_v<T> || std::is_same_v<T, __uint128_t>
  static void ToStrReverseWriteToCharArrayForceAndCapLength(const T &input, char_array_start<N> &out_char, const int &st_idx)
  {
    static_assert(N > CAP_FORCE_LENGTH, "cant force more chars than number of chars that can fit in the buffer bruh");

    char *__restrict__ it = &out_char.array[st_idx];

    static const constexpr auto BASE = 10;
    const bool NEGATIVE = input < 0;

    using UT = Helpers::Templating::Types::make_unsigned_t<T>;
    UT val = NEGATIVE ? static_cast<UT>(-(input + 1)) + 1 : static_cast<UT>(input);

    uint32_t i = 0;
    do
    {
      i++;
      const auto rem = val % BASE;
      val /= BASE;

      *--it = '0' + rem;

    } while(val && i < CAP_FORCE_LENGTH);

    const auto len = CAP_FORCE_LENGTH - i;

    it -= len;

    std::memset(it, '0', len);

    if(NEGATIVE)
    {
      *--it = '-';
    }

    out_char.start_idx = it - &out_char.array[0];
  }

  template <uint32_t CAP_LENGTH, int N, typename T>
    requires std::is_integral_v<T> || std::is_same_v<T, __uint128_t>
  static auto ToStrReverseWriteToCharArrayCapLengthStopAtNthCharReturnRemainder(const T &input, char_array_start<N> &out_char, const uint32_t &st_idx, const uint32_t stp_idx)
  {
    static const constexpr auto BASE = 10;
    const bool NEGATIVE = input < 0;

    char *__restrict__ it = &out_char.array[st_idx];

    using UT = Helpers::Templating::Types::make_unsigned_t<T>;
    UT val = NEGATIVE ? static_cast<UT>(-(input + 1)) + 1 : static_cast<UT>(input);
    UT rem;

    uint32_t i = 0;
    do
    {
      i++;
      rem = val % BASE;
      val /= BASE;

      *--it = '0' + rem;

    } while(val && i < stp_idx && i < CAP_LENGTH);

    if(NEGATIVE)
    {
      *--it = '-';
    }

    out_char.start_idx = it - &out_char.array[0];

    return (val) ? val % BASE : 0;
  }
} // namespace Helpers::Numeric::Integral

//
///

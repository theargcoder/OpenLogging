#pragma once

#include <limits>
#include <string>
#include <type_traits>

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
} // namespace Helpers::Numeric::OpenLogging

//
///

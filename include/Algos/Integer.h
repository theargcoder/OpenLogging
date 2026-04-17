#pragma once

#if defined(_MSC_VER) || defined(__x86_64__) || defined(__i386__)
#include <immintrin.h> // x86 SIMD
#elif defined(__ARM_NEON) || defined(__aarch64__)
#include <arm_neon.h> // ARM SIMD
#endif

#include "include/Helpers/Math.h"
#include "include/Helpers/Simd.h"
#include "include/Helpers/Templating.h"

#include <cstdint>
#include <cstring>
#include <limits>
#include <string>
#include <type_traits>

namespace Helpers::Numeric::Integral
{
  template <int N>
  struct char_array
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

    char_array<MAX_DIGITS10> buff;
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

  template <typename T>
    requires(std::is_integral_v<T> && std::is_unsigned_v<T>) || std::is_same_v<T, __uint128_t>
  static uint32_t ToStrFowardWriteSIMDReturnLen(char *__restrict__ buff, const T &input)
  {
#if defined(_MSC_VER) || defined(__x86_64__) || defined(__i386__)
#error "this functions has not been implemented for this architecture"
#elif defined(__ARM_NEON) || defined(__aarch64__)
    return Helpers::Simd::ARM64::WriteCharsToPtrFowardReturnLength<T>(buff, input);
#endif
  }

  template <typename T>
    requires std::is_integral_v<T>
  static std::string ToStrSIMD(const T &input)
  {

    char buff[32];

    const bool NEGATIVE = input < 0;
    using UT = Helpers::Templating::Types::make_unsigned_t<T>;
    UT val = NEGATIVE ? static_cast<UT>(-(input + 1)) + 1 : static_cast<UT>(input);

    uint32_t len = 0;
    if(NEGATIVE)
    {
      buff[len++] = '-';
    }

#if defined(_MSC_VER) || defined(__x86_64__) || defined(__i386__)
#error "this functions has not been implemented for this architecture"
#elif defined(__ARM_NEON) || defined(__aarch64__)
    len += Helpers::Simd::ARM64::WriteCharsToPtrFowardReturnLength<UT>((&buff[0] + len), val);
#endif

    return std::string(&buff[0], len);
  }

  template <int N, typename T>
    requires(std::is_integral_v<T> && std::is_unsigned_v<T>) || std::is_same_v<T, __uint128_t>
  static void ToStrReverseWriteToCharArrayResult(T &val, T &rem, char_array<N> &out_char)
  {
    char *__restrict__ it = &out_char.array[0] + out_char.start_idx;

    do
    {
      Helpers::Math::Magic::Modulo::mod_by_10_pow_n_void<1>(val, rem);

      *--it = '0' + rem;

    } while(val);

    out_char.start_idx = it - &out_char.array[0];
  }

  template <bool FORCE_SIGN = false, int N, typename T>
    requires std::is_integral_v<T> || std::is_same_v<T, __uint128_t>
  static void ToStrReverseWriteToCharArray(const T &input, char_array<N> &out_char, const int &st_idx)
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
  static void ToStrReverseWriteToCharArrayForceAndCapLength(const T &input, char_array<N> &out_char, const int &st_idx)
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
  static auto ToStrReverseWriteToCharArrayCapLengthStopAtNthCharReturnRemainder(const T &input, char_array<N> &out_char, const uint32_t &st_idx, const uint32_t stp_idx)
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

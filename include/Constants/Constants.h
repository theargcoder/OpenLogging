#pragma once

#include "include/Helpers/Math.h"
#include <array>
#include <cmath>
#include <cstdint>
#include <limits>
#include <type_traits>

namespace Constants
{
  struct Size
  {
    const constexpr static char MAX_FMT_SIZE = 8;
  };

  struct Interactive
  {
    const constexpr static bool debug_blink = true;
  };

  struct Delimiters
  {
    const constexpr static char open = '{';
    const constexpr static char close = '}';
  };

  struct Colors
  {
    const constexpr static uint8_t bool_value_true = 214;
    const constexpr static uint8_t bool_value_false = 214;
    const constexpr static uint8_t nullptrs = 196;
    const constexpr static uint8_t pointers = 237;
    const constexpr static uint8_t chars = 8;
    const constexpr static uint8_t ints = 130;
    const constexpr static uint8_t floats = ints;
    const constexpr static uint8_t doubles = ints;
    const constexpr static uint8_t strings = 76;
    const constexpr static uint8_t unknown = 12;
  };

  struct Ansi
  {
    const constexpr static char *begin = "\033[";
    const constexpr static char *none = "0;";
    const constexpr static char *italic = "3;";
    const constexpr static char *bold = "1;";
    const constexpr static char *blink = "5;";
    const constexpr static char *st_color = "38;5;";
    const constexpr static char *en_color = "m";
    const constexpr static char *reset = "\033[0m";
  };
} // namespace Constants

namespace Constants::Tables
{
  struct Integral
  {
  private:
    static const constexpr auto BASE = 10;
    static constexpr auto SIZE = 100;

  public:
    char DIGITS[SIZE][2];

  public:
    consteval Integral()
    {
      for(int i = 0; i < SIZE; i++)
      {
        DIGITS[i][0] = '0' + i / BASE;
        DIGITS[i][1] = '0' + i % BASE;
      }
    }
  };

  template <typename T>
    requires std::is_floating_point_v<T>
  struct Floating
  {
  public:
    static const constexpr auto BASE = 10;
    static const constexpr auto MIN_BIN_EXP = std::numeric_limits<T>::min_exponent - std::numeric_limits<T>::digits; // Smallest binary exponent (subnormal limit)
    static const constexpr auto MAX_BIN_EXP = std::numeric_limits<T>::max_exponent - 1;                              // Largest binary exponent
    static const constexpr auto BIAS = -MIN_BIN_EXP;                                                                 // Offset so that table[BIAS] corresponds to 2^0
    static const constexpr auto SIZE = MAX_BIN_EXP - MIN_BIN_EXP + 1;
    static const constexpr auto MAX_DIGITS10 = std::numeric_limits<T>::digits10;
    static const constexpr auto MAX_EXP_DIGITS10 = static_cast<decltype(MIN_BIN_EXP)>(Helpers::Math::Constexpr::log10(T{ std::numeric_limits<T>::max_exponent10 }));

  public:
    using smallest_underlying_unsigned = uint64_t;
    // we need that extra digit to avoid rounding errors (errors can happen after our min precision is met)
    static constexpr auto ACTUAL_DIGITS10 = std::numeric_limits<smallest_underlying_unsigned>::digits10;

  private:
    static constexpr auto INTERNAL_DIGITS10 = std::numeric_limits<__uint128_t>::digits10;

  public:
    smallest_underlying_unsigned DIGITS[SIZE];

  private:
    template <typename Type>
      requires std::is_integral_v<Type>
    consteval auto cabs(Type v) noexcept
    {
      using U = std::make_unsigned_t<Type>;
      return v < 0 ? U(-(v + 1)) + 1 : U(v);
    }

    template <typename UInt>
      requires(std::is_unsigned_v<UInt> || std::is_same_v<UInt, __uint128_t>)
    consteval UInt mul_trim10(UInt a, UInt b) noexcept
    {
      // --- 128-bit Path ---
      static const constexpr auto width = 64U;
      // Split inputs into 64-bit high and low parts
      auto a0 = static_cast<uint64_t>(a);
      auto a1 = static_cast<uint64_t>(a >> width);
      auto b0 = static_cast<uint64_t>(b);
      auto b1 = static_cast<uint64_t>(b >> width);

      // Cross multiplications
      __uint128_t p00 = static_cast<__uint128_t>(a0) * b0;
      __uint128_t p01 = static_cast<__uint128_t>(a0) * b1;
      __uint128_t p10 = static_cast<__uint128_t>(a1) * b0;
      __uint128_t p11 = static_cast<__uint128_t>(a1) * b1;

      // Accumulate into a 256-bit array: {x0, x1, x2, x3}
      auto r0 = static_cast<uint64_t>(p00);
      __uint128_t c1 = (p00 >> width) + static_cast<uint64_t>(p01) + static_cast<uint64_t>(p10);
      auto r1 = static_cast<uint64_t>(c1);
      __uint128_t c2 = (c1 >> width) + (p01 >> width) + (p10 >> width) + static_cast<uint64_t>(p11);
      auto r2 = static_cast<uint64_t>(c2);
      auto r3 = static_cast<uint64_t>((c2 >> width) + (p11 >> width));

      std::array<uint64_t, 4> x = { r0, r1, r2, r3 };

      // Trim decimal overflow: divide the 256-bit number by 10 until the top 128 bits are empty
      while(x[2] != 0 || x[3] != 0)
      {
        uint64_t rem = 0;
        for(int i = 3; i >= 0; --i)
        {
          __uint128_t val = (static_cast<__uint128_t>(rem) << width) | x[i];
          x[i] = static_cast<uint64_t>(val / BASE);
          rem = static_cast<uint64_t>(val % BASE);
        }
      }

      // Pack the remaining 128 bits back together
      return (static_cast<__uint128_t>(x[1]) << width) | x[0];
    }

    static consteval __uint128_t ipow10(std::size_t n) noexcept
    {
      __uint128_t r = 1;
      for(std::size_t i = 0; i < n; ++i)
      {
        r *= BASE;
      }
      return r;
    }

    static consteval __uint128_t trim_to_actual_digits(__uint128_t v) noexcept
    {
      constexpr auto lower = ipow10(INTERNAL_DIGITS10 - 1);
      constexpr auto upper = ipow10(INTERNAL_DIGITS10);

      while(v < lower)
      {
        v *= BASE;
      }

      if(v >= upper)
      {
        v /= BASE;
      }

      return v;
    }

    template <typename UInt>
      requires(std::is_unsigned_v<UInt> || std::is_same_v<UInt, __uint128_t>)
    consteval UInt step_and_trim(UInt v, UInt factor) noexcept
    {
      v = mul_trim10(v, factor);

      constexpr UInt upper = ipow10(INTERNAL_DIGITS10);
      if(v >= upper)
      {
        v /= BASE;
      }

      return v;
    }

  private:
    static constexpr const auto sldjflkjafs = std::numeric_limits<__uint128_t>::digits10;
    static const constexpr __uint128_t DISCARD_POWER = 100'000'000'000; // 11 digits (38 - 11 === 27)
                                                                        // 0 to 18,446,744,073,709,551,615
                                                                        // 1844'6744'0737'0955'1615
    static const constexpr __uint128_t LOW_PART_POWER = 100'000'000;    // 9 digits (27 - 8 === 19)
                                                                        // table[+1023] = 1609'7680'4482'7758'8922

    consteval auto uint_64_bit_msb_part(const __uint128_t &input)
    {
      // 1. Remove the bottom 12 digits (discard them)
      const __uint128_t remaining = input / DISCARD_POWER;

      // 3. The rest goes into 'hig'
      return static_cast<smallest_underlying_unsigned>(remaining / LOW_PART_POWER);
    }

    template <bool NEGATIVE>
    consteval void build_side() noexcept
    {
      using U = __uint128_t;

      U full_value = trim_to_actual_digits(U{ 1 });
      DIGITS[0 + BIAS] = uint_64_bit_msb_part(full_value);

      if constexpr(NEGATIVE)
      {
        for(int k = -1; k >= MIN_BIN_EXP; --k)
        {
          full_value = step_and_trim(full_value, U{ 5 });
          DIGITS[k + BIAS] = uint_64_bit_msb_part(full_value);
        }
      }
      else
      {
        for(int k = 1; k <= MAX_BIN_EXP; ++k)
        {
          full_value = step_and_trim(full_value, U{ 2 });
          DIGITS[k + BIAS] = uint_64_bit_msb_part(full_value);
        }
      }
    }

  public:
    consteval Floating() noexcept
    {
      build_side<true>();  // k = -1, -2, ...
      build_side<false>(); // k = +1, +2, ...
    }
  };

  template <typename Type, uint32_t BASE, std::size_t... I>
  static constexpr auto GetExponentialRoundingTableImpl(std::index_sequence<I...> /*unused*/)
  {
    constexpr auto N = sizeof...(I);
    return std::array<Type, N + 1>{ BASE * Helpers::Math::Constexpr::ipow(Type{ 10 }, N - I - 1)..., BASE };
  }

  template <typename Type, uint32_t BASE>
  static constexpr auto GetExponentialRoundingTable()
  {
    const constexpr auto N = std::numeric_limits<Type>::digits10 - 1;
    return GetExponentialRoundingTableImpl<Type, BASE>(std::make_index_sequence<N>());
  }

  template <typename T, uint32_t BASE, std::size_t... I>
  static constexpr auto GetRoundingTableImpl(std::index_sequence<I...> /*unused*/)
  {
    constexpr auto N = sizeof...(I);
    auto array = std::array<T, N + 1>{ BASE * Helpers::Math::Constexpr::ipow(T{ 10 }, N - I)... };
    array[N] = BASE;
    return array;
  }

  template <typename T, uint32_t BASE>
  static constexpr auto GetRoundingTable()
  {
    constexpr auto N = std::numeric_limits<T>::digits10;
    using IntType = Constants::Tables::Floating<T>::smallest_underlying_unsigned;
    return GetRoundingTableImpl<IntType, BASE>(std::make_index_sequence<N>());
  }
} // namespace Constants::Tables

//

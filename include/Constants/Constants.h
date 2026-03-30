#pragma once

#include <cstdint>
#include <limits>
#include <type_traits>

#include "include/Helpers/Math.h"

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
    requires std::is_integral_v<T>
  consteval auto cabs(T v) noexcept
  {
    using U = std::make_unsigned_t<T>;
    return v < 0 ? U(-(v + 1)) + 1 : U(v);
  }

  template <typename UInt>
    requires std::is_unsigned_v<UInt>
  consteval UInt mul_trim10(UInt a, UInt b) noexcept
  {
    using Wide = __uint128_t;
    constexpr Wide maxv = static_cast<Wide>(std::numeric_limits<UInt>::max());

    Wide x = static_cast<Wide>(a) * static_cast<Wide>(b);

    // Trim decimal overflow while keeping the leading digits.
    // This is the same idea as your old "divide by 10 when too large".
    while(x > maxv)
    {
      x /= 10;
    }

    return static_cast<UInt>(x);
  }

  template <typename UInt, typename Exp>
    requires std::is_unsigned_v<UInt> && std::is_integral_v<Exp>
  consteval UInt pow_trim10(UInt base, Exp exp) noexcept
  {
    UInt result = 1;
    auto e = cabs(exp);

    while(e != 0)
    {
      if(e & 1U)
      {
        result = mul_trim10(result, base);
      }

      e >>= 1U;

      if(e != 0)
      {
        base = mul_trim10(base, base);
      }
    }

    return result;
  }

  template <typename T>
    requires std::is_floating_point_v<T>
  struct Floating
  {
  public:
    static const constexpr T BASE = T{ 10 };
    static constexpr auto MIN_BIN_EXP = std::numeric_limits<T>::min_exponent - std::numeric_limits<T>::digits; // Smallest binary exponent (subnormal limit)
    static constexpr auto MAX_BIN_EXP = std::numeric_limits<T>::max_exponent - 1;                              // Largest binary exponent
    static constexpr auto BIAS = -MIN_BIN_EXP;                                                                 // Offset so that table[BIAS] corresponds to 2^0
    static constexpr auto SIZE = MAX_BIN_EXP - MIN_BIN_EXP + 1;
    static constexpr auto MAX_DIGITS10 = std::numeric_limits<T>::digits10;
    static constexpr auto ACTUAL_DIGITS10 = MAX_DIGITS10 + 1; // we need that extra digit to avoid rounding errors (errors can happen after our min precision is met)
    static constexpr auto MAX_EXP_DIGITS10 = static_cast<decltype(MIN_BIN_EXP)>(Helpers::Math::Constexpr::log10(T{ std::numeric_limits<T>::max_exponent10 }));

  public:
    using smallest_underlying = std::conditional_t<(MAX_DIGITS10 < std::numeric_limits<int>::digits10), int32_t, int64_t>;

    smallest_underlying DIGITS[SIZE]{};

  private:
    consteval auto compute(const auto &k_pow)
    {
      smallest_underlying res = 1;
      if(k_pow < 0)
      {
        // For 2^-n, your leading digits come from 5^n when decimal places are ignored.
        const auto res_abs = pow_trim10(std::make_unsigned_t<smallest_underlying>(5), k_pow);
        if(res_abs > std::numeric_limits<smallest_underlying>::max())
        {
          res = static_cast<smallest_underlying>(res_abs / 10);
        }
        else
        {
          res = static_cast<smallest_underlying>(res_abs);
        }
      }
      else if(k_pow > 0)
      {
        const auto res_abs = pow_trim10(std::make_unsigned_t<smallest_underlying>(2), k_pow);
        if(res_abs > std::numeric_limits<smallest_underlying>::max())
        {
          res = static_cast<smallest_underlying>(res_abs / 10);
        }
        else
        {
          res = static_cast<smallest_underlying>(res_abs);
        }
      }

      const auto n_digits = Helpers::Math::Constexpr::log10(res) + 1;

      if(n_digits >= ACTUAL_DIGITS10)
      {
        const auto digits_reduction = n_digits - ACTUAL_DIGITS10;
        res /= Helpers::Math::Constexpr::ipow(smallest_underlying{ 10 }, digits_reduction);
      }

      if(n_digits < ACTUAL_DIGITS10)
      {
        const auto digits_reduction = ACTUAL_DIGITS10 - n_digits;
        res *= Helpers::Math::Constexpr::ipow(smallest_underlying{ 10 }, digits_reduction);
      }

      return res;
    }

    template <bool INCREMENT, int SUB_BASE, int ST_PT, int DELIM>
    consteval void loop()
    {
      for(int k = ST_PT; (INCREMENT ? (k <= DELIM) : (k >= DELIM)); (INCREMENT) ? (k++) : (k--))
      {
        const auto idx = k + BIAS;

        const auto res = compute(k);

        DIGITS[idx] = res;
      }
    }

  public:
    consteval Floating()
    {
      loop<false, 5, 0, MIN_BIN_EXP>(); // negative exponents: multiply by 5 each step but scale as needed

      loop<true, 2, 0, MAX_BIN_EXP>(); // positive exponents: multiply by 2 each step but scale as needed
    }
  };

  template <typename T, uint32_t BASE, size_t... I>
  static constexpr auto GetRoundingTableImpl(std::index_sequence<I...> /*unused*/)
  {
    constexpr auto N = sizeof...(I);
    return std::array<T, N>{ BASE * Helpers::Math::Constexpr::ipow(T{ 10 }, N - I)... };
  }

  template <typename T, uint32_t BASE>
  static constexpr auto GetRoundingTable()
  {
    constexpr auto N = std::numeric_limits<T>::digits10;
    using IntType = Constants::Tables::Floating<T>::smallest_underlying;
    return GetRoundingTableImpl<IntType, BASE>(std::make_index_sequence<N>());
  }
} // namespace Constants::Tables

//

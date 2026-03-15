#pragma once

#include <cmath>
#include <cstdint>
#include <iostream>
#include <limits>
#include <type_traits>

#include "include/Helpers/Math.h"
#include "include/Helpers/Templating.h"

struct Constants
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

  struct Tables
  {
    template <auto POSSIBLE_DIGITS, typename T>
      requires std::is_integral_v<T>
    struct Truncate
    {
      static const constexpr T BASE = 10;
      T EXP_TO_RES[POSSIBLE_DIGITS + 1];

      consteval Truncate()
      {
        for(T i = 0; i <= T{ POSSIBLE_DIGITS }; i++)
        {
          EXP_TO_RES[i] = Helpers::Math::Constexpr::pow(BASE, i);
        }
      }
    };

    template <typename T>
      requires std::is_floating_point_v<T>
    struct Floating
    {
    public:
      static const constexpr T BASE = T{ 10 };
      static constexpr auto MIN_BIN_EXP = std::numeric_limits<T>::min_exponent - std::numeric_limits<T>::digits + 1; // Smallest binary exponent (subnormal limit)
      static constexpr auto MAX_BIN_EXP = std::numeric_limits<T>::max_exponent - 1;                                  // Largest binary exponent
      static constexpr auto BIAS = -MIN_BIN_EXP;                                                                     // Offset so that table[BIAS] corresponds to 2^0
      static constexpr auto SIZE = MAX_BIN_EXP - MIN_BIN_EXP + 1;

      static constexpr auto MAX_DIGITS10 = std::numeric_limits<T>::digits10;
      static constexpr auto ACTUAL_DIGITS10 = MAX_DIGITS10 + 1; // we need that extra digit to avoid rounding errors (errors can happen after our min precision is met)
      static constexpr auto MAX_EXP_DIGITS10 = static_cast<int>(std::log10(std::numeric_limits<T>::max_exponent10));
      static constexpr T MIN_SIG_FIGS = Helpers::Math::Constexpr::pow(BASE, ACTUAL_DIGITS10);
      static constexpr T MAX_SIG_FIGS = Helpers::Math::Constexpr::pow(BASE, ACTUAL_DIGITS10 + 1);

    private:
      // normalize works on a copy and returns the normalized value (no refs)
      [[nodiscard]] consteval T normalize(T val) const
      {
        while(val < T{ MIN_SIG_FIGS })
        {
          val *= BASE;
        }
        while(val >= T{ MAX_SIG_FIGS })
        {
          val /= BASE;
        }
        return val;
      }

      // loop multiplies incrementally and guards against overflow by dividing as needed
      template <bool INCREMENT, int SUB_BASE, int ST_PT, int DELIM>
      consteval void loop(T &val)
      {
        for(int k = ST_PT; (INCREMENT ? (k <= DELIM) : (k >= DELIM)); (INCREMENT) ? (k++) : (k--))
        {
          const auto idx = k + BIAS;

          const T norm = normalize(val);
          DIGITS[idx] = static_cast<smallest_underlying>(norm + T{ 0.5 }) / BASE; // round to closest

          while(val >= T{ MAX_SIG_FIGS } / T{ SUB_BASE })
          {
            val /= T{ BASE };
          }

          val *= T{ SUB_BASE };
        }
      }

    public:
      using smallest_underlying = std::conditional_t<(MAX_DIGITS10 < std::numeric_limits<int>::digits10), int32_t, int64_t>;

      smallest_underlying DIGITS[SIZE]{};

      consteval Floating()
      {
        T val = T{ 1 };

        loop<false, 5, 0, MIN_BIN_EXP>(val); // negative exponents: multiply by 5 each step but scale as needed

        val = T{ 1 };

        loop<true, 2, 0, MAX_BIN_EXP>(val); // positive exponents: multiply by 2 each step but scale as needed
      }
    };
  };
};

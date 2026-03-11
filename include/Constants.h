#pragma once

#include <cstdint>
#include <limits>
#include <type_traits>

#include "include/Helpers/Math.h"
#include "include/Helpers/Templating.h"

struct Constants
{
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

  struct IntegralDigitsTable
  {
  private:
  public:
    static const constexpr auto SIZE = 1000;
    static const constexpr auto BASE = 10;
    char table[SIZE][3]{};
    consteval IntegralDigitsTable()
    {
      for(int i = 0; i < SIZE; ++i)
      {
        const char hundreds = '0' + i / (BASE * BASE);
        const char tens = '0' + (i / BASE) % BASE;
        const char ones = '0' + i % BASE;

        Helpers::Templating::Fillers::fill_char_array(table, i, hundreds, tens, ones);
      }
    }
  };

  template <typename T>
    requires std::is_floating_point_v<T>
  struct FloatingDigitsTable
  {
  public:
    static const constexpr T BASE = T{ 10 };
    static const constexpr auto BIAS = std::numeric_limits<T>::max_exponent - 1;
    static const constexpr auto MAX_DIGITS10 = std::numeric_limits<T>::digits10;                   // safe LIMIT
    static const constexpr auto SIZE = BIAS * 2 + 1;                                               // -MAX_EXP -> 0 -> MAX_EXP - 1;
    static const constexpr T MIN_SIG_FIGS = Helpers::Math::Constexpr::pow(BASE, MAX_DIGITS10 - 1); // min precision
    static const constexpr T MAX_SIG_FIGS = Helpers::Math::Constexpr::pow(BASE, MAX_DIGITS10);     // upper bound for precision

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
    template <bool PLUS, int SUB_BASE>
    consteval void loop(T &val)
    {
      for(int k = 0; k <= BIAS; ++k)
      {
        const auto idx = (PLUS) ? BIAS + k : BIAS - k;

        // normalize the current val and store as integer
        const T norm = normalize(val);
        table[idx] = static_cast<smallest_underlying>(norm + T{ 0.5 }); // round to closest

        // prepare next val safely: if multiplying by SUB_BASE would push val >= MAX_SIG_FIGS, scale down first
        while(val >= T{ MAX_SIG_FIGS } / T{ SUB_BASE })
        {
          val /= T{ BASE };
        }

        val *= T{ SUB_BASE };
      }
    }

  public:
    using smallest_underlying = std::conditional_t<(MAX_DIGITS10 < std::numeric_limits<int>::digits10), int32_t, int64_t>;

    smallest_underlying table[SIZE]{};

    consteval FloatingDigitsTable()
    {
      T val = T{ 1 };

      loop<false, 5>(val); // negative exponents: multiply by 5 each step but scale as needed

      val = T{ 1 };

      loop<true, 2>(val); // positive exponents: multiply by 2 each step but scale as needed
    }
  };
};

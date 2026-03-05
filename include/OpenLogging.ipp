#pragma once

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <iostream>
#include <limits>
#include <type_traits>

#include "OpenLogging.h"

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
};

struct Helpers
{
public:
  struct Numeric;
  struct String;

  struct Template
  {
    struct Types
    {
      template <typename T>
      static constexpr bool is_char_v()
      {
        using BaseType = std::remove_cvref_t<T>;
        return std::is_same_v<BaseType, char>;
      }

      template <typename T>
      static constexpr bool is_numeric_v()
      {
        using BaseType = std::remove_cvref_t<T>;
        return (std::is_integral_v<BaseType> && !std::is_same_v<BaseType, bool>) || std::is_floating_point_v<BaseType>;
      }

      template <typename T>
      static constexpr bool is_not_numeric_v()
      {
        return !is_numeric_v<T>();
      }

      template <typename T>
      static constexpr bool is_string_v()
      {
        using BaseType = std::remove_cvref_t<T>;
        return std::is_same_v<BaseType, char *> || std::is_same_v<BaseType, const char *> || std::is_same_v<BaseType, std::string>;
      }

      template <typename T>
      static constexpr bool is_bool_v()
      {
        using BaseType = std::remove_cvref_t<T>;
        return std::is_same_v<BaseType, bool>;
      }
    };
  };

  template <size_t M, typename T>
    requires(Template::Types::is_numeric_v<T>())
  static std::string PritntWithFormat(const T &var, const char (&format)[M])
  {
    std::string number_in_str = ::Helpers::Numeric::ToStr(var);

    // @@@ do the .3f or 3d logic bs here ....

    return number_in_str;
  }

  template <size_t M, typename T>
    requires(Template::Types::is_string_v<T>())
  static std::string PritntWithFormat(const T &var, const char (&format)[M])
  {
    return std::string(var);
  }

  template <size_t M, typename T>
  static std::string PritntWithFormat(const T &var, const char (&format)[M])
  {
    return "NOT IMPLEMENTED YET";
  }

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

        fill_char_array(table, i, hundreds, tens, ones);
      }
    }
  };

  struct FloatingDigitsTable
  {
  public:
    static const constexpr int BIAS = 1023;
    static const constexpr auto MAX_DIGITS10 = 18; // its log_10( 2 ^ 63);

  private:
    static const constexpr int SIZE = BIAS * 2 + 1; // -MAX_EXP -> 0 -> MAX_EXP - 1;
    static const constexpr uint64_t MAX = std::numeric_limits<int64_t>::max();

  public:
    int64_t table[SIZE]{};

    consteval FloatingDigitsTable()
    {
      unsigned __int128 val = 1;

      for(int k = 0; k <= BIAS; ++k)
      {
        table[BIAS - k] = static_cast<int64_t>(val);

        val *= 5;

        while(val > MAX)
          val /= 10;
      }

      val = 1;

      for(int k = 0; k <= BIAS; ++k)
      {
        table[BIAS + k] = static_cast<int64_t>(val);

        val *= 2;

        while(val > MAX)
          val /= 10;
      }
    }
  };

  template <size_t M, size_t P, typename... Chars>
    requires(std::is_same_v<char, Chars> && ...)
  static consteval void fill_char_array(char (&array)[M][P], auto &idx, const Chars &...args)
  {
    size_t a = 0;
    ((array[idx][a++] = args), ...);
  }

  template <size_t A, size_t X, size_t Y, typename... Args>
    requires(std::is_integral_v<Args> && ...)
  static void fill_decrement_char_array(char (&dest)[A], const char (&src)[X][Y], uint32_t &dest_idx, const auto &src_idx, const Args &...args)
  {
    ((dest[dest_idx--] = src[src_idx][args]), ...);
  }

  // impl
  struct Numeric
  {
    template <typename T>
      requires std::is_integral_v<T>
    static std::string ToStr(const T &input)
    {
      const constexpr auto MAX_DIGITS10 = std::numeric_limits<T>::digits10 + 1;

      if(input == 0)
      {
        return "0";
      }

      // 2 exra for lookuptable
      char buff[MAX_DIGITS10 + 4]; // +2 for NULL terminaor and for case of having a '-'
      std::memset(&buff[0], '\0', sizeof(buff));

      const auto &IntegralStruct = IntegralDigitsTable();
      const auto &lookup_table = IntegralStruct.table;

      uint32_t i = MAX_DIGITS10 + 3; // not touching null terminator just in case
      if constexpr(std::is_same_v<int8_t, T> || std::is_same_v<uint8_t, T>)
      {
        if constexpr(std::is_signed_v<T>)
        {
          fill_decrement_char_array(buff, lookup_table, i, std::abs(input), 2, 1, 0);
        }
        else
        {
          fill_decrement_char_array(buff, lookup_table, i, input, 2, 1, 0);
        }
      }
      else
      {
        static constexpr T BASE = 1000;
        T rem, quot;
        rem = input % BASE, quot = input;
        while(quot || rem)
        {
          if constexpr(std::is_signed_v<T>)
          {
            fill_decrement_char_array(buff, lookup_table, i, std::abs(rem), 2, 1, 0);
          }
          else
          {
            fill_decrement_char_array(buff, lookup_table, i, rem, 2, 1, 0);
          }

          quot /= BASE;
          rem = quot % BASE;
        }
      }

      i++;

      if(buff[i] == '0')
      {
        buff[i++] = '\0';
        if(buff[i] == '0')
        {
          buff[i++] = '\0';
        }
      }

      if(input < 0)
      {
        buff[--i] = '-';
      }

      return std::string(buff + i, sizeof(buff) - i);
    }

    template <typename T>
      requires std::is_floating_point_v<T> && (std::numeric_limits<T>::radix == 2)
    static std::string ToStr(const T &input)
    {
      const constexpr T LOG_10_2 = 0.30103;
      const auto &FloatingStruct = FloatingDigitsTable();
      const auto &table = FloatingStruct.table;

      std::string buff;

      if(input == 0)
      {
        return "0";
      }

      int exp = 0;
      T mantissa = std::frexp(input, &exp);

      std::cout << "mantissa = " << std::format("{:e} \n", mantissa);
      std::cout << "exp = " << exp << "\n";

      const auto mantissa_2 = mantissa * 2.0;
      const auto exp_2 = table[exp - 1 + FloatingStruct.BIAS];

      const auto digits_10 = mantissa_2 * exp_2;
      const auto digits_10_int = static_cast<int64_t>(digits_10);

      std::cout << "mantissa_2 = " << mantissa_2 << '\n';
      std::cout << "exp_2 = " << exp_2 << '\n';
      std::cout << "digits_10 (double )= " << digits_10 << '\n';
      std::cout << "digits_10_int = " << digits_10_int << '\n';

      std::cout << "to_str = " << std::format("{:e} \n", input);

      buff = ::Helpers::Numeric::ToStr(digits_10_int);

      buff.push_back('e');

      buff += ::Helpers::Numeric::ToStr(static_cast<int>(exp * LOG_10_2) - FloatingStruct.MAX_DIGITS10);

      return buff;
    }
  };

  struct String
  {
    template <bool ANSI_SCAPE_SEQUENCES, size_t M, typename T>
    static std::string AnsiConcatenation(const T &var, const char (&format)[M], const uint8_t &color_of_this_template)
    {
      std::string buffer;

      if constexpr(ANSI_SCAPE_SEQUENCES)
      {
        if constexpr(M > 0)
        {
          buffer += Constants::Ansi::begin + ((Constants::Interactive::debug_blink) ? Constants::Ansi::blink : std::string{}) + Constants::Ansi::st_color;
          buffer += Helpers::Numeric::ToStr(color_of_this_template) + Constants::Ansi::en_color + ::Helpers::PritntWithFormat(var, format);
          buffer += Constants::Ansi::reset;
        }
        else if constexpr(Helpers::Template::Types::is_string_v<T>())
        {
          buffer += Constants::Ansi::begin + ((Constants::Interactive::debug_blink) ? Constants::Ansi::blink : std::string{}) + Constants::Ansi::st_color;
          buffer += Helpers::Numeric::ToStr(color_of_this_template) + Constants::Ansi::en_color + std::string(var);
          buffer += Constants::Ansi::reset;
        }
      }
      else
      {
        if constexpr(M > 0)
        {
          buffer += ::Helpers::PritntWithFormat(var, format);
        }
        else if constexpr(Helpers::Template::Types::is_string_v<T>())
        {
          buffer += std::string(var);
        }
      }

      return buffer;
    }
  };
};

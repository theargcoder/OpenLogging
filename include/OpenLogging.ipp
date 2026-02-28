#pragma once

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cstring>
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

  // impl
  struct Numeric
  {
    template <typename T>
      requires std::is_integral_v<T>
    static std::string ToStr(const T &input)
    {
      const constexpr T BASE = T{ 10 };
      const constexpr auto MAX_DIGITS10 = std::numeric_limits<T>::digits10 + 1;

      char buff[MAX_DIGITS10 + 2];       // +2 for NULL terminaor and for case of having a '-'
      char buff_final[MAX_DIGITS10 + 2]; //
      std::memset(&buff[0], '\0', sizeof(buff));
      std::memset(&buff_final[0], '\0', sizeof(buff));

      if(input == 0)
      {
        return "0";
      }

      T rem = input % BASE, quot = input;

      auto i = MAX_DIGITS10 + 1;
      while(quot || rem)
      {
        if constexpr(std::is_signed_v<T>)
        {
          buff[i] = '0' + std::abs(rem);
        }
        else
        {
          buff[i] = '0' + rem;
        }

        quot /= BASE;
        rem = quot % BASE;
        --i;
      }

      if(input < 0)
      {
        buff[i] = '-';
      }

      i = 0;
      for(const auto &cha : buff)
      {
        if(cha == '\0')
        {
          continue;
        }
        buff_final[i++] = cha;
      }

      return std::string(&buff_final[0]);
    }

    template <typename T>
      requires std::is_floating_point_v<T>
    static std::string ToStr(const T &input)
    {
      static const constexpr auto BASE = 10;
      static const constexpr auto MAX_PRECISION = std::numeric_limits<T>::digits10;

      std::string buff;

      if(input == 0)
      {
        return "0";
      }

      int exp10 = std::floor(std::log10(std::abs(input)));
      T power_of_10 = std::pow(T{ BASE }, exp10);
      std::cout << "\n\n\t input  === " << input << '\n';
      std::cout << "\\t exp10 . power_of_10 === " << exp10 << " . " << power_of_10 << '\n' << '\n';

      bool once = true;
      if(input < T{ 0 })
      {
        buff.push_back('-');
      }

      T input_abs = std::abs(input);

      for(int i = 0; i < MAX_PRECISION; ++i)
      {
        int digit = static_cast<int>(input_abs / power_of_10);
        buff.push_back('0' + std::abs(digit));

        if(once && (i == exp10 || exp10 < 0 || (i == 0 && std::abs(exp10) > std::numeric_limits<T>::max_digits10)))
        {
          once = false;
          buff.push_back('.');
        }

        input_abs -= static_cast<T>(digit) * power_of_10;
        power_of_10 /= T{ BASE };
      }

      buff.push_back('e');
      buff += ToStr(exp10);

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

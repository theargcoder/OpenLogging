#pragma once

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <string>

#include "include/Constants/Constants.h"

#include "include/Helpers/Helpers.h"

#include "include/Helpers/Numeric.h"
#include "include/Helpers/Print.h"

struct Helpers::String
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
        buffer += Helpers::Numeric::ToStr(color_of_this_template) + Constants::Ansi::en_color + Helpers::Print::WithFormat(var, format);
        buffer += Constants::Ansi::reset;
      }
      else if constexpr(Helpers::Templating::Types::is_string_v<T>())
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
        buffer += Helpers::Print::WithFormat(var, format);
      }
      else if constexpr(Helpers::Templating::Types::is_string_v<T>())
      {
        buffer += std::string(var);
      }
    }

    return buffer;
  }
};

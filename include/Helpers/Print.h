#pragma once

#include "include/Helpers/Helpers.h"

#include "include/Helpers/Numeric.h"
#include "include/Helpers/Templating.h"
#include <limits>
#include <type_traits>

struct Helpers::Print
{
  template <size_t M, typename T>
    requires(Helpers::Templating::Types::is_numeric_v<T>())
  static std::string WithFormat(const T &var, const char (&format)[M])
  {
    std::string number_in_str;

    if constexpr(std::is_floating_point_v<T>)
    {
      const auto prescicion = (format[0] != '\0') ? std::stoi(format) : std::numeric_limits<T>::digits10;
      number_in_str += Helpers::Numeric::ToStr(var, prescicion);
    }
    else
    {
      number_in_str += Helpers::Numeric::ToStr(var);
    }

    return number_in_str;
  }

  template <size_t M, typename T>
    requires(Helpers::Templating::Types::is_string_v<T>())
  static std::string WithFormat(const T &var, const char (&format)[M])
  {
    return std::string(var);
  }

  template <size_t M, typename T>
  static std::string WithFormat(const T &var, const char (&format)[M])
  {
    return "NOT IMPLEMENTED YET";
  }
};

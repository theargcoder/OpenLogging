#pragma once

#include "include/Helpers/Helpers.h"

#include "include/Helpers/Numeric.h"
#include "include/Helpers/Templating.h"

struct Helpers::Print
{
  template <size_t M, typename T>
    requires(Helpers::Templating::Types::is_numeric_v<T>())
  static std::string WithFormat(const T &var, const char (&format)[M])
  {
    std::string number_in_str = Helpers::Numeric::ToStr(var);

    // @@@ do the .3f or 3d logic bs here ....

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

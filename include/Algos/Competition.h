#pragma once

#include <charconv>

#include <array>
#include <charconv>
#include <string>
#include <type_traits>

extern "C"
{
#include "../ryu/ryu/ryu.h"
}

namespace Helpers::Numeric::Std
{
  template <typename Type>
  static auto to_string(Type value)
  {
    std::array<char, 64> buf;

    if constexpr(std::is_floating_point_v<Type>)
    {
      int precision;
      if constexpr(std::is_same_v<Type, double>)
        precision = 14;
      else
        precision = 5;

      auto [ptr, ec] = std::to_chars(buf.data(), buf.data() + buf.size(), value, std::chars_format::scientific, precision);

      if(ec != std::errc{})
      {
        return std::string{};
      }

      return std::string(buf.data(), ptr);
    }
    else if constexpr(std::is_integral_v<Type>)
    {
      auto [ptr, ec] = std::to_chars(buf.data(), buf.data() + buf.size(), value);

      if(ec != std::errc{})
      {
        return std::string{};
      }

      return std::string(buf.data(), ptr);
    }
  }
} // namespace Helpers::Numeric::Std

namespace Helpers::Numeric::Ryu
{
  static auto ToStr(double v)
  {
    char buffer[32];
    int len = d2s_buffered_n(v, &buffer[0]);
    return std::string(&buffer[0], len);
  }

  static auto ToStr(float v)
  {
    char buffer[32];
    int len = f2s_buffered_n(v, &buffer[0]);
    return std::string(&buffer[0], len);
  }
} // namespace Helpers::Numeric::Ryu

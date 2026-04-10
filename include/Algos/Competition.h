#pragma once

#include <charconv>

#include <array>
#include <charconv>
#include <cstdint>
#include <string>
#include <type_traits>

extern "C"
{
#include "../ryu/ryu/ryu.h"
}

namespace Helpers::Numeric::Std
{
  template <bool SCIENTIFIC, typename Type>
  static auto to_string(Type value, const auto &PRECISION)
  {
    std::array<char, 64> buf;

    if constexpr(std::is_floating_point_v<Type>)
    {
      if constexpr(SCIENTIFIC)
      {
        auto [ptr, ec] = std::to_chars(buf.data(), buf.data() + buf.size(), value, std::chars_format::scientific, PRECISION);
        if(ec != std::errc{})
        {
          return std::string{};
        }

        return std::string(buf.data(), ptr);
      }
      else
      {
        auto [ptr, ec] = std::to_chars(buf.data(), buf.data() + buf.size(), value, std::chars_format::fixed, PRECISION);
        if(ec != std::errc{})
        {
          return std::string{};
        }

        return std::string(buf.data(), ptr);
      }
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
  static auto ToStr(double v, const auto &PRECISION)
  {
    char buffer[32];
    int len = d2exp_buffered_n(v, PRECISION, &buffer[0]);
    return std::string(&buffer[0], len);
  }

  static auto ToStr(float v)
  {
    char buffer[32];
    int len = f2s_buffered_n(v, &buffer[0]);
    return std::string(&buffer[0], len);
  }

} // namespace Helpers::Numeric::Ryu

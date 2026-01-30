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

template <OpenLogging::LogType type, typename... types>
void OpenLogging::general_logger(const _valid_string_format_<sizeof...(types)> &fmt, const types &...args)
{
  if constexpr(sizeof...(args) == 0)
  {
    constexpr int32_t one_hundred = 100;

    auto time_now = std::chrono::high_resolution_clock::now();
    long time_seconds = std::chrono::duration_cast<std::chrono::seconds>(time_now.time_since_epoch()).count();
    long time_microseconds_div_100 = std::chrono::duration_cast<std::chrono::microseconds>(time_now.time_since_epoch()).count() / one_hundred;
    const auto time_fmt = _time_formatted_(time_seconds);
    const auto time_str_micro_segment = _time_mili_format_(time_microseconds_div_100);

    const std::string begin_log_str = time_fmt + time_str_micro_segment + ' ';

    std::cout << begin_log_str;
    std::cout << OpenLogging::_return_log_type_str_<type>();
    std::cout << fmt.str << '\n';
  }
  else
  {
    constexpr size_t buff_size_for_format = 10;
    constexpr int32_t one_hundred = 100;
    const auto *str = fmt.str;
    const auto N = fmt.N;
    static char format[buff_size_for_format];
    auto time_now = std::chrono::high_resolution_clock::now();
    long time_seconds = std::chrono::duration_cast<std::chrono::seconds>(time_now.time_since_epoch()).count();
    long time_microseconds_div_100 = std::chrono::duration_cast<std::chrono::microseconds>(time_now.time_since_epoch()).count() / one_hundred;
    const auto time_fmt = _time_formatted_(time_seconds);
    const auto time_str_micro_segment = _time_mili_format_(time_microseconds_div_100);

    const std::string begin_log_str = time_fmt + time_str_micro_segment + ' ';

    std::cout << begin_log_str;
    std::cout << OpenLogging::_return_log_type_str_<type>();
    size_t i = 0;
    bool is_curr_backlash, is_prev_backlash = str[0] == '\\';
    (
        [&](const auto &_arg)
        {
          for(; i < N; i++)
          {
            is_curr_backlash = str[i] == '\\';
            if(is_prev_backlash && is_curr_backlash)
            {
              is_prev_backlash = false;
              std::cout << str[i];
              continue;
            }
            else if(is_prev_backlash || (!is_curr_backlash && str[i] != _open_))
            {
              std::cout << str[i];
            }
            else if(str[i] == _open_)
            {
              i++; // we skip the starting formatting delimiter
              int f = 0;
              // guaranteed to have the closing delim due to the __valid_string_format__'s nature
              while(str[i] != _close_)
                format[++f] = str[i++];

              // i++; // we skip the ending formatting delimiter

              std::cout << _to_string_into_buff_(_arg, format);
            }
            is_prev_backlash = str[i] == '\\';
          }
        }(args),
        ...);

    std::cout << '\n';
  }
}

namespace OpenLogging_::Helpers_
{
  template <typename T>
    requires std::is_floating_point_v<T>
  static std::string _numeric_to_str_buff_(T input)
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

    for(int i = 0; i < MAX_PRECISION; ++i)
    {
      int digit = static_cast<int>(input / power_of_10);
      buff.push_back('0' + std::abs(digit));

      input -= static_cast<T>(digit) * power_of_10;
      power_of_10 /= T{ BASE };

      if(i == 0 && exp10 >= 0)
      {
        // Handle decimal point logic here
        // or whatever bruh
      }
    }

    // @@@ TODO
    // idea return the std::fexp exponent after all the precision bits to do 1023802E+-EXPONENT separated by ,???

    return buff;
  }

  template <typename T>
    requires std::is_integral_v<T> && std::is_unsigned_v<T>
  static std::string _numeric_to_str_buff_(T input)
  {
    static const constexpr T BASE = T{ 10 };

    std::string buff;

    if(input == 0)
    {
      return "0";
    }

    auto quot = input / BASE;
    auto rem = input % BASE;

    while(quot || rem)
    {
      buff.push_back('0' + rem);

      quot /= BASE;
      rem = quot % BASE;
    }

    std::ranges::reverse(buff);

    return buff;
  }

  template <typename T>
    requires std::is_integral_v<T> && std::is_signed_v<T>
  static std::string _numeric_to_str_buff_(T input)
  {
    static const constexpr T BASE = T{ 10 };

    if(input == 0)
    {
      return "0";
    }

    std::string buff;

    auto div = std::div(input, BASE);

    while(div.rem || div.quot)
    {
      buff.push_back('0' + std::abs(div.rem));
      div = std::div(div.quot, BASE);
    }

    if(input < 0)
    {
      buff.push_back('-');
    }

    std::ranges::reverse(buff);

    return buff;
  }

} // namespace OpenLogging_::Helpers_

template <size_t M, typename T>
std::string OpenLogging::_print_with_format_(const T &arg, const char (&format)[M])
{
  if constexpr(std::is_pointer_v<T>)
  {
    auto input_str = OpenLogging_::Helpers_::_numeric_to_str_buff_(reinterpret_cast<std::uintptr_t>(arg));
    return input_str;
  }
  else if constexpr(std::is_integral_v<T>)
  {
    auto input_str = OpenLogging_::Helpers_::_numeric_to_str_buff_(arg);
    // const auto input_str = std::to_string(arg);
    return input_str;
  }
  else if constexpr(std::is_floating_point_v<T>)
  {
    auto input_str = OpenLogging_::Helpers_::_numeric_to_str_buff_(arg);
    // const auto input_str = std::to_string(arg);
    return input_str;
  }
  else
  {
    return "TBD";
  }
}

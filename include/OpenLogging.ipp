#pragma once

#include <chrono>
#include <iostream>

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

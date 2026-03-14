#pragma once

#include <chrono>
#include <cstddef>
#include <iostream>

#include "include/Constants/Constants.h"

#include "include/Structures/Structures.h"

#include "include/Helpers/Helpers.h"

#include "include/Helpers/String.h"

class OpenLogging
{
private:
private:
  enum class LogType : uint8_t
  {
    DEBUG,
    INFO,
    WARN,
    ERROR,
    FATAL,
  };

  constexpr static const char *debug_begin_str = "\033[1;39;255;48;5;32m[DEBUG]\033[0m - ";
  constexpr static const char *info_begin_str = "\033[1;39;255;48;5;237m[INFO]\033[0m  - ";
  constexpr static const char *warn_begin_str = "\033[1;39;255;48;5;130m[WARN]\033[0m  - ";
  constexpr static const char *error_begin_str = "\033[1;39;255;48;5;124m[ERROR]\033[0m - ";
  constexpr static const char *fatal_begin_str = "\033[1;5;39;5;255;48;5;196m[FATAL]\033[0m - ";

  template <LogType type>
  static consteval decltype(auto) _return_log_type_str_()
  {
    if constexpr(type == LogType::DEBUG)
    {
      return debug_begin_str;
    }
    else if constexpr(type == LogType::INFO)
    {
      return info_begin_str;
    }
    else if constexpr(type == LogType::WARN)
    {
      return warn_begin_str;
    }
    else if constexpr(type == LogType::ERROR)
    {
      return error_begin_str;
    }
    else if constexpr(type == LogType::FATAL)
    {
      return fatal_begin_str;
    }
  }

private:
  // non specific type logger function
  template <LogType type, typename... types>
  static void general_logger(const Structures::valid_string_format<sizeof...(types)> &fmt, const types &...args)
  {
    if constexpr(sizeof...(args) == 0)
    {
      const constexpr int32_t one_hundred = 100;

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
      const constexpr int32_t one_hundred = 100;
      auto time_now = std::chrono::high_resolution_clock::now();
      long time_seconds = std::chrono::duration_cast<std::chrono::seconds>(time_now.time_since_epoch()).count();
      long time_microseconds_div_100 = std::chrono::duration_cast<std::chrono::microseconds>(time_now.time_since_epoch()).count() / one_hundred;
      const auto time_fmt = _time_formatted_(time_seconds);
      const auto time_str_micro_segment = _time_mili_format_(time_microseconds_div_100);

      const std::string begin_log_str = time_fmt + time_str_micro_segment + ' ';

      std::cout << begin_log_str;
      std::cout << OpenLogging::_return_log_type_str_<type>();
      std::cout << OpenLogging::format<true>(fmt, args...);
      std::cout << '\n';
    }
  }

public:
  // format
  template <bool ANSI_SCAPE_SEQUENCES = true, typename... types>
  static std::string format(const Structures::valid_string_format<sizeof...(types)> fmt, const types... args)
  {
    constexpr size_t buff_size_for_format = 10;
    const auto *str = fmt.str;
    const auto N = fmt.N;
    static char format[buff_size_for_format];

    std::string result;

    size_t i = 0;
    bool is_curr_backlash, is_prev_backlash = str[0] == '\\';
    (
        [&](const auto &_arg)
        {
          for(; i < N - 1; i++)
          {
            is_curr_backlash = str[i] == '\\';
            if(is_prev_backlash && is_curr_backlash)
            {
              is_prev_backlash = false;
              result += str[i];
              continue;
            }
            else if(is_prev_backlash || (!is_curr_backlash && str[i] != ::Constants::Delimiters::open && str[i] != ::Constants::Delimiters::close))
            {
              result += str[i];
            }
            else if(str[i] == ::Constants::Delimiters::open)
            {
              i++; // we skip the starting formatting delimiter
              int f = 0;
              // guaranteed to have the closing delim due to the __Structures::valid_string_format__'s nature
              while(str[i] != ::Constants::Delimiters::close)
                format[++f] = str[i++];

              format[f] = '\0';

              // i++; // we skip the ending formatting delimiter

              result += _to_string_into_buff_<ANSI_SCAPE_SEQUENCES>(_arg, format);
            }
            is_prev_backlash = str[i] == '\\';
          }
        }(args),
        ...);

    return result;
  }

public:
  // debug
  template <typename... types>
  static void debug(const Structures::valid_string_format<sizeof...(types)> fmt, const types... args)
  {
    general_logger<LogType::DEBUG>(fmt, args...);
  };
  // info
  template <typename... types>
  static void info(const Structures::valid_string_format<sizeof...(types)> fmt, const types... args)
  {
    general_logger<LogType::INFO>(fmt, args...);
  };
  // warn
  template <typename... types>
  static void warn(const Structures::valid_string_format<sizeof...(types)> fmt, const types... args)
  {
    general_logger<LogType::WARN>(fmt, args...);
  };
  // error
  template <typename... types>
  static void error(const Structures::valid_string_format<sizeof...(types)> fmt, const types... args)
  {
    general_logger<LogType::ERROR>(fmt, args...);
  };
  // fatal
  template <typename... types>
  static void fatal(const Structures::valid_string_format<sizeof...(types)> fmt, const types... args)
  {
    general_logger<LogType::FATAL>(fmt, args...);
  };

private:
  static constexpr std::string _time_formatted_(long &time_v)
  {
    struct tm *timeinfo = nullptr;

    const constexpr uint8_t buffsize = 27;
    std::array<char, buffsize> buffer = {};

    timeinfo = localtime(&time_v);

    strftime(buffer.data(), buffsize, "%a %d-%b-%y %H:%M:%S.", timeinfo);

    return { buffer.data() };
  }

  static constexpr std::string _time_mili_format_(long &time_v)
  {
    const constexpr uint8_t buffsize = 4;
    const constexpr uint8_t BASE = 10;
    std::array<char, buffsize> buffer = {};

    long cpy = time_v / BASE;
    long cpy_cpy = time_v;

    for(int i = buffsize - 1; i >= 0; i--)
    {
      buffer.at(i) = static_cast<char>('0' + (cpy_cpy % cpy));
      cpy_cpy = cpy;
      cpy /= BASE;
    }

    return { buffer.data() };
  }

  template <bool ANSI_SCAPE_SEQUENCES, size_t M, typename T>
  static std::string _to_string_into_buff_(const T &var, const char (&format)[M])
  {
    if constexpr(std::is_same_v<T, std::nullptr_t>)
    {
      return Helpers::String::AnsiConcatenation<ANSI_SCAPE_SEQUENCES>("nullptr", format, ::Constants::Colors::nullptrs);
    }
    else if constexpr(std::is_same_v<T, bool>)
    {
      return Helpers::String::AnsiConcatenation<ANSI_SCAPE_SEQUENCES>((var) ? "true" : "false", format,
                                                                      (var) ? ::Constants::Colors::bool_value_true : ::Constants::Colors::bool_value_false);
    }
    else if constexpr(Helpers::Templating::Types::is_char_v<T>())
    {
      return Helpers::String::AnsiConcatenation<ANSI_SCAPE_SEQUENCES>(var, format, ::Constants::Colors::chars);
    }
    else if constexpr(std::is_integral_v<T>)
    {
      return Helpers::String::AnsiConcatenation<ANSI_SCAPE_SEQUENCES>(var, format, ::Constants::Colors::ints);
    }
    else if constexpr(std::is_same_v<T, float>)
    {
      return Helpers::String::AnsiConcatenation<ANSI_SCAPE_SEQUENCES>(var, format, ::Constants::Colors::floats);
    }
    else if constexpr(std::is_same_v<T, double> || std::is_same_v<T, long double>)
    {
      return Helpers::String::AnsiConcatenation<ANSI_SCAPE_SEQUENCES>(var, format, ::Constants::Colors::doubles);
    }
    else if constexpr(::Helpers::Templating::Types::is_string_v<T>())
    {
      return Helpers::String::AnsiConcatenation<ANSI_SCAPE_SEQUENCES>(var, format, ::Constants::Colors::strings);
    }
    else if constexpr(std::is_pointer_v<T>)
    {
      return Helpers::String::AnsiConcatenation<ANSI_SCAPE_SEQUENCES>(reinterpret_cast<std::uintptr_t>(var), format, ::Constants::Colors::pointers);
    }
    else
    {
      return Helpers::String::AnsiConcatenation<ANSI_SCAPE_SEQUENCES>("unknown type", format, ::Constants::Colors::unknown);
    }
  }
}; // class OpenLogging

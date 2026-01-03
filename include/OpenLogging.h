#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <ctime>
#include <iostream>
#include <utility>

#ifdef OPENLOGGING_FORMATTING_CHAR_BEG

#if OPENLOGGING_FORMATTING_CHAR_BEG == 0
#error "OPENLOGGING_FORMATTING_CHAR_BEG cannot be NULL (ASCII 0)"
#elif OPENLOGGING_FORMATTING_CHAR_BEG >= 48 && OPENLOGGING_FORMATTING_CHAR_BEG <= 57
#error "OPENLOGGING_FORMATTING_CHAR_BEG cannot be digits 0-9"
#elif OPENLOGGING_FORMATTING_CHAR_BEG >= 65 && OPENLOGGING_FORMATTING_CHAR_BEG <= 90
#error "OPENLOGGING_FORMATTING_CHAR_BEG cannot be A-Z"
#elif OPENLOGGING_FORMATTING_CHAR_BEG >= 97 && OPENLOGGING_FORMATTING_CHAR_BEG <= 122
#error "OPENLOGGING_FORMATTING_CHAR_BEG cannot be a-z"
#elif OPENLOGGING_FORMATTING_CHAR_BEG == 255
#error "OPENLOGGING_FORMATTING_CHAR_BEG cannot be ASCII 255 (nbsp)"
#endif

#endif // OPENLOGGING_FORMATTING_CHAR_BEG

#ifdef OPENLOGGING_FORMATTING_CHAR_END

#if OPENLOGGING_FORMATTING_CHAR_END == 0
#error "OPENLOGGING_FORMATTING_CHAR_END cannot be NULL (ASCII 0)"
#elif OPENLOGGING_FORMATTING_CHAR_END >= 48 && OPENLOGGING_FORMATTING_CHAR_END <= 57
#error "OPENLOGGING_FORMATTING_CHAR_END cannot be digits 0-9"
#elif OPENLOGGING_FORMATTING_CHAR_END >= 65 && OPENLOGGING_FORMATTING_CHAR_END <= 90
#error "OPENLOGGING_FORMATTING_CHAR_END cannot be A-Z"
#elif OPENLOGGING_FORMATTING_CHAR_END >= 97 && OPENLOGGING_FORMATTING_CHAR_END <= 122
#error "OPENLOGGING_FORMATTING_CHAR_END cannot be a-z"
#elif OPENLOGGING_FORMATTING_CHAR_END == 255
#error "OPENLOGGING_FORMATTING_CHAR_END cannot be ASCII 255 (nbsp)"
#endif

#endif // OPENLOGGING_FORMATTING_CHAR_END

#ifndef OPENLOGGING_FORMATTING_CHAR_BEG
#define OPENLOGGING_FORMATTING_CHAR_BEG '{'
#endif

#ifndef OPENLOGGING_FORMATTING_CHAR_END
#define OPENLOGGING_FORMATTING_CHAR_END '}'
#endif

#ifndef OPENLOGGING_BOOL_VALUE_TRUE_COLOR
#define OPENLOGGING_BOOL_VALUE_TRUE_COLOR 64
#endif

#ifndef OPENLOGGING_BOOL_VALUE_FALSE_COLOR
#define OPENLOGGING_BOOL_VALUE_FALSE_COLOR 91
#endif

#ifndef OPENLOGGING_POINTERS_COLOR
#define OPENLOGGING_POINTERS_COLOR 237
#endif

#ifndef OPENLOGGING_NULLPTR_COLOR
#define OPENLOGGING_NULLPTR_COLOR 196
#endif

#ifndef OPENLOGGING_CHAR_COLOR
#define OPENLOGGING_CHAR_COLOR 8
#endif

#ifndef OPENLOGGING_INTS_COLOR
#define OPENLOGGING_INTS_COLOR 130
#endif

#ifndef OPENLOGGING_FLOATS_COLOR
#define OPENLOGGING_FLOATS_COLOR 1
#endif

#ifndef OPENLOGGING_DOUBLES_COLOR
#define OPENLOGGING_DOUBLES_COLOR 2
#endif

#ifndef OPENLOGGING_STRINGS_COLOR
#define OPENLOGGING_STRINGS_COLOR 3
#endif

#ifndef OPENLOGGING_UNKNOWN_COLOR
#define OPENLOGGING_UNKNOWN_COLOR 12
#endif

#ifndef OPENLOGGING_FATAL_BLINK
#define OPENLOGGING_FATAL_BLINK 1
#endif

#ifndef OPENLOGGING_ERROR_BLINK
#define OPENLOGGING_ERROR_BLINK 1
#endif

#ifndef OPENLOGGING_WARNING_BLINK
#define OPENLOGGING_WARNING_BLINK 1
#endif

#ifndef OPENLOGGING_DEBUG_BLINK
#define OPENLOGGING_DEBUG_BLINK 1
#endif

#ifndef OPENLOGGING_LOG_TO_STDOUT
#define OPENLOGGING_LOG_TO_STDOUT 1
#endif

class OpenLogging
{
private:
  constexpr static char _open_ = OPENLOGGING_FORMATTING_CHAR_BEG;
  constexpr static char _close_ = OPENLOGGING_FORMATTING_CHAR_END;
  constexpr static bool _debug_blink_ = OPENLOGGING_DEBUG_BLINK;
  constexpr static uint8_t _color_bool_value_true_ = OPENLOGGING_BOOL_VALUE_TRUE_COLOR;
  constexpr static uint8_t _color_bool_value_false_ = OPENLOGGING_BOOL_VALUE_FALSE_COLOR;
  constexpr static uint8_t _color_nullptr_ = OPENLOGGING_NULLPTR_COLOR;
  constexpr static uint8_t _color_pointers_ = OPENLOGGING_POINTERS_COLOR;
  constexpr static uint8_t _color_char_ = OPENLOGGING_CHAR_COLOR;
  constexpr static uint8_t _color_ints_ = OPENLOGGING_INTS_COLOR;
  constexpr static uint8_t _color_floats_ = OPENLOGGING_FLOATS_COLOR;
  constexpr static uint8_t _color_doubles_ = OPENLOGGING_DOUBLES_COLOR;
  constexpr static uint8_t _color_strings_ = OPENLOGGING_STRINGS_COLOR;
  constexpr static uint8_t _color_unknown_ = OPENLOGGING_UNKNOWN_COLOR;
  constexpr static const char *_ansi_begin_ = "\033[";
  constexpr static const char *_ansi_none_ = "0;";
  constexpr static const char *_ansi_bold_ = "1;";
  constexpr static const char *_ansi_blink_ = "5;";
  constexpr static const char *_ansi_st_color_ = "38;5;";
  constexpr static const char *_ansi_en_color_ = "m";
  constexpr static const char *_ansi_reset_ = "\033[0m";

private:
  template <size_t N_VALIDICS>
  struct _valid_string_format_
  {
    const char *str;
    size_t N;

    template <size_t M>
    consteval _valid_string_format_(const char (&in_str)[M]) : str(in_str), N(M)
    {
      if(validate_string(in_str) != N_VALIDICS)
      {
        throw "Placeholder count mismatch! Check your format string vs the number of arguments provided.";
      }
    }

  private:
    template <size_t M>
    [[nodiscard]] consteval size_t validate_string(const char (&in_str)[M])
    {
      size_t A = 0, B = 0;
      for(size_t i = 0; i < M - 1; i++) // ignore null terminator
      {
        if(in_str[i] == _open_)
        {
          A++;
        }
        if(in_str[i] == _close_)
        {
          B++;
        }
      }
      if(A != B)
      {
        throw "Invalid formatted string, You have extra or missing formatting delimiters check the string please";
      }
      return A;
    }
  };

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
  static void general_logger(const _valid_string_format_<sizeof...(types)> &fmt, const types &...args);

public:
  // debug
  template <typename... types>
  static void debug(const _valid_string_format_<sizeof...(types)> fmt, const types... args)
  {
    general_logger<LogType::DEBUG>(fmt, args...);
  };
  // info
  template <typename... types>
  static void info(const _valid_string_format_<sizeof...(types)> fmt, const types... args)
  {
    general_logger<LogType::INFO>(fmt, args...);
  };
  // warn
  template <typename... types>
  static void warn(const _valid_string_format_<sizeof...(types)> fmt, const types... args)
  {
    general_logger<LogType::WARN>(fmt, args...);
  };
  // error
  template <typename... types>
  static void error(const _valid_string_format_<sizeof...(types)> fmt, const types... args)
  {
    general_logger<LogType::ERROR>(fmt, args...);
  };
  // fatal
  template <typename... types>
  static void fatal(const _valid_string_format_<sizeof...(types)> fmt, const types... args)
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

  template <size_t M, typename T>
  static std::string &_to_string_into_buff_(const T &var, const char (&format)[M])
  {
    if(format)
    {
    }
    static std::string _buffer_;

    if constexpr(std::is_same_v<T, std::nullptr_t>)
    {
      _buffer_ += _ansi_begin_;
      _buffer_ += _ansi_bold_;
      _buffer_ += (_debug_blink_) ? _ansi_blink_ : "";
      _buffer_ += _ansi_st_color_;
      _buffer_ += std::to_string(_color_nullptr_) + _ansi_en_color_ + "0 (nullptr)";
      _buffer_ += _ansi_reset_;
    }
    else if constexpr(std::is_same_v<T, bool>)
    {
      _buffer_ += _ansi_begin_;
      _buffer_ += _ansi_bold_;
      _buffer_ += _ansi_st_color_;
      if(var)
      {
        _buffer_ += std::to_string(_color_bool_value_true_) + _ansi_en_color_ + "true";
      }
      else
      {
        _buffer_ += std::to_string(_color_bool_value_false_) + _ansi_en_color_ + "false";
      }
      _buffer_ += _ansi_reset_;
    }
    else if constexpr(std::is_same_v<T, char>)
    {
      _buffer_ += _ansi_begin_;
      _buffer_ += _ansi_bold_;
      _buffer_ += _ansi_st_color_;
      _buffer_ += std::to_string(_color_char_) + _ansi_en_color_ + var;
      _buffer_ += _ansi_reset_;
    }
    else if constexpr(std::is_same_v<T, int8_t> || std::is_same_v<T, int16_t> || std::is_same_v<T, int32_t> || std::is_same_v<T, int64_t>
                      || std::is_same_v<T, uint8_t> || std::is_same_v<T, uint16_t> || std::is_same_v<T, uint32_t> || std::is_same_v<T, uint64_t>)
    {
      _buffer_ += _ansi_begin_;
      _buffer_ += _ansi_bold_;
      _buffer_ += _ansi_st_color_;
      _buffer_ += std::to_string(_color_ints_) + _ansi_en_color_ + std::to_string(var);
      _buffer_ += _ansi_reset_;
    }
    else if constexpr(std::is_same_v<T, float>)
    {
      _buffer_ += _ansi_begin_;
      _buffer_ += _ansi_bold_;
      _buffer_ += _ansi_st_color_;
      _buffer_ += std::to_string(_color_floats_) + _ansi_en_color_ + std::to_string(var);
      _buffer_ += _ansi_reset_;
    }
    else if constexpr(std::is_same_v<T, double> || std::is_same_v<T, long double>)
    {
      _buffer_ += _ansi_begin_;
      _buffer_ += _ansi_bold_;
      _buffer_ += _ansi_st_color_;
      _buffer_ += std::to_string(_color_doubles_) + _ansi_en_color_ + std::to_string(var);
      _buffer_ += _ansi_reset_;
    }
    else if constexpr(std::is_same_v<T, std::string>)
    {
      _buffer_ += _ansi_begin_;
      _buffer_ += _ansi_bold_;
      _buffer_ += _ansi_st_color_;
      _buffer_ += std::to_string(_color_strings_) + _ansi_en_color_ + var;
      _buffer_ += _ansi_reset_;
    }
    else if constexpr(std::is_same_v<T, const char *> || std::is_same_v<T, char *>)
    {
      _buffer_ += _ansi_begin_;
      _buffer_ += _ansi_bold_;
      _buffer_ += _ansi_st_color_;
      _buffer_ += std::to_string(_color_strings_) + _ansi_en_color_ + std::string(var);
      _buffer_ += _ansi_reset_;
    }
    else if constexpr(std::is_pointer_v<T>)
    {
      _buffer_ += _ansi_begin_;
      _buffer_ += _ansi_bold_;
      _buffer_ += _ansi_st_color_;
      _buffer_ += std::to_string(_color_pointers_) + _ansi_en_color_ + std::to_string(reinterpret_cast<std::uintptr_t>(var));
      _buffer_ += _ansi_reset_;
    }
    else
    {
      _buffer_ += _ansi_begin_;
      _buffer_ += _ansi_bold_;
      _buffer_ += _ansi_st_color_;
      _buffer_ += std::to_string(11) + _ansi_en_color_ + "unknown type";
      _buffer_ += _ansi_reset_;
    }

    return _buffer_;
  }
}; // class OpenLogging

#include "OpenLogging.ipp"

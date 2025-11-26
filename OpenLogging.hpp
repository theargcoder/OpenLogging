#pragma once

#include <chrono>
#include <cstddef>
#include <ctime>
#include <iostream>
#include <string.h>

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

#ifndef OPENLOGGING_BOOL_VALUE_FALSE_COLOR
#define OPENLOGGING_BOOL_VALUE_FALSE_COLOR 91
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

namespace OpenLogging
{
  constexpr char __open__ = OPENLOGGING_FORMATTING_CHAR_BEG;
  constexpr char __close__ = OPENLOGGING_FORMATTING_CHAR_END;
  constexpr static uint8_t __color_bool_value_true__ = OPENLOGGING_BOOL_VALUE_TRUE_COLOR;
  constexpr static uint8_t __color_bool_value_false__ = OPENLOGGING_BOOL_VALUE_FALSE_COLOR;
  constexpr static uint8_t __color_nullptr__ = OPENLOGGING_NULLPTR_COLOR;
  constexpr static uint8_t __color_pointers__ = OPENLOGGING_POINTERS_COLOR;
  constexpr static uint8_t __color_char__ = OPENLOGGING_CHAR_COLOR;
  constexpr static uint8_t __color_ints__ = OPENLOGGING_INTS_COLOR;
  constexpr static uint8_t __color_floats__ = OPENLOGGING_FLOATS_COLOR;
  constexpr static uint8_t __color_doubles__ = OPENLOGGING_DOUBLES_COLOR;
  constexpr static uint8_t __color_strings__ = OPENLOGGING_STRINGS_COLOR;
  constexpr static uint8_t __color_unknown__ = OPENLOGGING_UNKNOWN_COLOR;
  constexpr static const char *__ansi_begin__ = "\033[";
  constexpr static const char *__ansi_none__ = "0;";
  constexpr static const char *__ansi_bold__ = "1;";
  constexpr static const char *__ansi_blink__ = "5;";
  constexpr static const char *__ansi_st_color__ = "38;5;";
  constexpr static const char *__ansi_en_color__ = "m";
  constexpr static const char *__ansi_reset__ = "\033[0m";

  template <size_t N>
  consteval bool validate_string (const char (&str)[N])
  {
    int A = 0, B = 0;
    for (size_t i = 0; i < N - 1; ++i) // ignore null terminator
    {
      if (str[i] == __open__)
        ++A;
      if (str[i] == __close__)
        ++B;
    }
    return A == B;
  }

  struct __valid_string_format__
  {
    const char *str;
    size_t N;

    template <size_t M>
    consteval __valid_string_format__ (const char (&str)[M]) : str (str), N (M)
    {
      if (!validate_string (str))
        throw "Invalid formatted string, You have extra or missing formatting delimiters check the string please";
    }
  };

  static constexpr std::string __time_formatted__ (long &time_v)
  {
    struct tm *timeinfo;

    const uint8_t BUFFSIZE = 32;
    char buffer[BUFFSIZE];

    timeinfo = localtime (&time_v);

    size_t len = strftime (buffer, BUFFSIZE, "%a  %d-%m-%y  %H:%M:%S.", timeinfo);

    return std::string (buffer);
  }

  static constexpr std::string __time_mili_format__ (long &time_v)
  {
    const uint8_t BUFFSIZE = 4;
    char buffer[BUFFSIZE];

    long cpy = time_v / 10;
    long cpy_cpy = time_v;

    for (int i = BUFFSIZE - 1; i >= 0; i--)
    {
      buffer[i] = '0' + (cpy_cpy % cpy);
      cpy_cpy = cpy;
      cpy /= 10;
    }

    return std::string (buffer);
  }

  template <typename T>
  static std::string __to_string_into_buff__ (const T &var, const char (&format)[10], const bool &blink)
  {
    static std::string __buffer__;
    __buffer__.reserve (4096);

    size_t i = 0;
    if constexpr (std::is_same_v<T, std::nullptr_t>)
    {
      __buffer__ += __ansi_begin__;
      __buffer__ += __ansi_bold__;
      __buffer__ += (OPENLOGGING_DEBUG_BLINK) ? __ansi_blink__ : "";
      __buffer__ += __ansi_st_color__;
      __buffer__ += std::to_string (__color_nullptr__) + __ansi_en_color__ + "0 (nullptr)";
      __buffer__ += __ansi_reset__;
    }
    else if constexpr (std::is_same_v<T, bool>)
    {
      __buffer__ += __ansi_begin__;
      __buffer__ += __ansi_bold__;
      __buffer__ += __ansi_st_color__;
      if (var)
        __buffer__ += std::to_string (__color_bool_value_true__) + __ansi_en_color__ + "true";
      else
        __buffer__ += std::to_string (__color_bool_value_false__) + __ansi_en_color__ + "false";
      __buffer__ += __ansi_reset__;
    }
    else if constexpr (std::is_same_v<T, char>)
    {
      __buffer__ += __ansi_begin__;
      __buffer__ += __ansi_bold__;
      __buffer__ += __ansi_st_color__;
      __buffer__ += std::to_string (__color_char__) + __ansi_en_color__ + var;
      __buffer__ += __ansi_reset__;
    }
    else if constexpr (std::is_same_v<T, int8_t> || std::is_same_v<T, int16_t> || std::is_same_v<T, int32_t>
                       || std::is_same_v<T, int64_t> || std::is_same_v<T, uint8_t> || std::is_same_v<T, uint16_t>
                       || std::is_same_v<T, uint32_t> || std::is_same_v<T, uint64_t>)
    {
      __buffer__ += __ansi_begin__;
      __buffer__ += __ansi_bold__;
      __buffer__ += __ansi_st_color__;
      __buffer__ += std::to_string (__color_ints__) + __ansi_en_color__ + std::to_string (var);
      __buffer__ += __ansi_reset__;
    }
    else if constexpr (std::is_same_v<T, float>)
    {
      __buffer__ += __ansi_begin__;
      __buffer__ += __ansi_bold__;
      __buffer__ += __ansi_st_color__;
      __buffer__ += std::to_string (__color_floats__) + __ansi_en_color__ + std::to_string (var);
      __buffer__ += __ansi_reset__;
    }
    else if constexpr (std::is_same_v<T, double> || std::is_same_v<T, long double>)
    {
      __buffer__ += __ansi_begin__;
      __buffer__ += __ansi_bold__;
      __buffer__ += __ansi_st_color__;
      __buffer__ += std::to_string (__color_doubles__) + __ansi_en_color__ + std::to_string (var);
      __buffer__ += __ansi_reset__;
    }
    else if constexpr (std::is_same_v<T, std::string>)
    {
      __buffer__ += __ansi_begin__;
      __buffer__ += __ansi_bold__;
      __buffer__ += __ansi_st_color__;
      __buffer__ += std::to_string (__color_strings__) + __ansi_en_color__ + var;
      __buffer__ += __ansi_reset__;
    }
    else if constexpr (std::is_same_v<T, const char *> || std::is_same_v<T, char *>)
    {
      __buffer__ += __ansi_begin__;
      __buffer__ += __ansi_bold__;
      __buffer__ += __ansi_st_color__;
      __buffer__ += std::to_string (__color_strings__) + __ansi_en_color__ + std::string (var);
      __buffer__ += __ansi_reset__;
    }
    else if constexpr (std::is_pointer_v<T>)
    {
      __buffer__ += __ansi_begin__;
      __buffer__ += __ansi_bold__;
      __buffer__ += __ansi_st_color__;
      __buffer__
          += std::to_string (__color_pointers__) + __ansi_en_color__ + std::to_string (reinterpret_cast<std::uintptr_t> (var));
      __buffer__ += __ansi_reset__;
    }
    else
    {
      __buffer__ += __ansi_begin__;
      __buffer__ += __ansi_bold__;
      __buffer__ += __ansi_st_color__;
      __buffer__ += std::to_string (11) + __ansi_en_color__ + "UNKNOWN TYPE";
      __buffer__ += __ansi_reset__;
    }

    return __buffer__;
  }

  template <typename... Types>
  void log (const __valid_string_format__ fmt, Types... args)
  {
    const auto *str = fmt.str;
    const auto N = fmt.N;
    static char format[10] = { '\0' };
    (
        [&] (const auto __arg)
        {
          auto time_now = std::chrono::high_resolution_clock::now ();
          long time_seconds = std::chrono::duration_cast<std::chrono::seconds> (time_now.time_since_epoch ()).count ();
          long time_microseconds_div_100
              = std::chrono::duration_cast<std::chrono::microseconds> (time_now.time_since_epoch ()).count () / 100;
          const auto time_fmt = __time_formatted__ (time_seconds);
          const auto time_str_micro_segment = __time_mili_format__ (time_microseconds_div_100);

          const std::string begin_log_str = time_fmt + time_str_micro_segment + "  ";

          std::cout << begin_log_str;
          for (size_t i = 0; i < N; i++)
          {
            if (str[i] != __open__ && str[i] != __close__)
            {
              std::cout << str[i];
            }
            else if (str[i] == __open__)
            {
              i++; // WE skip the formatting delimiter
              int f = 0;
              // guaranteed to have the closing delim due to the __valid_string_format__'s nature
              while (str[i] != __close__)
                format[++f] = str[i++];

              std::string str = __to_string_into_buff__ (__arg, format, OPENLOGGING_DEBUG_BLINK);

              std::cout << str << "\n";

              ;
            }
          }
        }(args),
        ...);
  }

} // namespace OpenLogging

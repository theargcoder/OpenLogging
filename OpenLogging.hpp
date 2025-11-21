#pragma once

#include <cstddef>
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

namespace OpenLogging
{
constexpr char __open__ = OPENLOGGING_FORMATTING_CHAR_BEG;
constexpr char __close__ = OPENLOGGING_FORMATTING_CHAR_END;
constexpr static uint8_t __color_bool_value_true__ = OPENLOGGING_BOOL_VALUE_TRUE_COLOR;
constexpr static uint8_t __color_bool_value_false__ = OPENLOGGING_BOOL_VALUE_FALSE_COLOR;
constexpr static const char *__ansi_begin__ = "\033[";
constexpr static const char *__ansi_end__ = "m";
constexpr static const char *__ansi_reset__ = "\033[0m";

template <size_t N>
constexpr bool validate_string (const char (&str)[N])
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

template <typename T>
static std::string __to_string_into_buff__ (T &var)
{
  static std::string __buffer__;
  __buffer__.reserve (4096);

  size_t i = 0;
  if constexpr (std::is_same_v<T, bool>)
  {
    if (var)
      __buffer__ += __ansi_begin__ + std::to_string (__color_bool_value_true__) + __ansi_end__ + "true";
    else
      __buffer__ += __ansi_begin__ + std::to_string (__color_bool_value_false__) + __ansi_end__ + "false";
  }

  return __buffer__;
}

template <typename... Types>
void log (const __valid_string_format__ fmt, Types... args)
{
  const auto *str = fmt.str;
  const auto N = fmt.N;
  (
      [&] (auto __arg)
      {
        for (size_t i = 0, argc = 0; i < N; i++)
        {
          if (str[i] != __open__ && str[i] != __close__)
          {
            std::cout << str[i];
          }
          else if (str[i] == __open__)
          {
            i++;                     // WE skip the formatting delimiter
            if (str[i] == __close__) // direct print if its just followed by a closed delim (no formatting args)
              std::cout << __to_string_into_buff__ (__arg);
          }
          else
          {
            argc++;
          }
        }
      }(args),
      ...);

  std::cout << "\n";
}

} // namespace OpenLogging

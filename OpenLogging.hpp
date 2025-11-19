#pragma once

#include <cstddef>
#include <fstream>
#include <memory>
#include <string.h>
#include <string>

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

namespace OpenLogging
{
constexpr char __open__ = OPENLOGGING_FORMATTING_CHAR_BEG;
constexpr char __close__ = OPENLOGGING_FORMATTING_CHAR_END;

template <size_t N>
consteval bool
validate_string (const char (&str)[N])
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

template <typename... Types, size_t N>
consteval void
log (const char (&str)[N], Types... args)
{
  if (!validate_string (str))
    throw "Invalid formatted string, You have extra or missing formatting delimiters check the string please";
}
}

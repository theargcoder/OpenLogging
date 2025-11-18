#include <cstddef>
#include <fstream>
#include <memory>
#include <stack>
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
static char __open__ = OPENLOGGING_FORMATTING_CHAR_BEG;
static char __close__ = OPENLOGGING_FORMATTING_CHAR_END;
std::unique_ptr<std::string> FILE = nullptr;

template <const char *, typename... Types>
void
Log (const char *str, Types... args)
{
  if (FILE)
    {
      std::ofstream file_out (*FILE, std::ios::out);
      std::stack<char> stk;

      bool in_format = false;
      size_t pos = 0;

      auto ch = str;
      while (*ch != '\0')
        {
          auto empty = stk.empty ();
          if (empty)
            {
              if (*ch == __open__)
                stk.push (__open__);
              else if (*ch == __close__)
                throw 0;
              // TODO static assert???? idk
            }
        }
    }
  else
    {
      // TODO
    }
}
}

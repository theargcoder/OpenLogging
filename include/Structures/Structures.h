#include <cassert>
#include <cstddef>
#include <cstring>
#include <iostream>

#include "include/Constants/Constants.h"

namespace Structures
{
  template <size_t N_VALIDICS>
  struct valid_string_format
  {
    const char *str;
    char fmts[N_VALIDICS][Constants::Size::MAX_FMT_SIZE]{};
    size_t N;

    template <size_t M>
    consteval valid_string_format(const char (&in_str)[M]) : str(in_str), N(M)
    {
      if(validate_string(in_str) != N_VALIDICS)
        throw "Placeholder count mismatch! Check your format string vs the number of arguments provided.";

      if constexpr(N_VALIDICS > 0)
      {
        parse_fmts(in_str);
      }

      /*
      for(int i = 0; i < N_VALIDICS; i++)
      {
        for(int j = 0; j < Constants::Size::MAX_FMT_SIZE; j++)
        {
          std::cout << fmts[i][j];
        }
        std::cout << '\n';
      }
      */
    }

  private:
    template <size_t M>
    consteval size_t parse_fmts(const char (&in_str)[M])
    {
      size_t fmt_idx = 0;

      for(size_t i = 0; i < M - 1; i++)
      {
        // Use in_str[i] instead of str[i] for more reliable constexpr tracking
        if(in_str[i] != Constants::Delimiters::open)
        {
          continue;
        }

        // Safety check for array bounds
        if(fmt_idx >= N_VALIDICS)
        {
          break;
        }

        i++; // Move past '{'
        size_t p = 0;

        // Loop until '}' or MAX_SIZE
        while(i < M - 1 && in_str[i] != Constants::Delimiters::close)
        {
          if(p >= Constants::Size::MAX_FMT_SIZE - 1)
          {
            throw "format size exceeds limit!";
          }

          this->fmts[fmt_idx][p++] = in_str[i++];
        }

        if(in_str[i] != Constants::Delimiters::close)
        {
          throw "Unclosed delimiter or buffer overflow!";
        }

        this->fmts[fmt_idx][p] = '\0';
        fmt_idx++;
      }
      return fmt_idx;
    }

    template <size_t M>
    [[nodiscard]] consteval size_t validate_string(const char (&in_str)[M])
    {
      bool prev_prev_is_backlash = false, prev_is_backlash = in_str[0] == '\\';
      bool prev_open = in_str[0] == Constants::Delimiters::open, prev_close = in_str[0] == Constants::Delimiters::close;
      bool open = false, close = false;

      size_t A = prev_open, B = prev_close;

      for(size_t i = 1; i < M - 1; i++) // ignore null terminator
      {
        const char &ch = in_str[i];
        open = ch == ::Constants::Delimiters::open, close = ch == ::Constants::Delimiters::close;
        prev_prev_is_backlash = prev_is_backlash;
        prev_is_backlash = in_str[i - 1] == '\\';

        if((!prev_prev_is_backlash && prev_is_backlash) || (!open && !close))
        {
          continue;
        }
        else if(open && !close)
        {
          A++;
          if(prev_open && !prev_is_backlash)
            throw "Invalid formatted string, you have 2 succesive open delimiters";

          prev_open = true;
          prev_close = false;
        }
        else if(!open && close)
        {
          B++;
          if(prev_close && !prev_is_backlash)
            throw "Invalid formatted string, you have 2 succesive closed delimiters";

          prev_close = true;
          prev_open = false;
        }
      }
      if(A != B)
      {
        throw "Invalid formatted string, You have extra or missing formatting delimiters check the string please";
      }
      return A;
    }
  };
} // namespace Structures

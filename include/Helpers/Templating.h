#pragma once

#include <cstdint>
#include <string>
#include <type_traits>

#include "include/Helpers/Helpers.h"

struct Helpers::Templating
{
  struct Types
  {
    template <typename T>
    static constexpr bool is_char_v()
    {
      using BaseType = std::remove_cvref_t<T>;
      return std::is_same_v<BaseType, char>;
    }

    template <typename T>
    static constexpr bool is_numeric_v()
    {
      using BaseType = std::remove_cvref_t<T>;
      return (std::is_integral_v<BaseType> && !std::is_same_v<BaseType, bool>) || std::is_floating_point_v<BaseType>;
    }

    template <typename T>
    static constexpr bool is_not_numeric_v()
    {
      return !is_numeric_v<T>();
    }

    template <typename T>
    static constexpr bool is_string_v()
    {
      using BaseType = std::remove_cvref_t<T>;
      return std::is_same_v<BaseType, char *> || std::is_same_v<BaseType, const char *> || std::is_same_v<BaseType, std::string>;
    }

    template <typename T>
    static constexpr bool is_bool_v()
    {
      using BaseType = std::remove_cvref_t<T>;
      return std::is_same_v<BaseType, bool>;
    }
  };

  struct Fillers
  {
    template <size_t M, size_t P, typename... Chars>
      requires(std::is_same_v<char, Chars> && ...)
    static consteval void fill_char_array(char (&array)[M][P], auto &idx, const Chars &...args)
    {
      size_t a = 0;
      ((array[idx][a++] = args), ...);
    }

    template <size_t A, size_t X, size_t Y, typename... Args>
      requires(std::is_integral_v<Args> && ...)
    static void fill_decrement_char_array(char (&dest)[A], const char (&src)[X][Y], uint32_t &dest_idx, const auto &src_idx, const Args &...args)
    {
      ((dest[dest_idx--] = src[src_idx][args]), ...);
    }
  };
};

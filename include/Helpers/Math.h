#pragma once

#include "include/Helpers/Helpers.h"

struct Helpers::Math
{
  struct Constexpr
  {
    template <typename T>
    static constexpr T pow(T base, int exp)
    {
      return exp == 0 ? 1 : base * Helpers::Math::Constexpr::pow(base, exp - 1);
    }
  };
};

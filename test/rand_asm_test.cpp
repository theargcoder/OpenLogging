// #include "include/Algos/Floating/Exponential.h"
#include "include/Helpers/Simd.h"

#include <cstdint>
#include <stdio.h>

int main(int argc, char **argv)
{
  /*
  double dobl = 2.018572034E127;
  float flot = 1.290372E5;
  Helpers::Numeric::Floating::ExponentialNotation::ToStr(flot);
  Helpers::Numeric::Floating::ExponentialNotation::ToStr(dobl);
  */
  uint32_t rand_num = 1'123'456'789U;
  char buff[64];
  const auto len = Helpers::Simd::x86_64::WriteCharsToPtrFowardReturnLength<uint32_t>(&buff[0], rand_num);

  buff[len] = '\0';

  printf("%s \n", &buff[0]);

  return 0;
}

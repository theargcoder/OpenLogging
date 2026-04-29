// #include "include/Algos/Floating/Exponential.h"
#include "include/Helpers/Math.h"
#include "include/Helpers/Simd.h"

#include <cstdint>
#include <stdio.h>
#include <string>

int main(int argc, char **argv)
{
  /*
  double dobl = 2.018572034E127;
  float flot = 1.290372E5;
  Helpers::Numeric::Floating::ExponentialNotation::ToStr(flot);
  Helpers::Numeric::Floating::ExponentialNotation::ToStr(dobl);
  */

  Helpers::Assembly::pin_thread_to_cpu(3);
  {
    uint16_t rand_num = 12;
    char buff[64];

    const auto st_simdy = Helpers::Assembly::rdtsc();

    uint32_t len;
    for(int i = 0; i < 1000; i++)
    {
      len = Helpers::Simd::x86_64::WriteCharsToPtrFowardReturnLength<uint16_t>(&buff[0], rand_num);
    }

    const auto en_simdy = Helpers::Assembly::rdtsc();

    buff[len] = '\0';

    printf("uint16_t our simdy = %s , it took = %ld cycles \n", &buff[0], en_simdy - st_simdy);

    std::string tmp;

    const auto st_std = Helpers::Assembly::rdtsc();

    for(int i = 0; i < 1000; i++)
    {
      tmp = std::to_string(rand_num);
    }

    const auto en_std = Helpers::Assembly::rdtsc();

    printf("uint16_ to_str = %s , it took = %ld cycles \n", tmp.c_str(), en_std - st_std);
  }

  {
    uint32_t rand_num = 1234567891;
    char buff[64];

    const auto st_simdy = Helpers::Assembly::rdtsc();

    uint32_t len;
    for(int i = 0; i < 1000; i++)
    {
      len = Helpers::Simd::x86_64::WriteCharsToPtrFowardReturnLength<uint32_t>(&buff[0], rand_num);
    }

    const auto en_simdy = Helpers::Assembly::rdtsc();

    buff[len] = '\0';

    printf("uint32_t our simdy = %s , it took = %ld cycles \n", &buff[0], en_simdy - st_simdy);

    std::string tmp;

    const auto st_std = Helpers::Assembly::rdtsc();

    for(int i = 0; i < 1000; i++)
    {
      tmp = std::to_string(rand_num);
    }

    const auto en_std = Helpers::Assembly::rdtsc();

    printf("uint32_ to_str = %s , it took = %ld cycles \n", tmp.c_str(), en_std - st_std);
  }

  return 0;
}

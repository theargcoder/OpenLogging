#include "include/Algos/Floating/Exponential.h"

int main(int argc, char **argv)
{
  double dobl = 2.018572034E127;
  float flot = 1.290372E5;
  Helpers::Numeric::Floating::ExponentialNotation::ToStr(flot);
  Helpers::Numeric::Floating::ExponentialNotation::ToStr(dobl);
  return 0;
}

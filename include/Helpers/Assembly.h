#include <cstdint>

namespace Helpers::Assembly
{
  __attribute__((always_inline)) static auto umulh32(const uint64_t &a, const uint32_t &b)
  {
    return static_cast<uint32_t>((a * b) >> 32U);
  }

  __attribute__((always_inline)) static uint64_t umulh64(const uint64_t &a, const uint64_t &b)
  {
#if defined(__x86_64__)
    uint64_t hi;
    uint64_t lo = a;
    asm("mul %[b]" : "+a"(lo), "=d"(hi) : [b] "r"(b) : "cc");
    return hi;
#elif defined(__aarch64__)
    uint64_t hi;
    asm("umulh %0, %1, %2" : "=r"(hi) : "r"(a), "r"(b));
    return hi;
#else
    return (uint64_t)((__uint128_t)a * b >> 64U);
#endif
  }
};

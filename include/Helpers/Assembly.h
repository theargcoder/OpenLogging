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

  inline uint64_t rdtsc()
  {
#if defined(__x86_64__)
    unsigned aux;
    return __rdtscp(&aux);
#elif defined(__aarch64__)
    uint64_t val;
    asm volatile("isb" ::: "memory");
    asm volatile("mrs %0, cntvct_el0" : "=r"(val));
    return val;
#else
#error "Unsupported architecture"
#endif
  }

  inline uint64_t rdtsc_freq()
  {
#if defined(__x86_64__)
    // This is the tricky part on x86
    // Best option: calibrate once using chrono
    static uint64_t freq = []
    {
      using namespace std::chrono;

      auto start_tsc = __rdtscp((unsigned *)nullptr);
      auto start = steady_clock::now();

      std::this_thread::sleep_for(std::chrono::milliseconds(100));

      auto end_tsc = __rdtscp((unsigned *)nullptr);
      auto end = steady_clock::now();

      auto ns = duration_cast<nanoseconds>(end - start).count();
      return (end_tsc - start_tsc) * 1'000'000'000ull / ns;
    }();

    return freq;

#elif defined(__aarch64__)
    uint64_t freq;
    asm volatile("mrs %0, cntfrq_el0" : "=r"(freq));
    return freq;

#else
#error "Unsupported architecture"
#endif
  }

  inline uint64_t rdtsc_to_ns(uint64_t ticks)
  {
    static const uint64_t freq = rdtsc_freq();

    // avoid overflow with 128-bit math
    return (uint64_t)((__uint128_t)ticks * 1'000'000'000ULL / freq);
  }

};

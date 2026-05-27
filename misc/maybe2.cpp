#include <cmath>
#include <cstdint> // Added for explicit uint32_t and uint64_t types
#include <cstring>

int main()
{
  constexpr uint32_t k = 1074;
  // 36 words * 32 bits = 1152 bits. Perfectly fits k=1100 + 10^8 overflow.
  constexpr int NUM_WORDS = 36;
  const uint32_t P = std::floor(k * std::log10(2));

  // Calculate exactly how many times we need to multiply by 10^8
  uint32_t E = (P > 0) ? (P - 1) >> 3U : 0;
  uint32_t rprime[NUM_WORDS] = { 0 };
  rprime[0] = 1;
  // Initialize our base as a BigInt equivalent to 10^8
  uint32_t base[NUM_WORDS] = { 0 };
  base[0] = 100'000'000U;

  // --- Exponentiation by Squaring Loop ---
  while(E > 0)
  {
    // If E is odd, multiply the current result by the base
    if(E & 1)
    {
      uint32_t temp[NUM_WORDS] = { 0 };
      for(int i = 0; i < NUM_WORDS; ++i)
      {
        uint64_t carry = 0;
        for(int j = 0; j < NUM_WORDS - i; ++j)
        {
          uint64_t p = (uint64_t)rprime[i] * base[j] + temp[i + j] + carry;
          temp[i + j] = (uint32_t)p;
          carry = p >> 32;
        }
      }
      // Copy temp back to rprime
      for(int w = 0; w < NUM_WORDS; ++w)
        rprime[w] = temp[w];
    }

    // Square the base: base = base * base
    // Optimization: Skip the final squaring if E is about to become 0
    if(E > 1)
    {
      uint32_t temp[NUM_WORDS] = { 0 };
      for(int i = 0; i < NUM_WORDS; ++i)
      {
        uint64_t carry = 0;
        for(int j = 0; j < NUM_WORDS - i; ++j)
        {
          uint64_t p = (uint64_t)base[i] * base[j] + temp[i + j] + carry;
          temp[i + j] = (uint32_t)p;
          carry = p >> 32;
        }
      }
      // Copy temp back to base
      for(int w = 0; w < NUM_WORDS; ++w)
        base[w] = temp[w];
    }

    E >>= 1; // Divide exponent by 2
  }

  // Calculate exactly how many times we need to multiply by 10^8
  E = (P > 0) ? (P - 1) >> 3U : 0;
  uint32_t rrprime[NUM_WORDS] = { 0 };
  rrprime[0] = 1;
  // Initialize our base as a BigInt equivalent to 10^8
  uint32_t rbase[NUM_WORDS] = { 0 };
  rbase[0] = 100'000'000U;

  // --- Exponentiation by Squaring Loop ---
  while(E > 0)
  {
    // If E is odd, multiply the current result by the base
    if(E & 1)
    {
      uint32_t temp[NUM_WORDS] = { 0 };
      for(int i = 0; i < NUM_WORDS; ++i)
      {
        uint64_t carry = 0;
        for(int j = 0; j < NUM_WORDS - i; ++j)
        {
          uint64_t p = (uint64_t)rrprime[i] * rbase[j] + temp[i + j] + carry;
          temp[i + j] = (uint32_t)p;
          carry = p >> 32;
        }
      }
      // Copy temp back to rprime
      for(int w = 0; w < NUM_WORDS; ++w)
        rrprime[w] = temp[w];
    }

    // Square the base: base = base * base
    // Optimization: Skip the final squaring if E is about to become 0
    if(E > 1)
    {
      uint32_t temp[NUM_WORDS] = { 0 };
      for(int i = 0; i < NUM_WORDS; ++i)
      {
        uint64_t carry = 0;
        for(int j = 0; j < NUM_WORDS - i; ++j)
        {
          uint64_t p = (uint64_t)rbase[i] * rbase[j] + temp[i + j] + carry;
          temp[i + j] = (uint32_t)p;
          carry = p >> 32;
        }
      }
      // Copy temp back to base
      for(int w = 0; w < NUM_WORDS; ++w)
        rbase[w] = temp[w];
    }

    E >>= 1; // Divide exponent by 2
  }

  return 0;
}

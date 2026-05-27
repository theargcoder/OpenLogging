#include <cmath>
#include <cstdint> // Added for explicit uint32_t and uint64_t types
#include <cstring>
#include <iomanip>
#include <iostream>

int main()
{
  constexpr uint32_t k = 1074;
  // 36 words * 32 bits = 1152 bits. Perfectly fits k=1100 + 10^8 overflow.
  // ALL heap allocation (std::vector) is eliminated.
  constexpr int NUM_WORDS = 36;
  uint32_t rprime[NUM_WORDS] = { 0 };
  // CRITICAL FIX: Reset rprime cleanly for this iteration of k
  rprime[0] = 1;
  for(int w = 1; w < NUM_WORDS; ++w)
    rprime[w] = 0;
  // Power of 10 needed for the first 8-digit chunk
  uint32_t P = std::floor(k * std::log10(2));
  rprime[0] = 1; // Initialize Rprime = 1
  uint32_t iprime = 0;
  // 1. One-time Initialization Loop: Compute R = 10^P inline
  for(; iprime + 8 < P; iprime += 8)
  {
    uint32_t carry = 0;
    for(int w = 0; w < NUM_WORDS; ++w)
    {
      // 32-bit x 32-bit + 32-bit fits safely inside 64-bit math
      uint64_t p = (uint64_t)rprime[w] * 100'000'000U + carry;
      rprime[w] = (uint32_t)p;
      carry = (uint32_t)(p >> 32);
    }
    uint32_t jasdlj = 134; // Kept from original snippet
  }

  std::cout << "lo hicimos; we didt muy bien \n";

  uint32_t rrprime[NUM_WORDS] = { 0 };

  // CRITICAL FIX: Reset rprime cleanly for this iteration of k
  rrprime[0] = 1;
  for(int w = 1; w < NUM_WORDS; ++w)
    rrprime[w] = 0;

  // Calculate exactly how many times we need to multiply by 10^8
  uint32_t E = (P > 0) ? (P - 1) / 8 : 0;

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
          uint64_t p = (uint64_t)rrprime[i] * base[j] + temp[i + j] + carry;
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

  // Total digits in 5^k
  uint32_t total_digits = std::floor(k * std::log10(5)) + 1;

  /*
  // 36 words * 32 bits = 1152 bits. Perfectly fits k=1100 + 10^8 overflow.
  // ALL heap allocation (std::vector) is eliminated.
  constexpr int NUM_WORDS = 36;
  */
  uint32_t r[NUM_WORDS] = { 0 };
  r[0] = 1; // Initialize R = 1

  // 1. One-time Initialization Loop: Compute R = 10^P inline
  for(uint32_t i = 0; i < P; ++i)
  {
    uint32_t carry = 0;
    for(int w = 0; w < NUM_WORDS; ++w)
    {
      // 32-bit x 32-bit + 32-bit fits safely inside 64-bit math
      uint64_t p = (uint64_t)r[w] * 10ULL + carry;
      r[w] = (uint32_t)p;
      carry = (uint32_t)(p >> 32);
    }
    uint32_t jasdlj = 134; // Kept from original snippet
  }

  // Pre-calculate shift and mask values for O(1) chunk extraction / modulo
  uint32_t word_idx = k / 32;
  uint32_t bit_shift = k % 32;
  uint32_t mod_mask = (bit_shift == 0) ? 0 : ((1U << bit_shift) - 1);

  // Initial extraction & Modulo
  uint64_t chunk = 0;
  // 2. Main Processing Loop: Dynamically compute chunks inline
  uint32_t digits_computed = 0;
  int chunk_count = 2;

  while(digits_computed < total_digits)
  {
    // Step A & B: Multiply by 10^8 and cascade the carry
    uint32_t carry = 0; // Carry max is 10^8, fits safely in uint32_t
    for(int w = 0; w < NUM_WORDS; ++w)
    {
      // Max size: (2^32 - 1) * 10^8 + 10^8 = 2^32 * 10^8. Fits comfortably in uint64_t.
      uint64_t p = (uint64_t)r[w] * 100'000'000U + carry;
      r[w] = (uint32_t)p;
      carry = (uint32_t)(p >> 32);
    }

    // Step C: Extract next8 chunk (R >> k)
    chunk = 0;
    if(word_idx < NUM_WORDS)
    {
      chunk = r[word_idx] >> bit_shift;
    }
    if(word_idx + 1 < NUM_WORDS && bit_shift != 0)
    {
      // Cast to uint64_t before shifting to prevent any 32-bit boundary overflow
      chunk |= ((uint64_t)r[word_idx + 1] << (32 - bit_shift));
    }
    uint32_t next8 = (uint32_t)chunk;

    // Step D: Apply Modulo 2^k instantly
    if(word_idx < NUM_WORDS)
    {
      r[word_idx] &= mod_mask;
      for(int w = word_idx + 1; w < NUM_WORDS; ++w)
      {
        r[w] = 0;
      }
    }

    // Step E: Print immediately
    std::cout << "Chunk " << chunk_count++ << ": " << std::setfill('0') << std::setw(8) << next8 << "\n";

    digits_computed += 8;
  }

  return 0;
}

/*
 *
 *  128 BIT VERSION SAME SHIT
 *
 *
 *
#include <cmath>
#include <iomanip>
#include <iostream>

int main()
{
  uint32_t k = 1027; // Supports any k in [1, 1100]

  // Total digits in 5^k
  uint32_t total_digits = std::floor(k * std::log10(5)) + 1;

  // Power of 10 needed for the first 8-digit chunk
  uint32_t P = std::floor(k * std::log10(2));

  // 18 words * 64 bits = 1152 bits. Perfectly fits k=1100 + 10^8 overflow.
  // ALL heap allocation (std::vector) is eliminated.
  constexpr int NUM_WORDS = 18;
  uint64_t r[NUM_WORDS] = { 0 };
  r[0] = 1; // Initialize R = 1

  // 1. One-time Initialization Loop: Compute R = 10^P inline
  for(uint32_t i = 0; i < P; ++i)
  {
    uint64_t carry = 0;
    for(int w = 0; w < NUM_WORDS; ++w)
    {
      unsigned __int128 p = (unsigned __int128)r[w] * 10ULL + carry;
      r[w] = (uint64_t)p;
      carry = (uint64_t)(p >> 64);
    }
    uint64_t jasdlj = 134;
  }

  // Pre-calculate shift and mask values for O(1) chunk extraction / modulo
  uint32_t word_idx = k / 64;
  uint32_t bit_shift = k % 64;
  uint64_t mod_mask = (bit_shift == 0) ? 0 : ((1ULL << bit_shift) - 1);

  // Initial extraction & Modulo
  uint64_t chunk = 0;
  // 2. Main Processing Loop: Dynamically compute chunks inline
  uint32_t digits_computed = 0;
  int chunk_count = 2;

  while(digits_computed < total_digits)
  {
    // Step A & B: Multiply by 10^8 and cascade the carry
    uint64_t carry = 0;
    for(int w = 0; w < NUM_WORDS; ++w)
    {
      unsigned __int128 p = (unsigned __int128)r[w] * 100000000ULL + carry;
      r[w] = (uint64_t)p;
      carry = (uint64_t)(p >> 64);
    }

    // Step C: Extract next8 chunk (R >> k)
    chunk = 0;
    if(word_idx < NUM_WORDS)
    {
      chunk = r[word_idx] >> bit_shift;
    }
    if(word_idx + 1 < NUM_WORDS && bit_shift != 0)
    {
      chunk |= (r[word_idx + 1] << (64 - bit_shift));
    }
    uint32_t next8 = (uint32_t)chunk;

    // Step D: Apply Modulo 2^k instantly
    if(word_idx < NUM_WORDS)
    {
      r[word_idx] &= mod_mask;
      for(int w = word_idx + 1; w < NUM_WORDS; ++w)
      {
        r[w] = 0;
      }
    }

    // Step E: Print immediately
    std::cout << "Chunk " << chunk_count++ << ": " << std::setfill('0') << std::setw(8) << next8 << "\n";

    digits_computed += 8;
  }

  return 0;
}
*/

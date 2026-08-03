#include <algorithm>
#include <cmath>
#include <cstdint> // Added for explicit uint32_t and uint64_t types
#include <cstdint>
#include <iomanip>
#include <iostream>

int main()
{
  // Hacker's Delight magic constants for 32-bit division by 100,000,000 No 128-bit math required. Fits inside a standard uint64_t.
  constexpr uint64_t MAGIC_10E8 = 1441151881ULL;
  constexpr int SHIFT_10E8 = 57;

  std::cout << "{ ";
  for(int k = 0; k < 1075; k += 64)
  {
    // Total digits in 5^k
    uint32_t total_digits = std::floor(k * std::log10(5)) + 1;

    // Calculate number of 8-digit chunks needed
    uint32_t max_chunks = (total_digits + 7) / 8;

    // ALL heap allocation eliminated. Storage strictly 32-bit.
    constexpr int NUM_WORDS = 96;
    uint32_t r[NUM_WORDS] = { 0 };

    // Initialize the first chunk to 1
    r[0] = 1;

    // Main Processing Loop: Compute chunks of base 10^8 bottom-up
    int powers_left = k;

    // Process a max of 2 powers of 5 per step so that: (10^8 - 1) * 25 + carry <= ~2.5 billion (Fits strictly in uint32_t)
    while(powers_left > 0)
    {
      int chunk_powers = std::min(powers_left, 2);

      // Calculate multiplier: either 5^2 (25) or 5^1 (5)
      uint32_t multiplier = (chunk_powers == 2) ? 25 : 5;

      // Carry is strictly 32-bit
      uint32_t carry = 0;

      for(unsigned int &w : r)
      {
        // 32-bit math (base 10^8 x multiplier + carry).
        uint32_t p = w * multiplier + carry;

        // Hacker's Delight Division: 32x32 -> 64-bit math
        carry = (uint32_t)(((uint64_t)p * MAGIC_10E8) >> SHIFT_10E8);

        // Hacker's Delight Modulo: instantly resolves the remainder
        w = p - carry * 100'000'000U;
      }

      powers_left -= chunk_powers;
    }

    std::cout << "{ ";
    for(unsigned int w : r)
    {
      if(w == 0)
      {
        continue;
      }
      std::cout << w << ", ";
    }
    std::cout << "\b },\n";
  }
  std::cout << "\b }\n";

  std::cout << "{ ";
  for(int k = 0; k < 1075; k += 64)
  {
    std::cout << k << ", ";
  }
  std::cout << "\b }\n";

  return 0;
}

/*
int main()
{
  // Hacker's Delight magic constants for 32-bit division by 100,000,000
  // No 128-bit math required. Fits inside a standard uint64_t.
  constexpr uint64_t MAGIC_10E8 = 1441151881ULL;
  constexpr int SHIFT_10E8 = 57;

  for(int k = 1074; k < 1075; k++)
  {
    // Total digits in 5^k
    uint32_t total_digits = std::floor(k * std::log10(5)) + 1;

    // Calculate number of 8-digit chunks needed
    uint32_t max_chunks = (total_digits + 7) / 8;

    // ALL heap allocation eliminated. Storage strictly 32-bit.
    constexpr int NUM_WORDS = 96;
    uint32_t r[NUM_WORDS] = { 0 };

    // Initialize the first chunk to 1
    r[0] = 1;

    // Main Processing Loop: Compute chunks of base 10^8 bottom-up
    int powers_left = k;

    // Process a max of 2 powers of 5 per step so that:
    // (10^8 - 1) * 25 + carry <= ~2.5 billion (Fits strictly in uint32_t)
    while(powers_left > 0)
    {
      int chunk_powers = std::min(powers_left, 2);

      // Calculate multiplier: either 5^2 (25) or 5^1 (5)
      uint32_t multiplier = (chunk_powers == 2) ? 25 : 5;

      // Carry is strictly 32-bit
      uint32_t carry = 0;

      for(unsigned int &w : r)
      {
        // 32-bit math (base 10^8 x multiplier + carry).
        // Max p is ~2.5 * 10^9, so it never overflows uint32_t.
        uint32_t p = w * multiplier + carry;

        // Hacker's Delight Division: 32x32 -> 64-bit math
        carry = (uint32_t)(((uint64_t)p * MAGIC_10E8) >> SHIFT_10E8);

        // Hacker's Delight Modulo: instantly resolves the remainder
        w = p - carry * 100'000'000U;
      }

      powers_left -= chunk_powers;
    }

    std::cout << "k = " << k << " : ";

    for(int w = NUM_WORDS - 1; w >= 0; --w)
    {
      if(r[w] == 0)
      {
        continue;
      }
      std::cout << std::setfill('0') << std::setw(8) << r[w];
    }
    std::cout << "\n";
  }

  return 0;
}
*/

/*
int main()
{
  for(int k = 1074; k < 1075; k++)
  {
    // Total digits in 5^k
    uint32_t total_digits = std::floor(k * std::log10(5)) + 1;

    // Calculate number of 8-digit chunks needed
    // 5^1100 has ~770 digits, which needs ~97 chunks. 110 is plenty.
    uint32_t max_chunks = (total_digits + 7) / 8;

    // ALL heap allocation eliminated. Storage strictly 32-bit.
    constexpr int NUM_WORDS = 110;
    uint32_t r[NUM_WORDS] = { 0 };

    // Initialize the first chunk to 1
    r[0] = 1;
    uint32_t active_chunks = 1;

    // Main Processing Loop: Compute chunks of base 10^8 bottom-up
    int powers_left = k;

    // Multiply by up to 5^13 per step.
    // (10^8 - 1) * 5^13 + carry comfortably fits in uint64_t.
    while(powers_left > 0)
    {
      int chunk_powers = std::min(powers_left, 13);

      // Calculate 5^(chunk_powers)
      uint32_t multiplier = 1;
      for(int i = 0; i < chunk_powers; ++i)
      {
        multiplier *= 5;
      }

      // Carry requires 64-bit for the intermediate multiplication
      uint64_t carry = 0;

      for(uint32_t w = 0; w < active_chunks; ++w)
      {
        // 32-bit (base 10^8) x 32-bit (multiplier) + 64-bit carry
        uint64_t p = (uint64_t)r[w] * multiplier + carry;
        r[w] = (uint32_t)(p % 100'000'000ULL);
        carry = p / 100'000'000ULL;
      }

      // Cascade any remaining carry into higher-order chunks
      while(carry > 0 && active_chunks < NUM_WORDS)
      {
        r[active_chunks++] = (uint32_t)(carry % 100'000'000ULL);
        carry /= 100'000'000ULL;
      }

      powers_left -= chunk_powers;
    }

    std::cout << "k = " << k << " : ";

    // All digits are now ready in memory!
    // We print them in reverse (highest magnitude chunk to lowest).
    bool first_chunk = true;
    for(int w = active_chunks - 1; w >= 0; --w)
    {
      if(first_chunk)
      {
        std::cout << r[w]; // Never pad the leading chunk
        first_chunk = false;
      }
      else
      {
        // Enforce 8-digit padding for all subsequent chunks
        std::cout << std::setfill('0') << std::setw(8) << r[w];
      }
    }
    std::cout << "\n";
  }

  return 0;
}
*/

/* 5^k all visible
 *
int main()
{
  constexpr unsigned k = 1074;
  constexpr uint32_t BASE = 10'000'000U; // 1e7

  std::array<unsigned, 180> rr = { 0 };
  rr[0] = 1;

  for(uint32_t i = 0; i < k; ++i)
  {
    uint32_t carry = 0;

    for(uint32_t &w : rr)
    {
      unsigned p = w * 5ULL + carry;

      // divide by 100,000,000 without DIV
      carry = (uint32_t)((p * 0x6B5FCA6BULL) >> 54U);

      // remainder
      w = (uint32_t)(p - (uint64_t)carry * BASE);
    }
  }

  std::cout << "k = ";

  for(int i = rr.size() - 1; i >= 0; i--)
  {
    if(rr[i] == 0)
    {
      continue;
    }
    std::cout << std::setfill('0') << std::setw(8) << rr[i];
  }

  std::cout << std::endl;

  return 0;
}
*/

/*
int main()
{
  for(int k = 984; k < 985; k++)
  {
    // 36 words * 32 bits = 1152 bits. Perfectly fits k=1100 + 10^8 overflow.
    // ALL heap allocation (std::vector) is eliminated.
    constexpr int NUM_WORDS = 36;
    const uint32_t P = std::floor(k * std::log10(2));

    uint32_t rrprime[NUM_WORDS] = { 0 };

    // CRITICAL FIX: Reset rprime cleanly for this iteration of k
    rrprime[0] = 1;

    // Total digits in 5^k
    uint32_t total_digits = std::floor(k * std::log10(5)) + 1;

    // 36 words * 32 bits = 1152 bits. Perfectly fits k=1100 + 10^8 overflow.
    // ALL heap allocation (std::vector) is eliminated.
    constexpr int NUM_WORDS = 36;

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
    }

    // Pre-calculate shift and mask values for O(1) chunk extraction / modulo
    uint32_t word_idx = k / 32;
    uint32_t bit_shift = k % 32;
    uint32_t mod_mask = (bit_shift == 0) ? 0 : ((1U << bit_shift) - 1);

    // Initial extraction & Modulo
    uint64_t chunk = 0;
    // 2. Main Processing Loop: Dynamically compute chunks inline
    uint32_t digits_computed = 0;

    std::cout << "k = " << k << " : ";

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
      std::cout << std::setfill('0') << std::setw(8) << next8;

      digits_computed += 8;
    }
    std::cout << "\n";
  }

  return 0;
}
*/

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

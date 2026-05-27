
#include <cstdint>
#include <iomanip>
#include <iostream>

int main()
{
  uint32_t k = 1024; // Test range [1, 1024]

  // 2^1024 has ~309 digits. 309 digits / 8 = ~39 chunks.
  // 40 words * 32 bits perfectly fits without any overflow.
  // ALL heap allocation is eliminated.
  constexpr int NUM_WORDS = 40;
  uint32_t r[NUM_WORDS] = { 0 };
  r[0] = 1; // Initialize R = 1

  // 1. One-time Initialization Loop: Compute 2^k inline using Base-10^8
  uint32_t bits_remaining = k;
  while(bits_remaining > 0)
  {
    // Max step is 6 bits because (10^8 - 1) * 2^6 + carry
    // multiplied by our magic number safely fits inside a uint64_t.
    uint32_t step = (bits_remaining > 6) ? 6 : bits_remaining;
    uint32_t multiplier = 1U << step;

    uint32_t carry = 0; // Stored as u32
    for(int w = 0; w < NUM_WORDS; ++w)
    {
      // Grows to u64 for safe intermediate math just like original snippet
      uint64_t p = (uint64_t)r[w] * multiplier + carry;

      // --- MAGIC NUMBER DIVISION (NO / OPERATOR) ---
      // Simulates: carry = p / 100,000,000
      carry = (uint32_t)((p * 1441151881ULL) >> 57);

      // --- MAGIC NUMBER MODULO (NO % OPERATOR) ---
      // Simulates: r[w] = p % 100,000,000
      r[w] = (uint32_t)(p - (uint64_t)carry * 100'000'000ULL);
    }
    bits_remaining -= step;
  }

  // Find the highest active chunk to drop leading zero chunks
  int start_word = NUM_WORDS - 1;
  while(start_word > 0 && r[start_word] == 0)
  {
    start_word--;
  }

  // 2. Main Processing Loop: Streams out 8-digit chunks exactly every iteration
  int chunk_count = 1;
  for(int w = start_word; w >= 0; --w)
  {
    uint32_t next8 = r[w]; // Extracted instantly in O(1)

    // Step E: Print immediately per iteration
    if(w == start_word)
    {
      // Very first chunk has no leading zeroes
      std::cout << "Chunk " << chunk_count++ << ": " << next8 << "\n";
    }
    else
    {
      // Middle/trailing chunks get padded to exactly 8 digits
      std::cout << "Chunk " << chunk_count++ << ": " << std::setfill('0') << std::setw(8) << next8 << "\n";
    }
  }

  return 0;
}

/*
#include <cstdint>
#include <iomanip>
#include <iostream>

int main()
{
  std::cout << ",{ ";
  for(uint32_t k = 1; k < 2048; k *= 2) // Supports any k in [1, 1100]
  {
    // 36 words * 32 bits is enough for k=1152
    constexpr int NUM_WORDS = 40;
    uint32_t r[NUM_WORDS] = { 0 };
    r[0] = 1; // Initialize R = 1

    // Magic number for exact division by 10^8 using only shifts and multiplies
    // M = ceil(2^60 / 10^8)
    constexpr uint64_t MAGIC_10_8 = 11529215047ULL;
    constexpr uint32_t SHIFT_VAL = 60;

    // 1. Compute exactly 2^k in Base-10^8 chunks
    for(uint32_t i = 0; i < k; ++i)
    {
      uint32_t carry = 0;
      for(int w = 0; w < NUM_WORDS; ++w)
      {
        // Max p = 99,999,999 * 2 + 1 = 199,999,999
        uint64_t p = ((uint64_t)r[w] << 1U) + carry;

        // division by 10^8 converted to multiply and shift
        carry = (uint32_t)((p * MAGIC_10_8) >> SHIFT_VAL);

        // modulo 10^8 converted to subtraction
        r[w] = (uint32_t)(p - ((uint64_t)carry * 100'000'000));
      }
    }

    std::cout << ",{ ";
    for(int n = 0; n < NUM_WORDS; n++)
    {
      std::cout << r[n] << ", ";
    }

    std::cout << "} \n " << std::endl;
  }
  std::cout << "} \n " << std::endl;
  return 0;
}
*/

/*

#include <cstdint>
#include <iomanip>
#include <iostream>

int main()
{
  uint32_t k = 512; // Supports any k in [1, 1024]

  // 36 words * 32 bits is enough for k=1152
  constexpr int NUM_WORDS = 40;
  uint32_t r[NUM_WORDS] = { 0 };
  r[0] = 1; // Initialize R = 1

  // Magic number for exact division by 10^8 using only shifts and multiplies
  // M = ceil(2^60 / 10^8)
  constexpr uint64_t MAGIC_10_8 = 11529215047ULL;
  constexpr uint32_t SHIFT_VAL = 60;

  // 1. Compute exactly 2^k in Base-10^8 chunks
  for(uint32_t i = 0; i < k; ++i)
  {
    uint32_t carry = 0;
    for(int w = 0; w < NUM_WORDS; ++w)
    {
      // Max p = 99,999,999 * 2 + 1 = 199,999,999
      uint64_t p = ((uint64_t)r[w] << 1U) + carry;

      // division by 10^8 converted to multiply and shift
      carry = (uint32_t)((p * MAGIC_10_8) >> SHIFT_VAL);

      // modulo 10^8 converted to subtraction
      r[w] = (uint32_t)(p - ((uint64_t)carry * 100'000'000));
    }
  }

  // 2. Find the most significant word (O(1) search)
  int top = NUM_WORDS - 1;
  while(top > 0 && r[top] == 0)
  {
    top--;
  }

  // 3. Align the most significant chunk to exactly 8 digits
  uint32_t top_val = r[top];
  uint32_t missing_digits = 0;
  uint32_t temp = top_val;

  // Count missing digits without division (temp *= 10 using shifts)
  while(temp < 10000000)
  {
    temp = (temp << 3) + (temp << 1);
    missing_digits++;
  }

  // 4. Cascade shift the entire array by 10^missing_digits to pull up lower digits
  for(uint32_t m = 0; m < missing_digits; ++m)
  {
    uint32_t carry = 0;
    for(int w = 0; w <= top; ++w)
    {
      // Max p = 99,999,999 * 10 + 9 = 999,999,999
      uint64_t p = (uint64_t)r[w] * 10ULL + carry;
      carry = (uint32_t)((p * MAGIC_10_8) >> SHIFT_VAL);
      r[w] = (uint32_t)(p - carry * 100000000ULL);
    }
    // If the shift caused an overflow into a new chunk, advance the top pointer
    if(carry > 0)
    {
      top++;
      r[top] = carry;
    }
  }

  for(int n = NUM_WORDS - 1; n >= 0; n--)
  {
    std::cout << std::setfill('0') << std::setw(8) << r[n];
  }

  std::cout << std::endl;

  return 0;
}
*/

//

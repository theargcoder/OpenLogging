
#include <cmath>
#include <cstdint>
#include <iomanip>
#include <iostream>

int main()
{
  uint32_t k = 128;

  // Total digits in 5^128
  uint32_t total_digits = std::floor(k * std::log10(5)) + 1; // 90 digits

  // Power of 10 needed for the first 8-digit chunk
  // uint32_t P = (std::floor(k * std::log10(2)) + 8); // For k=128, P=46
  uint32_t P = (std::floor(k * std::log10(2))); // For k=128, P=46

  // Representing a 192-bit fixed integer space using 32-bit limbs
  // r0 = bits 0-31, r1 = bits 32-63, r2 = bits 64-95,
  // r3 = bits 96-127, r4 = bits 128-159, r5 = bits 160-191
  uint32_t r0 = 1, r1 = 0, r2 = 0, r3 = 0;

  for(uint32_t i = 0; i < P; ++i)
  {
    // Use uint64_t for intermediate steps to safely capture the carry (overflow)
    uint64_t p0 = (uint64_t)r0 * 10;
    r0 = (uint32_t)p0;

    uint64_t p1 = (uint64_t)r1 * 10 + (p0 >> 32);
    r1 = (uint32_t)p1;

    uint64_t p2 = (uint64_t)r2 * 10 + (p1 >> 32);
    r2 = (uint32_t)p2;

    uint64_t p3 = (uint64_t)r3 * 10 + (p2 >> 32);
    r3 = (uint32_t)p3;
  }

  // Extract the first chunk (bits 160-191, which live in r5)
  uint32_t first_chunk = r3;

  std::cout << "Chunk 1: " << std::setfill('0') << std::setw(8) << first_chunk << "\n";

  // 2. Main Processing Loop: Dynamically compute chunks inline
  uint32_t digits_computed = 8;
  int chunk_count = 2;

  while(digits_computed < total_digits)
  {
    // Step A: Multiply the lower 64 bits by 10^8
    unsigned __int128 p0 = (unsigned __int128)r0 * 100000000ULL;
    r0 = (uint64_t)p0;

    // Step B: Multiply the upper 64 bits and add the overflow/carry from p0
    unsigned __int128 p1 = (unsigned __int128)r1 * 100000000ULL + (p0 >> 64);
    r1 = (uint64_t)p1;

    // Step C: Extract next8 chunk & Apply Modulo 2^128 instantly
    // The bits overflowing past bit 128 represent (R >> 128), captured perfectly by p1 >> 64
    uint32_t next8 = (uint32_t)(p1 >> 64);

    // Step D: Print immediately
    std::cout << "Chunk " << chunk_count++ << ": " << std::setfill('0') << std::setw(8) << next8 << "\n";

    digits_computed += 8;
  }

  return 0;
}

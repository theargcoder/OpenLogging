#include <algorithm>
#include <iomanip>
#include <iostream>
#include <vector>

using BigInt = std::vector<uint32_t>;

// Extracts the next 8 least-significant digits.
// Uses purely 64-bit ops downcast to 32-bit results.
uint32_t extract_chunk_rtl(BigInt &a)
{
  uint64_t rem = 0;
  // Iterate from most significant word to least
  for(int i = (int)a.size() - 1; i >= 0; --i)
  {
    uint64_t val = (rem << 32) | a[i];
    a[i] = (uint32_t)(val / 100000000ULL); // Divide by 10^8
    rem = val % 100000000ULL;              // Keep remainder
  }

  // Trim leading zero words to keep the BigInt minimal
  while(!a.empty() && a.back() == 0)
  {
    a.pop_back();
  }
  return (uint32_t)rem;
}

int main()
{
  uint32_t k = 1027; // Works flawlessly for the entire [1, 1027] range

  // 1. O(1) INITIALIZATION: Computing 2^k requires ZERO multiplication.
  // We simply set the k-th bit in our 32-bit array.
  uint32_t words_needed = (k / 32) + 1;
  BigInt N(words_needed, 0);
  N[k / 32] = (1u << (k % 32));

  // 2. EXTRACT CHUNKS: Generate the 8-digit blocks Right-to-Left
  std::vector<uint32_t> chunks;
  while(!N.empty())
  {
    chunks.push_back(extract_chunk_rtl(N));
  }

  // 3. REVERSE: Align them for Left-to-Right dynamic reading
  std::reverse(chunks.begin(), chunks.end());

  // Output the results
  int chunk_count = 1;
  for(uint32_t chunk : chunks)
  {
    // The very first chunk might not have exactly 8 digits,
    // so we don't pad it with leading zeros.
    if(chunk_count == 1)
    {
      std::cout << "Chunk " << chunk_count++ << ": " << chunk << "\n";
    }
    else
    {
      // All subsequent chunks are strictly 8 digits
      std::cout << "Chunk " << chunk_count++ << ": " << std::setfill('0') << std::setw(8) << chunk << "\n";
    }
  }

  return 0;
}

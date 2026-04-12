def generate_magic_math_exact(divisor, bit_width):
    # 1. Find the shift (k) using exact integer bit_length
    # equivalent to math.floor(math.log2(divisor * 2**bit_width))
    val = divisor * (1 << bit_width)
    k = val.bit_length() - 1
    
    # 2. Find the magic multiplier (M) using EXACT integer ceiling division
    # (A + B - 1) // B avoids floating point casting completely
    M = ((1 << k) + divisor - 1) // divisor
    
    # Print the result
    hex_M = hex(M).upper().replace('X', 'x')
    if bit_width == 32:
        print(f"else if constexpr(N == {len(str(divisor))-1}) {{ return static_cast<uint32_t>((uint64_t(n) * {hex_M}ULL) >> {k}); }}")
    else:
        print(f"else if constexpr(N == {len(str(divisor))-1}) {{ return static_cast<uint64_t>( ((unsigned __int128)n * {hex_M}ULL) >> {k}); }}")

# Generate for 32-bit (Powers of 10 from 1 to 9)
print("--- 32-BIT ---")
for p in range(1, 10):
    generate_magic_math(10**p, 32)

# Generate for 64-bit (Powers of 10 from 1 to 20)
print("--- EXACT 64-BIT CONSTANTS ---")
for p in range(1, 20):
    generate_magic_math_exact(10**p, 64)

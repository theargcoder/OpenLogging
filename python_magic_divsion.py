import math

def generate_magic_math(divisor, bit_width):
    # 1. Find the shift (k)
    k = math.floor(math.log2(divisor * (2**bit_width)))
    
    # 2. Find the magic multiplier (M)
    M = math.ceil((2**k) / divisor)
    
    # Print the result
    hex_M = hex(M).upper().replace('X', 'x')
    if bit_width == 32:
        print(f"Div by {divisor:<12}: (uint64_t(n) * {hex_M}ULL) >> {k}")
    else:
        print(f"Div by {divisor:<12}: ((unsigned __int128)n * {hex_M}ULL) >> {k}")

# Generate for 32-bit (Powers of 10 from 1 to 9)
print("--- 32-BIT ---")
for p in range(1, 10):
    generate_magic_math(10**p, 32)

# Generate for 64-bit (Powers of 10 from 1 to 19)
print("\n--- 64-BIT ---")
for p in range(1, 20):
    generate_magic_math(10**p, 64)

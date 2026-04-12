import math

def generate_magic_modulo(divisor, bit_width):
    # 1. Find the shift (k)
    # For safety in 32-bit, we ensure k is at least bit_width to keep precision
    k = math.floor(math.log2(divisor * (2**bit_width)))
    
    # 2. Find the magic multiplier (M)
    M = math.ceil((2**k) / divisor)
    
    hex_M = hex(M).upper().replace('X', 'x')
    
    if bit_width == 32:
        # q = quotient
        q_expr = f"((uint64_t(n) * {hex_M}ULL) >> {k})"
        print(f"Mod {divisor:<12}: n - ({q_expr} * {divisor}U)")
    else:
        # q = quotient
        q_expr = f"(((unsigned __int128)n * {hex_M}ULL) >> {k})"
        print(f"Mod {divisor:<12}: n - ({q_expr} * {divisor}ULL)")

# Generate for 32-bit (Powers of 10)
print("--- 32-BIT MODULO ---")
for p in range(1, 10):
    generate_magic_modulo(10**p, 32)

# Generate for 64-bit (Powers of 10)
print("\n--- 64-BIT MODULO ---")
for p in range(1, 20):
    generate_magic_modulo(10**p, 64)

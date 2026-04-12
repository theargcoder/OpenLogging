def generate_magic_mod_exact(divisor, bit_width):
    # 1. Exact integer shift (k)
    val = divisor * (1 << bit_width)
    k = val.bit_length() - 1
    
    # 2. Exact magic multiplier (M)
    M = ((1 << k) + divisor - 1) // divisor
    
    hex_M = hex(M).upper().replace('X', 'x')
    p = len(str(divisor)) - 1
    
    if bit_width == 32:
        print(f"    else if constexpr(N == {p}) {{ return static_cast<uint32_t>(n - (((uint64_t(n) * {hex_M}ULL) >> {k}) * {divisor}U)); }}")
    else:
        print(f"    else if constexpr(N == {p}) {{ return static_cast<uint64_t>(n - ((((unsigned __int128)n * {hex_M}ULL) >> {k}) * {divisor}ULL)); }}")

print("--- 32-BIT EXACT MODULO ---")
for p in range(1, 10):
    generate_magic_mod_exact(10**p, 32)

print("\n--- 64-BIT EXACT MODULO ---")
for p in range(1, 20):
    generate_magic_mod_exact(10**p, 64)

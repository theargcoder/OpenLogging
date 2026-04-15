def generate_magic_mod_exact(divisor, bit_width):
    # Modulo is just: n - (n/d * d)
    # We reuse the same M and k logic for the division part.
    f = divisor.bit_length() - 1
    k = bit_width + f
    M = ((1 << k) + divisor - 1) // divisor
    
    # Verification
    max_n = (1 << bit_width) - 1
    if ((max_n * M) >> k) != (max_n // divisor):
        k += 1
        M = ((1 << k) + divisor - 1) // divisor

    hex_M = hex(M).upper().replace('X', 'x')
    p = len(str(divisor)) - 1

    if bit_width == 32:
        suffix = "ULL" if M > 0xFFFFFFFF else "U"
        print(f"else if constexpr(N == {p}) {{ return static_cast<uint32_t>(n - ((uint64_t(n) * {hex_M}{suffix} >> {k}) * {divisor}U)); }}")
    else:
        if M > 0xFFFFFFFFFFFFFFFF:
            m_high = M >> 64
            m_low = M & 0xFFFFFFFFFFFFFFFF
            m_str = f"((unsigned __int128({hex(m_high)}) << 64) | {hex(m_low)}ULL)"
        else:
            m_str = f"{hex_M}ULL"
        print(f"else if constexpr(N == {p}) {{ return static_cast<uint64_t>(n - (((unsigned __int128)n * {m_str} >> {k}) * {divisor}ULL)); }}")

print("\n// --- 32-BIT MODULO ---")
for p in range(1, 10):
    generate_magic_mod_exact(10**p, 32)

print("\n// --- 64-BIT MODULO ---")
for p in range(1, 20):
    generate_magic_mod_exact(10**p, 64)

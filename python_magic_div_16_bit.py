def get_magic_numbers(d, W):
    # Hacker's Delight: Magic Number Algorithm
    # nc is the highest value < 2^W that leaves remainder d-1 (the true worst-case)
    nc = ((1 << W) - 1) - (((1 << W) - 1) % d)

    for p in range(W - 1, 2 * W):
        two_p = 1 << p
        M = (two_p + d - 1) // d
        # Verification: ensures error is strictly less than 1/d for all n
        if M * d <= two_p + two_p // nc:
            return M, p
    raise ValueError("Failed to find magic number")

def generate_cpp_code(d, W):
    M, p = get_magic_numbers(d, W)
    p_val = len(str(d)) - 1
    t_name = "uint16_t"

    # Format magic number nicely
    hex_M = hex(M).upper().replace('X', 'x')

    if M < (1 << W):
        # Standard Reciprocal Multiplication (Fits in type)
        # 16-bit * 16-bit needs a 32-bit intermediate variable to hold the result
        div_expr = f"(uint32_t(n) * {hex_M}U) >> {p}"
        
        div_code = f"    else if constexpr(N == {p_val}) {{ return static_cast<{t_name}>({div_expr}); }}"
        mod_code = f"    else if constexpr(N == {p_val}) {{ return static_cast<{t_name}>(n - ({div_expr}) * {d}U); }}"
        return div_code, mod_code

    else:
        # Overflow condition: M >= 2^W. We must use the "Add and Shift" technique
        M_rem = M - (1 << W)
        hex_M_rem = hex(M_rem).upper().replace('X', 'x')
        shift = p - W
        
        # Shift right by W (16) after multiplying by the remaining magic number
        t_expr = f"static_cast<uint16_t>((uint32_t(n) * {hex_M_rem}U) >> 16)"
        
        div_code = (
            f"    else if constexpr(N == {p_val}) {{\n"
            f"        {t_name} t = {t_expr};\n"
            f"        return (((n - t) >> 1) + t) >> {shift - 1};\n"
            f"    }}"
        )
        mod_code = (
            f"    else if constexpr(N == {p_val}) {{\n"
            f"        {t_name} t = {t_expr};\n"
            f"        {t_name} q = (((n - t) >> 1) + t) >> {shift - 1};\n"
            f"        return static_cast<{t_name}>(n - (q * {d}U));\n"
            f"    }}"
        )
        return div_code, mod_code

# --- GENERATE OUTPUT ---
div_16, mod_16 = [], []

# For 16-bit, max value is 65535, so we only need powers of 10 up to 10^4
for p in range(1, 5):
    d, m = generate_cpp_code(10**p, 16)
    div_16.append(d)
    mod_16.append(m)

print("// --- 16-BIT DIVISION ---")
print("\n".join(div_16))
print("\n// --- 16-BIT MODULO ---")
print("\n".join(mod_16))

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
    t_name = "uint32_t" if W == 32 else "uint64_t"
    
    # Format magic number nicely
    hex_M = hex(M).upper().replace('X', 'x')
    
    if M < (1 << W):
        # Standard Reciprocal Multiplication (Fits in type)
        if W == 32:
            div_expr = f"(uint64_t(n) * {hex_M}ULL) >> {p}"
        else:
            div_expr = f"((unsigned __int128)n * {hex_M}ULL) >> {p}"
            
        div_code = f"    else if constexpr(N == {p_val}) {{ return static_cast<{t_name}>({div_expr}); }}"
        mod_code = f"    else if constexpr(N == {p_val}) {{ return static_cast<{t_name}>(n - ({div_expr}) * {d}ULL); }}"
        return div_code, mod_code
    
    else:
        # Overflow condition: M >= 2^W. We must use the "Add and Shift" technique
        M_rem = M - (1 << W)
        hex_M_rem = hex(M_rem).upper().replace('X', 'x')
        shift = p - W
        
        if W == 32:
            t_expr = f"static_cast<uint32_t>((uint64_t(n) * {hex_M_rem}ULL) >> 32)"
        else:
            t_expr = f"static_cast<uint64_t>(((unsigned __int128)n * {hex_M_rem}ULL) >> 64)"
            
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
            f"        return static_cast<{t_name}>(n - (q * {d}ULL));\n"
            f"    }}"
        )
        return div_code, mod_code

# --- GENERATE OUTPUT ---
div_32, mod_32, div_64, mod_64 = [], [], [], []

for p in range(1, 10):
    d, m = generate_cpp_code(10**p, 32)
    div_32.append(d); mod_32.append(m)

for p in range(1, 20):
    d, m = generate_cpp_code(10**p, 64)
    div_64.append(d); mod_64.append(m)

print("// --- 32-BIT DIVISION ---")
print("\n".join(div_32))
print("\n// --- 64-BIT DIVISION ---")
print("\n".join(div_64))
print("\n// --- 32-BIT MODULO ---")
print("\n".join(mod_32))
print("\n// --- 64-BIT MODULO ---")
print("\n".join(mod_64))

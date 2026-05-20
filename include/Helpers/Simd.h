#pragma once

#include <ammintrin.h>
#include <array>
#include <bit>
#include <cstdint>
#include <emmintrin.h>
#include <limits>
#if defined(_MSC_VER) || defined(__x86_64__) || defined(__i386__)
#include <immintrin.h> // x86 SIMD
#elif defined(__ARM_NEON) || defined(__aarch64__)
#include <arm_neon.h> // ARM SIMD
#endif

#include <type_traits>

#include "include/Helpers/Math.h"

#if defined(__ARM_NEON) || defined(__aarch64__)
namespace Helpers::Simd::ARM64
{
  __attribute__((always_inline)) static auto umul_hi_32x4(const uint32x4_t &v_a, const uint32x4_t &v_b) noexcept
  {
    const uint64x2_t prod_low = vmull_u32(vget_low_u32(v_a), vget_low_u32(v_b));
    const uint64x2_t prod_high = vmull_high_u32(v_a, v_b);

    // Re-combine the high 32-bits of each 64-bit result into one 128-bit vector This is essentially 'mulhi'
    return vcombine_u32(vshrn_n_u64(prod_low, 32), vshrn_n_u64(prod_high, 32));
  }

  __attribute__((always_inline)) static auto umul_low_32x4(const uint32x4_t &v_a, const uint32x4_t &v_b) noexcept
  {
    return vmulq_u32(v_a, v_b);
  }

  __attribute__((always_inline)) static auto umul_hi_u16x8(const uint16x8_t &v_a, const uint16x8_t &v_b) noexcept
  {
    // 1. Widen multiply the low 4 elements (16-bit * 16-bit -> 32-bit)
    const uint32x4_t prod_low = vmull_u16(vget_low_u16(v_a), vget_low_u16(v_b));

    // 2. Widen multiply the high 4 elements (16-bit * 16-bit -> 32-bit)
    const uint32x4_t prod_high = vmull_high_u16(v_a, v_b);

    // 3. Shift right by 16 to keep only the "high" part of the 32-bit products
    // 4. Narrow the 32-bit results back down to 16-bit and combine
    return vcombine_u16(vshrn_n_u32(prod_low, 16), vshrn_n_u32(prod_high, 16));
  }

  __attribute__((always_inline)) static auto umul_low_u16x8(const uint16x8_t &v_a, const uint16x8_t &v_b) noexcept
  {
    // This returns the lower 16 bits of each 32-bit product
    return vmulq_u16(v_a, v_b);
  }

  template <typename T>
    requires(std::is_integral_v<T> && std::is_unsigned_v<T>)
  static uint32_t WriteCharsToPtrFowardReturnLength(char *__restrict__ buff, const auto &input) noexcept;

  template <>
  uint32_t WriteCharsToPtrFowardReturnLength<uint64_t>(char *__restrict__ buff, const uint64_t &input) noexcept
  {
    static const constexpr uint16x8_t v_magics_u16_10e3 = { 0x625U, 0x47AFU, 0xCCCDU, 0xFFFF, 0x625U, 0x47AFU, 0xCCCDU, 0xFFFF };
    static const constexpr int16x8_t v_first_shifts = { -9, -6, -3, 0, -9, -6, -3, 0 };
    static const constexpr uint16_t MAX_uin16 = std::numeric_limits<uint16_t>::max();
    static const constexpr uint16_t v_magic_div_10e3 = 0xCCCDU;
    static const constexpr uint16_t v_magic_div_10e3_shf = -3;

    if(input == 0)
    {
      *buff = '0';
      *(buff + 1) = '\0';
      return 1U;
    }

    uint32_t lane_1 = Helpers::Math::Magic::Division::div_by_10_pow_n<16>(input);
    uint32_t lane_2 = Helpers::Math::Magic::Modulo::mod_by_10_pow_n<4>(Helpers::Math::Magic::Division::div_by_10_pow_n<12>(input));
    uint32_t lane_3 = Helpers::Math::Magic::Modulo::mod_by_10_pow_n<4>(Helpers::Math::Magic::Division::div_by_10_pow_n<8>(input));
    uint32_t lane_4 = Helpers::Math::Magic::Modulo::mod_by_10_pow_n<4>(Helpers::Math::Magic::Division::div_by_10_pow_n<4>(input));
    uint32_t lane_5 = Helpers::Math::Magic::Modulo::mod_by_10_pow_n<4>(input);

    const uint16x8_t top_mid_16x8 = vcombine_u16(vdup_n_u16(lane_1), vdup_n_u16(lane_2));
    const uint16x8_t mid_low_16x8 = vcombine_u16(vdup_n_u16(lane_3), vdup_n_u16(lane_4));
    const uint16x8_t low_low_16x8 = vdupq_n_u16(lane_5);

    const uint16x8_t v_top_prod_16x8 = umul_hi_u16x8(top_mid_16x8, v_magics_u16_10e3);
    const uint16x8_t v_mid_prod_16x8 = umul_hi_u16x8(mid_low_16x8, v_magics_u16_10e3);
    const uint16x8_t v_low_prod_16x8 = umul_hi_u16x8(low_low_16x8, v_magics_u16_10e3);

    const uint16x8_t v_top_minus_v_subs = vandq_u16(vsubq_u16(top_mid_16x8, v_top_prod_16x8), uint16x8_t{ MAX_uin16, MAX_uin16, 0U, 0U, MAX_uin16, MAX_uin16, 0U, 0U });
    const uint16x8_t v_mid_minus_v_subs = vandq_u16(vsubq_u16(mid_low_16x8, v_mid_prod_16x8), uint16x8_t{ MAX_uin16, MAX_uin16, 0U, 0U, MAX_uin16, MAX_uin16, 0U, 0U });
    const uint16x8_t v_low_minus_v_subs = vandq_u16(vsubq_u16(low_low_16x8, v_low_prod_16x8), uint16x8_t{ MAX_uin16, MAX_uin16, 0U, 0U, MAX_uin16, MAX_uin16, 0U, 0U });

    const uint16x8_t v_top_shf = vshlq_u16(v_top_minus_v_subs, vdupq_n_u16(-1));
    const uint16x8_t v_mid_shf = vshlq_u16(v_mid_minus_v_subs, vdupq_n_u16(-1));
    const uint16x8_t v_low_shf = vshlq_u16(v_low_minus_v_subs, vdupq_n_u16(-1));

    const uint16x8_t v_top_add = vaddq_u16(v_top_shf, v_top_prod_16x8);
    const uint16x8_t v_mid_add = vaddq_u16(v_mid_shf, v_mid_prod_16x8);
    const uint16x8_t v_low_add = vaddq_u16(v_low_shf, v_low_prod_16x8);

    const uint16x8_t v_top_digs_16x8 = vaddq_u16(vshlq_u16(v_top_add, v_first_shifts), vandq_u16(vcgtq_u16(top_mid_16x8, vdupq_n_u16(0)), uint16x8_t{ 0, 0, 0, 1, 0, 0, 0, 1 }));
    const uint16x8_t v_mid_digs_16x8 = vaddq_u16(vshlq_u16(v_mid_add, v_first_shifts), vandq_u16(vcgtq_u16(mid_low_16x8, vdupq_n_u16(0)), uint16x8_t{ 0, 0, 0, 1, 0, 0, 0, 1 }));
    const uint16x8_t v_low_digs_16x8 = vaddq_u16(vshlq_u16(v_low_add, v_first_shifts), vandq_u16(vcgtq_u16(low_low_16x8, vdupq_n_u16(0)), uint16x8_t{ 0, 0, 0, 1, 0, 0, 0, 1 }));

    const uint16x8_t v_top_digs_16x8_div_10 = vshlq_u16(umul_hi_u16x8(v_top_digs_16x8, vdupq_n_u16(v_magic_div_10e3)), vdupq_n_u16(v_magic_div_10e3_shf));
    const uint16x8_t v_mid_digs_16x8_div_10 = vshlq_u16(umul_hi_u16x8(v_mid_digs_16x8, vdupq_n_u16(v_magic_div_10e3)), vdupq_n_u16(v_magic_div_10e3_shf));
    const uint16x8_t v_low_digs_16x8_div_10 = vshlq_u16(umul_hi_u16x8(v_low_digs_16x8, vdupq_n_u16(v_magic_div_10e3)), vdupq_n_u16(v_magic_div_10e3_shf));

    const uint16x8_t v_top_digs_16x8_div_10_mul_10 = umul_low_u16x8(v_top_digs_16x8_div_10, vdupq_n_u16(10U));
    const uint16x8_t v_mid_digs_16x8_div_10_mul_10 = umul_low_u16x8(v_mid_digs_16x8_div_10, vdupq_n_u16(10U));
    const uint16x8_t v_low_digs_16x8_div_10_mul_10 = umul_low_u16x8(v_low_digs_16x8_div_10, vdupq_n_u16(10U));

    const uint16x8_t top_full_res = vsubq_u16(v_top_digs_16x8, v_top_digs_16x8_div_10_mul_10);
    const uint16x8_t mid_full_res = vsubq_u16(v_mid_digs_16x8, v_mid_digs_16x8_div_10_mul_10);
    const uint16x8_t low_full_res = vandq_u16(vsubq_u16(v_low_digs_16x8, v_low_digs_16x8_div_10_mul_10), uint16x8_t{ MAX_uin16, MAX_uin16, MAX_uin16, MAX_uin16, 0, 0, 0, 0 });

    // 3. Narrow 16x8 to 8x8
    const uint16x8_t v_1_2_mask = vandq_u16(vcgtq_u16(top_full_res, vdupq_n_u16(0)), uint16x8_t{ 128, 64, 32, 16, 8, 4, 2, 1 });
    const uint16x8_t v_3_4_mask = vandq_u16(vcgtq_u16(mid_full_res, vdupq_n_u16(0)), uint16x8_t{ 128, 64, 32, 16, 8, 4, 2, 1 });
    const uint16x8_t v_5_0_mask = vandq_u16(vcgtq_u16(low_full_res, vdupq_n_u16(0)), uint16x8_t{ 128, 64, 32, 16, 8, 4, 2, 1 });
    const uint32_t v_1_2_bitmask = vaddlvq_u8(v_1_2_mask);
    const uint32_t v_3_4_bitmask = vaddlvq_u8(v_3_4_mask);
    const uint32_t v_5_0_bitmask = vaddlvq_u8(v_5_0_mask);
    const uint32_t combined_mask = (v_1_2_bitmask << 24U) | (v_3_4_bitmask << 16U) | (v_5_0_bitmask << 8U);

    const uint16_t leading_z = std::countl_zero(combined_mask);

    const uint8x16_t v_1234_combined = vcombine_u8(vmovn_u16(top_full_res), vmovn_u16(mid_full_res));
    const uint8x8_t v_5_0_16_to8bit = vmovn_u16(low_full_res);

    const uint8x16_t v_1234_and = vaddq_s8(v_1234_combined, vdupq_n_u8('0'));
    const uint8x8_t v_50_and = vadd_s8(v_5_0_16_to8bit, int8x8_t{ '0', '0', '0', '0', 0, 0, 0, 0 });

    if(leading_z <= 16)
    {
      static const constexpr int8x16_t indices = int8x16_t{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };

      const int8x16_t shift_vector = vdupq_n_s8(leading_z);
      const int8x16_t selector = vaddq_s8(indices, shift_vector);

      // 3. Use TBL to "pick" the bytes at those new positions
      const uint8x16_t result = vqtbl1q_u8(v_1234_and, selector);

      vst1q_s8(reinterpret_cast<int8_t *>(buff), result);
      vst1_s8(reinterpret_cast<int8_t *>(buff + (16 - leading_z)), v_50_and);
    }
    else
    {
      static const constexpr int8x8_t indices = int8x8_t{ 0, 1, 2, 3, 4, 5, 6, 7 };
      const int8x8_t shift_vector = vdup_n_s8(leading_z - 16);
      const int8x8_t selector = vadd_s8(indices, shift_vector);
      // 3. Use TBL to "pick" the bytes at those new positions
      const uint8x8_t result = vtbl1_u8(v_50_and, selector);

      vst1_s8(reinterpret_cast<int8_t *>(buff), result);
    }

    const uint32_t len = (std::numeric_limits<uint64_t>::digits10 + 1) - leading_z;

    return len;
  }

  template <>
  uint32_t WriteCharsToPtrFowardReturnLength<uint32_t>(char *__restrict__ buff, const uint32_t &input) noexcept
  {

    static const constexpr auto M_MAGIC_U16 = uint16x8_t{ 0, 0x8313U, 0xA3D8U, 0x199AU, 0, 0x8313U, 0xA3D8U, 0x199AU };
    static const constexpr auto M_SHIFTS_U16 = int16x8_t{ 0, -9, -6, 0, 0, -9, -6, 0 };

    static const constexpr auto INDICES = uint8x16_t{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
    static const constexpr auto M_MAGIC_U32 = uint32x2_t{ 0x431BDE83ULL, 0x51EB851FULL };
    static const constexpr auto M_SHIFTS_U32 = int64x2_t{ -50, -37 };
    static const constexpr uint32_t LEN_TABLE[] = { 0, 10, 100, 1'000, 10'000, 100'000, 1'000'000, 10'000'000, 100'000'000, 1'000'000'000 };

    const auto A = vdup_n_u32(input);
    const auto prod = vmull_u32(A, M_MAGIC_U32);
    const auto inputs = vmovn_u64(vshlq_u64(prod, M_SHIFTS_U32));
    const auto inputs_x_10 = vmul_n_s32(inputs, 1'0000U);
    const auto inputs_slided = vext_u32(vdup_n_u32(0), inputs_x_10, 1);

    const auto full_inputs = vsub_u32(inputs, inputs_slided);
    const uint32_t lane_1 = vget_lane_u32(full_inputs, 0);
    const uint32_t lane_2 = vget_lane_u32(full_inputs, 1);
    uint16_t remainder = input - (vget_lane_u32(inputs, 1) * 100);
    uint16_t remrem;
    Helpers::Math::Magic::Modulo::mod_by_10_pow_n_void<1>(remainder, remrem);

    const auto n = vcombine_u16(vdup_n_u16(lane_1), vdup_n_u16(lane_2));
    const auto t = umul_hi_u16x8(n, M_MAGIC_U16);
    const auto n_div = vshlq_u16(t, M_SHIFTS_U16);
    const auto res_times_10 = vmulq_n_u16(n_div, 10U);
    const auto res_slided = vsetq_lane_u16(lane_1, vextq_u16(n_div, vdupq_n_u16(lane_2), 1), 3);

    const uint32_t bits = (sizeof(std::remove_cvref_t<decltype(input)>) * 8) - std::countl_zero(input);
    uint32_t len = (bits * 1233) >> 12;

    len += (input >= LEN_TABLE[len]);

    const uint16_t lead_z = std::numeric_limits<std::remove_cvref_t<decltype(input)>>::digits10 + 1 - len;

    const auto full_res = vmovn_u16(vsubq_u16(res_slided, res_times_10));

    const uint8x16_t combined = vcombine_u8(full_res, uint8x8_t{ static_cast<uint8_t>(remainder), static_cast<uint8_t>(remrem) });

    const uint8x16_t out = vaddq_u8(vqtbl1q_u8(combined, vaddq_u8(INDICES, vdupq_n_u8(lead_z))), vdupq_n_u8('0'));

    vst1q_s8(reinterpret_cast<int8_t *>(buff), out);

    return len;
  }

  template <>
  uint32_t WriteCharsToPtrFowardReturnLength<uint16_t>(char *__restrict__ buff, const uint16_t &input) noexcept
  {
    static const constexpr auto M_MAGIC_U16 = uint16x8_t{ 0xA36F, 0x625U, 0x47AFU, 0x999AU, 0, 0, 0, 0 };
    static const constexpr auto M_SHIFTS_U16 = int16x8_t{ -13, -9, -6, -3, 0, 0, 0, 0 };
    static const constexpr uint32_t LEN_TABLE[] = { 0, 10, 100, 1'000, 10'000 };

    static const constexpr auto INDICES = uint8x8_t{ 0, 1, 2, 3, 4, 5, 6, 7 };

    const auto n = vdupq_n_u16(input);
    const auto t = umul_hi_u16x8(n, M_MAGIC_U16);
    const auto n_sub_t = vsubq_u16(n, t);
    const auto n_sub_t_shf = vshrq_n_u16(n_sub_t, 1);
    const auto n_sub_t_shf_add_t = vaddq_u16(n_sub_t_shf, t);
    const auto n_sub_t_shf_add_t_shf = vsetq_lane_u16(input, vshlq_u16(n_sub_t_shf_add_t, M_SHIFTS_U16), 4);
    const auto res_times_10 = vmulq_n_u16(n_sub_t_shf_add_t_shf, 10U);
    const auto res_slided = vextq_u16(vdupq_n_u16(0), res_times_10, 7);

    const uint32_t bits = (sizeof(std::remove_cvref_t<decltype(input)>) * 8) - std::countl_zero(input);
    uint32_t len = (bits * 1233) >> 12;

    len += (input >= LEN_TABLE[len]);

    const uint16_t lead_z = std::numeric_limits<std::remove_cvref_t<decltype(input)>>::digits10 + 1 - len;

    const auto full_res = vsubq_u16(n_sub_t_shf_add_t_shf, res_slided);

    const uint8x8_t chars = vadd_u8(vmovn_u16(full_res), vdup_n_u8('0'));
    const uint8x8_t out = vtbl1_u8(chars, vadd_u8(INDICES, vdup_n_u8(lead_z)));

    vst1_s8(reinterpret_cast<int8_t *>(buff), out);

    return len;
  }

  template <>
  uint32_t WriteCharsToPtrFowardReturnLength<uint8_t>(char *__restrict__ buff, const uint8_t &input) noexcept
  {
    return WriteCharsToPtrFowardReturnLength<uint16_t>(buff, static_cast<uint16_t>(input));
  }
}
#endif

#if defined(_MSC_VER) || defined(__x86_64__) || defined(__i386__)
namespace Helpers::Simd::x86_64
{
  template <typename T>
    requires(std::is_integral_v<T> && std::is_unsigned_v<T>)
  static uint32_t WriteCharsToPtrFowardReturnLength(char *__restrict__ buff, const auto &input) noexcept;

  using uint128_t = unsigned __int128;

#define ENTRY(digits, offset) ((((uint128_t)(digits)) << 64) - (offset))

#if defined(__AVX512BW__) && defined(__AVX512VL__)

  __attribute__((always_inline)) static inline __m512i umul_hi_32x16(const __m512i a, const __m512i b) noexcept
  {
    const __m512i even_prod = _mm512_mul_epu32(a, b);
    const __m512i odd_prod = _mm512_mul_epu32(_mm512_srli_epi64(a, 32U), _mm512_srli_epi64(b, 32U));
    return _mm512_mask_blend_epi32(0xAAAAU, _mm512_srli_epi64(even_prod, 32U), odd_prod);
  }

  // Helper to fix the narrowing conversion warning
  inline __m512i set1_u64(uint64_t val)
  {
    return _mm512_set1_epi64(static_cast<long long>(val));
  }

  // Full 64-bit Unsigned Multiply High
  inline __m512i _mm512_mulhi_epu64(const __m512i &a, const __m512i &b)
  {
    const __m512i a_high = _mm512_srli_epi64(a, 32);
    const __m512i b_high = _mm512_srli_epi64(b, 32);

    const __m512i ll = _mm512_mul_epu32(a, b);           // AL * BL
    const __m512i lh = _mm512_mul_epu32(a, b_high);      // AL * BH
    const __m512i hl = _mm512_mul_epu32(a_high, b);      // AH * BL
    const __m512i hh = _mm512_mul_epu32(a_high, b_high); // AH * BH

    __m512i mid = _mm512_add_epi64(_mm512_srli_epi64(ll, 32), _mm512_and_si512(lh, _mm512_set1_epi64(0xFFFFFFFF)));
    mid = _mm512_add_epi64(mid, _mm512_and_si512(hl, _mm512_set1_epi64(0xFFFFFFFF)));

    __m512i result = _mm512_add_epi64(hh, _mm512_srli_epi64(lh, 32));
    result = _mm512_add_epi64(result, _mm512_srli_epi64(hl, 32));
    result = _mm512_add_epi64(result, _mm512_srli_epi64(mid, 32));

    return result;
  }

  inline __m256i _mm256_mulhi_epu64(const __m256i &a, const __m256i &b)
  {
    // Mask to isolate the lower 32 bits of each 64-bit lane
    const __m256i mask = _mm256_set1_epi64x(0xFFFFFFFF);

    const __m256i a_high = _mm256_srli_epi64(a, 32);
    const __m256i b_high = _mm256_srli_epi64(b, 32);

    // _mm256_mul_epu32 multiplies the low 32 bits of each 64-bit slot
    const __m256i ll = _mm256_mul_epu32(a, b);           // AL * BL
    const __m256i lh = _mm256_mul_epu32(a, b_high);      // AL * BH
    const __m256i hl = _mm256_mul_epu32(a_high, b);      // AH * BL
    const __m256i hh = _mm256_mul_epu32(a_high, b_high); // AH * BH

    // Extract carries from the low * low product and add to middle products
    __m256i mid = _mm256_add_epi64(_mm256_srli_epi64(ll, 32), _mm256_and_si256(lh, mask));
    mid = _mm256_add_epi64(mid, _mm256_and_si256(hl, mask));

    // Combine high product with carries from the middle products
    __m256i result = _mm256_add_epi64(hh, _mm256_srli_epi64(lh, 32));
    result = _mm256_add_epi64(result, _mm256_srli_epi64(hl, 32));
    result = _mm256_add_epi64(result, _mm256_srli_epi64(mid, 32));

    return result;
  }

  /**
   * @brief Maps a bit-width to the appropriate unsigned integer type.
   * Special case: 8-bit lanes are mapped to uint16_t to prevent debugger ASCII rendering.
   */
  template <size_t BitWidth>
  struct LaneType
  {
    using type = std::conditional_t<BitWidth == 8, uint16_t,
                                    std::conditional_t<BitWidth == 16, uint16_t, std::conditional_t<BitWidth == 32, uint32_t, std::conditional_t<BitWidth == 64, uint64_t, void>>>>;
  };

  /**
   * @brief Extracts lanes from __m128i, __m256i, or __m512i registers into a std::array.
   * @tparam BitWidth The size of the lane in bits (8, 16, 32, 64).
   * @param reg The SIMD register to extract from.
   */
  template <size_t BitWidth, typename RegType>
  inline auto extract_lanes(const RegType &reg)
  {
    constexpr size_t total_bytes = sizeof(RegType);
    constexpr size_t num_lanes = (total_bytes * 8) / BitWidth;
    using T = typename LaneType<BitWidth>::type;

    std::array<T, num_lanes> result;

    if constexpr(BitWidth == 8)
    {
      // Intermediary buffer to avoid debugger ASCII char issues
      alignas(RegType) uint8_t buffer[total_bytes];

      if constexpr(std::is_same_v<RegType, __m64>)
      {
        _mm_stream_pi(reinterpret_cast<__m64 *>(buffer), reg);
      }
      else if constexpr(std::is_same_v<RegType, __m128i>)
      {
        _mm_storeu_si128(reinterpret_cast<__m128i *>(buffer), reg);
      }
      else if constexpr(std::is_same_v<RegType, __m256i>)
      {
        _mm256_storeu_si256(reinterpret_cast<__m256i *>(buffer), reg);
      }
      else if constexpr(std::is_same_v<RegType, __m512i>)
      {
        _mm512_storeu_si512(reinterpret_cast<void *>(buffer), reg);
      }

      for(size_t i = 0; i < num_lanes; ++i)
      {
        result[i] = static_cast<uint16_t>(buffer[i]);
      }
    }
    else
    {
      // Direct store for 16, 32, and 64-bit lanes
      if constexpr(std::is_same_v<RegType, __m64>)
      {
        _mm_stream_pi(reinterpret_cast<__m64 *>(result.data()), reg);
      }
      else if constexpr(std::is_same_v<RegType, __m128i>)
      {
        _mm_storeu_si128(reinterpret_cast<__m128i *>(result.data()), reg);
      }
      else if constexpr(std::is_same_v<RegType, __m256i>)
      {
        _mm256_storeu_si256(reinterpret_cast<__m256i *>(result.data()), reg);
      }
      else if constexpr(std::is_same_v<RegType, __m512i>)
      {
        _mm512_storeu_si512(reinterpret_cast<void *>(result.data()), reg);
      }
    }

    return result;
  }

  template <>
  uint32_t WriteCharsToPtrFowardReturnLength<uint64_t>(char *__restrict__ buff, const uint64_t &input) noexcept
  {
    // clang-format off
    static const uint128_t table[64] = { ENTRY(1, 0), ENTRY(1, 0), ENTRY(1, 0), ENTRY(2, 10), ENTRY(2, 10), ENTRY(2, 10), ENTRY(3, 100), ENTRY(3, 100), ENTRY(3, 100), ENTRY(4, 1000), ENTRY(4, 1000), ENTRY(4, 1000), ENTRY(4, 1000), ENTRY(5, 10000), ENTRY(5, 10000), ENTRY(5, 10000), ENTRY(6, 100000), ENTRY(6, 100000), ENTRY(6, 100000), ENTRY(7, 1000000), ENTRY(7, 1000000), ENTRY(7, 1000000), ENTRY(7, 1000000), ENTRY(8, 10000000), ENTRY(8, 10000000), ENTRY(8, 10000000), ENTRY(9, 100000000), ENTRY(9, 100000000), ENTRY(9, 100000000), ENTRY(10, 1000000000), ENTRY(10, 1000000000), ENTRY(10, 1000000000), ENTRY(10, 1000000000), ENTRY(11, 10000000000ULL), ENTRY(11, 10000000000ULL), ENTRY(11, 10000000000ULL), ENTRY(12, 100000000000ULL), ENTRY(12, 100000000000ULL), ENTRY(12, 100000000000ULL), ENTRY(13, 1000000000000ULL), ENTRY(13, 1000000000000ULL), ENTRY(13, 1000000000000ULL), ENTRY(13, 1000000000000ULL), ENTRY(14, 10000000000000ULL), ENTRY(14, 10000000000000ULL), ENTRY(14, 10000000000000ULL), ENTRY(15, 100000000000000ULL), ENTRY(15, 100000000000000ULL), ENTRY(15, 100000000000000ULL), ENTRY(16, 1000000000000000ULL), ENTRY(16, 1000000000000000ULL), ENTRY(16, 1000000000000000ULL), ENTRY(16, 1000000000000000ULL), ENTRY(17, 10000000000000000ULL), ENTRY(17, 10000000000000000ULL), ENTRY(17, 10000000000000000ULL), ENTRY(18, 100000000000000000ULL), ENTRY(18, 100000000000000000ULL), ENTRY(18, 100000000000000000ULL), ENTRY(19, 1000000000000000000ULL), ENTRY(19, 1000000000000000000ULL), ENTRY(19, 1000000000000000000ULL), ENTRY(19, 1000000000000000000ULL), ENTRY(20, 10000000000000000000ULL) };
    // clang-format on

    const __m128i M_MAGICS_u16 = _mm_set1_epi64x(0x0000'199A'A3D8'8313);
    const __m128i M_SHIFTS_u16 = _mm_set1_epi64x(0x0000'0000'0006'0009);

    const __m128i INDICES = _mm_setr_epi8(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);

    const uint64_t res_1 = ((uint128_t)input * 0x39A5652FB1137857ULL) >> 115U;
    const uint64_t prod_2 = ((uint128_t)input * 0x232F33025BD42233ULL) >> 101U;
    const uint64_t prod_3 = ((uint128_t)input * 0xABCC77118461CEFDULL) >> 90U;
    const uint64_t prod_4 = ((uint128_t)input * 0x346DC5D63886594BULL) >> 75U;

    const uint64_t res_2 = prod_2 - (res_1 * 10'000);
    const uint64_t res_3 = prod_3 - (prod_2 * 10'000);
    const uint64_t res_4 = prod_4 - (prod_3 * 10'000);
    const uint64_t res_5 = input - (prod_4 * 10'000);

    const __m128i u64_res_1 = _mm_set1_epi16(static_cast<uint16_t>(res_1));
    const __m128i u64_res_2 = _mm_set1_epi16(static_cast<uint16_t>(res_2));
    const __m128i u64_res_3 = _mm_set1_epi16(static_cast<uint16_t>(res_3));
    const __m128i u64_res_4 = _mm_set1_epi16(static_cast<uint16_t>(res_4));
    const __m128i u64_res_5 = _mm_set1_epi16(static_cast<uint16_t>(res_5));

    const unsigned comp = 63U - __builtin_clzll(input | 1ULL);
    const unsigned len = ((uint128_t)input + table[comp]) >> 64U;

    const unsigned lead_z_top = 20U - len;
    const unsigned lead_z_bot = (lead_z_top < 16U) ? 0U : lead_z_top;
    const unsigned bot_offset = (len < 4U) ? 0U : 16U - lead_z_top;

    const __m128i u16_orig_12 = _mm_blend_epi32(u64_res_1, u64_res_2, 0b1100);
    const __m128i u16_orig_34 = _mm_blend_epi32(u64_res_3, u64_res_4, 0b1100);

    const __m128i u16_prod_12 = _mm_mulhi_epu16(u16_orig_12, M_MAGICS_u16);
    const __m128i u16_prod_34 = _mm_mulhi_epu16(u16_orig_34, M_MAGICS_u16);
    const __m128i u16_prod_5 = _mm_mulhi_epu16(u64_res_5, M_MAGICS_u16);

    const __m128i u16_shifted_12 = _mm_srlv_epi16(u16_prod_12, M_SHIFTS_u16);
    const __m128i u16_shifted_34 = _mm_srlv_epi16(u16_prod_34, M_SHIFTS_u16);
    const __m128i u16_shifted_5 = _mm_srlv_epi16(u16_prod_5, M_SHIFTS_u16);

    const __m128i u16_shifted_12_x8 = _mm_slli_epi16(u16_shifted_12, 3);
    const __m128i u16_shifted_34_x8 = _mm_slli_epi16(u16_shifted_34, 3);
    const __m128i u16_shifted_5_x8 = _mm_slli_epi16(u16_shifted_5, 3);
    const __m128i u16_shifted_12_x2 = _mm_slli_epi16(u16_shifted_12, 1);
    const __m128i u16_shifted_34_x2 = _mm_slli_epi16(u16_shifted_34, 1);
    const __m128i u16_shifted_5_x2 = _mm_slli_epi16(u16_shifted_5, 1);

    const __m128i u16_shf_x10_12 = _mm_add_epi16(u16_shifted_12_x8, u16_shifted_12_x2);
    const __m128i u16_shf_x10_34 = _mm_add_epi16(u16_shifted_34_x8, u16_shifted_34_x2);
    const __m128i u16_shf_x10_5 = _mm_add_epi16(u16_shifted_5_x8, u16_shifted_5_x2);

    const __m128i ZERO_CHAR = _mm_set1_epi8('0');
    const __m128i LEAZ_Z_TOP_MID_LANES = _mm_set1_epi8(lead_z_top);
    const __m128i LEAZ_Z_BOT_LANES = _mm_set1_epi8(lead_z_bot);

    const __m128i u16_slided_12 = _mm_slli_si128(u16_shf_x10_12, 2);
    const __m128i u16_slided_34 = _mm_slli_si128(u16_shf_x10_34, 2);
    const __m128i u16_slided_5 = _mm_slli_si128(u16_shf_x10_5, 2);
    const __m128i u16_to_sub_12 = _mm_blend_epi16(u16_shifted_12, u16_orig_12, 0b1000'1000);
    const __m128i u16_to_sub_34 = _mm_blend_epi16(u16_shifted_34, u16_orig_34, 0b1000'1000);
    const __m128i u16_to_sub_5 = _mm_blend_epi16(u16_shifted_5, u64_res_5, 0b1000'1000);

    const __m128i u16_res_12 = _mm_sub_epi16(u16_to_sub_12, u16_slided_12);
    const __m128i u16_res_34 = _mm_sub_epi16(u16_to_sub_34, u16_slided_34);
    const __m128i u16_res_5 = _mm_sub_epi16(u16_to_sub_5, u16_slided_5);

    const __m128i u8_res_packed_top = _mm_packus_epi16(u16_res_12, u16_res_34);
    const __m128i u8_res_packed_bot = _mm_packus_epi16(u16_res_5, u16_res_5);

    const __m128i final_indices_top_mid = _mm_add_epi8(INDICES, LEAZ_Z_TOP_MID_LANES);
    const __m128i final_indices_bot = _mm_add_epi8(INDICES, LEAZ_Z_BOT_LANES);

    const __m128i ascii_vec_top = _mm_add_epi8(u8_res_packed_top, ZERO_CHAR);
    const __m128i ascii_vec_bot = _mm_add_epi8(u8_res_packed_bot, ZERO_CHAR);

    const __m128i output_chars_top = _mm_shuffle_epi8(ascii_vec_top, final_indices_top_mid);
    const __m128i output_chars_bot = _mm_shuffle_epi8(ascii_vec_bot, final_indices_bot);

    _mm_storeu_si128(reinterpret_cast<__m128i *>(buff), output_chars_top);

    _mm_storeu_si32(reinterpret_cast<void *>(buff + bot_offset), output_chars_bot);

    return len;
  }

  template <>
  uint32_t WriteCharsToPtrFowardReturnLength<uint32_t>(char *__restrict__ buff, const uint32_t &input) noexcept
  {
    static const uint64_t table[] = { 4294967296ULL,  8589934582ULL,  8589934582ULL,  8589934582ULL,  12884901788ULL, 12884901788ULL, 12884901788ULL, 17179868184ULL,
                                      17179868184ULL, 17179868184ULL, 21474826480ULL, 21474826480ULL, 21474826480ULL, 21474826480ULL, 25769703776ULL, 25769703776ULL,
                                      25769703776ULL, 30063771072ULL, 30063771072ULL, 30063771072ULL, 34349738368ULL, 34349738368ULL, 34349738368ULL, 34349738368ULL,
                                      38554705664ULL, 38554705664ULL, 38554705664ULL, 41949672960ULL, 41949672960ULL, 41949672960ULL, 42949672960ULL, 42949672960ULL };

    const __m512i val = _mm512_set1_epi32(input);
    // clang-format off
    const __m512i M_MAGIC_10_0 = _mm512_setr_epi32(0x12E0BE83U, 0x5798EE24U, 0xAD7F29ACU, 0x0C6F7A0CU, 0x4F8B588FU, 0xA36E2EB2U, 0x0624DD30U, 0x47AE147BU, 0x9999999AU, 0, 0, 0, 0, 0, 0, 0);
    // clang-format on
    const __m512i M_SHIFTS_10_0 = _mm512_setr_epi32(29, 26, 23, 19, 16, 13, 9, 6, 3, 0, 0, 0, 0, 0, 0, 0);
    const __m128i INDICES = _mm_setr_epi8(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);

    const __m512i prod = umul_hi_32x16(val, M_MAGIC_10_0);

    const unsigned comp = 31U - __builtin_clz(input | 1U);
    const unsigned len = (input + table[comp]) >> 32U;
    const unsigned lead_z = 10U - len;

    const __m512i n_sub_t = _mm512_sub_epi32(val, prod);
    const __m512i n_sub_t_shf = _mm512_srli_epi32(n_sub_t, 1);
    const __m512i n_sub_t_shf_add_t = _mm512_add_epi32(n_sub_t_shf, prod);
    const __m512i shifted_32 = _mm512_srlv_epi32(n_sub_t_shf_add_t, M_SHIFTS_10_0);

    const __m512i res_vec = _mm512_mask_blend_epi32(0x0200, shifted_32, val);

    const __m128i LEAD_Z_LANES = _mm_set1_epi8(lead_z);
    const __m128i ASCII_ZERO = _mm_set1_epi8('0');

    const __m512i res_times_2 = _mm512_slli_epi32(res_vec, 1);
    const __m512i res_times_8 = _mm512_slli_epi32(res_vec, 3);

    const __m512i res_times_10 = _mm512_add_epi32(res_times_8, res_times_2);

    const __m512i permuted = _mm512_maskz_alignr_epi32(0xFFFE, res_times_10, res_times_10, 15);

    const __m512i full_res = _mm512_sub_epi32(res_vec, permuted);

    const __m128i u8_packed = _mm512_cvtepi32_epi8(full_res);

    const __m128i ascii_vec = _mm_add_epi8(u8_packed, ASCII_ZERO);
    const __m128i final_indices = _mm_add_epi8(INDICES, LEAD_Z_LANES);
    const __m128i output_chars = _mm_shuffle_epi8(ascii_vec, final_indices);

    _mm_storeu_si64(reinterpret_cast<void *>(buff), output_chars);

    const __m128i top_top = _mm_srli_si128(output_chars, 8);

    _mm_storeu_si16(reinterpret_cast<void *>(buff + 8), top_top);

    return len;
  }

  template <>
  uint32_t WriteCharsToPtrFowardReturnLength<uint16_t>(char *__restrict__ buff, const uint16_t &input) noexcept
  {
    static const uint32_t table[] = { 65536, 65536, 65536, 131062, 131072, 131072, 196508, 196608, 196608, 261144, 262144, 262144, 262144, 317680, 327680, 327680 };

    const uint64_t u8_acii_zero = 0x3030'3030'3030'3030ULL;

    const uint64_t dig_1 = (uint64_t)input * 0xD1B71759U >> 45U;
    const unsigned prod_2 = (uint64_t)input * 0x10624DD3U >> 38U;
    const unsigned prod_3 = (uint64_t)input * 0x51EB851FU >> 37U;
    const unsigned prod_4 = (uint64_t)input * 0xCCCCCCCDU >> 35U;

    const unsigned comp = 31U - __builtin_clz(input | 1U);
    const unsigned len = (input + table[comp]) >> 16U;
    const unsigned lead_z = (5U - len) << 3U;

    const uint64_t dig_2 = prod_2 - ((dig_1 << 3U) + (dig_1 << 1U));
    const uint64_t dig_3 = prod_3 - ((prod_2 << 3U) + (prod_2 << 1U));
    const uint64_t dig_4 = prod_4 - ((prod_3 << 3U) + (prod_3 << 1U));
    const uint64_t dig_5 = input - ((prod_4 << 3U) + (prod_4 << 1U));

    const uint64_t u8_res = dig_1 | dig_2 << 8U | dig_3 << 16U | dig_4 << 24U | dig_5 << 32U;

    const uint64_t u8_res_shf = u8_res >> lead_z;
    const uint64_t u8_chars = u8_res_shf + u8_acii_zero;

    // Final Store (8 bytes)
    *reinterpret_cast<uint64_t *>(buff) = u8_chars;

    return len;
  }

  template <>
  uint32_t WriteCharsToPtrFowardReturnLength<uint8_t>(char *__restrict__ buff, const uint8_t &input) noexcept
  {
    const unsigned u8_acii_zero = 0x3030'3030U;

    const unsigned dig_1 = (unsigned)input * 0x0290U >> 16U;
    const unsigned prod_2 = (unsigned)input * 0x199A >> 16U;

    const unsigned len = (input < 10) ? 1U : (input < 100) ? 2U : 3U;
    const unsigned lead_z = (3U - len) << 3U;

    const unsigned dig_2 = prod_2 - ((dig_1 << 3U) + (dig_1 << 1U));
    const unsigned dig_3 = input - ((prod_2 << 3U) + (prod_2 << 1U));

    const unsigned u8_packed = dig_1 | dig_2 << 8U | dig_3 << 16U;

    const unsigned u8_res_shf = u8_packed >> lead_z;
    const unsigned u8_chars = u8_res_shf + u8_acii_zero;

    *reinterpret_cast<unsigned *>(buff) = u8_chars;

    return len;
  }

#elif defined(__AVX2__)
  /**
   * @brief Maps a bit-width to the appropriate unsigned integer type.
   * Special case: 8-bit lanes are mapped to uint16_t to prevent debugger ASCII rendering.
   */
  template <size_t BitWidth>
  struct LaneType
  {
    using type = std::conditional_t<BitWidth == 8, uint16_t,
                                    std::conditional_t<BitWidth == 16, uint16_t, std::conditional_t<BitWidth == 32, uint32_t, std::conditional_t<BitWidth == 64, uint64_t, void>>>>;
  };
  /**
   * @brief Extracts lanes from __m128i, __m256i, or __m512i registers into a std::array.
   * @tparam BitWidth The size of the lane in bits (8, 16, 32, 64).
   * @param reg The SIMD register to extract from.
   */
  template <size_t BitWidth, typename RegType>
  inline auto extract_lanes(const RegType &reg)
  {
    constexpr size_t total_bytes = sizeof(RegType);
    constexpr size_t num_lanes = (total_bytes * 8) / BitWidth;
    using T = typename LaneType<BitWidth>::type;

    std::array<T, num_lanes> result;

    if constexpr(BitWidth == 8)
    {
      // Intermediary buffer to avoid debugger ASCII char issues
      alignas(RegType) uint8_t buffer[total_bytes];

      if constexpr(std::is_same_v<RegType, __m64>)
      {
        _mm_stream_pi(reinterpret_cast<__m64 *>(buffer), reg);
      }
      else if constexpr(std::is_same_v<RegType, __m128i>)
      {
        _mm_storeu_si128(reinterpret_cast<__m128i *>(buffer), reg);
      }
      else if constexpr(std::is_same_v<RegType, __m256i>)
      {
        _mm256_storeu_si256(reinterpret_cast<__m256i *>(buffer), reg);
      }

      for(size_t i = 0; i < num_lanes; ++i)
      {
        result[i] = static_cast<uint16_t>(buffer[i]);
      }
    }
    else
    {
      // Direct store for 16, 32, and 64-bit lanes
      if constexpr(std::is_same_v<RegType, __m64>)
      {
        _mm_stream_pi(reinterpret_cast<__m64 *>(result.data()), reg);
      }
      else if constexpr(std::is_same_v<RegType, __m128i>)
      {
        _mm_storeu_si128(reinterpret_cast<__m128i *>(result.data()), reg);
      }
      else if constexpr(std::is_same_v<RegType, __m256i>)
      {
        _mm256_storeu_si256(reinterpret_cast<__m256i *>(result.data()), reg);
      }
    }

    return result;
  }

  // Compute high 32 bits of (a[i] * b[i]) for 8x uint32_t
  static inline __m256i _mm256_mulhi_epu32(const __m256i &a, const __m256i &b) noexcept
  {
    // Even lanes: 0,2,4,6
    const __m256i prod_even = _mm256_mul_epu32(a, b); // 64-bit results
    const __m256i hi_even = _mm256_srli_epi64(prod_even, 32);

    // Odd lanes: shift inputs right by 32 bits
    const __m256i a_odd = _mm256_srli_epi64(a, 32);
    const __m256i b_odd = _mm256_srli_epi64(b, 32);
    const __m256i prod_odd = _mm256_mul_epu32(a_odd, b_odd);
    const __m256i hi_odd = _mm256_srli_epi64(prod_odd, 32);

    return _mm256_blend_epi32(_mm256_slli_epi64(hi_odd, 32), hi_even, 0b01010101);
  }

  template <>
  uint32_t WriteCharsToPtrFowardReturnLength<uint64_t>(char *__restrict__ buff, const uint64_t &input) noexcept
  {
    // clang-format off
    static const uint128_t table[64] = { ENTRY(1, 0), ENTRY(1, 0), ENTRY(1, 0), ENTRY(2, 10), ENTRY(2, 10), ENTRY(2, 10), ENTRY(3, 100), ENTRY(3, 100), ENTRY(3, 100), ENTRY(4, 1000), ENTRY(4, 1000), ENTRY(4, 1000), ENTRY(4, 1000), ENTRY(5, 10000), ENTRY(5, 10000), ENTRY(5, 10000), ENTRY(6, 100000), ENTRY(6, 100000), ENTRY(6, 100000), ENTRY(7, 1000000), ENTRY(7, 1000000), ENTRY(7, 1000000), ENTRY(7, 1000000), ENTRY(8, 10000000), ENTRY(8, 10000000), ENTRY(8, 10000000), ENTRY(9, 100000000), ENTRY(9, 100000000), ENTRY(9, 100000000), ENTRY(10, 1000000000), ENTRY(10, 1000000000), ENTRY(10, 1000000000), ENTRY(10, 1000000000), ENTRY(11, 10000000000ULL), ENTRY(11, 10000000000ULL), ENTRY(11, 10000000000ULL), ENTRY(12, 100000000000ULL), ENTRY(12, 100000000000ULL), ENTRY(12, 100000000000ULL), ENTRY(13, 1000000000000ULL), ENTRY(13, 1000000000000ULL), ENTRY(13, 1000000000000ULL), ENTRY(13, 1000000000000ULL), ENTRY(14, 10000000000000ULL), ENTRY(14, 10000000000000ULL), ENTRY(14, 10000000000000ULL), ENTRY(15, 100000000000000ULL), ENTRY(15, 100000000000000ULL), ENTRY(15, 100000000000000ULL), ENTRY(16, 1000000000000000ULL), ENTRY(16, 1000000000000000ULL), ENTRY(16, 1000000000000000ULL), ENTRY(16, 1000000000000000ULL), ENTRY(17, 10000000000000000ULL), ENTRY(17, 10000000000000000ULL), ENTRY(17, 10000000000000000ULL), ENTRY(18, 100000000000000000ULL), ENTRY(18, 100000000000000000ULL), ENTRY(18, 100000000000000000ULL), ENTRY(19, 1000000000000000000ULL), ENTRY(19, 1000000000000000000ULL), ENTRY(19, 1000000000000000000ULL), ENTRY(19, 1000000000000000000ULL), ENTRY(20, 10000000000000000000ULL) };
    // clang-format on
    const __m256i M_MAGIC_u64 = _mm256_setr_epi64x(0x431BDE83ULL, 0xD1B71759ULL, 0x51EB851FULL, 0);
    const __m128i M_MAGIC_u16 = _mm_setr_epi32(33555, 41944, 6554, 0);
    const __m256i M_SHIFTS_u64 = _mm256_setr_epi64x(50, 45, 37, 0);
    const __m128i M_SHIFTS_u16 = _mm_setr_epi32(9, 6, 0, 0);
    const __m256i PERMUTE_SHF_64 = _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7);
    const __m128i INDICES = _mm_setr_epi8(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);

    uint64_t fir_8 = input, mid_8, las_4;
    Helpers::Math::Magic::Modulo::mod_by_10_pow_n_void<12>(fir_8, mid_8);
    Helpers::Math::Magic::Modulo::mod_by_10_pow_n_void<4>(mid_8, las_4);

    const __m256i VAL_U32_TOP = _mm256_set1_epi32(static_cast<uint32_t>(fir_8));
    const __m256i VAL_U32_MID = _mm256_set1_epi32(static_cast<uint32_t>(mid_8));
    const __m128i VAL_U32_BOT = _mm_set1_epi32(static_cast<uint32_t>(las_4));

    const __m256i prod_u32_top = _mm256_mul_epu32(VAL_U32_TOP, M_MAGIC_u64);
    const __m256i prod_u32_mid = _mm256_mul_epu32(VAL_U32_MID, M_MAGIC_u64);
    const __m128i prod_u32_bot = _mm_mulhi_epu16(VAL_U32_BOT, M_MAGIC_u16);

    const unsigned comp = 63U - __builtin_clzll(input | 1ULL);
    const unsigned len = ((uint128_t)input + table[comp]) >> 64U;

    const unsigned lead_z_top = 20U - len;
    const unsigned lead_z_bot = (lead_z_top < 16U) ? 0U : lead_z_top;
    const unsigned bot_offset = (len < 4U) ? 0U : 16U - lead_z_top;

    const __m256i shifted_u32_top = _mm256_srlv_epi64(prod_u32_top, M_SHIFTS_u64);
    const __m256i shifted_u32_mid = _mm256_srlv_epi64(prod_u32_mid, M_SHIFTS_u64);
    const __m128i shifted_u32_bot = _mm_srlv_epi32(prod_u32_bot, M_SHIFTS_u16);

    const __m128i permuted_u32_top = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(shifted_u32_top, PERMUTE_SHF_64));
    const __m128i permuted_u32_mid = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(shifted_u32_mid, PERMUTE_SHF_64));

    const __m128i permuted_u32_tosub_top = _mm_blend_epi32(permuted_u32_top, _mm256_castsi256_si128(VAL_U32_TOP), 0b10'00);
    const __m128i permuted_u32_tosub_mid = _mm_blend_epi32(permuted_u32_mid, _mm256_castsi256_si128(VAL_U32_MID), 0b10'00);
    const __m128i permuted_u32_tosub_bot = _mm_blend_epi32(shifted_u32_bot, VAL_U32_BOT, 0b10'00);

    const __m128i permuted_u32_top_x64 = _mm_slli_epi64(permuted_u32_top, 6);
    const __m128i permuted_u32_top_x32 = _mm_slli_epi64(permuted_u32_top, 5);
    const __m128i permuted_u32_mid_x64 = _mm_slli_epi64(permuted_u32_mid, 6);
    const __m128i permuted_u32_mid_x32 = _mm_slli_epi64(permuted_u32_mid, 5);

    const __m128i permuted_u32_bot_x8 = _mm_slli_epi32(shifted_u32_bot, 3);
    const __m128i permuted_u32_bot_x2 = _mm_slli_epi32(shifted_u32_bot, 1);

    const __m128i permuted_u32_top_x4 = _mm_slli_epi64(permuted_u32_top, 2);
    const __m128i permuted_u32_mid_x4 = _mm_slli_epi64(permuted_u32_mid, 2);
    const __m128i permuted_u32_top_x96 = _mm_add_epi64(permuted_u32_top_x64, permuted_u32_top_x32);
    const __m128i permuted_u32_mid_x96 = _mm_add_epi64(permuted_u32_mid_x64, permuted_u32_mid_x32);

    const __m128i permuted_u32_top_x100 = _mm_add_epi64(permuted_u32_top_x96, permuted_u32_top_x4);
    const __m128i permuted_u32_mid_x100 = _mm_add_epi64(permuted_u32_mid_x96, permuted_u32_mid_x4);
    const __m128i permuted_u32_bot_x10 = _mm_add_epi64(permuted_u32_bot_x8, permuted_u32_bot_x2);

    const __m128i F_6554 = _mm_set1_epi16(6554);

    const __m128i slided_u32_top = _mm_slli_si128(permuted_u32_top_x100, 4);
    const __m128i slided_u32_mid = _mm_slli_si128(permuted_u32_mid_x100, 4);
    const __m128i slided_u32_bot = _mm_slli_si128(permuted_u32_bot_x10, 4);

    const __m128i res_u32_top = _mm_sub_epi64(permuted_u32_tosub_top, slided_u32_top);
    const __m128i res_u32_mid = _mm_sub_epi64(permuted_u32_tosub_mid, slided_u32_mid);
    const __m128i res_u32_bot = _mm_sub_epi64(permuted_u32_tosub_bot, slided_u32_bot);

    const __m128i res_u32_top_shf_16 = _mm_slli_epi64(res_u32_top, 16);
    const __m128i res_u32_mid_shf_16 = _mm_slli_epi64(res_u32_mid, 16);
    const __m128i res_u32_packed_bot = _mm_packus_epi32(res_u32_bot, res_u32_bot);

    const __m128i res_u16_packed_top = _mm_or_si128(res_u32_top_shf_16, res_u32_top);
    const __m128i res_u16_packed_mid = _mm_or_si128(res_u32_mid_shf_16, res_u32_mid);

    const __m128i res_u16_prod_top = _mm_mulhi_epu16(res_u16_packed_top, F_6554);
    const __m128i res_u16_prod_mid = _mm_mulhi_epu16(res_u16_packed_mid, F_6554);

    const __m128i res_u16_prod_top_x8 = _mm_slli_epi16(res_u16_prod_top, 3);
    const __m128i res_u16_prod_mid_x8 = _mm_slli_epi16(res_u16_prod_mid, 3);
    const __m128i res_u16_prod_top_x2 = _mm_slli_epi16(res_u16_prod_top, 1);
    const __m128i res_u16_prod_mid_x2 = _mm_slli_epi16(res_u16_prod_mid, 1);

    const __m128i res_u16_prod_top_x10 = _mm_add_epi16(res_u16_prod_top_x8, res_u16_prod_top_x2);
    const __m128i res_u16_prod_mid_x10 = _mm_add_epi16(res_u16_prod_mid_x8, res_u16_prod_mid_x2);
    const __m128i res_u16_bot = _mm_packus_epi16(res_u32_packed_bot, res_u32_packed_bot);

    const __m128i ZERO_NUMS = _mm_setzero_si128();
    const __m128i ZERO_CHAR = _mm_set1_epi8('0');
    const __m128i LEAZ_Z_TOP_MID_LANES = _mm_set1_epi8(lead_z_top);
    const __m128i LEAZ_Z_BOT_LANES = _mm_set1_epi8(lead_z_bot);

    const __m128i res_u16_blend_top = _mm_blend_epi16(res_u16_prod_top_x10, ZERO_NUMS, 0b0101'0101);
    const __m128i res_u16_blend_mid = _mm_blend_epi16(res_u16_prod_mid_x10, ZERO_NUMS, 0b0101'0101);

    const __m128i res_u16_tosub_top = _mm_blend_epi16(res_u16_prod_top, res_u16_packed_top, 0b1010'1010);
    const __m128i res_u16_tosub_mid = _mm_blend_epi16(res_u16_prod_mid, res_u16_packed_mid, 0b1010'1010);

    const __m128i res_u16_top = _mm_sub_epi16(res_u16_tosub_top, res_u16_blend_top);
    const __m128i res_u16_mid = _mm_sub_epi16(res_u16_tosub_mid, res_u16_blend_mid);

    const __m128i final_indices_top_mid = _mm_add_epi8(INDICES, LEAZ_Z_TOP_MID_LANES);
    const __m128i final_indices_bot = _mm_add_epi8(INDICES, LEAZ_Z_BOT_LANES);

    const __m128i res_u8_packed_top = _mm_packus_epi16(res_u16_top, res_u16_mid);

    const __m128i ascii_vec_top = _mm_add_epi8(res_u8_packed_top, ZERO_CHAR);
    const __m128i ascii_vec_bot = _mm_add_epi8(res_u16_bot, ZERO_CHAR);

    const __m128i output_chars_top = _mm_shuffle_epi8(ascii_vec_top, final_indices_top_mid);
    const __m128i output_chars_bot = _mm_shuffle_epi8(ascii_vec_bot, final_indices_bot);

    _mm_storeu_si128(reinterpret_cast<__m128i *>(buff), output_chars_top);

    _mm_storeu_si32(reinterpret_cast<void *>(buff + bot_offset), output_chars_bot);

    return len;
  }

  template <>
  uint32_t WriteCharsToPtrFowardReturnLength<uint32_t>(char *__restrict__ buff, const uint32_t &input) noexcept
  {
    static const uint64_t table[] = { 4294967296ULL,  8589934582ULL,  8589934582ULL,  8589934582ULL,  12884901788ULL, 12884901788ULL, 12884901788ULL, 17179868184ULL,
                                      17179868184ULL, 17179868184ULL, 21474826480ULL, 21474826480ULL, 21474826480ULL, 21474826480ULL, 25769703776ULL, 25769703776ULL,
                                      25769703776ULL, 30063771072ULL, 30063771072ULL, 30063771072ULL, 34349738368ULL, 34349738368ULL, 34349738368ULL, 34349738368ULL,
                                      38554705664ULL, 38554705664ULL, 38554705664ULL, 41949672960ULL, 41949672960ULL, 41949672960ULL, 42949672960ULL, 42949672960ULL };

    const __m256i VAL = _mm256_set1_epi32(input);

    const __m256i M_MAGIC_u64 = { 0x55E63B89ULL, 0x431BDE83ULL, 0xD1B71759ULL, 0x51EB851FULL };
    const __m256i M_SHIFTS_u64 = { 57, 50, 45, 37 };

    const __m256i PERMUTE_SHF_64 = _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7);
    const __m128i INDICES = _mm_setr_epi8(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);

    const __m256i prod = _mm256_mul_epu32(VAL, M_MAGIC_u64);

    const unsigned comp = 31U - __builtin_clz(input | 1U);
    const unsigned len = (input + table[comp]) >> 32U;
    const unsigned lead_z = 10U - len;

    const __m256i shifted = _mm256_srlv_epi64(prod, M_SHIFTS_u64);
    const __m128i shifted_64 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(shifted, PERMUTE_SHF_64));

    const __m128i shifted_64_x_64 = _mm_slli_epi64(shifted_64, 6);
    const __m128i shifted_64_x_32 = _mm_slli_epi64(shifted_64, 5);

    const __m128i shifted_64_x_4 = _mm_slli_epi64(shifted_64, 2);
    const __m128i shifted_64_x_96 = _mm_add_epi64(shifted_64_x_64, shifted_64_x_32);

    const __m128i shifted_64_x_100 = _mm_add_epi64(shifted_64_x_96, shifted_64_x_4);

    const __m128i top_lanes = _mm_slli_si128(shifted_64_x_100, 4);
    const __m128i bot_lanes = _mm_shuffle_epi32(shifted_64_x_100, _MM_SHUFFLE(0, 1, 2, 3));

    const __m128i F_6554 = _mm_set1_epi16(6554);

    const __m128i res_u64_top = _mm_sub_epi64(shifted_64, top_lanes);
    const __m128i res_u64_bot = _mm_sub_epi64(_mm256_castsi256_si128(VAL), bot_lanes);

    const __m128i shifted_top_16 = _mm_slli_epi64(res_u64_top, 16);
    const __m128i shifted_bot_16 = _mm_slli_epi64(res_u64_bot, 16);

    const __m128i res_packed_top = _mm_or_si128(shifted_top_16, res_u64_top);
    const __m128i res_packed_bot = _mm_or_si128(shifted_bot_16, res_u64_bot);

    const __m128i res_prod_top = _mm_mulhi_epu16(res_packed_top, F_6554);
    const __m128i res_prod_bot = _mm_mulhi_epu16(res_packed_bot, F_6554);

    const __m128i res_shifted_top_x8 = _mm_slli_epi16(res_prod_top, 3);
    const __m128i res_shifted_bot_x8 = _mm_slli_epi16(res_prod_bot, 3);
    const __m128i res_shifted_top_x2 = _mm_slli_epi16(res_prod_top, 1);
    const __m128i res_shifted_bot_x2 = _mm_slli_epi16(res_prod_bot, 1);

    const __m128i res_shifted_top_x10 = _mm_add_epi16(res_shifted_top_x8, res_shifted_top_x2);
    const __m128i res_shifted_bot_x10 = _mm_add_epi16(res_shifted_bot_x8, res_shifted_bot_x2);

    const __m128i ZERO_NUMS = _mm_setzero_si128();
    const __m128i ZERO_CHAR = _mm_set1_epi8('0');
    const __m128i LEAD_Z_LANES = _mm_set1_epi8(lead_z);

    const __m128i res_shf_blen_top = _mm_blend_epi16(res_shifted_top_x10, ZERO_NUMS, 0b0101'0101);
    const __m128i res_shf_blen_bot = _mm_blend_epi16(res_shifted_bot_x10, ZERO_NUMS, 0b0101'0101);
    const __m128i res_to_sub_top = _mm_blend_epi16(res_prod_top, res_packed_top, 0b1010'1010);
    const __m128i res_to_sub_bot = _mm_blend_epi16(res_prod_bot, res_packed_bot, 0b1010'1010);
    const __m128i res_com_top = _mm_sub_epi16(res_to_sub_top, res_shf_blen_top);
    const __m128i res_com_bot = _mm_sub_epi16(res_to_sub_bot, res_shf_blen_bot);

    const __m128i trunc_u8 = _mm_packus_epi16(res_com_top, res_com_bot);

    const __m128i ascii_vec = _mm_add_epi8(trunc_u8, ZERO_CHAR);
    const __m128i final_indices = _mm_add_epi8(INDICES, LEAD_Z_LANES);
    const __m128i output_chars = _mm_shuffle_epi8(ascii_vec, final_indices);

    _mm_storeu_si64(reinterpret_cast<void *>(buff), output_chars);

    const __m128i top_top = _mm_srli_si128(output_chars, 8);

    _mm_storeu_si16(reinterpret_cast<void *>(buff + 8), top_top);

    return len;
  }

  template <>
  uint32_t WriteCharsToPtrFowardReturnLength<uint16_t>(char *__restrict__ buff, const uint16_t &input) noexcept
  {
    static const uint32_t table[] = { 65536, 65536, 65536, 131062, 131072, 131072, 196508, 196608, 196608, 261144, 262144, 262144, 262144, 317680, 327680, 327680 };

    const __m128i C_VAL = _mm_set1_epi32(input);

    const __m128i C_ZEROS = _mm_set1_epi8('0');
    const __m128i M_MAGIC_U16 = _mm_setr_epi32(0xA36F, 0x625, 0x47AF, 0x999A);
    const __m128i M_SHIFTS_U16 = _mm_setr_epi32(13, 9, 6, 3);

    const __m128i u16_prod = _mm_mulhi_epu16(C_VAL, M_MAGIC_U16);
    const __m128i u16_sub = _mm_sub_epi32(C_VAL, u16_prod);
    const __m128i u16_shf = _mm_srli_epi16(u16_sub, 1);
    const __m128i u16_add = _mm_add_epi16(u16_shf, u16_prod);

    const __m128i u16_32_div = _mm_srlv_epi32(u16_add, M_SHIFTS_U16);
    const __m128i u16_div = _mm_packus_epi32(u16_32_div, u16_32_div);

    const unsigned comp = 31U - __builtin_clz(input | 1U);
    const unsigned len = (input + table[comp]) >> 16U;
    const unsigned lead_z = (5U - len) << 3U;

    // Pack back to 16-bit and insert the original input into lane 4
    const __m128i u16_blend = _mm_blend_epi32(u16_div, C_VAL, 0b1100);

    const __m128i u16_div_x8 = _mm_slli_epi16(u16_blend, 3);
    const __m128i u16_div_x2 = _mm_slli_epi16(u16_blend, 1);

    // Digit Extraction logic
    const __m128i u16_div_x10 = _mm_add_epi16(u16_div_x8, u16_div_x2);

    const __m128i u16_div_x10_slided = _mm_slli_si128(u16_div_x10, 2);
    const __m128i u16_res = _mm_sub_epi16(u16_blend, u16_div_x10_slided);

    const __m128i u8_res = _mm_packus_epi16(u16_res, u16_res);

    // Table Lookup conversion to ASCII
    const __m128i u8_res_shf = _mm_srli_epi64(u8_res, lead_z);
    const __m128i u8_chars = _mm_add_epi8(u8_res_shf, C_ZEROS);

    // Final Store (8 bytes)
    _mm_storeu_si64(static_cast<void *>(buff), u8_chars);

    return len;
  }

  template <>
  uint32_t WriteCharsToPtrFowardReturnLength<uint8_t>(char *__restrict__ buff, const uint8_t &input) noexcept
  {
    const __m64 u16_val = _mm_set1_pi16(input);
    const __m64 u8_acii_zero = _mm_set1_pi8('0');
    const __m64 M_MAGIC_U16 = _mm_setr_pi16(656, 6554, 0, 0);

    const __m64 u16_prod = _mm_mulhi_pu16(u16_val, M_MAGIC_U16);

    unsigned len = 1U;
    len += (input >= 10);
    len += (input >= 100);

    const unsigned lead_z = (3U - len) << 3U;

    const __m64 u16_blend = _mm_insert_pi16(u16_prod, input, 2);
    const __m64 u16_slided = _mm_slli_si64(u16_blend, 16);

    const __m64 u16_slided_x8 = _mm_slli_pi16(u16_slided, 3);
    const __m64 u16_slided_x2 = _mm_slli_pi16(u16_slided, 1);

    const __m64 u16_slided_x10 = _mm_add_pi16(u16_slided_x8, u16_slided_x2);

    const __m64 u16_res = _mm_sub_pi16(u16_blend, u16_slided_x10);

    const __m64 u8_packed = _mm_packs_pu16(u16_res, u16_res);
    const __m64 u8_res_shf = _mm_srli_si64(u8_packed, lead_z);
    const __m64 u8_chars = _mm_add_pi8(u8_res_shf, u8_acii_zero);

    _mm_storeu_si32(static_cast<void *>(buff), _mm_movpi64_epi64(u8_chars));

    return len;
  }

#else
  template <>
  uint32_t WriteCharsToPtrFowardReturnLength<uint16_t>(char *__restrict__ buff, const uint16_t &input) noexcept
  {
    const __m128i C_VAL = _mm_set1_epi32(input);

    const __m128i C_ZEROS = _mm_set1_epi8('0');
    const __m128i M_MAGIC_U16 = _mm_setr_epi32(0xA36F, 0x625, 0x47AF, 0x999A);
    const __m128i M_SHIFTS_U16 = _mm_setr_epi32(13, 9, 6, 3);

    const __m128i u16_prod = _mm_mulhi_epu16(C_VAL, M_MAGIC_U16);
    const __m128i u16_sub = _mm_sub_epi32(C_VAL, u16_prod);
    const __m128i u16_shf = _mm_srli_epi16(u16_sub, 1);
    const __m128i u16_add = _mm_add_epi16(u16_shf, u16_prod);

    const __m128i u16_32_div = _mm_srlv_epi32(u16_add, M_SHIFTS_U16);
    const __m128i u16_div = _mm_packus_epi32(u16_32_div, u16_32_div);

    // Branchless Length Calculation (Optimized for modern CPUs)
    const constexpr uint16_t C_TABLE[] = { 0, 10, 100, 1'000, 10'000 };

    const uint32_t bits = 32U - __builtin_clz(input | 1U);
    uint32_t len = (bits * 1233) >> 12U;

    len += (input >= C_TABLE[len]);

    const unsigned lead_z = (5 - len) << 3U;

    // Pack back to 16-bit and insert the original input into lane 4
    const __m128i u16_blend = _mm_blend_epi32(u16_div, C_VAL, 0b1100);

    const __m128i u16_div_x8 = _mm_slli_epi16(u16_blend, 3);
    const __m128i u16_div_x2 = _mm_slli_epi16(u16_blend, 1);

    // Digit Extraction logic
    const __m128i u16_div_x10 = _mm_add_epi16(u16_div_x8, u16_div_x2);

    const __m128i u16_div_x10_slided = _mm_slli_si128(u16_div_x10, 2);
    const __m128i u16_res = _mm_sub_epi16(u16_blend, u16_div_x10_slided);

    const __m128i u8_res = _mm_packus_epi16(u16_res, u16_res);

    // Table Lookup conversion to ASCII
    const __m128i u8_res_shf = _mm_srli_epi64(u8_res, lead_z);
    const __m128i u8_chars = _mm_add_epi8(u8_res_shf, C_ZEROS);

    // Final Store (8 bytes)
    _mm_storeu_si64(static_cast<void *>(buff), u8_chars);

    return len;
  }

  template <>
  uint32_t WriteCharsToPtrFowardReturnLength<uint8_t>(char *__restrict__ buff, const uint8_t &input) noexcept
  {
    const unsigned input_shf16 = static_cast<uint16_t>(input << 6U);
    const __m64 u16_val = _mm_setr_pi16(input, input_shf16, input_shf16 + 1, 0);
    const __m64 u8_acii_zero = _mm_set1_pi8('0');
    const __m64 M_MAGIC_U16 = _mm_setr_pi16(0xA3D8, 0x199A, 0xFFFF, 0);

    const __m64 u16_prod = _mm_mulhi_pu16(u16_val, M_MAGIC_U16);

    uint32_t len = 1U;
    len += (input >= 10);
    len += (input >= 100);

    const unsigned lead_z = (3U - len) << 3U;

    const __m64 u16_shf = _mm_srli_pi16(u16_prod, 6);
    const __m64 u16_slided = _mm_slli_si64(u16_shf, 16);

    const __m64 u16_slided_x8 = _mm_slli_pi16(u16_slided, 3);
    const __m64 u16_slided_x2 = _mm_slli_pi16(u16_slided, 1);

    const __m64 u16_slided_x10 = _mm_add_pi16(u16_slided_x8, u16_slided_x2);

    const __m64 u16_res = _mm_sub_pi16(u16_shf, u16_slided_x10);

    const __m64 u8_packed = _mm_packs_pu16(u16_res, u16_res);
    const __m64 u8_res_shf = _mm_srli_si64(u8_packed, lead_z);
    const __m64 u8_chars = _mm_add_pi8(u8_res_shf, u8_acii_zero);

    _mm_storeu_si32(static_cast<void *>(buff), _mm_movpi64_epi64(u8_chars));

    return len;
  }

#endif
} // namespace Helpers::Simd::x86_64
#endif

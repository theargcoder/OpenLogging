#pragma once

#include <ammintrin.h>
#include <array>
#include <bit>
#include <cstdint>
#include <emmintrin.h>
#include <limits>
#include <smmintrin.h>
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
  static uint32_t WriteCharsToPtrFowardReturnLength(char *__restrict__ buff, const auto &input);

  template <>
  uint32_t WriteCharsToPtrFowardReturnLength<uint64_t>(char *__restrict__ buff, const uint64_t &input)
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
  uint32_t WriteCharsToPtrFowardReturnLength<uint32_t>(char *__restrict__ buff, const uint32_t &input)
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
  uint32_t WriteCharsToPtrFowardReturnLength<uint16_t>(char *__restrict__ buff, const uint16_t &input)
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
  uint32_t WriteCharsToPtrFowardReturnLength<uint8_t>(char *__restrict__ buff, const uint8_t &input)
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
  static uint32_t WriteCharsToPtrFowardReturnLength(char *__restrict__ buff, const auto &input);

#if defined(__AVX512BW__) && defined(__AVX512VL__)

  __attribute__((always_inline)) static inline __m512i umul_hi_32x16(__m512i a, __m512i b) noexcept
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

  template <>
  uint32_t WriteCharsToPtrFowardReturnLength<uint64_t>(char *__restrict__ buff, const uint64_t &input)
  {
    // div by [10^16, 10^12, 10^8, 10^4, 0]
    static const constexpr uint64_t M_MAGIC_U64[] = { 0x39A5652FB1137857ULL, 0x232F33025BD42233ULL, 0xABCC77118461CEFDULL, 0x346DC5D63886594BULL };
    static const constexpr uint64_t M_SHIFTS_U64[] = { 51U, 37U, 26ULL, 11ULL };

    // Padded to 16 elements for full 512-bit registers
    static const constexpr uint16_t M_MAGIC_U16[] = { 0x20C5, 0x51EC, 0xCCCD, 0x0001, 0x20C5, 0x51EC, 0xCCCD, 0x0001, 0x20C5, 0x51EC, 0xCCCD, 0x0001, 0x20C5, 0x51EC,
                                                      0xCCCD, 0x0001, 0x20C5, 0x51EC, 0xCCCD, 0x0001, 0,      0,      0,      0,      0,      0,      0,      0 };
    static const constexpr uint16_t M_SHIFTS_U16[] = { 17, 19, 19, 0, 17, 19, 19, 0, 17, 19, 19, 0, 17, 19, 19, 0, 17, 19, 19, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    static const constexpr uint8_t INDICES[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31 };

    static const constexpr uint64_t LEN_TABLE[] = { 0,
                                                    10,
                                                    100,
                                                    1000,
                                                    10000,
                                                    100000,
                                                    1000000,
                                                    10000000,
                                                    100000000,
                                                    1000000000,
                                                    10000000000ULL,
                                                    100000000000ULL,
                                                    1000000000000ULL,
                                                    10000000000000ULL,
                                                    100000000000000ULL,
                                                    1000000000000000ULL,
                                                    10000000000000000ULL,
                                                    100000000000000000ULL,
                                                    1000000000000000000ULL,
                                                    10000000000000000000ULL };

    const __m256i u64val = _mm256_set1_epi64x(input);
    const __m256i u64magics = _mm256_load_epi64((const void *)&M_MAGIC_U64[0]);

    const __m256i u64prod = _mm256_mulhi_epu64(u64val, u64magics);

    const __m256i u64shfts = _mm256_loadu_epi64((const void *)&M_SHIFTS_U64[0]);
    const __m256i u64shfted = _mm256_srlv_epi64(u64prod, u64shfts);
    const __m256i u64regshift = _mm256_alignr_epi64(u64shfted, _mm256_setzero_si256(), 7);
    const __m256i multipliers = _mm256_set1_epi64x(10'000ULL);
    const __m256i u64resultx10 = _mm256_mullo_epi64(u64regshift, multipliers);
    const __m256i u64final = _mm256_sub_epi64(u64shfted, u64resultx10);

    const __m128i low128 = _mm256_extracti128_si256(u64final, 0);
    const __m128i high128 = _mm256_extracti128_si256(u64final, 1);
    const auto val1 = static_cast<uint16_t>(_mm_extract_epi64(low128, 0));
    const auto val2 = static_cast<uint16_t>(_mm_extract_epi64(low128, 1));
    const auto val3 = static_cast<uint16_t>(_mm_extract_epi64(high128, 0));
    const auto val4 = static_cast<uint16_t>(_mm_extract_epi64(high128, 1));
    const auto val5 = static_cast<uint16_t>(input - _mm_extract_epi64(_mm256_extracti128_si256(u64shfted, 1), 1) * 10'000ULL);

    // 2. Load val5 into the lowest 64-bit slot of a new 256-bit register. Note: _mm256_set_epi64x takes arguments in reverse order (e3, e2, e1, e0).
    const __m256i upper256 = _mm256_set_epi64x(0, 0, 0, val5);

    // 3. Upcast your existing u64final to 512-bit (this occupies the lower 256 bits)
    // 4. Insert upper256 into the upper half (index 1) of the 512-bit register. _mm512_inserti64x4 inserts a 256-bit vector into a 512-bit vector.
    const __m512i v512 = _mm512_inserti64x4(_mm512_castsi256_si512(u64final), upper256, 1);

    // 5. Shuffle to broadcast the lower 16 bits of each 64-bit chunk 4 times.
    const __m512i shuf_mask = _mm512_broadcast_i32x4(_mm_setr_epi8(0, 1, 0, 1, 0, 1, 0, 1, 8, 9, 8, 9, 8, 9, 8, 9));

    const __m512i val = _mm512_shuffle_epi8(v512, shuf_mask);

    const __m512i magics = _mm512_loadu_si512((const void *)&M_MAGIC_U16[0]);

    // Emulate umul_hi_32x16 by calculating even and odd lane 64-bit products and blending the high 32-bits
    const __m512i t = _mm512_mulhi_epu16(val, magics);

    // Math setup
    const __m512i n_sub_t = _mm512_sub_epi16(val, t);
    const __m512i n_sub_t_shf = _mm512_srli_epi16(n_sub_t, 1);
    const __m512i n_sub_t_shf_add_t = _mm512_add_epi16(n_sub_t_shf, t);

    // AVX-512 Variable Shifts (using positive shift counts)
    const __m512i shift_counts = _mm512_loadu_si512((const void *)&M_SHIFTS_U16[0]);
    const __m512i shifted_16 = _mm512_srlv_epi16(n_sub_t_shf_add_t, shift_counts);

    // Override lane 9 (the 10^0 digit) with the original val to preserve precision 1 << 9 = 512 = 0x0200 | 0x8200 targets index 9 AND index 15
    const __m512i res_vec = _mm512_mask_blend_epi16(0x88888888, shifted_16, val);

    // Digit Extraction logic
    const __m512i res_times_10 = _mm512_mullo_epi16(res_vec, _mm512_set1_epi16(10));

    // 1. Updated slide indices to prevent Lane 9 from sliding into Lane 10
    const __m512i slide_indices = _mm512_set_epi16(31, 0, 1, 2, 3, 4, 5, 6, 7, 8, 15, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30);

    // 2. Updated mask to 0xFBFE (zeroes out Lane 0 indexed [3,7,11,15,19])
    const __m512i_u res_slided = _mm512_maskz_mov_epi16(0x88888888, _mm512_permutexvar_epi16(slide_indices, res_times_10));
    const __m512i full_res = _mm512_sub_epi16(res_vec, res_slided);

    const uint32_t bits = (sizeof(std::remove_cvref_t<decltype(input)>) * 8) - std::countl_zero(input);
    uint32_t len = (bits * 1233) >> 12;

    len += (input >= LEN_TABLE[len]);

    const uint16_t lead_z = std::numeric_limits<std::remove_cvref_t<decltype(input)>>::digits10 + 1 - len;

    // Table Lookup conversion to ASCII
    const __m256i ascii_vec = _mm256_add_epi8(_mm512_cvtepi16_epi8(full_res), _mm256_set1_epi8('0'));
    const __m256i indices_vec = _mm256_load_si256((const __m256i *)&INDICES[0]);
    const __m256i final_indices = _mm256_add_epi8(indices_vec, _mm256_set1_epi8((char)lead_z));
    const __m256i output_chars = _mm256_shuffle_epi8(ascii_vec, final_indices);

    // Final Store (16 bytes accommodates up to 10 digits comfortably)
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(buff), output_chars);

    return len;
  }

  template <>
  uint32_t WriteCharsToPtrFowardReturnLength<uint32_t>(char *__restrict__ buff, const uint32_t &input)
  {
    // Padded to 16 elements for full 512-bit registers
    static const constexpr uint32_t M_MAGIC_10_0[]
        = { 0x12E0BE83U, 0x5798EE24U, 0xAD7F29ACU, 0x0C6F7A0CU, 0x4F8B588FU, 0xA36E2EB2U, 0x0624DD30U, 0x47AE147BU, 0x9999999AU, 0xFFFFFFFFU, 0, 0, 0, 0, 0, 0 };

    // AVX-512 shifts must be positive. Padded to 16 elements.
    static const constexpr uint32_t M_SHIFTS_10_0[] = { 29, 26, 23, 19, 16, 13, 9, 6, 3, 0, 0, 0, 0, 0, 0, 0 };

    static const constexpr uint8_t INDICES[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };

    const __m512i val = _mm512_set1_epi32(input);
    const __m512i magics = _mm512_loadu_si512((const void *)&M_MAGIC_10_0[0]);

    // Emulate umul_hi_32x16 by calculating even and odd lane 64-bit products and blending the high 32-bits
    const __m512i t = umul_hi_32x16(val, magics);

    // Math setup
    const __m512i n_sub_t = _mm512_sub_epi32(val, t);
    const __m512i n_sub_t_shf = _mm512_srli_epi32(n_sub_t, 1);
    const __m512i n_sub_t_shf_add_t = _mm512_add_epi32(n_sub_t_shf, t);

    // AVX-512 Variable Shifts (using positive shift counts)
    const __m512i shift_counts = _mm512_loadu_si512((const void *)&M_SHIFTS_10_0[0]);
    const __m512i shifted_32 = _mm512_srlv_epi32(n_sub_t_shf_add_t, shift_counts);

    // Override lane 9 (the 10^0 digit) with the original val to preserve precision 1 << 9 = 512 = 0x0200
    const __m512i res_vec = _mm512_mask_blend_epi32(0x0200, shifted_32, val);

    // Digit Extraction logic
    const __m512i res_times_10 = _mm512_mullo_epi32(res_vec, _mm512_set1_epi32(10));

    // Slide vector right by 1 element using permutation across lanes
    const __m512i slide_indices = _mm512_setr_epi32(15, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14);
    const __m512i_u res_slided = _mm512_maskz_mov_epi32(0xFFFE, _mm512_permutexvar_epi32(slide_indices, res_times_10)); // Zero out lane 0

    const __m512i full_res = _mm512_sub_epi32(res_vec, res_slided);

    // Branchless Length Calculation
    static const constexpr uint32_t table[] = { 0, 10, 100, 1'000, 10'000, 100'000, 1'000'000, 10'000'000, 100'000'000, 1'000'000'000 };

    const uint32_t bits = (sizeof(std::remove_cvref_t<decltype(input)>) * 8) - std::countl_zero(input);
    uint32_t len = (bits * 1233) >> 12;

    len += (input >= table[len]);

    const uint16_t lead_z = std::numeric_limits<std::remove_cvref_t<decltype(input)>>::digits10 + 1 - len;

    // Table Lookup conversion to ASCII
    // _mm512_cvtepi32_epi8 seamlessly truncates 16x32-bit into 16x8-bit in a 128-bit vector
    const __m128i ascii_vec = _mm_add_epi8(_mm512_cvtepi32_epi8(full_res), _mm_set1_epi8('0'));
    const __m128i indices_vec = _mm_loadu_si128((const __m128i *)&INDICES[0]);
    const __m128i final_indices = _mm_add_epi8(indices_vec, _mm_set1_epi8((char)lead_z));
    const __m128i output_chars = _mm_shuffle_epi8(ascii_vec, final_indices);

    // Final Store (16 bytes accommodates up to 10 digits comfortably)
    _mm_storeu_si128(reinterpret_cast<__m128i *>(buff), output_chars);

    return len;
  }

  template <>
  uint32_t WriteCharsToPtrFowardReturnLength<uint16_t>(char *__restrict__ buff, const uint16_t &input)
  {
    // Constants
    const __m128i M_MAGIC_U16 = _mm_setr_epi16(0xA36F, 0x625, 0x47AF, 0x999A, 0xFFFF, 0, 0, 0);
    const __m128i MASK_REG_SHIFT = _mm_setr_epi16(0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0x0000);
    const __m128i INDICES = _mm_setr_epi8(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);

    // Line 240-244: Math setup
    const __m128i n = _mm_set1_epi16(input);
    const __m128i t = _mm_mulhi_epu16(n, M_MAGIC_U16);
    const __m128i n_sub_t_shf_add_t = _mm_add_epi16(_mm_srli_epi16(_mm_sub_epi16(n, t), 1), t);

    // FIX: Use AVX2 Variable Shifts to avoid memory round-trip
    // We upcast to 32-bit to use _mm_srlv_epi32
    const __m128i shift_counts = _mm_setr_epi32(13, 9, 6, 3);
    const __m128i low_32 = _mm_cvtepu16_epi32(n_sub_t_shf_add_t);
    const __m128i shifted_32 = _mm_srlv_epi32(low_32, shift_counts);

    // Pack back to 16-bit and insert the original input into lane 4
    const __m128i res_vec = _mm_insert_epi16(_mm_packus_epi32(shifted_32, _mm_setzero_si128()), input, 4);

    // Digit Extraction logic
    const __m128i res_times_10 = _mm_mullo_epi16(res_vec, _mm_set1_epi16(10));
    const __m128i res_slided = _mm_and_si128(_mm_alignr_epi8(res_times_10, _mm_setzero_si128(), 14), MASK_REG_SHIFT);
    const __m128i full_res = _mm_sub_epi16(res_vec, res_slided);

    // Branchless Length Calculation (Optimized for modern CPUs)
    static const constexpr uint32_t table[] = { 0, 10, 100, 1'000, 10'000 };

    const uint32_t bits = (sizeof(std::remove_cvref_t<decltype(input)>) * 8) - std::countl_zero(input);
    uint32_t len = (bits * 1233) >> 12;

    len += (input >= table[len]);

    const uint16_t lead_z = std::numeric_limits<std::remove_cvref_t<decltype(input)>>::digits10 + 1 - len;

    // Table Lookup conversion to ASCII
    __m128i ascii_vec = _mm_add_epi8(_mm_packus_epi16(full_res, _mm_setzero_si128()), _mm_set1_epi8('0'));
    __m128i final_indices = _mm_add_epi8(INDICES, _mm_set1_epi8((char)lead_z));
    __m128i output_chars = _mm_shuffle_epi8(ascii_vec, final_indices);

    // Final Store (8 bytes)
    _mm_storel_epi64(reinterpret_cast<__m128i *>(buff), output_chars);

    return len;
  }

#elif defined(__AVX2__)
  /**
   * @brief Extracts all eight 32-bit lanes from an __m256i register.
   * @param reg The AVX2 register to extract from.
   * @return A std::array containing the 8 uint32_t values.
   */
  inline std::array<uint64_t, 4> extract_lanes_64(const __m256i &reg)
  {
    std::array<uint64_t, 4> result;

    // Use an unaligned store to move the register contents into the array memory.
    // This is generally the fastest way to "convert" a SIMD register to a standard container.
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(result.data()), reg);

    return result;
  }

  inline std::array<uint32_t, 8> extract_lanes_32(const __m256i &reg)
  {
    std::array<uint32_t, 8> result;

    // Use an unaligned store to move the register contents into the array memory.
    // This is generally the fastest way to "convert" a SIMD register to a standard container.
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(result.data()), reg);

    return result;
  }

  inline std::array<uint32_t, 16> extract_lanes_16(const __m256i &reg)
  {
    std::array<uint32_t, 16> result;

    // Use an unaligned store to move the register contents into the array memory.
    // This is generally the fastest way to "convert" a SIMD register to a standard container.
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(result.data()), reg);

    return result;
  }

  inline std::array<uint16_t, 32> extract_lanes_8(const __m256i &reg)
  {
    std::array<uint8_t, 32> result;

    // Use an unaligned store to move the register contents into the array memory.
    // This is generally the fastest way to "convert" a SIMD register to a standard container.
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(result.data()), reg);

    std::array<uint16_t, 32> res_to_ret;

    for(int i = 0; i < 32; i++)
    {
      res_to_ret[i] = result[i];
    }

    return res_to_ret;
  }

  inline std::array<uint16_t, 8> extract_lanes_16(const __m128i &reg)
  {
    std::array<uint16_t, 8> result;

    // Use an unaligned store to move the register contents into the array memory.
    // This is generally the fastest way to "convert" a SIMD register to a standard container.
    _mm_store_si128(reinterpret_cast<__m128i *>(result.data()), reg);

    return result;
  }

  inline std::array<uint16_t, 16> extract_lanes_8(const __m128i &reg)
  {
    std::array<uint8_t, 16> result;

    // Use an unaligned store to move the register contents into the array memory.
    // This is generally the fastest way to "convert" a SIMD register to a standard container.
    _mm_store_si128(reinterpret_cast<__m128i *>(result.data()), reg);

    std::array<uint16_t, 16> res_to_ret;

    for(int i = 0; i < 16; i++)
    {
      res_to_ret[i] = result[i];
    }

    return res_to_ret;
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
  uint32_t WriteCharsToPtrFowardReturnLength<uint32_t>(char *__restrict__ buff, const uint32_t &input)
  {
    static const constexpr __m256i M_MAGIC_u64 = { 0x431BDE83ULL, 0x51EB851FULL, 0xCCCCCCCDULL, 0 };
    static const constexpr __m256i M_SHIFTS_u64 = { 50, 37, 35, 0 };
    static const constexpr uint32_t table[] = { 0, 10, 100, 1'000, 10'000, 100'000, 1'000'000, 10'000'000, 100'000'000, 1'000'000'000 };
    static const constexpr uint32_t M_MAGIC_U16[] = { 0x8313, 0xA3D8, 0x199A, 0x0, 0x8313, 0xA3D8, 0x199A, 0x0 };
    static const constexpr uint32_t M_SHIFTS_U16[] = { 16 + 9, 16 + 6, 16, 16, 16 + 9, 16 + 6, 16, 16 };
    static const constexpr uint8_t INDICES[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };

    const __m256i val = _mm256_set1_epi64x(input);
    const __m256i prod = _mm256_mul_epu32(val, M_MAGIC_u64);
    const __m256i shifted_64 = _mm256_blend_epi32(_mm256_srlv_epi64(prod, M_SHIFTS_u64), val, 0b11000000);

    const __m256i shifted_64_x_10 = _mm256_mul_epu32(shifted_64, _mm256_set_epi64x(0ULL, 10ULL, 10ULL, 10'000ULL));
    const __m256i permuted_u64 = _mm256_permutevar8x32_epi32(shifted_64_x_10, _mm256_setr_epi32(6, 7, 0, 1, 2, 3, 4, 5));
    const __m256i res_u64 = _mm256_sub_epi64(shifted_64, permuted_u64);

    // --- OPTIMIZATION 1: Eliminate extracts for top/bot ---
    // Instead of extracting to scalar and blending back, we use a single permute
    // to broadcast 'top' (dword 0) to the lower 128 bits and 'bot' (dword 2) to the upper 128 bits.
    const __m256i comb_u32 = _mm256_permutevar8x32_epi32(res_u64, _mm256_setr_epi32(0, 0, 0, 0, 2, 2, 2, 2));

    const __m256i mul_comb_u16 = _mm256_mullo_epi32(comb_u32, _mm256_load_si256((const __m256i *)&M_MAGIC_U16[0]));
    const __m256i mul_comb_u16_shf = _mm256_srlv_epi32(mul_comb_u16, _mm256_load_si256((const __m256i *)&M_SHIFTS_U16[0]));
    const __m256i mul_comb_x10 = _mm256_mullo_epi32(mul_comb_u16_shf, _mm256_set1_epi32(10));
    const __m256i comb_to_sub = _mm256_blend_epi32(mul_comb_u16_shf, comb_u32, 0b10001000);
    const __m256i comb_permut = _mm256_permutevar8x32_epi32(mul_comb_x10, _mm256_setr_epi32(3, 0, 1, 2, 7, 4, 5, 6));
    const __m256i comb_sub = _mm256_sub_epi32(comb_to_sub, comb_permut);

    const uint32_t bits = (sizeof(uint32_t) * 8) - __builtin_clz(input);
    uint32_t len = (bits * 1233) >> 12;
    len += (input >= table[len]);
    const uint8_t lead_z = 10 - len;

    const __m256i shuffled_res = _mm256_permutevar8x32_epi32(
        _mm256_shuffle_epi8(comb_sub, _mm256_setr_epi8(0, 4, 8, 12, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 0, 4, 8, 12, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1)),
        _mm256_setr_epi32(0, 4, 1, 2, 3, 5, 6, 7));

    // --- OPTIMIZATION 2: Eliminate extracts for rem1/rem2 ---
    // Extract the upper 128 bits of res_u64 directly into an XMM register.
    const __m128i upper_res = _mm256_extracti128_si256(res_u64, 1);

    // VPSHUFB maps rem1 (byte 0) to byte 8, and rem2 (byte 8) to byte 9, zeroing the rest.
    const __m128i rem_packed = _mm_shuffle_epi8(upper_res, _mm_setr_epi8(-1, -1, -1, -1, -1, -1, -1, -1, 0, 8, -1, -1, -1, -1, -1, -1));

    // Use zero-cost cast to grab the lower 128 bits, then blend the packed remainders directly.
    const __m128i packed_8 = _mm_blend_epi32(_mm256_castsi256_si128(shuffled_res), rem_packed, 0b0100);

    const __m128i ascii_vec = _mm_add_epi8(packed_8, _mm_set1_epi8('0'));
    const __m128i indices_vec = _mm_loadu_si128((const __m128i *)&INDICES[0]);
    const __m128i final_indices = _mm_add_epi8(indices_vec, _mm_set1_epi8((char)lead_z));
    const __m128i output_chars = _mm_shuffle_epi8(ascii_vec, final_indices);

    _mm_storeu_si128(reinterpret_cast<__m128i *>(buff), output_chars);

    return len;
  }

  template <>
  uint32_t WriteCharsToPtrFowardReturnLength<uint16_t>(char *__restrict__ buff, const uint16_t &input)
  {
    // Constants
    const __m128i M_MAGIC_U16 = _mm_setr_epi16(0xA36F, 0x625, 0x47AF, 0x999A, 0xFFFF, 0, 0, 0);
    const __m128i MASK_REG_SHIFT = _mm_setr_epi16(0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0x0000);
    const __m128i INDICES = _mm_setr_epi8(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);

    // Line 240-244: Math setup
    const __m128i n = _mm_set1_epi16(input);
    const __m128i t = _mm_mulhi_epu16(n, M_MAGIC_U16);
    const __m128i n_sub_t_shf_add_t = _mm_add_epi16(_mm_srli_epi16(_mm_sub_epi16(n, t), 1), t);

    // FIX: Use AVX2 Variable Shifts to avoid memory round-trip
    // We upcast to 32-bit to use _mm_srlv_epi32
    const __m128i shift_counts = _mm_setr_epi32(13, 9, 6, 3);
    __m128i low_32 = _mm_cvtepu16_epi32(n_sub_t_shf_add_t);
    __m128i shifted_32 = _mm_srlv_epi32(low_32, shift_counts);

    // Pack back to 16-bit and insert the original input into lane 4
    __m128i res_vec = _mm_packus_epi32(shifted_32, _mm_setzero_si128());
    res_vec = _mm_insert_epi16(res_vec, input, 4);

    // Digit Extraction logic
    const __m128i res_times_10 = _mm_mullo_epi16(res_vec, _mm_set1_epi16(10));
    const __m128i res_slided = _mm_and_si128(_mm_alignr_epi8(res_times_10, _mm_setzero_si128(), 14), MASK_REG_SHIFT);
    const __m128i full_res = _mm_sub_epi16(res_vec, res_slided);

    // Branchless Length Calculation (Optimized for modern CPUs)
    static const constexpr uint32_t table[] = { 0, 10, 100, 1'000, 10'000 };

    const uint32_t bits = (sizeof(std::remove_cvref_t<decltype(input)>) * 8) - std::countl_zero(input);
    uint32_t len = (bits * 1233) >> 12;

    len += (input >= table[len]);

    const uint8_t lead_z = std::numeric_limits<std::remove_cvref_t<decltype(input)>>::digits10 + 1 - len;

    // Table Lookup conversion to ASCII
    __m128i ascii_vec = _mm_add_epi8(_mm_packus_epi16(full_res, _mm_setzero_si128()), _mm_set1_epi8('0'));
    __m128i final_indices = _mm_add_epi8(INDICES, _mm_set1_epi8(lead_z));
    __m128i output_chars = _mm_shuffle_epi8(ascii_vec, final_indices);

    // Final Store (8 bytes)
    _mm_storel_epi64(reinterpret_cast<__m128i *>(buff), output_chars);

    return len;
  }

#else
  template <>
  uint32_t WriteCharsToPtrFowardReturnLength<uint16_t>(char *__restrict__ buff, const uint16_t &input)
  {
    static const constexpr uint32_t LEN_TABLE[] = { 0, 10, 100, 1'000, 10'000 };

    // _mm_set_epi16 populates registers in reverse order (lane 7 down to lane 0)
    const __m128i M_MAGIC_U16 = _mm_set_epi16(0, 0, 0, 0, 0x999A, 0x47AF, 0x0625, 0xA36F);

    // Simulating vshlq_u16 right shifts (-13, -9, -6, -3) via multiplication.
    // (x >> k) is mathematically equivalent to (x * 2^(16 - k)) >> 16.
    // We compute 2^(16-13)=8, 2^(16-9)=128, 2^(16-6)=1024, 2^(16-3)=8192.
    const __m128i M_SHIFT_MUL = _mm_set_epi16(0, 0, 0, 0, 8192, 1024, 128, 8);

    const __m128i n = _mm_set1_epi16(input);
    const __m128i t = _mm_mulhi_epu16(n, M_MAGIC_U16);

    const __m128i n_sub_t = _mm_sub_epi16(n, t);
    const __m128i n_sub_t_shf = _mm_srli_epi16(n_sub_t, 1);
    const __m128i n_sub_t_shf_add_t = _mm_add_epi16(n_sub_t_shf, t);

    // Apply "shifts" and manually insert the original input into lane 4
    const __m128i shifted = _mm_mulhi_epu16(n_sub_t_shf_add_t, M_SHIFT_MUL);
    const __m128i n_sub_t_shf_add_t_shf = _mm_insert_epi16(shifted, input, 4);

    const __m128i res_times_10 = _mm_mullo_epi16(n_sub_t_shf_add_t_shf, _mm_set1_epi16(10));

    // _mm_slli_si128 shifts left by bytes. Shifting 2 bytes = 1 element,
    // emulating ARM's vextq_u16(0, val, 7) behavior of shifting elements to higher lanes.
    const __m128i res_slided = _mm_slli_si128(res_times_10, 2);

    const uint32_t bits = (sizeof(std::remove_cvref_t<decltype(input)>) * 8) - std::countl_zero(input);
    uint32_t len = (bits * 1233) >> 12;
    len += (input >= LEN_TABLE[len]);

    const uint16_t lead_z = std::numeric_limits<std::remove_cvref_t<decltype(input)>>::digits10 + 1 - len;

    const __m128i full_res = _mm_sub_epi16(n_sub_t_shf_add_t_shf, res_slided);

    // vmovn_u16 narrow truncation mapped to _mm_packus_epi16
    const __m128i chars16 = _mm_add_epi16(full_res, _mm_set1_epi16('0'));
    const __m128i chars = _mm_packus_epi16(chars16, chars16);

    // vtbl1_u8 table lookup mapped to SSSE3 _mm_shuffle_epi8
    const __m128i indices = _mm_setr_epi8(0, 1, 2, 3, 4, 5, 6, 7, 0, 0, 0, 0, 0, 0, 0, 0);
    const __m128i mask = _mm_add_epi8(indices, _mm_set1_epi8(lead_z));
    const __m128i out = _mm_shuffle_epi8(chars, mask);

    // vst1_s8 equivalent (writes exactly the lowest 64 bits / 8 chars to memory)
    _mm_storel_epi64(reinterpret_cast<__m128i *>(buff), out);

    return len;
  }
#endif
  template <>
  uint32_t WriteCharsToPtrFowardReturnLength<uint8_t>(char *__restrict__ buff, const uint8_t &input)
  {
    return WriteCharsToPtrFowardReturnLength<uint16_t>(buff, static_cast<uint16_t>(input));
  }
} // namespace Helpers::Simd::x86_64
#endif

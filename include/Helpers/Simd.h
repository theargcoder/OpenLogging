#pragma once

#include <limits>
#if defined(_MSC_VER) || defined(__x86_64__) || defined(__i386__)
#include <immintrin.h> // x86 SIMD
#elif defined(__ARM_NEON) || defined(__aarch64__)
#include <arm_neon.h> // ARM SIMD
#endif

#include <type_traits>

#include "include/Helpers/Math.h"

namespace Helpers::Simd::ARM64
{
  __attribute__((always_inline)) static auto umul_hi_32x4_t(const uint32x4_t &v_a, const uint32x4_t &v_b) noexcept
  {
    const uint64x2_t prod_low = vmull_u32(vget_low_u32(v_a), vget_low_u32(v_b));
    const uint64x2_t prod_high = vmull_high_u32(v_a, v_b);

    // Re-combine the high 32-bits of each 64-bit result into one 128-bit vector This is essentially 'mulhi'
    return vcombine_u32(vshrn_n_u64(prod_low, 32), vshrn_n_u64(prod_high, 32));
  }

  __attribute__((always_inline)) static auto umul_low_32x4_t(const uint32x4_t &v_a, const uint32x4_t &v_b) noexcept
  {
    return vmulq_u32(v_a, v_b);
  }

  template <typename T>
    requires(std::is_integral_v<T> && std::is_unsigned_v<T>)
  static uint32_t WriteCharsToPtrFowardReturnLength(char *__restrict__ buff, const auto &input);

  template <>
  uint32_t WriteCharsToPtrFowardReturnLength<uint32_t>(char *__restrict__ buff, const uint32_t &input)
  {
    static const constexpr uint32x4_t v_magics_10e1_10e4 = { 0xD1B71759ULL, 0x10624DD3ULL, 0x51EB851FULL, 0xCCCCCCCDULL };
    static const constexpr int32x4_t v_shifts = { -13, -6, -5, -3 };

    static const constexpr uint32_t magic_div_10 = 0xCCCCCCCDULL;
    static const constexpr int32_t magic_div_10_shift = -3;

    static const constexpr uint32x4_t v_10s = { 10U, 10U, 10U, 10U };

    if(input == 0)
    {
      *buff = '0';
      *(buff + 1) = '\0';
      return 1U;
    }

    uint32_t top_val = Helpers::Math::Magic::Division::div_by_10_pow_n<6>(input) * 10;
    uint32_t middle_val = Helpers::Math::Magic::Modulo::mod_by_10_pow_n<4>(Helpers::Math::Magic::Division::div_by_10_pow_n<2>(input)) * 10;
    uint32_t bottom_val = Helpers::Math::Magic::Modulo::mod_by_10_pow_n<2>(input) * 1'000;

    // vdupq_n_u32 is the NEON equivalent of _mm_set1_epi32
    const uint32x4_t v_top = vdupq_n_u32(top_val);
    const uint32x4_t v_mid = vdupq_n_u32(middle_val);
    const uint32x4_t v_low = vdupq_n_u32(bottom_val);

    const uint32x4_t v_prod_top = umul_hi_32x4_t(v_top, v_magics_10e1_10e4);
    const uint32x4_t v_prod_mid = umul_hi_32x4_t(v_mid, v_magics_10e1_10e4);
    const uint32x4_t v_prod_low = umul_hi_32x4_t(v_low, v_magics_10e1_10e4);

    const uint32x4_t v_div_top = vshlq_u32(v_prod_top, v_shifts);
    const uint32x4_t v_div_mid = vshlq_u32(v_prod_mid, v_shifts);
    const uint32x4_t v_div_low = vshlq_u32(v_prod_low, v_shifts);

    const uint32x4_t v_div_by_10_top = vshlq_u32(umul_hi_32x4_t(v_div_top, vdupq_n_u32(magic_div_10)), vdupq_n_u32(magic_div_10_shift));
    const uint32x4_t v_div_by_10_mid = vshlq_u32(umul_hi_32x4_t(v_div_mid, vdupq_n_u32(magic_div_10)), vdupq_n_u32(magic_div_10_shift));
    const uint32x4_t v_div_by_10_low = vshlq_u32(umul_hi_32x4_t(v_div_low, vdupq_n_u32(magic_div_10)), vdupq_n_u32(magic_div_10_shift));

    const uint32x4_t v_div_by_10_mul_10_top = umul_low_32x4_t(v_div_by_10_top, v_10s);
    const uint32x4_t v_div_by_10_mul_10_mid = umul_low_32x4_t(v_div_by_10_mid, v_10s);
    const uint32x4_t v_div_by_10_mul_10_low = umul_low_32x4_t(v_div_by_10_low, v_10s);

    const uint32x4_t res_top = vsubq_u32(v_div_top, v_div_by_10_mul_10_top);
    const uint32x4_t res_mid = vsubq_u32(v_div_mid, v_div_by_10_mul_10_mid);
    const uint32x4_t res_low = vsubq_u32(v_div_low, v_div_by_10_mul_10_low);

    // 1. Narrow each 32x4 to 16x4
    const uint16x4_t top_16 = vmovn_u32(res_top); // [d0, d1, d2, d3] in 16-bit
    const uint16x4_t mid_16 = vmovn_u32(res_mid); // [d4, d5, d6, d7] in 16-bit
    const uint16x4_t low_16 = vmovn_u32(res_low); // [d4, d5, d6, d7] in 16-bit

    // 2. Combine top and bottom into one 16x8 register
    const uint16x8_t top_mid_16 = vcombine_u16(top_16, mid_16);
    const uint16x8_t low_0_16 = vcombine_u16(low_16, vdup_n_u16(0U));

    // 3. Narrow 16x8 to 8x8
    const uint16x8_t top_mid_mask = vandq_u16(vcgtq_u16(top_mid_16, vdupq_n_u16(0)), uint16x8_t{ 128, 64, 32, 16, 8, 4, 2, 1 });
    const uint16x8_t low_mask = vandq_u16(vcgtq_u16(low_0_16, vdupq_n_u16(0)), uint16x8_t{ 128, 64, 32, 16, 8, 4, 2, 1 });
    const uint16_t top_mid_bitmask = vaddlvq_u8(top_mid_mask);
    const uint16_t low_bitmask = vaddlvq_u8(low_mask);
    const uint16_t combined_mask = (top_mid_bitmask << 8U) | low_bitmask;

    const uint8x8_t top_mid_8 = vmovn_u16(top_mid_16); // [d0, d1, d2, d3, d4, d5, d6, d7] in 8-bit
    const uint8x8_t low_8 = vmovn_u16(low_0_16);       // [d0, d1, d2, d3, d4, d5, d6, d7] in 8-bit
    const int8x16_t combined = vcombine_u8(top_mid_8, low_8);

    const uint8x16_t v_and = vaddq_s8(combined, vandq_u8(vdupq_n_s8('0'), uint8x16_t{ 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 0, 0, 0, 0, 0 }));

    const uint16_t leading_z = std::countl_zero(combined_mask);
    static const constexpr int8x16_t indices = int8x16_t{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };

    const int8x16_t shift_vector = vdupq_n_s8(leading_z);
    const int8x16_t selector = vaddq_s8(indices, shift_vector);

    // 3. Use TBL to "pick" the bytes at those new positions
    const uint8x16_t result = vqtbl1q_u8(v_and, selector);

    vst1q_s8(reinterpret_cast<int8_t *>(buff), result);

    const uint32_t len = (std::numeric_limits<uint32_t>::digits10 + 1) - leading_z;

    return len;
  }

  template <>
  uint32_t WriteCharsToPtrFowardReturnLength<uint64_t>(char *__restrict__ buff, const uint64_t &input)
  {
    static const constexpr uint32x4_t v_magics_10e1_10e4 = { 0xD1B71759ULL, 0x10624DD3ULL, 0x51EB851FULL, 0xCCCCCCCDULL };
    static const constexpr int32x4_t v_shifts = { -13, -6, -5, -3 };

    static const constexpr uint32_t magic_div_10 = 0xCCCCCCCDULL;
    static const constexpr int32_t magic_div_10_shift = -3;

    static const constexpr uint32x4_t v_10s = { 10U, 10U, 10U, 10U };

    if(input == 0)
    {
      *buff = '0';
      *(buff + 1) = '\0';
      return 1U;
    }

    uint32_t lane_1 = Helpers::Math::Magic::Division::div_by_10_pow_n<16>(input) * 10;
    uint32_t lane_2 = Helpers::Math::Magic::Modulo::mod_by_10_pow_n<4>(Helpers::Math::Magic::Division::div_by_10_pow_n<12>(input)) * 10U;
    uint32_t lane_3 = Helpers::Math::Magic::Modulo::mod_by_10_pow_n<4>(Helpers::Math::Magic::Division::div_by_10_pow_n<8>(input)) * 10U;
    uint32_t lane_4 = Helpers::Math::Magic::Modulo::mod_by_10_pow_n<4>(Helpers::Math::Magic::Division::div_by_10_pow_n<4>(input)) * 10U;
    uint32_t lane_5 = Helpers::Math::Magic::Modulo::mod_by_10_pow_n<4>(input) * 10;

    // vdupq_n_u32 is the NEON equivalent of _mm_set1_epi32
    const uint32x4_t v_1 = vdupq_n_u32(lane_1);
    const uint32x4_t v_2 = vdupq_n_u32(lane_2);
    const uint32x4_t v_3 = vdupq_n_u32(lane_3);
    const uint32x4_t v_4 = vdupq_n_u32(lane_4);
    const uint32x4_t v_5 = vdupq_n_u32(lane_5);

    const uint32x4_t v_1_prod = umul_hi_32x4_t(v_1, v_magics_10e1_10e4);
    const uint32x4_t v_2_prod = umul_hi_32x4_t(v_2, v_magics_10e1_10e4);
    const uint32x4_t v_3_prod = umul_hi_32x4_t(v_3, v_magics_10e1_10e4);
    const uint32x4_t v_4_prod = umul_hi_32x4_t(v_4, v_magics_10e1_10e4);
    const uint32x4_t v_5_prod = umul_hi_32x4_t(v_5, v_magics_10e1_10e4);

    const uint32x4_t v_1_div = vshlq_u32(v_1_prod, v_shifts);
    const uint32x4_t v_2_div = vshlq_u32(v_2_prod, v_shifts);
    const uint32x4_t v_3_div = vshlq_u32(v_3_prod, v_shifts);
    const uint32x4_t v_4_div = vshlq_u32(v_4_prod, v_shifts);
    const uint32x4_t v_5_div = vshlq_u32(v_5_prod, v_shifts);

    const uint32x4_t v_1_div_by_10 = vshlq_u32(umul_hi_32x4_t(v_1_div, vdupq_n_u32(magic_div_10)), vdupq_n_u32(magic_div_10_shift));
    const uint32x4_t v_2_div_by_10 = vshlq_u32(umul_hi_32x4_t(v_2_div, vdupq_n_u32(magic_div_10)), vdupq_n_u32(magic_div_10_shift));
    const uint32x4_t v_3_div_by_10 = vshlq_u32(umul_hi_32x4_t(v_3_div, vdupq_n_u32(magic_div_10)), vdupq_n_u32(magic_div_10_shift));
    const uint32x4_t v_4_div_by_10 = vshlq_u32(umul_hi_32x4_t(v_4_div, vdupq_n_u32(magic_div_10)), vdupq_n_u32(magic_div_10_shift));
    const uint32x4_t v_5_div_by_10 = vshlq_u32(umul_hi_32x4_t(v_5_div, vdupq_n_u32(magic_div_10)), vdupq_n_u32(magic_div_10_shift));

    const uint32x4_t v_1_div_10_mul_10 = umul_low_32x4_t(v_1_div_by_10, v_10s);
    const uint32x4_t v_2_div_10_mul_10 = umul_low_32x4_t(v_2_div_by_10, v_10s);
    const uint32x4_t v_3_div_10_mul_10 = umul_low_32x4_t(v_3_div_by_10, v_10s);
    const uint32x4_t v_4_div_10_mul_10 = umul_low_32x4_t(v_4_div_by_10, v_10s);
    const uint32x4_t v_5_div_10_mul_10 = umul_low_32x4_t(v_5_div_by_10, v_10s);

    const uint32x4_t v_1_res = vsubq_u32(v_1_div, v_1_div_10_mul_10);
    const uint32x4_t v_2_res = vsubq_u32(v_2_div, v_2_div_10_mul_10);
    const uint32x4_t v_3_res = vsubq_u32(v_3_div, v_3_div_10_mul_10);
    const uint32x4_t v_4_res = vsubq_u32(v_4_div, v_4_div_10_mul_10);
    const uint32x4_t v_5_res = vsubq_u32(v_5_div, v_5_div_10_mul_10);

    // 1. Narrow each 32x4 to 16x4
    const uint16x4_t v_1_16 = vmovn_u32(v_1_res);
    const uint16x4_t v_2_16 = vmovn_u32(v_2_res);
    const uint16x4_t v_3_16 = vmovn_u32(v_3_res);
    const uint16x4_t v_4_16 = vmovn_u32(v_4_res);
    const uint16x4_t v_5_16 = vmovn_u32(v_5_res);

    // 2. Combine top and bottom into one 16x8 register
    const uint16x8_t v_1_2_16 = vcombine_u16(v_1_16, v_2_16);
    const uint16x8_t v_3_4_16 = vcombine_u16(v_3_16, v_4_16);
    const uint16x8_t v_5_0_16 = vcombine_u16(v_5_16, vdup_n_u16(0U));

    // 3. Narrow 16x8 to 8x8
    const uint16x8_t v_1_2_mask = vandq_u16(vcgtq_u16(v_1_2_16, vdupq_n_u16(0)), uint16x8_t{ 128, 64, 32, 16, 8, 4, 2, 1 });
    const uint16x8_t v_3_4_mask = vandq_u16(vcgtq_u16(v_3_4_16, vdupq_n_u16(0)), uint16x8_t{ 128, 64, 32, 16, 8, 4, 2, 1 });
    const uint16x8_t v_5_0_mask = vandq_u16(vcgtq_u16(v_5_0_16, vdupq_n_u16(0)), uint16x8_t{ 128, 64, 32, 16, 8, 4, 2, 1 });
    const uint16_t v_1_2_bitmask = vaddlvq_u8(v_1_2_mask);
    const uint32_t v_3_4_bitmask = vaddlvq_u8(v_3_4_mask);
    const uint32_t v_5_0_bitmask = vaddlvq_u8(v_5_0_mask);
    const uint32_t combined_mask = (v_1_2_bitmask << 24U) | (v_3_4_bitmask << 16U) | (v_5_0_bitmask << 8U);

    const uint8x8_t v_1_2_16_to8bit = vmovn_u16(v_1_2_16); // [d0, d1, d2, d3, d4, d5, d6, d7] in 8-bit
    const uint8x8_t v_3_4_16_to8bit = vmovn_u16(v_3_4_16); // [d0, d1, d2, d3, d4, d5, d6, d7] in 8-bit
    const uint8x8_t v_5_0_16_to8bit = vmovn_u16(v_5_0_16); // [d0, d1, d2, d3, d4, d5, d6, d7] in 8-bit
    const uint8x16_t v_1234_combined = vcombine_u8(v_1_2_16_to8bit, v_3_4_16_to8bit);

    const uint8x16_t v_1234_and = vaddq_s8(v_1234_combined, vandq_u8(vdupq_n_s8('0'), vdupq_n_u8(255)));
    const uint8x8_t v_50_and = vadd_s8(v_5_0_16_to8bit, vand_u8(vdup_n_s8('0'), uint8x8_t{ 255, 255, 255, 255, 0, 0, 0, 0 }));

    const uint16_t leading_z = std::countl_zero(combined_mask);

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
}

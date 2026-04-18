#pragma once

#include <cstdint>
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
    static const constexpr auto M_MAGIC_U16 = uint16x8_t{ 0x8313, 0xA3D8, 0x199A, 0xFFFF, 0x8313, 0xA3D8, 0x199A, 0xFFFF };
    static const constexpr auto M_SHIFTS_U16 = int16x8_t{ -9, -6, 0, 0, -9, -6, 0, 0 };
    static const constexpr auto MASK_REG_SHIFT = uint16x8_t{ 0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0xFFFF, 0xFFFF, 0xFFFF };
    static const constexpr auto MUL_CORRECTIONS = uint16x8_t{ 0, 0, 0, 1, 0, 0, 0, 1 };
    static const constexpr auto CHARS_OFFSET = uint8x8_t{ '0', '0', 0, 0, 0, 0, 0, 0 };
    static const constexpr auto SELECT_8x16_INICES = int8x16_t{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };

    if(input == 0)
    {
      *buff = '0';
      *(buff + 1) = '\0';
      return 1U;
    }

    uint16_t top_val = Helpers::Math::Magic::Division::div_by_10_pow_n<6>(input);
    uint16_t middle_val = Helpers::Math::Magic::Modulo::mod_by_10_pow_n<4>(Helpers::Math::Magic::Division::div_by_10_pow_n<2>(input));
    uint16_t bottom_val = Helpers::Math::Magic::Modulo::mod_by_10_pow_n<2>(input) * 100;

    uint16x8_t VEC_1_16x8, VEC_2_16x8, VEC_3_16x8, VEC_4_16x8;

    VEC_1_16x8 = vaddq_u16(vcombine_u16(vdup_n_u16(top_val), vdup_n_u16(middle_val)), MUL_CORRECTIONS);
    VEC_3_16x8 = vaddq_u16(vdupq_n_u16(bottom_val), MUL_CORRECTIONS);
    VEC_1_16x8 = umul_hi_u16x8(VEC_1_16x8, M_MAGIC_U16);
    VEC_3_16x8 = umul_hi_u16x8(VEC_3_16x8, M_MAGIC_U16);
    VEC_2_16x8 = vshlq_u16(VEC_1_16x8, M_SHIFTS_U16);
    VEC_4_16x8 = vshlq_u16(VEC_3_16x8, M_SHIFTS_U16);
    VEC_1_16x8 = vaddq_u16(vshlq_u16(VEC_2_16x8, vdupq_n_u16(3)), vshlq_u16(VEC_2_16x8, vdupq_n_u16(1)));
    VEC_3_16x8 = vaddq_u16(vshlq_u16(VEC_4_16x8, vdupq_n_u16(3)), vshlq_u16(VEC_4_16x8, vdupq_n_u16(1)));
    VEC_1_16x8 = vandq_u16(vextq_u16(vdupq_n_u16(0), VEC_1_16x8, 7), MASK_REG_SHIFT);
    VEC_3_16x8 = vandq_u16(vextq_u16(vdupq_n_u16(0), VEC_3_16x8, 7), MASK_REG_SHIFT);

    uint32_t len = 1;
    len += (input > 9);
    len += (input > 99);
    len += (input > 999);
    len += (input > 9'999);
    len += (input > 99'999);
    len += (input > 999'999);
    len += (input > 9'999'999);
    len += (input > 99'999'999);
    len += (input > 999'999'999);
    const uint32_t lead_z = 10 - len;

    VEC_2_16x8 = vsubq_u16(VEC_2_16x8, VEC_1_16x8);
    VEC_4_16x8 = vsubq_u16(VEC_4_16x8, VEC_3_16x8);

    uint8x8_t VEC_1_8x8, VEC_3_8x8;
    VEC_1_8x8 = vmovn_u16(VEC_2_16x8);
    VEC_3_8x8 = vmovn_u16(VEC_4_16x8);
    VEC_1_8x8 = vadd_s8(VEC_1_8x8, vdup_n_s8('0'));
    VEC_3_8x8 = vadd_s8(VEC_3_8x8, CHARS_OFFSET);

    uint8x16_t VEC_FULL;

    VEC_FULL = vcombine_u8(VEC_1_8x8, VEC_3_8x8);
    VEC_FULL = vqtbl1q_s8(VEC_FULL, vaddq_s8(SELECT_8x16_INICES, vdupq_n_s8(lead_z)));

    vst1q_s8(reinterpret_cast<int8_t *>(buff), VEC_FULL);

    return len;
  }

  template <>
  uint32_t WriteCharsToPtrFowardReturnLength<uint16_t>(char *__restrict__ buff, const uint16_t &input)
  {
    if(input == 0)
    {
      *buff = '0';
      *(buff + 1) = '\0';
      return 1U;
    }

    static const constexpr auto M_MAGIC_U16 = uint16x8_t{ 0xA36F, 0x625U, 0x47AFU, 0x999AU, 0xFFFFU, 0U, 0U, 0U };
    static const constexpr auto M_SHIFTS_U16 = int16x8_t{ -13, -9, -6, -3, 0, 0, 0, 0 };
    static const constexpr auto MASK_REG_SHIFT = uint16x8_t{ 0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0x0000 };
    static const constexpr auto INDICES = uint8x8_t{ 0, 1, 2, 3, 4, 5, 6, 7 };
    static const constexpr auto CHARS_OFFSET = uint8x8_t{ '0', '0', '0', '0', '0', 0, 0, 0 };

    const auto n = vdupq_n_u16(input);
    const auto t = umul_hi_u16x8(n, M_MAGIC_U16);
    const auto n_sub_t = vsubq_u16(n, t);
    const auto n_sub_t_shf = vshlq_u16(n_sub_t, vdupq_n_u16(-1));
    const auto n_sub_t_shf_add_t = vaddq_u16(n_sub_t_shf, t);
    const auto n_sub_t_shf_add_t_shf = vsetq_lane_u16(input, vshlq_u16(n_sub_t_shf_add_t, M_SHIFTS_U16), 4);
    const auto res_times_10 = umul_low_u16x8(n_sub_t_shf_add_t_shf, vdupq_n_u16(10));
    const auto res_slided = vandq_u16(vextq_u16(vdupq_n_u16(0), res_times_10, 7), MASK_REG_SHIFT);

    uint32_t len = 1;
    len += (input > 9);
    len += (input > 99);
    len += (input > 999);
    len += (input > 9999);
    const uint32_t lead_z = 5 - len;

    const auto full_res = vsubq_u16(n_sub_t_shf_add_t_shf, res_slided);

    const uint8x8_t VEC_1_8x8 = vtbl1_s8(vadd_s8(vmovn_u16(full_res), CHARS_OFFSET), vadd_s8(INDICES, vdup_n_s8(lead_z)));

    vst1_s8(reinterpret_cast<int8_t *>(buff), VEC_1_8x8);

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
  template <>
  uint32_t WriteCharsToPtrFowardReturnLength<uint16_t>(char *__restrict__ buff, const uint16_t &input)
  {
    if(input == 0)
    {
      *buff = '0';
      *(buff + 1) = '\0';
      return 1U;
    }

    static const constexpr auto M_MAGIC_U16 = uint16x8_t{ 0xA36F, 0x625U, 0x47AFU, 0x999AU, 0xFFFFU, 0U, 0U, 0U };
    static const constexpr auto M_SHIFTS_U16 = int16x8_t{ -13, -9, -6, -3, 0, 0, 0, 0 };
    static const constexpr auto MASK_REG_SHIFT = uint16x8_t{ 0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0x0000 };
    static const constexpr auto INDICES = uint8x8_t{ 0, 1, 2, 3, 4, 5, 6, 7 };
    static const constexpr auto CHARS_OFFSET = uint8x8_t{ '0', '0', '0', '0', '0', 0, 0, 0 };

    const auto n = vdupq_n_u16(input);
    const auto t = umul_hi_u16x8(n, M_MAGIC_U16);
    const auto n_sub_t = vsubq_u16(n, t);
    const auto n_sub_t_shf = vshlq_u16(n_sub_t, vdupq_n_u16(-1));
    const auto n_sub_t_shf_add_t = vaddq_u16(n_sub_t_shf, t);
    const auto n_sub_t_shf_add_t_shf = vsetq_lane_u16(input, vshlq_u16(n_sub_t_shf_add_t, M_SHIFTS_U16), 4);
    const auto res_times_10 = umul_low_u16x8(n_sub_t_shf_add_t_shf, vdupq_n_u16(10));
    const auto res_slided = vandq_u16(vextq_u16(vdupq_n_u16(0), res_times_10, 7), MASK_REG_SHIFT);

    uint32_t len = 1;
    len += (input > 9);
    len += (input > 99);
    len += (input > 999);
    len += (input > 9999);
    const uint32_t lead_z = 5 - len;

    const auto full_res = vsubq_u16(n_sub_t_shf_add_t_shf, res_slided);

    const uint8x8_t VEC_1_8x8 = vtbl1_s8(vadd_s8(vmovn_u16(full_res), CHARS_OFFSET), vadd_s8(INDICES, vdup_n_s8(lead_z)));

    vst1_s8(reinterpret_cast<int8_t *>(buff), VEC_1_8x8);

    return len;
  }
} // namespace Helpers::Simd::x86_64
#endif

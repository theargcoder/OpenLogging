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
    static const constexpr auto M_MAGIC_10_6 = uint32x4_t{ 0x12E0BE83U, 0x5798EE24U, 0xAD7F29ACU, 0x0C6F7A0CU };
    static const constexpr auto M_MAGIC_6_2 = uint32x4_t{ 0x4F8B588FU, 0xA36E2EB2ULL, 0x0624DD30UL, 0x47AE147BU };
    static const constexpr auto M_MAGIC_2_0 = uint32x4_t{ 0x9999999AU, 0xFFFFFFFF };

    static const constexpr auto M_SHIFTS_10_6 = int32x4_t{ -29, -26, -23, -19 };
    static const constexpr auto M_SHIFTS_6_2 = int32x4_t{ -16, -13, -9, -6 };
    static const constexpr auto M_SHIFTS_2_0 = int32x4_t{ -3, 0, 0, 0 };

    static const constexpr auto INDICES = uint8x16_t{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };

    if(input == 0)
    {
      *buff = '0';
      *(buff + 1) = '\0';
      return 1U;
    }

    const auto input_x4 = vdupq_n_u32(input);
    const auto mul_top = umul_hi_32x4(input_x4, M_MAGIC_10_6);
    const auto mul_mid = umul_hi_32x4(input_x4, M_MAGIC_6_2);
    const auto mul_bot = umul_hi_32x4(input_x4, M_MAGIC_2_0);
    const auto ntop_sub_top = vsubq_u32(input_x4, mul_top);
    const auto nmid_sub_mid = vsubq_u32(input_x4, mul_mid);
    const auto nbot_sub_bot = vsubq_u32(input_x4, mul_bot);
    const auto ntop_sub_t_shf = vshrq_n_u32(ntop_sub_top, 1);
    const auto nmid_sub_t_shf = vshrq_n_u32(nmid_sub_mid, 1);
    const auto nbot_sub_t_shf = vshrq_n_u32(nbot_sub_bot, 1);
    const auto ntop_sub_t_shf_add_top = vaddq_u32(ntop_sub_t_shf, mul_top);
    const auto nmid_sub_t_shf_add_top = vaddq_u32(nmid_sub_t_shf, mul_mid);
    const auto nbot_sub_t_shf_add_bot = vaddq_u32(nbot_sub_t_shf, mul_bot);
    const auto ntop_sub_t_shf_add_t_sh = vshlq_u32(ntop_sub_t_shf_add_top, M_SHIFTS_10_6);
    const auto nmid_sub_t_shf_add_t_sh = vshlq_u32(nmid_sub_t_shf_add_top, M_SHIFTS_6_2);
    const auto nbot_sub_t_shf_add_t_sh = vsetq_lane_u32(input, vshlq_u32(nbot_sub_t_shf_add_bot, M_SHIFTS_2_0), 1); // to correct the 0xFFFFFFF mask since its not valid
    const auto restop_time_10 = vmulq_n_u32(ntop_sub_t_shf_add_t_sh, 10U);
    const auto resmid_time_10 = vmulq_n_u32(nmid_sub_t_shf_add_t_sh, 10U);
    const auto resbot_time_10 = vmulq_n_u32(nbot_sub_t_shf_add_t_sh, 10U);
    const auto restop_slided = vextq_u32(vdupq_n_u32(0), restop_time_10, 3);
    const auto resmid_slided = vextq_u32(vdupq_n_u32(vdups_laneq_u32(restop_time_10, 3)), resmid_time_10, 3);
    const auto resbot_slided = vextq_u32(vdupq_n_u32(vdups_laneq_u32(resmid_time_10, 3)), resbot_time_10, 3);

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

    const auto top_full_res = vsubq_u32(ntop_sub_t_shf_add_t_sh, restop_slided);
    const auto mid_full_res = vsubq_u32(nmid_sub_t_shf_add_t_sh, resmid_slided);
    const auto bot_full_res = vsubq_u32(nbot_sub_t_shf_add_t_sh, resbot_slided);

    const uint8x8_t top_mid_chars = vmovn_u16(vcombine_u16(vmovn_u32(top_full_res), vmovn_u32(mid_full_res)));
    const uint8x8_t bot_chars = vmovn_u16(vcombine_u16(vmovn_u32(bot_full_res), vdup_n_u16(0U)));

    const uint8x16_t combined = vaddq_u8(vcombine_u8(top_mid_chars, bot_chars), vdupq_n_u8('0'));

    const uint8x16_t out = vqtbl1q_u8(combined, vaddq_u8(INDICES, vdupq_n_u8(lead_z)));

    vst1q_s8(reinterpret_cast<int8_t *>(buff), out);

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

    static const constexpr auto M_MAGIC_U16 = uint16x8_t{ 0xA36F, 0x625U, 0x47AFU, 0x999AU, 0, 0, 0, 0 };
    static const constexpr auto M_SHIFTS_U16 = int16x8_t{ -13, -9, -6, -3, 0, 0, 0, 0 };
    static const constexpr auto TENS = uint16x8_t{ 10, 10, 10, 10, 10, 10, 10, 10 };

    static const constexpr auto INDICES = uint8x8_t{ 0, 1, 2, 3, 4, 5, 6, 7 };
    static const constexpr auto CHARS_OFFSET = uint8x8_t{ '0', '0', '0', '0', '0', 0, 0, 0 };

    const auto n = vdupq_n_u16(input);
    const auto t = umul_hi_u16x8(n, M_MAGIC_U16);
    const auto n_sub_t = vsubq_u16(n, t);
    const auto n_sub_t_shf = vshlq_u16(n_sub_t, vdupq_n_u16(-1));
    const auto n_sub_t_shf_add_t = vaddq_u16(n_sub_t_shf, t);
    const auto n_sub_t_shf_add_t_shf = vsetq_lane_u16(input, vshlq_u16(n_sub_t_shf_add_t, M_SHIFTS_U16), 4);
    const auto res_times_10 = umul_low_u16x8(n_sub_t_shf_add_t_shf, TENS);
    const auto res_slided = vextq_u16(vdupq_n_u16(0), res_times_10, 7);

    uint32_t len = 1;
    len += (input > 9);
    len += (input > 99);
    len += (input > 999);
    len += (input > 9999);
    const uint32_t lead_z = 5 - len;

    const auto full_res = vsubq_u16(n_sub_t_shf_add_t_shf, res_slided);

    const uint8x8_t chars = vadd_u8(vmovn_u16(full_res), CHARS_OFFSET);
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
  __attribute__((always_inline)) static inline __m512i umul_hi_32x16(__m512i a, __m512i b) noexcept
  {
    const __m512i even_prod = _mm512_mul_epu32(a, b);
    const __m512i odd_prod = _mm512_mul_epu32(_mm512_srli_epi64(a, 32U), _mm512_srli_epi64(b, 32U));
    return _mm512_mask_blend_epi32(0xAAAAU, _mm512_srli_epi64(even_prod, 32U), odd_prod);
  }

  template <typename T>
    requires(std::is_integral_v<T> && std::is_unsigned_v<T>)
  static uint32_t WriteCharsToPtrFowardReturnLength(char *__restrict__ buff, const auto &input);

#if defined(__AVX512BW__) && defined(__AVX512VL__)
  template <>
  uint32_t WriteCharsToPtrFowardReturnLength<uint32_t>(char *__restrict__ buff, const uint32_t &input)
  {
    if(input == 0)
    {
      buff[0] = '0';
      buff[1] = '\0';
      return 1U;
    }

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
    if(input == 0)
    {
      buff[0] = '0';
      buff[1] = '\0';
      return 1U;
    }

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
    uint32_t len = 1;
    len += (input > 9);
    len += (input > 99);
    len += (input > 999);
    len += (input > 9999);
    const int32_t lead_z = 5 - len;

    // Table Lookup conversion to ASCII
    __m128i ascii_vec = _mm_add_epi8(_mm_packus_epi16(full_res, _mm_setzero_si128()), _mm_set1_epi8('0'));
    __m128i final_indices = _mm_add_epi8(INDICES, _mm_set1_epi8((char)lead_z));
    __m128i output_chars = _mm_shuffle_epi8(ascii_vec, final_indices);

    // Final Store (8 bytes)
    _mm_storel_epi64(reinterpret_cast<__m128i *>(buff), output_chars);

    return len;
  }

#elif defined(__AVX2__)
  template <>
  uint32_t WriteCharsToPtrFowardReturnLength<uint16_t>(char *__restrict__ buff, const uint16_t &input)
  {
    if(input == 0)
    {
      buff[0] = '0';
      buff[1] = '\0';
      return 1U;
    }

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
    uint32_t len = 1;
    len += (input > 9);
    len += (input > 99);
    len += (input > 999);
    len += (input > 9999);
    const int32_t lead_z = 5 - len;

    // Table Lookup conversion to ASCII
    __m128i ascii_vec = _mm_add_epi8(_mm_packus_epi16(full_res, _mm_setzero_si128()), _mm_set1_epi8('0'));
    __m128i final_indices = _mm_add_epi8(INDICES, _mm_set1_epi8((char)lead_z));
    __m128i output_chars = _mm_shuffle_epi8(ascii_vec, final_indices);

    // Final Store (8 bytes)
    _mm_storel_epi64(reinterpret_cast<__m128i *>(buff), output_chars);

    return len;
  }

#else

  template <>
  uint32_t WriteCharsToPtrFowardReturnLength<uint16_t>(char *__restrict__ buff, const uint16_t &input)
  {
    if(input == 0)
    {
      buff[0] = '0';
      buff[1] = '\0';
      return 1U;
    }

    // Constants mapped to __m128i
    const __m128i M_MAGIC_U16 = _mm_setr_epi16(0xA36F, 0x625, 0x47AF, 0x999A, 0xFFFF, 0, 0, 0);
    const __m128i MASK_REG_SHIFT = _mm_setr_epi16(0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0x0000);
    const __m128i INDICES = _mm_setr_epi8(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
    const __m128i CHARS_OFFSET = _mm_set1_epi8('0');

    // Line 240: vdupq_n_u16
    const __m128i n = _mm_set1_epi16(input);

    // Line 241: umul_hi_u16x8 (Directly supported in SSE)
    const __m128i t = _mm_mulhi_epu16(n, M_MAGIC_U16);

    // Line 242-243: vsubq_u16 and vshlq_u16 (right shift by 1)
    const __m128i n_sub_t = _mm_sub_epi16(n, t);
    const __m128i n_sub_t_shf = _mm_srli_epi16(n_sub_t, 1);

    // Line 244: vaddq_u16
    const __m128i n_sub_t_shf_add_t = _mm_add_epi16(n_sub_t_shf, t);

    // Line 245: Variable shift simulation + Lane set
    // x86 lacks _mm_srlv_epi16. We perform manual shifts for the first 4 lanes.
    alignas(16) uint16_t temp[8];
    _mm_store_si128((__m128i *)temp, n_sub_t_shf_add_t);
    temp[0] >>= 13;
    temp[1] >>= 9;
    temp[2] >>= 6;
    temp[3] >>= 3;
    // temp[4] is replaced by input per vsetq_lane_u16(..., 4)
    temp[4] = input;
    __m128i n_sub_t_shf_add_t_shf = _mm_load_si128((__m128i *)temp);

    // Line 246: umul_low_u16x8
    const __m128i res_times_10 = _mm_mullo_epi16(n_sub_t_shf_add_t_shf, _mm_set1_epi16(10));

    // Line 247: vextq_u16 (7 elements = 14 bytes) + vandq_u16
    // NEON vext(zero, res, 7) extracts the high bytes. In SSE: _mm_alignr_epi8
    const __m128i res_slided = _mm_and_si128(_mm_alignr_epi8(res_times_10, _mm_setzero_si128(), 14), MASK_REG_SHIFT);

    // Length calculation (Logic unchanged)
    uint32_t len = 1;
    len += (input > 9);
    len += (input > 99);
    len += (input > 999);
    len += (input > 9999);
    const int32_t lead_z = 5 - len;

    // Line 256: vsubq_u16
    const __m128i full_res = _mm_sub_epi16(n_sub_t_shf_add_t_shf, res_slided);

    // Line 258: Narrowing + Table Lookup (vtbl)
    // vmovn_u16 + vadd + vtbl is equivalent to pack + PSHUFB
    __m128i narrowed = _mm_packus_epi16(full_res, _mm_setzero_si128());
    __m128i ascii_vec = _mm_add_epi8(narrowed, CHARS_OFFSET);

    // Shift indices by lead_z to handle alignment
    __m128i shift_vec = _mm_set1_epi8((char)lead_z);
    __m128i final_indices = _mm_add_epi8(INDICES, shift_vec);
    __m128i VEC_1_8x8 = _mm_shuffle_epi8(ascii_vec, final_indices);

    // Line 260: Store (8 bytes for uint16 version)
    _mm_storel_epi64(reinterpret_cast<__m128i *>(buff), VEC_1_8x8);

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

#pragma once

#include <ammintrin.h>
#include <array>
#include <bit>
#include <cstdint>
#include <emmintrin.h>
#include <limits>
#include <tmmintrin.h>
#include <xmmintrin.h>
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

      if constexpr(std::is_same_v<RegType, __m128i>)
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
      if constexpr(std::is_same_v<RegType, __m128i>)
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
  uint32_t WriteCharsToPtrFowardReturnLength<uint64_t>(char *__restrict__ buff, const uint64_t &input)
  {

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

    // div by [10^18, 10^16, 10^14, 10^12, 10^10, 10^8, 10^6, 10^4]
    const __m512i M_MAGICS_u64 = _mm512_setr_epi64(0x2725DD1D243ABA0FULL, 0xCD2B297D889BC2B7ULL, 0x6849B86A12B9B01FULL, 0x19799812DEA11198ULL, 0xB7CDFD9D7BDBAB7EULL,
                                                   0x5798EE2308C39DFAULL, 0xC6F7A0B5ED8D36CULL, 0xA36E2EB1C432CA58ULL);

    const __m512i M_SHIFT_u64 = _mm512_setr_epi64(59, 53, 46, 39, 33, 26, 19, 13);

    const uint32_t bits = (sizeof(std::remove_cvref_t<decltype(input)>) * 8) - std::countl_zero(input);
    uint32_t len = (bits * 1233) >> 12;
    len += (input >= LEN_TABLE[len]);

    const unsigned lead_z = std::numeric_limits<std::remove_cvref_t<decltype(input)>>::digits10 + 1 - len;

    const __m512i u64val = _mm512_set1_epi64(input);
    const __m512i u64prod = _mm512_mulhi_epu64(u64val, M_MAGICS_u64);
    const uint64_t u64_lastd_prod = Helpers::Assembly::umulh64(input, 0x47AE147AE147AE15ULL);
    //); return (((n - t) >> 1) + t) >> 6; }

    // Math setup
    const __m512i u64_sub_prod = _mm512_sub_epi64(u64val, u64prod);
    const uint64_t u64_las_sub_prod = input - u64_lastd_prod;
    const __m512i u64_sub_prod_shf = _mm512_srli_epi64(u64_sub_prod, 1);
    const uint64_t u64_sub_las_sub_prod = u64_las_sub_prod >> 1;
    const __m512i u64_sub_prod_shf_add_t = _mm512_add_epi64(u64_sub_prod_shf, u64prod);
    const uint64_t u64_las_sub_prod_shf_add_t = u64_sub_las_sub_prod + u64_lastd_prod;
    const __m512i u64_dived = _mm512_srlv_epi64(u64_sub_prod_shf_add_t, M_SHIFT_u64);
    const uint64_t u64_last_divided = u64_las_sub_prod_shf_add_t >> 6U;

    const uint64_t u64_last_divided_x64 = u64_last_divided << 6U;
    const __m512i u64_dived_x64 = _mm512_slli_epi64(u64_dived, 6);
    const uint64_t u64_last_divided_x32 = u64_last_divided << 5U;
    const __m512i u64_dived_x32 = _mm512_slli_epi64(u64_dived, 5);

    const uint64_t u64_last_divided_x96 = u64_last_divided_x64 + u64_last_divided_x32;
    const __m512i u64_dived_x96 = _mm512_add_epi64(u64_dived_x64, u64_dived_x32);
    const __m512i u64_dived_x4 = _mm512_slli_epi64(u64_dived, 2);
    const uint64_t u64_last_divided_x4 = u64_last_divided << 2;

    const __m512i u64_dived_x100 = _mm512_add_epi64(u64_dived_x96, u64_dived_x4);
    const uint64_t u64_last_dived_x100 = u64_last_divided_x96 + u64_last_divided_x4;

    const __m128i upper_lane = _mm512_extracti64x2_epi64(u64_dived_x100, 3);
    const uint64_t top_last_dig = u64_last_divided - _mm_extract_epi64(upper_lane, 1);
    const uint64_t bot_last_dig = input - u64_last_dived_x100;
    const uint64_t last_dived = (bot_last_dig << 48U) | (bot_last_dig << 32U) | (top_last_dig << 16U) | top_last_dig;

    const __m512i u64_last_div = _mm512_set1_epi64(last_dived);
    const __m512i u64_perm_idx = _mm512_setr_epi64(0, 0, 1, 2, 3, 4, 5, 6);

    const __m512i u64_permuted = _mm512_maskz_permutexvar_epi64(0b11111110, u64_perm_idx, u64_dived_x100);

    const __m512i u64_res = _mm512_sub_epi64(u64_dived, u64_permuted);

    const __m256i u32_shrinked = _mm512_cvtepi64_epi32(u64_res);

    const __m256i u16_packed = _mm256_add_epi16(u32_shrinked, _mm256_slli_epi64(u32_shrinked, 16));

    const __m512i u16_packed_all = _mm512_mask_blend_epi64(0b1111'0000, _mm512_castsi256_si512(u16_packed), u64_last_div);

    const __m512i u16_packed_all_x128 = _mm512_slli_epi16(u16_packed_all, 7);
    const __m512i u16_packed_all_x64 = _mm512_slli_epi16(u16_packed_all, 6);

    const __m512i u16_packed_all_x8 = _mm512_slli_epi16(u16_packed_all, 3);
    const __m512i u16_packed_all_x4 = _mm512_slli_epi16(u16_packed_all, 2);

    const __m512i u16_packed_all_x192 = _mm512_add_epi16(u16_packed_all_x128, u16_packed_all_x64);
    const __m512i u16_packed_all_x12 = _mm512_add_epi16(u16_packed_all_x8, u16_packed_all_x4);

    const __m512i u16_packed_all_x204 = _mm512_add_epi16(u16_packed_all_x192, u16_packed_all_x12);
    const __m512i u16_packed_all_x205 = _mm512_add_epi16(u16_packed_all_x204, u16_packed_all);

    const __m512i u16_shfted = _mm512_srli_epi16(u16_packed_all_x205, 11);

    const __m512i u16_shfted_x8 = _mm512_slli_epi16(u16_shfted, 3);
    const __m512i u16_shfted_x2 = _mm512_slli_epi16(u16_shfted, 1);

    const __m512i u16_shfted_x10 = _mm512_maskz_add_epi16(0b1010'1010'1010'1010'1010, u16_shfted_x8, u16_shfted_x2);
    const __m512i u16_to_sub = _mm512_mask_blend_epi16(0b0101'0101'0101'0101'0101, u16_packed_all, u16_shfted);

    const __m512i u16_res = _mm512_sub_epi16(u16_to_sub, u16_shfted_x10);

    // conversion to ASCII
    const __m256i ascii_vec = _mm256_add_epi8(_mm512_cvtepi16_epi8(u16_res), _mm256_set1_epi8('0'));
    alignas(64) char temp[64];
    _mm256_store_si256(reinterpret_cast<__m256i *>(&temp[0]), ascii_vec);

    const __m256i output_chars = _mm256_loadu_si256(reinterpret_cast<const __m256i *>(&temp[0] + lead_z));

    _mm256_storeu_si256(reinterpret_cast<__m256i *>(buff), output_chars);

    return len;
  }

  template <>
  uint32_t WriteCharsToPtrFowardReturnLength<uint32_t>(char *__restrict__ buff, const uint32_t &input)
  {

    // Branchless Length Calculation
    static const constexpr uint32_t table[] = { 0, 10, 100, 1'000, 10'000, 100'000, 1'000'000, 10'000'000, 100'000'000, 1'000'000'000 };

    const uint32_t bits = (sizeof(std::remove_cvref_t<decltype(input)>) * 8) - std::countl_zero(input);
    uint32_t len = (bits * 1233) >> 12;

    len += (input >= table[len]);

    const uint16_t lead_z = std::numeric_limits<std::remove_cvref_t<decltype(input)>>::digits10 + 1 - len;

    const __m512i val = _mm512_set1_epi32(input);

    // AVX-512 shifts must be positive. Padded to 16 elements. Padded to 16 elements for full 512-bit registers
    const __m512i M_MAGIC_10_0
        = _mm512_setr_epi32(0x12E0BE83U, 0x5798EE24U, 0xAD7F29ACU, 0x0C6F7A0CU, 0x4F8B588FU, 0xA36E2EB2U, 0x0624DD30U, 0x47AE147BU, 0x9999999AU, 0, 0, 0, 0, 0, 0, 0);
    const __m512i M_SHIFTS_10_0 = _mm512_setr_epi32(29, 26, 23, 19, 16, 13, 9, 6, 3, 0, 0, 0, 0, 0, 0, 0);
    const __m128i INDICES = _mm_setr_epi8(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
    const __m512i slide_indices = _mm512_setr_epi32(15, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14);

    // Emulate umul_hi_32x16 by calculating even and odd lane 64-bit products and blending the high 32-bits
    const __m512i prod = umul_hi_32x16(val, M_MAGIC_10_0);

    // Math setup
    const __m512i n_sub_t = _mm512_sub_epi32(val, prod);
    const __m512i n_sub_t_shf = _mm512_srli_epi32(n_sub_t, 1);
    const __m512i n_sub_t_shf_add_t = _mm512_add_epi32(n_sub_t_shf, prod);
    const __m512i shifted_32 = _mm512_srlv_epi32(n_sub_t_shf_add_t, M_SHIFTS_10_0);

    // Override lane 9 (the 10^0 digit) with the original val to preserve precision 1 << 9 = 512 = 0x0200
    const __m512i res_vec = _mm512_mask_blend_epi32(0x0200, shifted_32, val);

    // Digit Extraction logic
    const __m512i res_times_2 = _mm512_slli_epi32(res_vec, 1);
    const __m512i res_times_8 = _mm512_slli_epi32(res_vec, 3);

    const __m512i res_times_10 = _mm512_add_epi32(res_times_8, res_times_2);

    const __m512i permuted = _mm512_permutexvar_epi32(slide_indices, res_times_10);

    const __m512i res_slided = _mm512_maskz_mov_epi32(0xFFFE, permuted);

    const __m512i full_res = _mm512_sub_epi32(res_vec, res_slided);

    // Table Lookup conversion to ASCII _mm512_cvtepi32_epi8 seamlessly truncates 16x32-bit into 16x8-bit in a 128-bit vector
    const __m128i ascii_vec = _mm_add_epi8(_mm512_cvtepi32_epi8(full_res), _mm_set1_epi8('0'));
    const __m128i final_indices = _mm_add_epi8(INDICES, _mm_set1_epi8(static_cast<char>(lead_z)));
    const __m128i output_chars = _mm_shuffle_epi8(ascii_vec, final_indices);

    // Final Store (16 bytes accommodates up to 10 digits comfortably)
    _mm_storeu_si128(reinterpret_cast<__m128i *>(buff), output_chars);

    return len;
  }

  template <>
  uint32_t WriteCharsToPtrFowardReturnLength<uint16_t>(char *__restrict__ buff, const uint16_t &input)
  {
    // Branchless Length Calculation (Optimized for modern CPUs)
    static const constexpr uint32_t table[] = { 0, 10, 100, 1'000, 10'000 };

    const uint32_t bits = (sizeof(std::remove_cvref_t<decltype(input)>) * 8) - std::countl_zero(input);
    uint32_t len = (bits * 1233) >> 12;

    len += (input >= table[len]);
    const __m128i val = _mm_set1_epi16(input);

    const unsigned lead_z = std::numeric_limits<std::remove_cvref_t<decltype(input)>>::digits10 + 1 - len;
    const __m128i M_MAGIC_U16 = _mm_setr_epi16(0xA36F, 0x625, 0x47AF, 0x999A, 0, 0, 0, 0);
    const __m128i INDICES = _mm_setr_epi8(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);

    const __m128i prod = _mm_mulhi_epu16(val, M_MAGIC_U16);
    const __m128i n_sub_t_shf_add_t = _mm_add_epi16(_mm_srli_epi16(_mm_sub_epi16(val, prod), 1), prod);

    // FIX: Use AVX2 Variable Shifts to avoid memory round-trip we upcast to 32-bit to use _mm_srlv_epi32
    const __m128i shift_counts = _mm_setr_epi32(13, 9, 6, 3);
    const __m128i low_32 = _mm_cvtepu16_epi32(n_sub_t_shf_add_t);
    const __m128i shifted_32 = _mm_srlv_epi32(low_32, shift_counts);

    // Pack back to 16-bit and insert the original input into lane 4
    const __m128i res_vec = _mm_blend_epi32(_mm_packus_epi32(shifted_32, shifted_32), val, 0b1100);

    const __m128i res_times_x8 = _mm_slli_epi16(res_vec, 3);
    const __m128i res_times_x2 = _mm_slli_epi16(res_vec, 1);

    // Digit Extraction logic
    const __m128i res_times_10 = _mm_add_epi16(res_times_x8, res_times_x2);

    const __m128i res_slided = _mm_slli_si128(res_times_10, 2);
    const __m128i full_res = _mm_sub_epi16(res_vec, res_slided);

    // Table Lookup conversion to ASCII
    const __m128i ascii_vec = _mm_add_epi8(_mm_packus_epi16(full_res, _mm_setzero_si128()), _mm_set1_epi8('0'));
    const __m128i final_indices = _mm_add_epi8(INDICES, _mm_set1_epi8(static_cast<char>(lead_z)));
    const __m128i output_chars = _mm_shuffle_epi8(ascii_vec, final_indices);

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

  inline std::array<uint16_t, 16> extract_lanes_16(const __m256i &reg)
  {
    std::array<uint16_t, 16> result;

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

  inline std::array<uint32_t, 4> extract_lanes_32(const __m128i &reg)
  {
    std::array<uint32_t, 4> result;

    // Use an unaligned store to move the register contents into the array memory.
    // This is generally the fastest way to "convert" a SIMD register to a standard container.
    _mm_store_si128(reinterpret_cast<__m128i *>(result.data()), reg);

    return result;
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

  // Compute high 32 bits of (a[i] * b[i]) for 8x uint32_t
  template <uint8_t MAGIC, uint8_t RIGHT_SHF>
  static inline __m256i _mm256_mulmagsr_retpackedepu8(const __m256i &a, const __m256i &b) noexcept
  {
    // note that b is expected to be in [a, b, c ,d] aka 4x64bit
    //    const auto a_lanes = extract_lanes_16(a);
    //    const auto b_lanes = extract_lanes_16(b);

    const __m256i prod_A = _mm256_mullo_epi16(a, _mm256_set1_epi16(MAGIC));
    const __m256i prod_B = _mm256_mullo_epi16(b, _mm256_set1_epi16(MAGIC));

    //    const auto prod_a_lanes = extract_lanes_16(prod_A);
    //    const auto prod_b_lanes = extract_lanes_16(prod_B);

    const __m256i blend_A = _mm256_blend_epi16(prod_A, _mm256_set1_epi16(0), 0b1010'1010);
    const __m256i blend_B = _mm256_blend_epi16(prod_B, _mm256_set1_epi16(0), 0b1010'1010);

    //    const auto blen_a_lanes = extract_lanes_16(blend_A);
    //    const auto blen_b_lanes = extract_lanes_16(blend_B);

    const __m256i shf_A = _mm256_srli_epi32(blend_A, RIGHT_SHF);
    const __m256i shf_B = _mm256_srli_epi32(blend_B, RIGHT_SHF);

    //    const auto shf_A_lanes = extract_lanes_16(shf_A);
    //   const auto shf_B_lanes = extract_lanes_16(shf_B);

    const __m256i packed_A = _mm256_packus_epi32(shf_A, shf_A);
    const __m256i packed_B = _mm256_packus_epi32(shf_B, shf_B);

    //   const auto packed_A_lanes = extract_lanes_16(packed_A);
    //   const auto packed_B_lanes = extract_lanes_16(packed_B);

    const __m256i packed_unpack_A = _mm256_unpacklo_epi16(packed_A, packed_A);
    const __m256i packed_unpack_B = _mm256_unpacklo_epi16(packed_B, packed_B);

    //    const auto packed_unpacked_A_lanes = extract_lanes_16(packed_unpack_A);
    //    const auto packed_unpacked_B_lanes = extract_lanes_16(packed_unpack_B);

    const __m128i div_A_top = _mm256_extracti128_si256(packed_unpack_A, 0);
    const __m128i div_A_bot = _mm256_extracti128_si256(packed_unpack_A, 1);
    const __m128i div_B_top = _mm256_extracti128_si256(packed_unpack_B, 0);

    const __m128i packpacked_top_A = _mm_blend_epi16(div_A_top, _mm_set1_epi16(0), 0b0101'0101);
    const __m128i packpacked_bot_A = _mm_blend_epi16(div_A_bot, _mm_set1_epi16(0), 0b0101'0101);
    const __m128i trunc_u8_A = _mm_packus_epi16(packpacked_top_A, packpacked_bot_A);
    const __m128i packpacked_top_B = _mm_blend_epi16(div_B_top, _mm_set1_epi16(0), 0b0101'0101);
    const __m128i packpacked_bot_B = _mm_set1_epi32(0);
    const __m128i trunc_u8_B = _mm_packus_epi16(packpacked_top_B, packpacked_bot_B);

    const __m256i trunc_u8_full = _mm256_set_m128i(trunc_u8_B, trunc_u8_A);

    //  const auto trunc_u8_A_lanes = extract_lanes_8(trunc_u8_A);
    //  const auto trunc_u8_B_lanes = extract_lanes_8(trunc_u8_B);
    //  const auto trunc_full_lanes = extract_lanes_8(trunc_u8_full);

    const __m256i trunc_u8_full_x8 = _mm256_slli_epi64(trunc_u8_full, 3);
    const __m256i trunc_u8_full_x2 = _mm256_slli_epi64(trunc_u8_full, 1);
    const __m256i trunc_u8_full_x10 = _mm256_add_epi8(trunc_u8_full_x8, trunc_u8_full_x2);

    //  const auto trunc_u8_full_x8_lanes = extract_lanes_8(trunc_u8_full_x8);
    //  const auto trunc_u8_full_x2_lanes = extract_lanes_8(trunc_u8_full_x2);
    //  const auto trunc_u8_full_x10_lanes = extract_lanes_8(trunc_u8_full_x10);

    const __m128i packed_A_top = _mm_blend_epi16(_mm256_extracti128_si256(a, 0), div_A_top, 0b0101'0101);
    const __m128i packed_A_bot = _mm_blend_epi16(_mm256_extracti128_si256(a, 1), div_A_bot, 0b0101'0101);
    const __m128i trunc_A = _mm_packus_epi16(packed_A_top, packed_A_bot);
    const __m128i packed_B_top = _mm_blend_epi16(_mm256_extracti128_si256(b, 0), div_B_top, 0b0101'0101);
    const __m128i packed_B_bot = _mm_set1_epi32(0);
    const __m128i trunc_B = _mm_packus_epi16(packed_B_top, packed_B_bot);

    const __m256i trunc_full = _mm256_set_m128i(trunc_B, trunc_A);

    const __m256i actual_res = _mm256_sub_epi8(trunc_full, trunc_u8_full_x10);

    //  const auto packed_A_top_lanes = extract_lanes_8(packed_A_top);
    //  const auto packed_A_bot_lanes = extract_lanes_8(packed_A_bot);
    //  const auto packed_B_top_lanes = extract_lanes_8(packed_B_top);
    //  const auto packed_B_bot_lanes = extract_lanes_8(packed_B_bot);
    //  const auto trunc_A_lanes = extract_lanes_8(trunc_A);
    //  const auto trunc_B_lanes = extract_lanes_8(trunc_B);
    //  const auto trunc_full_to_sub_lanes = extract_lanes_8(trunc_full);
    //  const auto actual_res_lanes = extract_lanes_8(actual_res);

    return actual_res;
  }

  template <>
  uint32_t WriteCharsToPtrFowardReturnLength<uint64_t>(char *__restrict__ buff, const uint64_t &input)
  {
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

    const uint64_t fir_8 = Helpers::Math::Magic::Division::div_by_10_pow_n<12>(input);
    const uint64_t div_e8 = Helpers::Math::Magic::Division::div_by_10_pow_n<4>(input);

    const uint64_t mid_8 = div_e8 - (fir_8 * 100'000'000ULL);
    const uint16_t las_4 = input - (div_e8 * 10'000ULL);

    const __m256i val_u32 = _mm256_set_epi64x(mid_8, mid_8, fir_8, fir_8);
    const __m256i u_32_prod = _mm256_mul_epu32(val_u32, _mm256_set1_epi64x(0xD1B71759ULL));
    const __m256i u_32_prod_shf = _mm256_srli_epi64(u_32_prod, 45);
    const __m256i u_32_prod_shf_x10 = _mm256_mul_epu32(u_32_prod_shf, _mm256_set1_epi64x(10'000));

    const __m256i u_32_blend = _mm256_blend_epi32(u_32_prod_shf_x10, _mm256_set1_epi64x(0), 0b00110011);
    const __m256i u_32_to_sub = _mm256_blend_epi32(u_32_prod_shf, val_u32, 0b11001100);

    const __m256i u_32_res = _mm256_sub_epi64(u_32_to_sub, u_32_blend);

    const auto top_1 = static_cast<uint16_t>(_mm256_extract_epi64(u_32_res, 0));
    const auto top_2 = static_cast<uint16_t>(_mm256_extract_epi64(u_32_res, 1));
    const auto bot_1 = static_cast<uint16_t>(_mm256_extract_epi64(u_32_res, 2));
    const auto bot_2 = static_cast<uint16_t>(_mm256_extract_epi64(u_32_res, 3));

    const __m128i u_16_top_packed = _mm_blend_epi32(_mm_set1_epi32(bot_2), _mm_set1_epi32(bot_1), 0b0011);
    const __m128i u_16_bot_packed = _mm_blend_epi32(_mm_set1_epi32(top_2), _mm_set1_epi32(top_1), 0b0011);
    const __m256i u_16_packed = _mm256_set_m128i(u_16_top_packed, u_16_bot_packed);
    const __m128i u_16_packed_2 = _mm_set1_epi32(las_4);

    const __m256i u_16_prod = _mm256_mullo_epi32(u_16_packed, _mm256_set1_epi32(0xA3D8));
    const __m128i u_16_prod_2 = _mm_mullo_epi32(u_16_packed_2, _mm_set1_epi32(0xA3D8));
    const __m256i u_16_shf = _mm256_srli_epi32(u_16_prod, 22);
    const __m128i u_16_shf_2 = _mm_srli_epi32(u_16_prod_2, 22);
    const __m256i u_16_shf_x64 = _mm256_slli_epi32(u_16_shf, 6);
    const __m128i u_16_shf_2_x64 = _mm_slli_epi32(u_16_shf_2, 6);
    const __m256i u_16_shf_x32 = _mm256_slli_epi32(u_16_shf, 5);
    const __m128i u_16_shf_2_x32 = _mm_slli_epi32(u_16_shf_2, 5);
    const __m256i u_16_shf_added = _mm256_add_epi16(u_16_shf_x64, u_16_shf_x32);
    const __m128i u_16_shf_2_added = _mm_add_epi16(u_16_shf_2_x64, u_16_shf_2_x32);
    const __m256i u_16_shf_x4 = _mm256_slli_epi32(u_16_shf, 2);
    const __m128i u_16_shf_2_x4 = _mm_slli_epi32(u_16_shf_2, 2);
    const __m256i u_16_shf_x100 = _mm256_add_epi16(u_16_shf_added, u_16_shf_x4);
    const __m128i u_16_shf_2_x100 = _mm_add_epi16(u_16_shf_2_added, u_16_shf_2_x4);
    const __m256i u_16_blended = _mm256_blend_epi32(u_16_shf_x100, _mm256_set1_epi32(0), 0b0101'0101);
    const __m128i u_16_2_blended = _mm_blend_epi32(u_16_shf_2_x100, _mm_set1_epi32(0), 0b0101);

    const __m256i u_16_to_sub = _mm256_blend_epi32(u_16_packed, u_16_shf, 0b0101'0101);
    const __m128i u_16_to_sub_2 = _mm_blend_epi32(u_16_packed_2, u_16_shf_2, 0b0101);

    const __m256i u_16_res = _mm256_sub_epi32(u_16_to_sub, u_16_blended);
    const __m128i u_16_res_2 = _mm_sub_epi32(u_16_to_sub_2, u_16_2_blended);

    const __m256i u_16_res_packed = _mm256_packus_epi32(u_16_res, u_16_res);
    const __m128i u_16_res_packed_2 = _mm_packus_epi32(u_16_res_2, u_16_res_2);

    const __m256i a = _mm256_unpacklo_epi16(u_16_res_packed, u_16_res_packed);
    const __m256i b = _mm256_set_m128i(_mm_set1_epi32(0), _mm_unpacklo_epi16(u_16_res_packed_2, u_16_res_packed_2));

    const uint32_t bits = (sizeof(std::remove_cvref_t<decltype(input)>) * 8) - std::countl_zero(input);
    uint32_t len = (bits * 1233) >> 12;

    len += (input >= LEN_TABLE[len]);

    const uint16_t lead_z = std::numeric_limits<std::remove_cvref_t<decltype(input)>>::digits10 + 1 - len;

    const __m256i prod_A = _mm256_mullo_epi16(a, _mm256_set1_epi16(205));
    const __m256i prod_B = _mm256_mullo_epi16(b, _mm256_set1_epi16(205));

    const __m256i blend_A = _mm256_blend_epi16(prod_A, _mm256_set1_epi16(0), 0b1010'1010);
    const __m256i blend_B = _mm256_blend_epi16(prod_B, _mm256_set1_epi16(0), 0b1010'1010);

    const __m256i shf_A = _mm256_srli_epi32(blend_A, 11);
    const __m256i shf_B = _mm256_srli_epi32(blend_B, 11);

    const __m256i packed_A = _mm256_packus_epi32(shf_A, shf_A);
    const __m256i packed_B = _mm256_packus_epi32(shf_B, shf_B);

    const __m256i packed_unpack_A = _mm256_unpacklo_epi16(packed_A, packed_A);
    const __m256i packed_unpack_B = _mm256_unpacklo_epi16(packed_B, packed_B);

    const __m128i div_A_top = _mm256_extracti128_si256(packed_unpack_A, 0);
    const __m128i div_A_bot = _mm256_extracti128_si256(packed_unpack_A, 1);
    const __m128i div_B_top = _mm256_extracti128_si256(packed_unpack_B, 0);

    const __m128i packpacked_top_A = _mm_blend_epi16(div_A_top, _mm_set1_epi16(0), 0b0101'0101);
    const __m128i packpacked_bot_A = _mm_blend_epi16(div_A_bot, _mm_set1_epi16(0), 0b0101'0101);
    const __m128i trunc_u8_A = _mm_packus_epi16(packpacked_top_A, packpacked_bot_A);
    const __m128i packpacked_top_B = _mm_blend_epi16(div_B_top, _mm_set1_epi16(0), 0b0101'0101);
    const __m128i packpacked_bot_B = _mm_set1_epi32(0);
    const __m128i trunc_u8_B = _mm_packus_epi16(packpacked_top_B, packpacked_bot_B);

    const __m256i trunc_u8_full = _mm256_set_m128i(trunc_u8_B, trunc_u8_A);

    const __m256i trunc_u8_full_x8 = _mm256_slli_epi64(trunc_u8_full, 3);
    const __m256i trunc_u8_full_x2 = _mm256_slli_epi64(trunc_u8_full, 1);
    const __m256i trunc_u8_full_x10 = _mm256_add_epi8(trunc_u8_full_x8, trunc_u8_full_x2);

    const __m128i packed_A_top = _mm_blend_epi16(_mm256_extracti128_si256(a, 0), div_A_top, 0b0101'0101);
    const __m128i packed_A_bot = _mm_blend_epi16(_mm256_extracti128_si256(a, 1), div_A_bot, 0b0101'0101);
    const __m128i trunc_A = _mm_packus_epi16(packed_A_top, packed_A_bot);
    const __m128i packed_B_top = _mm_blend_epi16(_mm256_extracti128_si256(b, 0), div_B_top, 0b0101'0101);
    const __m128i packed_B_bot = _mm_set1_epi32(0);
    const __m128i trunc_B = _mm_packus_epi16(packed_B_top, packed_B_bot);

    const __m256i trunc_full = _mm256_set_m128i(trunc_B, trunc_A);

    const __m256i actual_res = _mm256_sub_epi8(trunc_full, trunc_u8_full_x10);

    const __m256i ascii_vec = _mm256_add_epi8(actual_res, _mm256_set1_epi8('0'));

    alignas(64) char temp[64];
    _mm256_store_si256(reinterpret_cast<__m256i *>(&temp[0]), ascii_vec);

    const __m256i output_chars = _mm256_loadu_si256(reinterpret_cast<const __m256i *>(&temp[0] + lead_z));

    _mm256_storeu_si256(reinterpret_cast<__m256i *>(buff), output_chars);

    return len;
  }

  template <>
  uint32_t WriteCharsToPtrFowardReturnLength<uint32_t>(char *__restrict__ buff, const uint32_t &input)
  {
    static const constexpr uint32_t table[] = { 0, 10, 100, 1'000, 10'000, 100'000, 1'000'000, 10'000'000, 100'000'000, 1'000'000'000 };

    const uint32_t bits = (sizeof(std::remove_cvref_t<decltype(input)>) * 8) - __builtin_clz(input);
    uint32_t len = (bits * 1233) >> 12;
    len += (input >= table[len]);
    const unsigned lead_z = 10 - len;

    const __m256i val = _mm256_set1_epi32(input);

    const __m256i M_MAGIC_u64 = { 0x55E63B89ULL, 0x431BDE83ULL, 0xD1B71759ULL, 0x51EB851FULL };
    const __m256i M_SHIFTS_u64 = { 57, 50, 45, 37 };

    const __m256i prod = _mm256_mul_epu32(val, M_MAGIC_u64);
    const __m256i shifted = _mm256_srlv_epi64(prod, M_SHIFTS_u64);
    const __m256i shifted_64 = _mm256_blend_epi32(_mm256_permutevar8x32_epi32(shifted, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)), val, 0b0011'0000);

    const __m256i shifted_64_x_64 = _mm256_slli_epi64(shifted_64, 6);
    const __m256i shifted_64_x_32 = _mm256_slli_epi64(shifted_64, 5);
    const __m256i shifted_64_x_4 = _mm256_slli_epi64(shifted_64, 2);

    const __m256i shifted_64_x_96 = _mm256_add_epi64(shifted_64_x_64, shifted_64_x_32);
    const __m256i shifted_64_x_100 = _mm256_add_epi64(shifted_64_x_96, shifted_64_x_4);

    const __m256i permuted_u64 = _mm256_permutevar8x32_epi32(shifted_64_x_100, _mm256_setr_epi32(7, 0, 1, 2, 3, 4, 5, 6));
    const __m256i res_u64 = _mm256_sub_epi64(shifted_64, permuted_u64);

    const __m256i shifted_16 = _mm256_slli_epi64(res_u64, 16);

    const __m256i res_packed = _mm256_or_si256(shifted_16, res_u64);

    const __m256i res_packed_x128 = _mm256_slli_epi16(res_packed, 7);
    const __m256i res_packed_x64 = _mm256_slli_epi16(res_packed, 6);
    const __m256i res_packed_x8 = _mm256_slli_epi16(res_packed, 3);
    const __m256i res_packed_x4 = _mm256_slli_epi16(res_packed, 2);

    const __m256i res_packed_x196 = _mm256_add_epi16(res_packed_x128, res_packed_x64);
    const __m256i res_packed_x12 = _mm256_add_epi16(res_packed_x8, res_packed_x4);

    const __m256i res_prod = _mm256_add_epi16(_mm256_add_epi16(res_packed_x196, res_packed_x12), res_packed);

    const __m256i res_shifted = _mm256_srli_epi16(res_prod, 11);

    const __m256i res_shifted_x8 = _mm256_slli_epi16(res_shifted, 3);
    const __m256i res_shifted_x2 = _mm256_slli_epi16(res_shifted, 1);

    const __m256i res_shifted_x10 = _mm256_add_epi16(res_shifted_x8, res_shifted_x2);

    const __m256i res_shifted_blended = _mm256_blend_epi16(res_shifted_x10, _mm256_set1_epi16(0), 0b0000'0000'0101'0101);
    const __m256i res_to_sub = _mm256_blend_epi16(res_shifted, res_packed, 0b0000'0010'10'10'10);
    const __m256i res_comb = _mm256_sub_epi16(res_to_sub, res_shifted_blended);

    const __m128i res_top = _mm256_extracti128_si256(res_comb, 0);
    const __m128i res_bot = _mm256_extracti128_si256(res_comb, 1);
    const __m128i trunc_u8 = _mm_packus_epi16(res_top, res_bot);

    const __m128i INDICES = _mm_setr_epi8(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);

    const __m128i ascii_vec = _mm_add_epi8(trunc_u8, _mm_set1_epi8('0'));
    const __m128i final_indices = _mm_add_epi8(INDICES, _mm_set1_epi8(lead_z));
    const __m128i output_chars = _mm_shuffle_epi8(ascii_vec, final_indices);

    _mm_storeu_si128(reinterpret_cast<__m128i *>(buff), output_chars);

    return len;
  }

  template <>
  uint32_t WriteCharsToPtrFowardReturnLength<uint16_t>(char *__restrict__ buff, const uint16_t &input)
  {
    // Branchless Length Calculation (Optimized for modern CPUs)
    static const constexpr uint32_t table[] = { 0, 10, 100, 1'000, 10'000 };

    const uint32_t bits = (sizeof(std::remove_cvref_t<decltype(input)>) * 8) - std::countl_zero(input);
    uint32_t len = (bits * 1233) >> 12;

    len += (input >= table[len]);
    const __m128i val = _mm_set1_epi16(input);

    const unsigned lead_z = std::numeric_limits<std::remove_cvref_t<decltype(input)>>::digits10 + 1 - len;
    const __m128i M_MAGIC_U16 = _mm_setr_epi16(0xA36F, 0x625, 0x47AF, 0x999A, 0, 0, 0, 0);
    const __m128i INDICES = _mm_setr_epi8(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);

    const __m128i prod = _mm_mulhi_epu16(val, M_MAGIC_U16);
    const __m128i n_sub_t_shf_add_t = _mm_add_epi16(_mm_srli_epi16(_mm_sub_epi16(val, prod), 1), prod);

    // FIX: Use AVX2 Variable Shifts to avoid memory round-trip we upcast to 32-bit to use _mm_srlv_epi32
    const __m128i shift_counts = _mm_setr_epi32(13, 9, 6, 3);
    const __m128i low_32 = _mm_cvtepu16_epi32(n_sub_t_shf_add_t);
    const __m128i shifted_32 = _mm_srlv_epi32(low_32, shift_counts);

    // Pack back to 16-bit and insert the original input into lane 4
    const __m128i res_vec = _mm_blend_epi32(_mm_packus_epi32(shifted_32, shifted_32), val, 0b1100);

    const __m128i res_times_x8 = _mm_slli_epi16(res_vec, 3);
    const __m128i res_times_x2 = _mm_slli_epi16(res_vec, 1);

    // Digit Extraction logic
    const __m128i res_times_10 = _mm_add_epi16(res_times_x8, res_times_x2);

    const __m128i res_slided = _mm_slli_si128(res_times_10, 2);
    const __m128i full_res = _mm_sub_epi16(res_vec, res_slided);

    // Table Lookup conversion to ASCII
    const __m128i ascii_vec = _mm_add_epi8(_mm_packus_epi16(full_res, _mm_setzero_si128()), _mm_set1_epi8('0'));
    const __m128i final_indices = _mm_add_epi8(INDICES, _mm_set1_epi8(static_cast<char>(lead_z)));
    const __m128i output_chars = _mm_shuffle_epi8(ascii_vec, final_indices);

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

#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <immintrin.h>
#include <iostream>

// Develop with the 'maybe2n_base' new approach to avoid mod % 10e8 when extracting digits and rather have them all ready for any k in [0, 1074]
// this may make short precision fromat slower but longer precision faster i belive....
// maybe its not a worthy tradeoff since most users will ask for [0, 24] digits of precision ...
// yeaaa i am not doing it bc not worth the time investment, i dont even know if this is actually faster vs RYU magic stuff

#if defined(__AVX512BW__) && defined(__AVX512VL__)

int main()
{
  constexpr uint32_t POW_5_CORRECTION[] = { 1, 10, 100, 1'000, 10'000, 100'000, 1'000'000, 10'000'000, 100'000'000 };
  constexpr uint8_t POW_5_IDX[] = { 0, 24, 48, 72, 96, 120, 144, 168, 192, 216, 240 };
  constexpr uint8_t pow_5_e[] = { 0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40 };

  constexpr uint32_t POW_5_CACHE[] = {
    1,          0,          0,          0,          0,          0,          0,          0, // 0 - _mm256i loaded and 0 extended to a _mm256i
    0,          0,          0,          0,          0,          0,          0,          0, // 1 - again
    0,          0,          0,          0,          0,          0,          0,          0, // 2 - again

    2242703233, 762134875,  1262,       0,          0,          0,          0,          0, // 0 - _mm256i loaded and 0 extended to a _mm256i
    0,          0,          0,          0,          0,          0,          0,          0, // 1 - again
    0,          0,          0,          0,          0,          0,          0,          0, // 2 - again

    3211403009, 1849224548, 3668416493, 3913284084, 1593091,    0,          0,          0, // 0 - _mm256i loaded and 0 extended to a _mm256i
    0,          0,          0,          0,          0,          0,          0,          0, // 1 - again
    0,          0,          0,          0,          0,          0,          0,          0, // 2 - again

    3776417409, 3833115195, 474402842,  2046101519, 1659368615, 1657637457, 2010764683, 0, // 0 - _mm256i loaded and 0 extended to a _mm256i
    0,          0,          0,          0,          0,          0,          0,          0, // 1 - again
    0,          0,          0,          0,          0,          0,          0,          0, // 2 - again

    781532673,  64985353,   253049085,  594863151,  3553621484, 3288652808, 3167596762, 2788392729, // 0 - _mm256i loaded and 0 extended to a _mm256i
    3911132675, 590,        0,          0,          0,          0,          0,          0,          // 1 - again -
    0,          0,          0,          0,          0,          0,          0,          0,          // 2 - again

    4223872385, 1378013428, 3071223888, 1460925171, 3695318035, 3594083709, 4166588825, 9582620, // 0 - _mm256i loaded and 0 extended to a _mm256i
    2259883222, 2057664441, 314048097,  745834,     0,          0,          0,          0,       // 1 - again -
    0,          0,          0,          0,          0,          0,          0,          0,       // 2 - again

    2894159105, 2850127391, 1678958885, 1990048638, 4055408710, 4272372864, 3869429580, 1940770817, // 0 - _mm256i loaded and 0 extended to a _mm256i
    3594030316, 3284427401, 4246652165, 2708404372, 1313461594, 941374947,  0,          0,          // 1 - again -
    0,          0,          0,          0,          0,          0,          0,          0,          // 2 - again

    3031420033, 1112378521, 855339782,  4191278750, 248038208,  1136169807, 2230652081, 3378214371, // 0 - _mm256i loaded and 0 extended to a _mm256i
    1653863564, 613940998,  1392057263, 562793371,  894754680,  2060902835, 2039635717, 2771255238, // 1 - again -
    276,        0,          0,          0,          0,          0,          0,          0,          // 2 - again

    2553183233, 3201533787, 3638140786, 303378311,  1809731782, 3477761648, 3583367183, 649228654,  // 0 - _mm256i loaded and 0 extended to a _mm256i
    2915460784, 487929380,  1011012442, 1677677582, 3428152256, 1710878487, 1438394610, 2161952759, // 1 - again -
    4100910556, 1608314830, 349175,     0,          0,          0,          0,          0,          // 2 - again

    3940379521, 184513341,  2872588323, 2214530454, 38258512,   2980860351, 114267010,  2188874685, // 0 - _mm256i loaded and 0 extended to a _mm512i
    234079247,  2101059099, 1948702207, 947446250,  864457656,  507589568,  1321007357, 3911984176, // 1 - again -
    1011110295, 2382358050, 2389730781, 730678769,  440721283,  0,          0,          0,          // 2 - again

    1352440577, 3353128433, 943566124,  1942375735, 3933038544, 1369155590, 3288593144, 2689786274, // 0 - _mm256i loaded and 0 extended to a _mm256i
    3608159034, 1149596585, 3474119098, 1887901699, 1390164635, 3853669096, 119012174,  3192759855, // 1 - again -
    179034081,  2355293519, 3468291195, 3563470881, 2702541568, 3437458918, 2217683442, 129,        // 2 - again
  };

  for(int k = 0; k < 1075; k++)
  {
  backwards:
    // 36 words * 32 bits = 1152 bits. perfectly fits k=1100 + 10^8 overflow.
    const unsigned P = std::floor(k * std::log10(2));

    const unsigned P_DIV_8 = P >> 3U;

    const unsigned P_DIV_32 = P >> 5U;

    const unsigned K_DIV_32 = static_cast<unsigned>(k) >> 5U;

    const __m512i ZERO = _mm512_setzero_si512();

    // total digits in 5^k
    const uint32_t TOTAL_DIGITS = std::floor(k * std::log10(5)) + 1;
    constexpr unsigned NUM_WORDS = 40;
    std::array<uint32_t, NUM_WORDS> SIMD_PRE_WRITTEN = { 0 };
    std::array<uint32_t, NUM_WORDS> SLOW_PRE_ARR = { 0 };

    std::array<uint64_t, 33> SIMD_ARRAY;
    _mm512_storeu_si512(reinterpret_cast<__m512i *>(&SIMD_ARRAY[0]), ZERO);
    _mm512_storeu_si512(reinterpret_cast<__m512i *>(&SIMD_ARRAY[8]), ZERO);
    _mm512_storeu_si512(reinterpret_cast<__m512i *>(&SIMD_ARRAY[16]), ZERO);
    _mm512_storeu_si512(reinterpret_cast<__m512i *>(&SIMD_ARRAY[24]), ZERO);

    __m512i rrprime_1 = _mm512_cvtepu32_epi64(_mm256_loadu_si256(reinterpret_cast<const __m256i *>(&POW_5_CACHE[POW_5_IDX[P_DIV_32]])));
    __m512i rrprime_2 = _mm512_cvtepu32_epi64(_mm256_loadu_si256(reinterpret_cast<const __m256i *>(&POW_5_CACHE[POW_5_IDX[P_DIV_32] + 8U])));
    __m512i rrprime_3 = _mm512_cvtepu32_epi64(_mm256_loadu_si256(reinterpret_cast<const __m256i *>(&POW_5_CACHE[POW_5_IDX[P_DIV_32] + 16U])));
    __m512i rrprime_4 = ZERO;
    const __m512i R1_E8 = _mm512_set1_epi64(POW_5_CORRECTION[8]);
    const __m512i R1_REMMISSING = _mm512_set1_epi64(POW_5_CORRECTION[P - (P_DIV_8 << 3U)]);

    const unsigned E_0 = pow_5_e[P_DIV_32];

    for(unsigned e = E_0; e < P_DIV_8; e++)
    {
      const __m512i u64_prod_1 = _mm512_mul_epu32(rrprime_1, R1_E8);
      const __m512i u64_prod_2 = _mm512_mul_epu32(rrprime_2, R1_E8);
      const __m512i u64_prod_3 = _mm512_mul_epu32(rrprime_3, R1_E8);
      const __m512i u64_prod_4 = _mm512_mul_epu32(rrprime_4, R1_E8);
      const __m512i u32_lo_prod_1 = _mm512_mask_blend_epi32(0b1010'1010'1010'1010, u64_prod_1, ZERO);
      const __m512i u32_lo_prod_2 = _mm512_mask_blend_epi32(0b1010'1010'1010'1010, u64_prod_2, ZERO);
      const __m512i u32_lo_prod_3 = _mm512_mask_blend_epi32(0b1010'1010'1010'1010, u64_prod_3, ZERO);
      const __m512i u32_lo_prod_4 = _mm512_mask_blend_epi32(0b1010'1010'1010'1010, u64_prod_4, ZERO);
      const __m512i u32_hi_prod_1 = _mm512_srli_epi64(u64_prod_1, 32);
      const __m512i u32_hi_prod_2 = _mm512_srli_epi64(u64_prod_2, 32);
      const __m512i u32_hi_prod_3 = _mm512_srli_epi64(u64_prod_3, 32);
      const __m512i u32_hi_prod_4 = _mm512_srli_epi64(u64_prod_4, 32);
      const __m512i u32_hi_prod_slide_1 = _mm512_alignr_epi64(u32_hi_prod_1, ZERO, 7);
      const __m512i u32_hi_prod_slide_2 = _mm512_alignr_epi64(u32_hi_prod_2, u32_hi_prod_1, 7);
      const __m512i u32_hi_prod_slide_3 = _mm512_alignr_epi64(u32_hi_prod_3, u32_hi_prod_2, 7);
      const __m512i u32_hi_prod_slide_4 = _mm512_alignr_epi64(u32_hi_prod_4, u32_hi_prod_3, 7);
      rrprime_1 = _mm512_add_epi64(u32_lo_prod_1, u32_hi_prod_slide_1);
      rrprime_2 = _mm512_add_epi64(u32_lo_prod_2, u32_hi_prod_slide_2);
      rrprime_3 = _mm512_add_epi64(u32_lo_prod_3, u32_hi_prod_slide_3);
      rrprime_4 = _mm512_add_epi64(u32_lo_prod_4, u32_hi_prod_slide_4);
      const __m512i rrprime_slide_1 = _mm512_srli_epi64(_mm512_alignr_epi64(rrprime_1, ZERO, 7), 32);
      const __m512i rrprime_slide_2 = _mm512_srli_epi64(_mm512_alignr_epi64(rrprime_2, rrprime_1, 7), 32);
      const __m512i rrprime_slide_3 = _mm512_srli_epi64(_mm512_alignr_epi64(rrprime_3, rrprime_2, 7), 32);
      const __m512i rrprime_slide_4 = _mm512_srli_epi64(_mm512_alignr_epi64(rrprime_4, rrprime_3, 7), 32);
      rrprime_1 = _mm512_add_epi32(rrprime_1, rrprime_slide_1);
      rrprime_2 = _mm512_add_epi32(rrprime_2, rrprime_slide_2);
      rrprime_3 = _mm512_add_epi32(rrprime_3, rrprime_slide_3);
      rrprime_4 = _mm512_add_epi32(rrprime_4, rrprime_slide_4);
    }
    {
      const __m512i u64_prod_1 = _mm512_mul_epu32(rrprime_1, R1_REMMISSING);
      const __m512i u64_prod_2 = _mm512_mul_epu32(rrprime_2, R1_REMMISSING);
      const __m512i u64_prod_3 = _mm512_mul_epu32(rrprime_3, R1_REMMISSING);
      const __m512i u64_prod_4 = _mm512_mul_epu32(rrprime_4, R1_REMMISSING);
      const __m512i u32_lo_prod_1 = _mm512_mask_blend_epi32(0b1010'1010'1010'1010, u64_prod_1, ZERO);
      const __m512i u32_lo_prod_2 = _mm512_mask_blend_epi32(0b1010'1010'1010'1010, u64_prod_2, ZERO);
      const __m512i u32_lo_prod_3 = _mm512_mask_blend_epi32(0b1010'1010'1010'1010, u64_prod_3, ZERO);
      const __m512i u32_lo_prod_4 = _mm512_mask_blend_epi32(0b1010'1010'1010'1010, u64_prod_4, ZERO);
      const __m512i u32_hi_prod_1 = _mm512_srli_epi64(u64_prod_1, 32);
      const __m512i u32_hi_prod_2 = _mm512_srli_epi64(u64_prod_2, 32);
      const __m512i u32_hi_prod_3 = _mm512_srli_epi64(u64_prod_3, 32);
      const __m512i u32_hi_prod_4 = _mm512_srli_epi64(u64_prod_4, 32);
      const __m512i u32_hi_prod_slide_1 = _mm512_alignr_epi64(u32_hi_prod_1, ZERO, 7);
      const __m512i u32_hi_prod_slide_2 = _mm512_alignr_epi64(u32_hi_prod_2, u32_hi_prod_1, 7);
      const __m512i u32_hi_prod_slide_3 = _mm512_alignr_epi64(u32_hi_prod_3, u32_hi_prod_2, 7);
      const __m512i u32_hi_prod_slide_4 = _mm512_alignr_epi64(u32_hi_prod_4, u32_hi_prod_3, 7);
      rrprime_1 = _mm512_add_epi32(u32_lo_prod_1, u32_hi_prod_slide_1);
      rrprime_2 = _mm512_add_epi32(u32_lo_prod_2, u32_hi_prod_slide_2);
      rrprime_3 = _mm512_add_epi32(u32_lo_prod_3, u32_hi_prod_slide_3);
      rrprime_4 = _mm512_add_epi32(u32_lo_prod_4, u32_hi_prod_slide_4);
      const __m512i rrprime_slide_1 = _mm512_srli_epi64(_mm512_alignr_epi64(rrprime_1, ZERO, 7), 32);
      const __m512i rrprime_slide_2 = _mm512_srli_epi64(_mm512_alignr_epi64(rrprime_2, rrprime_1, 7), 32);
      const __m512i rrprime_slide_3 = _mm512_srli_epi64(_mm512_alignr_epi64(rrprime_3, rrprime_2, 7), 32);
      const __m512i rrprime_slide_4 = _mm512_srli_epi64(_mm512_alignr_epi64(rrprime_4, rrprime_3, 7), 32);
      rrprime_1 = _mm512_add_epi32(rrprime_1, rrprime_slide_1);
      rrprime_2 = _mm512_add_epi32(rrprime_2, rrprime_slide_2);
      rrprime_3 = _mm512_add_epi32(rrprime_3, rrprime_slide_3);
      rrprime_4 = _mm512_add_epi32(rrprime_4, rrprime_slide_4);
    }

    _mm512_storeu_si512(reinterpret_cast<__m512i *>(&SIMD_ARRAY[0]), rrprime_1);
    _mm512_storeu_si512(reinterpret_cast<__m512i *>(&SIMD_ARRAY[8]), rrprime_2);
    _mm512_storeu_si512(reinterpret_cast<__m512i *>(&SIMD_ARRAY[16]), rrprime_3);
    _mm512_storeu_si512(reinterpret_cast<__m512i *>(&SIMD_ARRAY[24]), rrprime_4);

    {
      const __m256i u32_pack_1 = _mm512_cvtepi64_epi32(rrprime_1);
      const __m256i u32_pack_2 = _mm512_cvtepi64_epi32(rrprime_2);
      const __m256i u32_pack_3 = _mm512_cvtepi64_epi32(rrprime_3);
      const __m256i u32_pack_4 = _mm512_cvtepi64_epi32(rrprime_4);

      _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_PRE_WRITTEN[0]), u32_pack_1);
      _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_PRE_WRITTEN[8]), u32_pack_2);
      _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_PRE_WRITTEN[16]), u32_pack_3);
      _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_PRE_WRITTEN[24]), u32_pack_4);
    }

    SLOW_PRE_ARR[0] = 1; // initialize r = 1

    for(uint32_t i = 0; i < P; ++i)
    {
      uint32_t carry = 0;
      for(unsigned int &w : SLOW_PRE_ARR)
      {
        uint64_t pp = (uint64_t)w * 10ull + carry;
        w = (uint32_t)pp;
        carry = (uint32_t)(pp >> 32);
      }
    }

    std::array<uint32_t, NUM_WORDS> x10byx10_arr = SLOW_PRE_ARR;

    size_t first = 0;

    while(first < SLOW_PRE_ARR.size() && SLOW_PRE_ARR[first] == 0)
    {
      ++first;
    }

    if(first != 0 && first != SLOW_PRE_ARR.size())
    {
      const size_t count = SLOW_PRE_ARR.size() - first;

      std::memmove(SLOW_PRE_ARR.data(), SLOW_PRE_ARR.data() + first, count * sizeof(uint32_t));

      std::memset(SLOW_PRE_ARR.data() + count, 0, first * sizeof(uint32_t));
    }

    if(SIMD_PRE_WRITTEN != SLOW_PRE_ARR)
    {
      std::cout << "k = " << k << "; p = " << P << '\n';
      goto backwards;
    }

    assert(SIMD_PRE_WRITTEN == SLOW_PRE_ARR);

    // unsigned lead_z = 24u - (k_div_32 - ((k * 1233u) >> 17u)) - (k < 32); //+ (k_div_32 & 0b0111);

    unsigned digits_computed = 0;
    unsigned word_idx = k / 32u;
    unsigned simdy_idx = (K_DIV_32 - ((k * 1233U) >> 17U));
    uint64_t chunk = 0;
    unsigned next_8;
    unsigned simdy_8;

    const unsigned bit_shift = k & ((1U << 5U) - 1);
    const unsigned mod_mask = (bit_shift == 0) ? 0 : ((1U << bit_shift) - 1);

    while(digits_computed < TOTAL_DIGITS)
    {
      // step a & b: multiply by 10^8 and cascade the carry
      uint32_t carry = 0; // carry max is 10^8, fits safely in uint32_t
      for(unsigned int &w : x10byx10_arr)
      {
        // max size: (2^32 - 1) * 10^8 + 10^8 = 2^32 * 10^8. fits comfortably in uint64_t.
        uint64_t pp = (uint64_t)w * 100'000'000u + carry;
        w = (uint32_t)pp;
        carry = (uint32_t)(pp >> 32);
      }

      {
        __m512i rrrprime_1 = _mm512_loadu_si512(reinterpret_cast<__m512i *>(&SIMD_ARRAY[0]));
        __m512i rrrprime_2 = _mm512_loadu_si512(reinterpret_cast<__m512i *>(&SIMD_ARRAY[8]));
        __m512i rrrprime_3 = _mm512_loadu_si512(reinterpret_cast<__m512i *>(&SIMD_ARRAY[16]));
        __m512i rrrprime_4 = _mm512_loadu_si512(reinterpret_cast<__m512i *>(&SIMD_ARRAY[24]));
        const __m512i u64_prod_1 = _mm512_mul_epu32(rrrprime_1, R1_E8);
        const __m512i u64_prod_2 = _mm512_mul_epu32(rrrprime_2, R1_E8);
        const __m512i u64_prod_3 = _mm512_mul_epu32(rrrprime_3, R1_E8);
        const __m512i u64_prod_4 = _mm512_mul_epu32(rrrprime_4, R1_E8);
        const __m512i u32_lo_prod_1 = _mm512_mask_blend_epi32(0b1010'1010'1010'1010, u64_prod_1, ZERO);
        const __m512i u32_lo_prod_2 = _mm512_mask_blend_epi32(0b1010'1010'1010'1010, u64_prod_2, ZERO);
        const __m512i u32_lo_prod_3 = _mm512_mask_blend_epi32(0b1010'1010'1010'1010, u64_prod_3, ZERO);
        const __m512i u32_lo_prod_4 = _mm512_mask_blend_epi32(0b1010'1010'1010'1010, u64_prod_4, ZERO);
        const __m512i u32_hi_prod_1 = _mm512_srli_epi64(u64_prod_1, 32);
        const __m512i u32_hi_prod_2 = _mm512_srli_epi64(u64_prod_2, 32);
        const __m512i u32_hi_prod_3 = _mm512_srli_epi64(u64_prod_3, 32);
        const __m512i u32_hi_prod_4 = _mm512_srli_epi64(u64_prod_4, 32);
        const __m512i u32_hi_prod_slide_1 = _mm512_alignr_epi64(u32_hi_prod_1, ZERO, 7);
        const __m512i u32_hi_prod_slide_2 = _mm512_alignr_epi64(u32_hi_prod_2, u32_hi_prod_1, 7);
        const __m512i u32_hi_prod_slide_3 = _mm512_alignr_epi64(u32_hi_prod_3, u32_hi_prod_2, 7);
        const __m512i u32_hi_prod_slide_4 = _mm512_alignr_epi64(u32_hi_prod_4, u32_hi_prod_3, 7);
        rrrprime_1 = _mm512_add_epi64(u32_lo_prod_1, u32_hi_prod_slide_1);
        rrrprime_2 = _mm512_add_epi64(u32_lo_prod_2, u32_hi_prod_slide_2);
        rrrprime_3 = _mm512_add_epi64(u32_lo_prod_3, u32_hi_prod_slide_3);
        rrrprime_4 = _mm512_add_epi64(u32_lo_prod_4, u32_hi_prod_slide_4);
        const auto tmp = _mm512_alignr_epi64(rrrprime_1, ZERO, 7);
        const auto tmp_shf = _mm512_srli_epi64(tmp, 32);
        const __m512i rrprime_slide_1 = _mm512_srli_epi64(_mm512_alignr_epi64(rrrprime_1, ZERO, 7), 32);
        const __m512i rrprime_slide_2 = _mm512_srli_epi64(_mm512_alignr_epi64(rrrprime_2, rrrprime_1, 7), 32);
        const __m512i rrprime_slide_3 = _mm512_srli_epi64(_mm512_alignr_epi64(rrrprime_3, rrrprime_2, 7), 32);
        const __m512i rrprime_slide_4 = _mm512_srli_epi64(_mm512_alignr_epi64(rrrprime_4, rrrprime_3, 7), 32);
        rrrprime_1 = _mm512_add_epi32(rrrprime_1, rrprime_slide_1);
        rrrprime_2 = _mm512_add_epi32(rrrprime_2, rrprime_slide_2);
        rrrprime_3 = _mm512_add_epi32(rrrprime_3, rrprime_slide_3);
        rrrprime_4 = _mm512_add_epi32(rrrprime_4, rrprime_slide_4);
        rrrprime_1 = _mm512_mask_blend_epi32(0b1010'1010'1010'1010, rrrprime_1, ZERO);
        rrrprime_2 = _mm512_mask_blend_epi32(0b1010'1010'1010'1010, rrrprime_2, ZERO);
        rrrprime_3 = _mm512_mask_blend_epi32(0b1010'1010'1010'1010, rrrprime_3, ZERO);
        rrrprime_4 = _mm512_mask_blend_epi32(0b1010'1010'1010'1010, rrrprime_4, ZERO);
        _mm512_storeu_si512(reinterpret_cast<__m512i *>(&SIMD_ARRAY[0]), rrrprime_1);
        _mm512_storeu_si512(reinterpret_cast<__m512i *>(&SIMD_ARRAY[8]), rrrprime_2);
        _mm512_storeu_si512(reinterpret_cast<__m512i *>(&SIMD_ARRAY[16]), rrrprime_3);
        _mm512_storeu_si512(reinterpret_cast<__m512i *>(&SIMD_ARRAY[24]), rrrprime_4);
      }

      // step c: extract next8 chunk (r >> k)
      chunk = x10byx10_arr[word_idx] >> bit_shift;
      // cast to uint64_t before shifting to prevent any 32-bit boundary overflow
      chunk |= ((uint64_t)x10byx10_arr[word_idx + 1] << (32 - bit_shift));
      next_8 = (uint32_t)chunk;

      x10byx10_arr[word_idx] &= mod_mask;
      std::memset(static_cast<void *>(&x10byx10_arr[word_idx + 1]), 0, (NUM_WORDS - word_idx - 1) * 4);

      chunk = SIMD_ARRAY[simdy_idx] >> bit_shift;
      chunk |= (SIMD_ARRAY[simdy_idx + 1] << (32 - bit_shift));
      simdy_8 = (uint32_t)chunk;
      SIMD_ARRAY[simdy_idx] &= mod_mask;
      _mm512_storeu_si512(reinterpret_cast<__m512i *>(&SIMD_ARRAY[simdy_idx + 1]), ZERO);

      if(simdy_8 != next_8)
      {
        std::cout << "k = " << k << "; p = " << P << '\n';
        goto backwards;
      }

      assert(simdy_8 == next_8);

      // step e: print immediately
      // std::cout << std::setfill('0') << std::setw(8) << simdy_8;

      digits_computed += 8;
    }
    //
  }

  return 0;
}

#elif defined(__AVX2__)

int main()
{
  constexpr uint32_t POW_5_CORRECTION[] = { 1, 10, 100, 1'000, 10'000, 100'000, 1'000'000, 10'000'000, 100'000'000 };
  constexpr uint8_t POW_5_IDX[] = { 0, 24, 48, 72, 96, 120, 144, 168, 192, 216, 240 };
  constexpr uint8_t pow_5_e[] = { 0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40 };

  constexpr uint32_t POW_5_CACHE[] = {
    1,          0,          0,          0,          0,          0,          0,          0, // 0 - _mm256i loaded and 0 extended to a _mm256i
    0,          0,          0,          0,          0,          0,          0,          0, // 1 - again
    0,          0,          0,          0,          0,          0,          0,          0, // 2 - again

    2242703233, 762134875,  1262,       0,          0,          0,          0,          0, // 0 - _mm256i loaded and 0 extended to a _mm256i
    0,          0,          0,          0,          0,          0,          0,          0, // 1 - again
    0,          0,          0,          0,          0,          0,          0,          0, // 2 - again

    3211403009, 1849224548, 3668416493, 3913284084, 1593091,    0,          0,          0, // 0 - _mm256i loaded and 0 extended to a _mm256i
    0,          0,          0,          0,          0,          0,          0,          0, // 1 - again
    0,          0,          0,          0,          0,          0,          0,          0, // 2 - again

    3776417409, 3833115195, 474402842,  2046101519, 1659368615, 1657637457, 2010764683, 0, // 0 - _mm256i loaded and 0 extended to a _mm256i
    0,          0,          0,          0,          0,          0,          0,          0, // 1 - again
    0,          0,          0,          0,          0,          0,          0,          0, // 2 - again

    781532673,  64985353,   253049085,  594863151,  3553621484, 3288652808, 3167596762, 2788392729, // 0 - _mm256i loaded and 0 extended to a _mm256i
    3911132675, 590,        0,          0,          0,          0,          0,          0,          // 1 - again -
    0,          0,          0,          0,          0,          0,          0,          0,          // 2 - again

    4223872385, 1378013428, 3071223888, 1460925171, 3695318035, 3594083709, 4166588825, 9582620, // 0 - _mm256i loaded and 0 extended to a _mm256i
    2259883222, 2057664441, 314048097,  745834,     0,          0,          0,          0,       // 1 - again -
    0,          0,          0,          0,          0,          0,          0,          0,       // 2 - again

    2894159105, 2850127391, 1678958885, 1990048638, 4055408710, 4272372864, 3869429580, 1940770817, // 0 - _mm256i loaded and 0 extended to a _mm256i
    3594030316, 3284427401, 4246652165, 2708404372, 1313461594, 941374947,  0,          0,          // 1 - again -
    0,          0,          0,          0,          0,          0,          0,          0,          // 2 - again

    3031420033, 1112378521, 855339782,  4191278750, 248038208,  1136169807, 2230652081, 3378214371, // 0 - _mm256i loaded and 0 extended to a _mm256i
    1653863564, 613940998,  1392057263, 562793371,  894754680,  2060902835, 2039635717, 2771255238, // 1 - again -
    276,        0,          0,          0,          0,          0,          0,          0,          // 2 - again

    2553183233, 3201533787, 3638140786, 303378311,  1809731782, 3477761648, 3583367183, 649228654,  // 0 - _mm256i loaded and 0 extended to a _mm256i
    2915460784, 487929380,  1011012442, 1677677582, 3428152256, 1710878487, 1438394610, 2161952759, // 1 - again -
    4100910556, 1608314830, 349175,     0,          0,          0,          0,          0,          // 2 - again

    3940379521, 184513341,  2872588323, 2214530454, 38258512,   2980860351, 114267010,  2188874685, // 0 - _mm256i loaded and 0 extended to a _mm512i
    234079247,  2101059099, 1948702207, 947446250,  864457656,  507589568,  1321007357, 3911984176, // 1 - again -
    1011110295, 2382358050, 2389730781, 730678769,  440721283,  0,          0,          0,          // 2 - again

    1352440577, 3353128433, 943566124,  1942375735, 3933038544, 1369155590, 3288593144, 2689786274, // 0 - _mm256i loaded and 0 extended to a _mm256i
    3608159034, 1149596585, 3474119098, 1887901699, 1390164635, 3853669096, 119012174,  3192759855, // 1 - again -
    179034081,  2355293519, 3468291195, 3563470881, 2702541568, 3437458918, 2217683442, 129,        // 2 - again
  };

  for(int k = 0; k < 1075; k++)
  {
  backwards:
    // 36 words * 32 bits = 1152 bits. perfectly fits k=1100 + 10^8 overflow.
    const unsigned P = std::floor(k * std::log10(2));

    const unsigned P_DIV_8 = P >> 3U;

    const unsigned P_DIV_32 = P >> 5U;

    const unsigned K_DIV_32 = static_cast<unsigned>(k) >> 5U;

    const __m256i ZERO = _mm256_setzero_si256();

    // total digits in 5^k
    const uint32_t TOTAL_DIGITS = std::floor(k * std::log10(5)) + 1;
    constexpr unsigned NUM_WORDS = 40;
    std::array<uint32_t, NUM_WORDS> SIMD_PRE_WRITTEN = { 0 };
    std::array<uint32_t, NUM_WORDS> SLOW_PRE_ARR = { 0 };

    std::array<uint64_t, 29> SIMD_ARRAY; // 29 to avoid UB when word_idx + 1 in simd mod write 0
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[0]), ZERO);
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[4]), ZERO);
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[8]), ZERO);
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[12]), ZERO);
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[16]), ZERO);
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[20]), ZERO);
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[24]), ZERO);
    SIMD_ARRAY[28] = 0;

    __m256i rrprime_1 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&POW_5_CACHE[POW_5_IDX[P_DIV_32]])));
    __m256i rrprime_2 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&POW_5_CACHE[POW_5_IDX[P_DIV_32] + 4U])));
    __m256i rrprime_3 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&POW_5_CACHE[POW_5_IDX[P_DIV_32] + 8U])));
    __m256i rrprime_4 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&POW_5_CACHE[POW_5_IDX[P_DIV_32] + 12U])));
    __m256i rrprime_5 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&POW_5_CACHE[POW_5_IDX[P_DIV_32] + 16U])));
    __m256i rrprime_6 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&POW_5_CACHE[POW_5_IDX[P_DIV_32] + 20U])));
    __m256i rrprime_7 = ZERO;
    const __m256i R1_E8 = _mm256_set1_epi64x(POW_5_CORRECTION[8]);
    const __m256i R1_REMMISSING = _mm256_set1_epi64x(POW_5_CORRECTION[P - (P_DIV_8 << 3U)]);

    const unsigned e_o = pow_5_e[P_DIV_32];

    for(unsigned e = e_o; e < P_DIV_8; e++)
    {
      const __m256i u64_prod_1 = _mm256_mul_epu32(rrprime_1, R1_E8);
      const __m256i u64_prod_2 = _mm256_mul_epu32(rrprime_2, R1_E8);
      const __m256i u64_prod_3 = _mm256_mul_epu32(rrprime_3, R1_E8);
      const __m256i u64_prod_4 = _mm256_mul_epu32(rrprime_4, R1_E8);
      const __m256i u64_prod_5 = _mm256_mul_epu32(rrprime_5, R1_E8);
      const __m256i u64_prod_6 = _mm256_mul_epu32(rrprime_6, R1_E8);
      const __m256i u64_prod_7 = _mm256_mul_epu32(rrprime_7, R1_E8);
      const __m256i u32_lo_prod_1 = _mm256_blend_epi32(u64_prod_1, ZERO, 0b1010'1010);
      const __m256i u32_lo_prod_2 = _mm256_blend_epi32(u64_prod_2, ZERO, 0b1010'1010);
      const __m256i u32_lo_prod_3 = _mm256_blend_epi32(u64_prod_3, ZERO, 0b1010'1010);
      const __m256i u32_lo_prod_4 = _mm256_blend_epi32(u64_prod_4, ZERO, 0b1010'1010);
      const __m256i u32_lo_prod_5 = _mm256_blend_epi32(u64_prod_5, ZERO, 0b1010'1010);
      const __m256i u32_lo_prod_6 = _mm256_blend_epi32(u64_prod_6, ZERO, 0b1010'1010);
      const __m256i u32_lo_prod_7 = _mm256_blend_epi32(u64_prod_7, ZERO, 0b1010'1010);
      const __m256i u32_hi_prod_1 = _mm256_srli_epi64(u64_prod_1, 32);
      const __m256i u32_hi_prod_2 = _mm256_srli_epi64(u64_prod_2, 32);
      const __m256i u32_hi_prod_3 = _mm256_srli_epi64(u64_prod_3, 32);
      const __m256i u32_hi_prod_4 = _mm256_srli_epi64(u64_prod_4, 32);
      const __m256i u32_hi_prod_5 = _mm256_srli_epi64(u64_prod_5, 32);
      const __m256i u32_hi_prod_6 = _mm256_srli_epi64(u64_prod_6, 32);
      const __m256i u32_hi_prod_7 = _mm256_srli_epi64(u64_prod_7, 32);
      const __m256i u32_hi_prod_perm_1 = _mm256_permute4x64_epi64(u32_hi_prod_1, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_2 = _mm256_permute4x64_epi64(u32_hi_prod_2, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_3 = _mm256_permute4x64_epi64(u32_hi_prod_3, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_4 = _mm256_permute4x64_epi64(u32_hi_prod_4, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_5 = _mm256_permute4x64_epi64(u32_hi_prod_5, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_6 = _mm256_permute4x64_epi64(u32_hi_prod_6, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_7 = _mm256_permute4x64_epi64(u32_hi_prod_7, 0b10'01'00'11);
      const __m256i u32_hi_prod_slide_1 = _mm256_blend_epi32(u32_hi_prod_perm_1, ZERO, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_2 = _mm256_blend_epi32(u32_hi_prod_perm_2, u32_hi_prod_perm_1, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_3 = _mm256_blend_epi32(u32_hi_prod_perm_3, u32_hi_prod_perm_2, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_4 = _mm256_blend_epi32(u32_hi_prod_perm_4, u32_hi_prod_perm_3, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_5 = _mm256_blend_epi32(u32_hi_prod_perm_5, u32_hi_prod_perm_4, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_6 = _mm256_blend_epi32(u32_hi_prod_perm_6, u32_hi_prod_perm_5, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_7 = _mm256_blend_epi32(u32_hi_prod_perm_7, u32_hi_prod_perm_6, 0b00'00'00'11);
      rrprime_1 = _mm256_add_epi64(u32_lo_prod_1, u32_hi_prod_slide_1);
      rrprime_2 = _mm256_add_epi64(u32_lo_prod_2, u32_hi_prod_slide_2);
      rrprime_3 = _mm256_add_epi64(u32_lo_prod_3, u32_hi_prod_slide_3);
      rrprime_4 = _mm256_add_epi64(u32_lo_prod_4, u32_hi_prod_slide_4);
      rrprime_5 = _mm256_add_epi64(u32_lo_prod_5, u32_hi_prod_slide_5);
      rrprime_6 = _mm256_add_epi64(u32_lo_prod_6, u32_hi_prod_slide_6);
      rrprime_7 = _mm256_add_epi64(u32_lo_prod_7, u32_hi_prod_slide_7);
      const __m256i rrprime_perm_1 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_1, 0b10'01'00'11), 32U);
      const __m256i rrprime_perm_2 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_2, 0b10'01'00'11), 32U);
      const __m256i rrprime_perm_3 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_3, 0b10'01'00'11), 32U);
      const __m256i rrprime_perm_4 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_4, 0b10'01'00'11), 32U);
      const __m256i rrprime_perm_5 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_5, 0b10'01'00'11), 32U);
      const __m256i rrprime_perm_6 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_6, 0b10'01'00'11), 32U);
      const __m256i rrprime_perm_7 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_7, 0b10'01'00'11), 32U);
      const __m256i rrprime_slide_1 = _mm256_blend_epi32(rrprime_perm_1, ZERO, 0b00'00'00'11);
      const __m256i rrprime_slide_2 = _mm256_blend_epi32(rrprime_perm_2, rrprime_perm_1, 0b00'00'00'11);
      const __m256i rrprime_slide_3 = _mm256_blend_epi32(rrprime_perm_3, rrprime_perm_2, 0b00'00'00'11);
      const __m256i rrprime_slide_4 = _mm256_blend_epi32(rrprime_perm_4, rrprime_perm_3, 0b00'00'00'11);
      const __m256i rrprime_slide_5 = _mm256_blend_epi32(rrprime_perm_5, rrprime_perm_4, 0b00'00'00'11);
      const __m256i rrprime_slide_6 = _mm256_blend_epi32(rrprime_perm_6, rrprime_perm_5, 0b00'00'00'11);
      const __m256i rrprime_slide_7 = _mm256_blend_epi32(rrprime_perm_7, rrprime_perm_6, 0b00'00'00'11);
      rrprime_1 = _mm256_add_epi32(rrprime_1, rrprime_slide_1);
      rrprime_2 = _mm256_add_epi32(rrprime_2, rrprime_slide_2);
      rrprime_3 = _mm256_add_epi32(rrprime_3, rrprime_slide_3);
      rrprime_4 = _mm256_add_epi32(rrprime_4, rrprime_slide_4);
      rrprime_5 = _mm256_add_epi32(rrprime_5, rrprime_slide_5);
      rrprime_6 = _mm256_add_epi32(rrprime_6, rrprime_slide_6);
      rrprime_7 = _mm256_add_epi32(rrprime_7, rrprime_slide_7);
    }
    {
      const __m256i u64_prod_1 = _mm256_mul_epu32(rrprime_1, R1_REMMISSING);
      const __m256i u64_prod_2 = _mm256_mul_epu32(rrprime_2, R1_REMMISSING);
      const __m256i u64_prod_3 = _mm256_mul_epu32(rrprime_3, R1_REMMISSING);
      const __m256i u64_prod_4 = _mm256_mul_epu32(rrprime_4, R1_REMMISSING);
      const __m256i u64_prod_5 = _mm256_mul_epu32(rrprime_5, R1_REMMISSING);
      const __m256i u64_prod_6 = _mm256_mul_epu32(rrprime_6, R1_REMMISSING);
      const __m256i u64_prod_7 = _mm256_mul_epu32(rrprime_7, R1_REMMISSING);
      const __m256i u32_lo_prod_1 = _mm256_blend_epi32(u64_prod_1, ZERO, 0b1010'1010);
      const __m256i u32_lo_prod_2 = _mm256_blend_epi32(u64_prod_2, ZERO, 0b1010'1010);
      const __m256i u32_lo_prod_3 = _mm256_blend_epi32(u64_prod_3, ZERO, 0b1010'1010);
      const __m256i u32_lo_prod_4 = _mm256_blend_epi32(u64_prod_4, ZERO, 0b1010'1010);
      const __m256i u32_lo_prod_5 = _mm256_blend_epi32(u64_prod_5, ZERO, 0b1010'1010);
      const __m256i u32_lo_prod_6 = _mm256_blend_epi32(u64_prod_6, ZERO, 0b1010'1010);
      const __m256i u32_lo_prod_7 = _mm256_blend_epi32(u64_prod_7, ZERO, 0b1010'1010);
      const __m256i u32_hi_prod_1 = _mm256_srli_epi64(u64_prod_1, 32);
      const __m256i u32_hi_prod_2 = _mm256_srli_epi64(u64_prod_2, 32);
      const __m256i u32_hi_prod_3 = _mm256_srli_epi64(u64_prod_3, 32);
      const __m256i u32_hi_prod_4 = _mm256_srli_epi64(u64_prod_4, 32);
      const __m256i u32_hi_prod_5 = _mm256_srli_epi64(u64_prod_5, 32);
      const __m256i u32_hi_prod_6 = _mm256_srli_epi64(u64_prod_6, 32);
      const __m256i u32_hi_prod_7 = _mm256_srli_epi64(u64_prod_7, 32);
      const __m256i u32_hi_prod_perm_1 = _mm256_permute4x64_epi64(u32_hi_prod_1, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_2 = _mm256_permute4x64_epi64(u32_hi_prod_2, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_3 = _mm256_permute4x64_epi64(u32_hi_prod_3, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_4 = _mm256_permute4x64_epi64(u32_hi_prod_4, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_5 = _mm256_permute4x64_epi64(u32_hi_prod_5, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_6 = _mm256_permute4x64_epi64(u32_hi_prod_6, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_7 = _mm256_permute4x64_epi64(u32_hi_prod_7, 0b10'01'00'11);
      const __m256i u32_hi_prod_slide_1 = _mm256_blend_epi32(u32_hi_prod_perm_1, ZERO, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_2 = _mm256_blend_epi32(u32_hi_prod_perm_2, u32_hi_prod_perm_1, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_3 = _mm256_blend_epi32(u32_hi_prod_perm_3, u32_hi_prod_perm_2, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_4 = _mm256_blend_epi32(u32_hi_prod_perm_4, u32_hi_prod_perm_3, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_5 = _mm256_blend_epi32(u32_hi_prod_perm_5, u32_hi_prod_perm_4, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_6 = _mm256_blend_epi32(u32_hi_prod_perm_6, u32_hi_prod_perm_5, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_7 = _mm256_blend_epi32(u32_hi_prod_perm_7, u32_hi_prod_perm_6, 0b00'00'00'11);
      rrprime_1 = _mm256_add_epi64(u32_lo_prod_1, u32_hi_prod_slide_1);
      rrprime_2 = _mm256_add_epi64(u32_lo_prod_2, u32_hi_prod_slide_2);
      rrprime_3 = _mm256_add_epi64(u32_lo_prod_3, u32_hi_prod_slide_3);
      rrprime_4 = _mm256_add_epi64(u32_lo_prod_4, u32_hi_prod_slide_4);
      rrprime_5 = _mm256_add_epi64(u32_lo_prod_5, u32_hi_prod_slide_5);
      rrprime_6 = _mm256_add_epi64(u32_lo_prod_6, u32_hi_prod_slide_6);
      rrprime_7 = _mm256_add_epi64(u32_lo_prod_7, u32_hi_prod_slide_7);
      const __m256i rrprime_perm_1 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_1, 0b10'01'00'11), 32U);
      const __m256i rrprime_perm_2 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_2, 0b10'01'00'11), 32U);
      const __m256i rrprime_perm_3 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_3, 0b10'01'00'11), 32U);
      const __m256i rrprime_perm_4 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_4, 0b10'01'00'11), 32U);
      const __m256i rrprime_perm_5 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_5, 0b10'01'00'11), 32U);
      const __m256i rrprime_perm_6 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_6, 0b10'01'00'11), 32U);
      const __m256i rrprime_perm_7 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_7, 0b10'01'00'11), 32U);
      const __m256i rrprime_slide_1 = _mm256_blend_epi32(rrprime_perm_1, ZERO, 0b00'00'00'11);
      const __m256i rrprime_slide_2 = _mm256_blend_epi32(rrprime_perm_2, rrprime_perm_1, 0b00'00'00'11);
      const __m256i rrprime_slide_3 = _mm256_blend_epi32(rrprime_perm_3, rrprime_perm_2, 0b00'00'00'11);
      const __m256i rrprime_slide_4 = _mm256_blend_epi32(rrprime_perm_4, rrprime_perm_3, 0b00'00'00'11);
      const __m256i rrprime_slide_5 = _mm256_blend_epi32(rrprime_perm_5, rrprime_perm_4, 0b00'00'00'11);
      const __m256i rrprime_slide_6 = _mm256_blend_epi32(rrprime_perm_6, rrprime_perm_5, 0b00'00'00'11);
      const __m256i rrprime_slide_7 = _mm256_blend_epi32(rrprime_perm_7, rrprime_perm_6, 0b00'00'00'11);
      rrprime_1 = _mm256_add_epi32(rrprime_1, rrprime_slide_1);
      rrprime_2 = _mm256_add_epi32(rrprime_2, rrprime_slide_2);
      rrprime_3 = _mm256_add_epi32(rrprime_3, rrprime_slide_3);
      rrprime_4 = _mm256_add_epi32(rrprime_4, rrprime_slide_4);
      rrprime_5 = _mm256_add_epi32(rrprime_5, rrprime_slide_5);
      rrprime_6 = _mm256_add_epi32(rrprime_6, rrprime_slide_6);
      rrprime_7 = _mm256_add_epi32(rrprime_7, rrprime_slide_7);
    }

    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[0]), rrprime_1);
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[4]), rrprime_2);
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[8]), rrprime_3);
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[12]), rrprime_4);
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[16]), rrprime_5);
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[20]), rrprime_6);
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[24]), rrprime_7);

    {
      const __m128i u32_pack_1 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_1, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));
      const __m128i u32_pack_2 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_2, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));
      const __m128i u32_pack_3 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_3, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));
      const __m128i u32_pack_4 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_4, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));
      const __m128i u32_pack_5 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_5, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));
      const __m128i u32_pack_6 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_6, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));
      const __m128i u32_pack_7 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_7, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));

      _mm_storeu_si128(reinterpret_cast<__m128i *>(&SIMD_PRE_WRITTEN[0]), u32_pack_1);
      _mm_storeu_si128(reinterpret_cast<__m128i *>(&SIMD_PRE_WRITTEN[4]), u32_pack_2);
      _mm_storeu_si128(reinterpret_cast<__m128i *>(&SIMD_PRE_WRITTEN[8]), u32_pack_3);
      _mm_storeu_si128(reinterpret_cast<__m128i *>(&SIMD_PRE_WRITTEN[12]), u32_pack_4);
      _mm_storeu_si128(reinterpret_cast<__m128i *>(&SIMD_PRE_WRITTEN[16]), u32_pack_5);
      _mm_storeu_si128(reinterpret_cast<__m128i *>(&SIMD_PRE_WRITTEN[20]), u32_pack_6);
      _mm_storeu_si128(reinterpret_cast<__m128i *>(&SIMD_PRE_WRITTEN[24]), u32_pack_7);
    }

    SLOW_PRE_ARR[0] = 1; // initialize r = 1

    for(uint32_t i = 0; i < P; ++i)
    {
      uint32_t carry = 0;
      for(unsigned int &w : SLOW_PRE_ARR)
      {
        uint64_t pp = (uint64_t)w * 10ull + carry;
        w = (uint32_t)pp;
        carry = (uint32_t)(pp >> 32);
      }
    }

    std::array<uint32_t, NUM_WORDS> x10byx10_arr = SLOW_PRE_ARR;

    size_t first = 0;

    while(first < SLOW_PRE_ARR.size() && SLOW_PRE_ARR[first] == 0)
    {
      ++first;
    }

    if(first != 0 && first != SLOW_PRE_ARR.size())
    {
      const size_t count = SLOW_PRE_ARR.size() - first;

      std::memmove(SLOW_PRE_ARR.data(), SLOW_PRE_ARR.data() + first, count * sizeof(uint32_t));

      std::memset(SLOW_PRE_ARR.data() + count, 0, first * sizeof(uint32_t));
    }

    if(SIMD_PRE_WRITTEN != SLOW_PRE_ARR)
    {
      std::cout << "k = " << k << "; p = " << P << '\n';
      goto backwards;
    }

    assert(SIMD_PRE_WRITTEN == SLOW_PRE_ARR);

    // unsigned lead_z = 24u - (k_div_32 - ((k * 1233u) >> 17u)) - (k < 32); //+ (k_div_32 & 0b0111);

    unsigned digits_computed = 0;
    unsigned word_idx = k / 32u;
    unsigned simdy_idx = (K_DIV_32 - ((k * 1233U) >> 17U));
    uint64_t chunk = 0;
    unsigned next_8;
    unsigned simdy_8;

    const unsigned bit_shift = k & ((1U << 5U) - 1);
    const unsigned mod_mask = (bit_shift == 0) ? 0 : ((1U << bit_shift) - 1);

    while(digits_computed < TOTAL_DIGITS)
    {
      // step a & b: multiply by 10^8 and cascade the carry
      uint32_t carry = 0; // carry max is 10^8, fits safely in uint32_t
      for(unsigned int &w : x10byx10_arr)
      {
        // max size: (2^32 - 1) * 10^8 + 10^8 = 2^32 * 10^8. fits comfortably in uint64_t.
        uint64_t pp = (uint64_t)w * 100'000'000u + carry;
        w = (uint32_t)pp;
        carry = (uint32_t)(pp >> 32);
      }

      {
        __m256i rrrprime_1 = _mm256_loadu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[0]));
        __m256i rrrprime_2 = _mm256_loadu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[4]));
        __m256i rrrprime_3 = _mm256_loadu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[8]));
        __m256i rrrprime_4 = _mm256_loadu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[12]));
        __m256i rrrprime_5 = _mm256_loadu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[16]));
        __m256i rrrprime_6 = _mm256_loadu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[20]));
        __m256i rrrprime_7 = _mm256_loadu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[24]));
        const __m256i u64_prod_1 = _mm256_mul_epu32(rrrprime_1, R1_E8);
        const __m256i u64_prod_2 = _mm256_mul_epu32(rrrprime_2, R1_E8);
        const __m256i u64_prod_3 = _mm256_mul_epu32(rrrprime_3, R1_E8);
        const __m256i u64_prod_4 = _mm256_mul_epu32(rrrprime_4, R1_E8);
        const __m256i u64_prod_5 = _mm256_mul_epu32(rrrprime_5, R1_E8);
        const __m256i u64_prod_6 = _mm256_mul_epu32(rrrprime_6, R1_E8);
        const __m256i u64_prod_7 = _mm256_mul_epu32(rrrprime_7, R1_E8);
        const __m256i u32_lo_prod_1 = _mm256_blend_epi32(u64_prod_1, ZERO, 0b1010'1010);
        const __m256i u32_lo_prod_2 = _mm256_blend_epi32(u64_prod_2, ZERO, 0b1010'1010);
        const __m256i u32_lo_prod_3 = _mm256_blend_epi32(u64_prod_3, ZERO, 0b1010'1010);
        const __m256i u32_lo_prod_4 = _mm256_blend_epi32(u64_prod_4, ZERO, 0b1010'1010);
        const __m256i u32_lo_prod_5 = _mm256_blend_epi32(u64_prod_5, ZERO, 0b1010'1010);
        const __m256i u32_lo_prod_6 = _mm256_blend_epi32(u64_prod_6, ZERO, 0b1010'1010);
        const __m256i u32_lo_prod_7 = _mm256_blend_epi32(u64_prod_7, ZERO, 0b1010'1010);
        const __m256i u32_hi_prod_1 = _mm256_srli_epi64(u64_prod_1, 32);
        const __m256i u32_hi_prod_2 = _mm256_srli_epi64(u64_prod_2, 32);
        const __m256i u32_hi_prod_3 = _mm256_srli_epi64(u64_prod_3, 32);
        const __m256i u32_hi_prod_4 = _mm256_srli_epi64(u64_prod_4, 32);
        const __m256i u32_hi_prod_5 = _mm256_srli_epi64(u64_prod_5, 32);
        const __m256i u32_hi_prod_6 = _mm256_srli_epi64(u64_prod_6, 32);
        const __m256i u32_hi_prod_7 = _mm256_srli_epi64(u64_prod_7, 32);
        const __m256i u32_hi_prod_perm_1 = _mm256_permute4x64_epi64(u32_hi_prod_1, 0b10'01'00'11);
        const __m256i u32_hi_prod_perm_2 = _mm256_permute4x64_epi64(u32_hi_prod_2, 0b10'01'00'11);
        const __m256i u32_hi_prod_perm_3 = _mm256_permute4x64_epi64(u32_hi_prod_3, 0b10'01'00'11);
        const __m256i u32_hi_prod_perm_4 = _mm256_permute4x64_epi64(u32_hi_prod_4, 0b10'01'00'11);
        const __m256i u32_hi_prod_perm_5 = _mm256_permute4x64_epi64(u32_hi_prod_5, 0b10'01'00'11);
        const __m256i u32_hi_prod_perm_6 = _mm256_permute4x64_epi64(u32_hi_prod_6, 0b10'01'00'11);
        const __m256i u32_hi_prod_perm_7 = _mm256_permute4x64_epi64(u32_hi_prod_7, 0b10'01'00'11);
        const __m256i u32_hi_prod_slide_1 = _mm256_blend_epi32(u32_hi_prod_perm_1, ZERO, 0b00'00'00'11);
        const __m256i u32_hi_prod_slide_2 = _mm256_blend_epi32(u32_hi_prod_perm_2, u32_hi_prod_perm_1, 0b00'00'00'11);
        const __m256i u32_hi_prod_slide_3 = _mm256_blend_epi32(u32_hi_prod_perm_3, u32_hi_prod_perm_2, 0b00'00'00'11);
        const __m256i u32_hi_prod_slide_4 = _mm256_blend_epi32(u32_hi_prod_perm_4, u32_hi_prod_perm_3, 0b00'00'00'11);
        const __m256i u32_hi_prod_slide_5 = _mm256_blend_epi32(u32_hi_prod_perm_5, u32_hi_prod_perm_4, 0b00'00'00'11);
        const __m256i u32_hi_prod_slide_6 = _mm256_blend_epi32(u32_hi_prod_perm_6, u32_hi_prod_perm_5, 0b00'00'00'11);
        const __m256i u32_hi_prod_slide_7 = _mm256_blend_epi32(u32_hi_prod_perm_7, u32_hi_prod_perm_6, 0b00'00'00'11);
        rrrprime_1 = _mm256_add_epi64(u32_lo_prod_1, u32_hi_prod_slide_1);
        rrrprime_2 = _mm256_add_epi64(u32_lo_prod_2, u32_hi_prod_slide_2);
        rrrprime_3 = _mm256_add_epi64(u32_lo_prod_3, u32_hi_prod_slide_3);
        rrrprime_4 = _mm256_add_epi64(u32_lo_prod_4, u32_hi_prod_slide_4);
        rrrprime_5 = _mm256_add_epi64(u32_lo_prod_5, u32_hi_prod_slide_5);
        rrrprime_6 = _mm256_add_epi64(u32_lo_prod_6, u32_hi_prod_slide_6);
        rrrprime_7 = _mm256_add_epi64(u32_lo_prod_7, u32_hi_prod_slide_7);
        const __m256i rrprime_perm_1 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrrprime_1, 0b10'01'00'11), 32U);
        const __m256i rrprime_perm_2 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrrprime_2, 0b10'01'00'11), 32U);
        const __m256i rrprime_perm_3 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrrprime_3, 0b10'01'00'11), 32U);
        const __m256i rrprime_perm_4 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrrprime_4, 0b10'01'00'11), 32U);
        const __m256i rrprime_perm_5 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrrprime_5, 0b10'01'00'11), 32U);
        const __m256i rrprime_perm_6 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrrprime_6, 0b10'01'00'11), 32U);
        const __m256i rrprime_perm_7 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrrprime_7, 0b10'01'00'11), 32U);
        const __m256i rrprime_slide_1 = _mm256_blend_epi32(rrprime_perm_1, ZERO, 0b00'00'00'11);
        const __m256i rrprime_slide_2 = _mm256_blend_epi32(rrprime_perm_2, rrprime_perm_1, 0b00'00'00'11);
        const __m256i rrprime_slide_3 = _mm256_blend_epi32(rrprime_perm_3, rrprime_perm_2, 0b00'00'00'11);
        const __m256i rrprime_slide_4 = _mm256_blend_epi32(rrprime_perm_4, rrprime_perm_3, 0b00'00'00'11);
        const __m256i rrprime_slide_5 = _mm256_blend_epi32(rrprime_perm_5, rrprime_perm_4, 0b00'00'00'11);
        const __m256i rrprime_slide_6 = _mm256_blend_epi32(rrprime_perm_6, rrprime_perm_5, 0b00'00'00'11);
        const __m256i rrprime_slide_7 = _mm256_blend_epi32(rrprime_perm_7, rrprime_perm_6, 0b00'00'00'11);
        rrrprime_1 = _mm256_add_epi32(rrrprime_1, rrprime_slide_1);
        rrrprime_2 = _mm256_add_epi32(rrrprime_2, rrprime_slide_2);
        rrrprime_3 = _mm256_add_epi32(rrrprime_3, rrprime_slide_3);
        rrrprime_4 = _mm256_add_epi32(rrrprime_4, rrprime_slide_4);
        rrrprime_5 = _mm256_add_epi32(rrrprime_5, rrprime_slide_5);
        rrrprime_6 = _mm256_add_epi32(rrrprime_6, rrprime_slide_6);
        rrrprime_7 = _mm256_add_epi32(rrrprime_7, rrprime_slide_7);
        rrrprime_1 = _mm256_blend_epi32(rrrprime_1, ZERO, 0b1010'1010);
        rrrprime_2 = _mm256_blend_epi32(rrrprime_2, ZERO, 0b1010'1010);
        rrrprime_3 = _mm256_blend_epi32(rrrprime_3, ZERO, 0b1010'1010);
        rrrprime_4 = _mm256_blend_epi32(rrrprime_4, ZERO, 0b1010'1010);
        rrrprime_5 = _mm256_blend_epi32(rrrprime_5, ZERO, 0b1010'1010);
        rrrprime_6 = _mm256_blend_epi32(rrrprime_6, ZERO, 0b1010'1010);
        rrrprime_7 = _mm256_blend_epi32(rrrprime_7, ZERO, 0b1010'1010);
        _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[0]), rrrprime_1);
        _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[4]), rrrprime_2);
        _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[8]), rrrprime_3);
        _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[12]), rrrprime_4);
        _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[16]), rrrprime_5);
        _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[20]), rrrprime_6);
        _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[24]), rrrprime_7);
      }

      // step c: extract next8 chunk (r >> k)
      chunk = x10byx10_arr[word_idx] >> bit_shift;
      // cast to uint64_t before shifting to prevent any 32-bit boundary overflow
      chunk |= ((uint64_t)x10byx10_arr[word_idx + 1] << (32 - bit_shift));
      next_8 = (uint32_t)chunk;

      x10byx10_arr[word_idx] &= mod_mask;
      std::memset(static_cast<void *>(&x10byx10_arr[word_idx + 1]), 0, (NUM_WORDS - word_idx - 1) * 4);

      chunk = SIMD_ARRAY[simdy_idx] >> bit_shift;
      chunk |= (SIMD_ARRAY[simdy_idx + 1] << (32 - bit_shift));
      simdy_8 = (uint32_t)chunk;
      SIMD_ARRAY[simdy_idx] &= mod_mask;
      _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[simdy_idx + 1]), ZERO);

      if(simdy_8 != next_8)
      {
        std::cout << "k = " << k << "; p = " << P << '\n';
        goto backwards;
      }

      assert(simdy_8 == next_8);

      // step e: print immediately
      // std::cout << std::setfill('0') << std::setw(8) << simdy_8;

      digits_computed += 8;
    }
    //
  }

  return 0;
}

#else
#error "this algorithm is not supported for this architecture; this architecture is too old (pre __AVX2__)"
#endif

/*
int main()
{
  uint32_t k = 1024; // Test range [1, 1024]

  // 2^1024 has ~309 digits. 309 digits / 8 = ~39 chunks.
  // 40 words * 32 bits perfectly fits without any overflow.
  // ALL heap allocation is eliminated.
  constexpr int NUM_WORDS = 40;
  uint32_t r[NUM_WORDS] = { 0 };
  r[0] = 1; // Initialize R = 1

  // 1. One-time Initialization Loop: Compute 2^k inline using Base-10^8
  uint32_t bits_remaining = k;
  while(bits_remaining > 0)
  {
    // Max step is 6 bits because (10^8 - 1) * 2^6 + carry
    // multiplied by our magic number safely fits inside a uint64_t.
    uint32_t step = (bits_remaining > 6) ? 6 : bits_remaining;
    uint32_t multiplier = 1U << step;

    uint32_t carry = 0; // Stored as u32
    for(int w = 0; w < NUM_WORDS; ++w)
    {
      // Grows to u64 for safe intermediate math just like original snippet
      uint64_t p = (uint64_t)r[w] * multiplier + carry;

      // --- MAGIC NUMBER DIVISION (NO / OPERATOR) ---
      // Simulates: carry = p / 100,000,000
      carry = (uint32_t)((p * 1441151881ULL) >> 57);

      // --- MAGIC NUMBER MODULO (NO % OPERATOR) ---
      // Simulates: r[w] = p % 100,000,000
      r[w] = (uint32_t)(p - (uint64_t)carry * 100'000'000ULL);
    }
    bits_remaining -= step;
  }

  // Find the highest active chunk to drop leading zero chunks
  int start_word = NUM_WORDS - 1;
  while(start_word > 0 && r[start_word] == 0)
  {
    start_word--;
  }

  // 2. Main Processing Loop: Streams out 8-digit chunks exactly every iteration
  int chunk_count = 1;
  for(int w = start_word; w >= 0; --w)
  {
    uint32_t next8 = r[w]; // Extracted instantly in O(1)

    // Step E: Print immediately per iteration
    if(w == start_word)
    {
      // Very first chunk has no leading zeroes
      std::cout << "Chunk " << chunk_count++ << ": " << next8 << "\n";
    }
    else
    {
      // Middle/trailing chunks get padded to exactly 8 digits
      std::cout << "Chunk " << chunk_count++ << ": " << std::setfill('0') << std::setw(8) << next8 << "\n";
    }
  }

  return 0;
}
*/

/*
#include <cstdint>
#include <iomanip>
#include <iostream>

int main()
{
  std::cout << ",{ ";
  for(uint32_t k = 1; k < 2048; k *= 2) // Supports any k in [1, 1100]
  {
    // 36 words * 32 bits is enough for k=1152
    constexpr int NUM_WORDS = 40;
    uint32_t r[NUM_WORDS] = { 0 };
    r[0] = 1; // Initialize R = 1

    // Magic number for exact division by 10^8 using only shifts and multiplies
    // M = ceil(2^60 / 10^8)
    constexpr uint64_t MAGIC_10_8 = 11529215047ULL;
    constexpr uint32_t SHIFT_VAL = 60;

    // 1. Compute exactly 2^k in Base-10^8 chunks
    for(uint32_t i = 0; i < k; ++i)
    {
      uint32_t carry = 0;
      for(int w = 0; w < NUM_WORDS; ++w)
      {
        // Max p = 99,999,999 * 2 + 1 = 199,999,999
        uint64_t p = ((uint64_t)r[w] << 1U) + carry;

        // division by 10^8 converted to multiply and shift
        carry = (uint32_t)((p * MAGIC_10_8) >> SHIFT_VAL);

        // modulo 10^8 converted to subtraction
        r[w] = (uint32_t)(p - ((uint64_t)carry * 100'000'000));
      }
    }

    std::cout << ",{ ";
    for(int n = 0; n < NUM_WORDS; n++)
    {
      std::cout << r[n] << ", ";
    }

    std::cout << "} \n " << std::endl;
  }
  std::cout << "} \n " << std::endl;
  return 0;
}
*/

/*

#include <cstdint>
#include <iomanip>
#include <iostream>

int main()
{
  uint32_t k = 512; // Supports any k in [1, 1024]

  // 36 words * 32 bits is enough for k=1152
  constexpr int NUM_WORDS = 40;
  uint32_t r[NUM_WORDS] = { 0 };
  r[0] = 1; // Initialize R = 1

  // Magic number for exact division by 10^8 using only shifts and multiplies
  // M = ceil(2^60 / 10^8)
  constexpr uint64_t MAGIC_10_8 = 11529215047ULL;
  constexpr uint32_t SHIFT_VAL = 60;

  // 1. Compute exactly 2^k in Base-10^8 chunks
  for(uint32_t i = 0; i < k; ++i)
  {
    uint32_t carry = 0;
    for(int w = 0; w < NUM_WORDS; ++w)
    {
      // Max p = 99,999,999 * 2 + 1 = 199,999,999
      uint64_t p = ((uint64_t)r[w] << 1U) + carry;

      // division by 10^8 converted to multiply and shift
      carry = (uint32_t)((p * MAGIC_10_8) >> SHIFT_VAL);

      // modulo 10^8 converted to subtraction
      r[w] = (uint32_t)(p - ((uint64_t)carry * 100'000'000));
    }
  }

  // 2. Find the most significant word (O(1) search)
  int top = NUM_WORDS - 1;
  while(top > 0 && r[top] == 0)
  {
    top--;
  }

  // 3. Align the most significant chunk to exactly 8 digits
  uint32_t top_val = r[top];
  uint32_t missing_digits = 0;
  uint32_t temp = top_val;

  // Count missing digits without division (temp *= 10 using shifts)
  while(temp < 10000000)
  {
    temp = (temp << 3) + (temp << 1);
    missing_digits++;
  }

  // 4. Cascade shift the entire array by 10^missing_digits to pull up lower digits
  for(uint32_t m = 0; m < missing_digits; ++m)
  {
    uint32_t carry = 0;
    for(int w = 0; w <= top; ++w)
    {
      // Max p = 99,999,999 * 10 + 9 = 999,999,999
      uint64_t p = (uint64_t)r[w] * 10ULL + carry;
      carry = (uint32_t)((p * MAGIC_10_8) >> SHIFT_VAL);
      r[w] = (uint32_t)(p - carry * 100000000ULL);
    }
    // If the shift caused an overflow into a new chunk, advance the top pointer
    if(carry > 0)
    {
      top++;
      r[top] = carry;
    }
  }

  for(int n = NUM_WORDS - 1; n >= 0; n--)
  {
    std::cout << std::setfill('0') << std::setw(8) << r[n];
  }

  std::cout << std::endl;

  return 0;
}
*/

//

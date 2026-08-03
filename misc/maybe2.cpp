#include <array>
#include <cassert>
#include <cmath>
#include <cstdint> // Added for explicit uint32_t and uint64_t types
#include <cstring>
#include <iomanip>
#include <iostream>

#if defined(_MSC_VER) || defined(__x86_64__) || defined(__i386__)
#include <immintrin.h> // x86 SIMD
#elif defined(__ARM_NEON) || defined(__aarch64__)
#include <arm_neon.h> // ARM SIMD
#endif

namespace
{
  template <unsigned N>
  __attribute__((always_inline)) void do_multiplication(const int &k, const unsigned *POW_5_CACHE)
  {
    if constexpr(N == 1)
    {
    }
  }
} // namespace

int main()
{
  constexpr uint8_t POW_5_E[] = { 0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40 };

  constexpr uint32_t POW_5_CACHE[] = {
<<<<<<< HEAD
    1,        0,        0,        0, // __m128i single load cvtepied to __m256i
    0,        0,        0,        0, // 1 - anotherone
    0,        0,        0,        0, // 2 - anotherone
    0,        0,        0,        0, // 3 - anotherone
    0,        0,        0,        0, // 4 - anotherone
    0,        0,        0,        0, // 5 - anotherone
    0,        0,        0,        0, // 6 - anotherone
    0,        0,        0,        0, // 7 - anotherone
    0,        0,        0,        0, // 8 - anotherone
    0,        0,        0,        0, // 9 - anotherone
    0,        0,        0,        0, // 10 - anotherone
    0,        0,        0,        0, // 11 - anotherone
    0,        0,        0,        0, // 12 - anotherone
    0,        0,        0,        0, // 13 - anotherone
    0,        0,        0,        0, // 14 - anotherone
    0,        0,        0,        0, // 15 - anotherone
    0,        0,        0,        0, // 16 - anotherone
    0,        0,        0,        0, // 17 - anotherone
    0,        0,        0,        0, // 18 - anotherone
    0,        0,        0,        0, // 19 - anotherone
    0,        0,        0,        0, // 20 - anotherone
    0,        0,        0,        0, // 21 - anotherone
    0,        0,        0,        0, // 22 - anotherone

    12890625, 36817932, 60431486, 87926569, // 0 - __m128i single load cvtepied to __m256i
    18803771, 38919302, 19454666, 43055614, // 1 - anotherone
    99218413, 5571876,  38735877, 29,       // 2 - anotherone
    0,        0,        0,        0,        // 3 - anotherone
    0,        0,        0,        0,        // 4 - anotherone
    0,        0,        0,        0,        // 5 - anotherone
    0,        0,        0,        0,        // 6 - anotherone
    0,        0,        0,        0,        // 7 - anotherone
    0,        0,        0,        0,        // 8 - anotherone
    0,        0,        0,        0,        // 9 - anotherone
    0,        0,        0,        0,        // 10 - anotherone
    0,        0,        0,        0,        // 11 - anotherone
    0,        0,        0,        0,        // 12 - anotherone
    0,        0,        0,        0,        // 13 - anotherone
    0,        0,        0,        0,        // 14 - anotherone
    0,        0,        0,        0,        // 15 - anotherone
    0,        0,        0,        0,        // 16 - anotherone
    0,        0,        0,        0,        // 17 - anotherone
    0,        0,        0,        0,        // 18 - anotherone
    0,        0,        0,        0,        // 19 - anotherone
    0,        0,        0,        0,        // 20 - anotherone
    0,        0,        0,        0,        // 21 - anotherone
    0,        0,        0,        0,        // 22 - anotherone

    12890625, 23927307, 61113554, 98344692, // 0 - __m128i single load cvtepied to __m256i
    82685940, 98747201, 16094308, 1513058,  // 1 - anotherone
    33861629, 7388585,  92618786, 11045551, // 2 - anotherone
    71180559, 80397767, 22770288, 91113245, // 3 - anotherone
    1593091,  0,        0,        0,        // 4 - anotherone
    0,        0,        0,        0,        // 5 - anotherone
    0,        0,        0,        0,        // 6 - anotherone
    0,        0,        0,        0,        // 7 - anotherone
    0,        0,        0,        0,        // 8 - anotherone
    0,        0,        0,        0,        // 9 - anotherone
    0,        0,        0,        0,        // 10 - anotherone
    0,        0,        0,        0,        // 11 - anotherone
    0,        0,        0,        0,        // 12 - anotherone
    0,        0,        0,        0,        // 13 - anotherone
    0,        0,        0,        0,        // 14 - anotherone
    0,        0,        0,        0,        // 15 - anotherone
    0,        0,        0,        0,        // 16 - anotherone
    0,        0,        0,        0,        // 17 - anotherone
    0,        0,        0,        0,        // 18 - anotherone
    0,        0,        0,        0,        // 19 - anotherone
    0,        0,        0,        0,        // 20 - anotherone
    0,        0,        0,        0,        // 21 - anotherone
    0,        0,        0,        0,        // 22 - anotherone

    12890625, 11036682, 74686247, 70580747, // 0 - __m128i single load cvtepied to __m256i
    65300377, 31946693, 50989468, 66359699, // 1 - anotherone
    93946149, 65605472, 47228822, 71507503, // 2 - anotherone
    24270579, 59180316, 3470168,  13850237, // 3 - anotherone
    36443628, 71116000, 28003995, 38635186, // 4 - anotherone
    94444625, 61685550, 863,      0,        // 5 - anotherone
    0,        0,        0,        0,        // 6 - anotherone
    0,        0,        0,        0,        // 7 - anotherone
    0,        0,        0,        0,        // 8 - anotherone
    0,        0,        0,        0,        // 9 - anotherone
    0,        0,        0,        0,        // 10 - anotherone
    0,        0,        0,        0,        // 11 - anotherone
    0,        0,        0,        0,        // 12 - anotherone
    0,        0,        0,        0,        // 13 - anotherone
    0,        0,        0,        0,        // 14 - anotherone
    0,        0,        0,        0,        // 15 - anotherone
    0,        0,        0,        0,        // 16 - anotherone
    0,        0,        0,        0,        // 17 - anotherone
    0,        0,        0,        0,        // 18 - anotherone
    0,        0,        0,        0,        // 19 - anotherone
    0,        0,        0,        0,        // 20 - anotherone
    0,        0,        0,        0,        // 21 - anotherone
    0,        0,        0,        0,        // 22 - anotherone

    12890625, 98146057, 1149564,  91744110, // 0 - __m128i single load cvtepied to __m256i
    42329149, 90635119, 24452290, 74541517, // 1 - anotherone
    28927090, 92338075, 20461226, 19914931, // 2 - anotherone
    91753374, 42001963, 53790631, 46334430, // 3 - anotherone
    9753926,  49486635, 66739996, 86038327, // 4 - anotherone
    37155453, 41833163, 2333879,  76569915, // 5 - anotherone
    94138586, 32715584, 54692198, 46816763, // 6 - anotherone
    0,        0,        0,        0,        // 7 - anotherone
    0,        0,        0,        0,        // 8 - anotherone
    0,        0,        0,        0,        // 9 - anotherone
    0,        0,        0,        0,        // 10 - anotherone
    0,        0,        0,        0,        // 11 - anotherone
    0,        0,        0,        0,        // 12 - anotherone
    0,        0,        0,        0,        // 13 - anotherone
    0,        0,        0,        0,        // 14 - anotherone
    0,        0,        0,        0,        // 15 - anotherone
    0,        0,        0,        0,        // 16 - anotherone
    0,        0,        0,        0,        // 17 - anotherone
    0,        0,        0,        0,        // 18 - anotherone
    0,        0,        0,        0,        // 19 - anotherone
    0,        0,        0,        0,        // 20 - anotherone
    0,        0,        0,        0,        // 21 - anotherone
    0,        0,        0,        0,        // 22 - anotherone

    12890625, 85255432, 40503507, 48944154, // 0 - __m128i single load cvtepied to __m256i
    2344950,  13747753, 13827972, 78071105, // 1 - anotherone
    52827180, 38068409, 82066824, 66882658, // 2 - anotherone
    97356950, 43871649, 76179778, 7063576,  // 3 - anotherone
    62670278, 71342443, 58466020, 5775089,  // 4 - anotherone
    29664591, 57842862, 40742905, 99473932, // 5 - anotherone
    66978892, 32123947, 81606060, 97010955, // 6 - anotherone
    82361884, 30543546, 40245558, 64922327, // 7 - anotherone
    41837315, 25379,    0,        0,        // 8 - anotherone
    0,        0,        0,        0,        // 9 - anotherone
    0,        0,        0,        0,        // 10 - anotherone
    0,        0,        0,        0,        // 11 - anotherone
    0,        0,        0,        0,        // 12 - anotherone
    0,        0,        0,        0,        // 13 - anotherone
    0,        0,        0,        0,        // 14 - anotherone
    0,        0,        0,        0,        // 15 - anotherone
    0,        0,        0,        0,        // 16 - anotherone
    0,        0,        0,        0,        // 17 - anotherone
    0,        0,        0,        0,        // 18 - anotherone
    0,        0,        0,        0,        // 19 - anotherone
    0,        0,        0,        0,        // 20 - anotherone
    0,        0,        0,        0,        // 21 - anotherone
    0,        0,        0,        0,        // 22 - anotherone

    12890625, 72364807, 92748075, 29290255, // 0 - __m128i single load cvtepied to __m256i
    46811097, 14147175, 24176824, 37844232, // 1 - anotherone
    24728885, 98848681, 81258974, 48523067, // 2 - anotherone
    36399244, 28159503, 26488589, 45224459, // 3 - anotherone
    99663056, 31543307, 40942492, 890775,   // 4 - anotherone
    77556159, 38689000, 30766881, 80972948, // 5 - anotherone
    81289743, 20078646, 98486656, 51936415, // 6 - anotherone
    20462563, 22803578, 87165186, 45187091, // 7 - anotherone
    48268780, 24664295, 51613602, 52617006, // 8 - anotherone
    63667897, 82973977, 75821026, 13,       // 9 - anotherone
    0,        0,        0,        0,        // 10 - anotherone
    0,        0,        0,        0,        // 11 - anotherone
    0,        0,        0,        0,        // 12 - anotherone
    0,        0,        0,        0,        // 13 - anotherone
    0,        0,        0,        0,        // 14 - anotherone
    0,        0,        0,        0,        // 15 - anotherone
    0,        0,        0,        0,        // 16 - anotherone
    0,        0,        0,        0,        // 17 - anotherone
    0,        0,        0,        0,        // 18 - anotherone
    0,        0,        0,        0,        // 19 - anotherone
    0,        0,        0,        0,        // 20 - anotherone
    0,        0,        0,        0,        // 21 - anotherone
    0,        0,        0,        0,        // 22 - anotherone

    12890625, 59474182, 57883268, 19891789, // 0 - __m128i single load cvtepied to __m256i
    90081533, 65732649, 51846968, 29274335, // 1 - anotherone
    64600361, 36516628, 72060583, 35021533, // 2 - anotherone
    63699265, 27422975, 82300322, 20031686, // 3 - anotherone
    5885071,  41421726, 63914765, 65543650, // 4 - anotherone
    26294144, 74266435, 74952040, 31878137, // 5 - anotherone
    81260792, 20846914, 16482959, 2660486,  // 6 - anotherone
    80952253, 45341272, 55836101, 15116032, // 7 - anotherone
    42780336, 74890400, 83049361, 38507011, // 8 - anotherone
    33206278, 42715183, 60040689, 76471534, // 9 - anotherone
    29338373, 96531546, 6743290,  7312002,  // 10 - anotherone
    745834,   0,        0,        0,        // 11 - anotherone
    0,        0,        0,        0,        // 12 - anotherone
    0,        0,        0,        0,        // 13 - anotherone
    0,        0,        0,        0,        // 14 - anotherone
    0,        0,        0,        0,        // 15 - anotherone
    0,        0,        0,        0,        // 16 - anotherone
    0,        0,        0,        0,        // 17 - anotherone
    0,        0,        0,        0,        // 18 - anotherone
    0,        0,        0,        0,        // 19 - anotherone
    0,        0,        0,        0,        // 20 - anotherone
    0,        0,        0,        0,        // 21 - anotherone
    0,        0,        0,        0,        // 22 - anotherone

    12890625, 46583557, 35909086, 7858130,  // 0 - __m128i single load cvtepied to __m256i
    59400826, 90549496, 60937654, 46854319, // 1 - anotherone
    30137308, 47438283, 40813271, 64976600, // 2 - anotherone
    99836803, 69530310, 37659264, 40881129, // 3 - anotherone
    79372006, 10121537, 22022885, 60098026, // 4 - anotherone
    28182903, 58853540, 26431305, 30076959, // 5 - anotherone
    11917655, 58476191, 16558628, 65837500, // 6 - anotherone
    82196683, 26392072, 96450387, 62626466, // 7 - anotherone
    89528634, 65810762, 18636822, 5450082,  // 8 - anotherone
    65758491, 88431705, 17610809, 11360491, // 9 - anotherone
    68817242, 83185410, 85806158, 32411358, // 10 - anotherone
    92188827, 32843323, 34379260, 63059953, // 11 - anotherone
    52194906, 31746119, 404,      0,        // 12 - anotherone
    0,        0,        0,        0,        // 13 - anotherone
    0,        0,        0,        0,        // 14 - anotherone
    0,        0,        0,        0,        // 15 - anotherone
    0,        0,        0,        0,        // 16 - anotherone
    0,        0,        0,        0,        // 17 - anotherone
    0,        0,        0,        0,        // 18 - anotherone
    0,        0,        0,        0,        // 19 - anotherone
    0,        0,        0,        0,        // 20 - anotherone
    0,        0,        0,        0,        // 21 - anotherone
    0,        0,        0,        0,        // 22 - anotherone

    12890625, 33692932, 26825529, 80298653, // 0 - __m128i single load cvtepied to __m256i
    94904168, 45898467, 72653202, 94755038, // 1 - anotherone
    93193412, 52111858, 33905426, 43312635, // 2 - anotherone
    4316200,  9652344,  46355256, 36557909, // 3 - anotherone
    57829271, 66908547, 57223525, 88200147, // 4 - anotherone
    85245514, 75901101, 25534275, 5366715,  // 5 - anotherone
    80307590, 56656847, 94522357, 53852460, // 6 - anotherone
    27196536, 63455020, 97146204, 38187954, // 7 - anotherone
    9246733,  63878272, 38062796, 57688470, // 8 - anotherone
    11380580, 40626592, 30234391, 26766131, // 9 - anotherone
    69340090, 31925646, 92594249, 45371758, // 10 - anotherone
    9166570,  38274147, 13155347, 39061475, // 11 - anotherone
    72604608, 40366232, 80872212, 20832630, // 12 - anotherone
    31071411, 3975269,  49008403, 21918093, // 13 - anotherone
    0,        0,        0,        0,        // 14 - anotherone
    0,        0,        0,        0,        // 15 - anotherone
    0,        0,        0,        0,        // 16 - anotherone
    0,        0,        0,        0,        // 17 - anotherone
    0,        0,        0,        0,        // 18 - anotherone
    0,        0,        0,        0,        // 19 - anotherone
    0,        0,        0,        0,        // 20 - anotherone
    0,        0,        0,        0,        // 21 - anotherone
    0,        0,        0,        0,        // 22 - anotherone

    12890625, 20802307, 30632597, 24322733, // 0 - __m128i single load cvtepied to __m256i
    49617379, 11445123, 67547569, 50569835, // 1 - anotherone
    67262698, 1893786,  78092516, 84994366, // 2 - anotherone
    74064624, 72924940, 16030810, 81633289, // 3 - anotherone
    41636990, 53924495, 27432587, 65553471, // 4 - anotherone
    66057806, 38905810, 40844438, 76217811, // 5 - anotherone
    63932947, 14708443, 440751,   75439013, // 6 - anotherone
    52241383, 12526408, 61240168, 71398916, // 7 - anotherone
    9470344,  24352410, 89888501, 40012985, // 8 - anotherone
    60073909, 70502138, 55961549, 20570223, // 9 - anotherone
    56448341, 36740442, 7829986,  53903649, // 10 - anotherone
    88968971, 14027367, 79217364, 43956136, // 11 - anotherone
    1631643,  78886645, 25093565, 20127223, // 12 - anotherone
    89057984, 28597865, 95276091, 39834493, // 13 - anotherone
    29054450, 46939443, 69202536, 47488969, // 14 - anotherone
    82228934, 11881,    0,        0,        // 15 - anotherone
    0,        0,        0,        0,        // 16 - anotherone
    0,        0,        0,        0,        // 17 - anotherone
    0,        0,        0,        0,        // 18 - anotherone
    0,        0,        0,        0,        // 19 - anotherone
    0,        0,        0,        0,        // 20 - anotherone
    0,        0,        0,        0,        // 21 - anotherone
    0,        0,        0,        0,        // 22 - anotherone

    12890625, 7911682,  47330290, 27039745, // 0 - __m128i single load cvtepied to __m256i
    89456900, 76329204, 659543,   89314513, // 1 - anotherone
    50367729, 10144597, 93672434, 70227291, // 2 - anotherone
    59607713, 21605513, 98823779, 55991991, // 3 - anotherone
    97529227, 95394377, 33456482, 71686865, // 4 - anotherone
    68536250, 77223259, 94059799, 83506650, // 5 - anotherone
    26685244, 91147685, 42664879, 23577135, // 6 - anotherone
    38846903, 89874517, 36838071, 58163972, // 7 - anotherone
    2141121,  75557931, 29152176, 67930773, // 8 - anotherone
    58553576, 99696070, 20696027, 2342593,  // 9 - anotherone
    47840348, 27056717, 28634380, 7259100,  // 10 - anotherone
    4927887,  80605088, 29537590, 13745453, // 11 - anotherone
    59045222, 74645734, 71904625, 49474722, // 12 - anotherone
    49146671, 85621808, 61260668, 89725225, // 13 - anotherone
    3696462,  26932756, 32293236, 30757099, // 14 - anotherone
    65633584, 13314439, 25052048, 50336885, // 15 - anotherone
    8227036,  95971333, 44114876, 6,        // 16 - anotherone
    0,        0,        0,        0,        // 17 - anotherone
    0,        0,        0,        0,        // 18 - anotherone
    0,        0,        0,        0,        // 19 - anotherone
    0,        0,        0,        0,        // 20 - anotherone
    0,        0,        0,        0,        // 21 - anotherone
    0,        0,        0,        0,        // 22 - anotherone

    12890625, 95021057, 76918607, 75559064, // 0 - __m128i single load cvtepied to __m256i
    93229799, 26274010, 29538564, 54792111, // 1 - anotherone
    60598235, 13238409, 35131333, 48242770, // 2 - anotherone
    76832770, 70923846, 99238382, 50433919, // 3 - anotherone
    24468835, 50788035, 70468258, 37184642, // 4 - anotherone
    60475589, 84813955, 6596266,  71600613, // 5 - anotherone
    50284037, 77518631, 32652230, 94682357, // 6 - anotherone
    28203237, 82573046, 73310462, 99146738, // 7 - anotherone
    69153691, 58415613, 47288467, 70587186, // 8 - anotherone
    45305868, 38022974, 51159120, 26742109, // 9 - anotherone
    74165051, 70971848, 92549028, 60177187, // 10 - anotherone
    36556661, 39740493, 20587097, 72783066, // 11 - anotherone
    85408841, 74337366, 95639295, 12736340, // 12 - anotherone
    95934324, 74825701, 26051124, 43191684, // 13 - anotherone
    15649069, 56455311, 37868484, 34231007, // 14 - anotherone
    84417061, 29504413, 20783371, 74337233, // 15 - anotherone
    21855713, 85509018, 10019730, 23541746, // 16 - anotherone
    12938018, 17783003, 72892747, 37446497, // 17 - anotherone
    349175,   0,        0,        0,        // 18 - anotherone
    0,        0,        0,        0,        // 18 - anotherone
    0,        0,        0,        0,        // 19 - anotherone
    0,        0,        0,        0,        // 20 - anotherone
    0,        0,        0,        0,        // 21 - anotherone
    0,        0,        0,        0,        // 22 - anotherone

    12890625, 82130432, 19397550, 56990066, // 0 - __m128i single load cvtepied to __m256i
    52633770, 30695772, 55161160, 65431869, // 1 - anotherone
    54435342, 52135210, 67582027, 22178361, // 2 - anotherone
    45662149, 91858009, 88417422, 79935803, // 3 - anotherone
    77224690, 78333901, 23956413, 37558796, // 4 - anotherone
    54784028, 69676666, 94248976, 24391821, // 5 - anotherone
    65565657, 44986285, 95772611, 27224092, // 6 - anotherone
    39702911, 49202196, 74595943, 14175039, // 7 - anotherone
    45896806, 17481105, 14683103, 41547070, // 8 - anotherone
    15883199, 37433330, 45316856, 6328403,  // 9 - anotherone
    37165440, 8686134,  73496245, 29234248, // 10 - anotherone
    895025,   36001475, 57385367, 66451464, // 11 - anotherone
    20381473, 27116116, 31065816, 18604120, // 12 - anotherone
    69119138, 65080674, 31586602, 3085059,  // 13 - anotherone
    39906385, 91257172, 11614740, 87190497, // 14 - anotherone
    57480764, 92504006, 55350450, 98520204, // 15 - anotherone
    84173687, 48798656, 2505297,  49206327, // 16 - anotherone
    26707219, 73252581, 60322630, 54838450, // 17 - anotherone
    49424763, 50643491, 60288424, 55640255, // 18 - anotherone
    66839537, 28834978, 189,      0,        // 19 - anotherone
    0,        0,        0,        0,        // 20 - anotherone
    0,        0,        0,        0,        // 21 - anotherone
    0,        0,        0,        0,        // 22 - anotherone

    12890625, 69239807, 74767118, 58442124, // 0 - __m128i single load cvtepied to __m256i
    72257130, 29813027, 75738021, 41711742, // 1 - anotherone
    30887242, 37579931, 13268257, 90118688, // 2 - anotherone
    25870432, 38707920, 92621288, 75004620, // 3 - anotherone
    3249224,  33315057, 63932247, 64952842, // 4 - anotherone
    27029643, 21294253, 95447096, 47773580, // 5 - anotherone
    69229590, 88712012, 66558197, 56291399, // 6 - anotherone
    90348349, 71668038, 50909283, 85414400, // 7 - anotherone
    61078027, 42478563, 10377116, 68993929, // 8 - anotherone
    89380307, 47371512, 89712946, 85362783, // 9 - anotherone
    24296365, 65366613, 71319642, 42647562, // 10 - anotherone
    5355883,  3947957,  25684223, 70221991, // 11 - anotherone
    65133714, 87986168, 47983985, 99509759, // 12 - anotherone
    20234596, 25749706, 79412382, 11776446, // 13 - anotherone
    73970722, 96090587, 90448513, 7921097,  // 14 - anotherone
    39986773, 56120138, 98063699, 88753174, // 15 - anotherone
    83650732, 60992752, 71803363, 59593387, // 16 - anotherone
    12255325, 38513683, 7561870,  91329359, // 17 - anotherone
    83375844, 48343160, 40068095, 38474475, // 18 - anotherone
    5222620,  8028993,  90530683, 82968521, // 19 - anotherone
    73222912, 6233400,  324594,   10261342, // 20 - anotherone
    0,        0,        0,        0,        // 21 - anotherone
    0,        0,        0,        0,        // 22 - anotherone

    12890625, 56349182, 43027311, 67024615, // 0 - __m128i single load cvtepied to __m256i
    69578822, 21755996, 53411689, 42273851, // 1 - anotherone
    37352728, 21466491, 93494493, 17315231, // 2 - anotherone
    76695971, 47245793, 8335217,  42684328, // 3 - anotherone
    94724182, 57903306, 18822547, 71271179, // 4 - anotherone
    76350385, 64179153, 61855404, 75060403, // 5 - anotherone
    46331102, 65700422, 34778692, 70285190, // 6 - anotherone
    10511211, 7071594,  8771803,  45371416, // 7 - anotherone
    18314079, 34294051, 96628257, 84158693, // 8 - anotherone
    86657834, 60377765, 29007481, 39782222, // 9 - anotherone
    98924902, 80975371, 33802858, 12998108, // 10 - anotherone
    52714415, 24802899, 84483951, 46612783, // 11 - anotherone
    14561770, 28849964, 29012627, 36289511, // 12 - anotherone
    38437487, 99452219, 57621869, 77192529, // 13 - anotherone
    21251589, 14790991, 22794992, 97371801, // 14 - anotherone
    65416453, 12701662, 35520938, 73562593, // 15 - anotherone
    80391261, 59090179, 53686064, 82619902, // 16 - anotherone
    83830005, 28269449, 44429705, 40030963, // 17 - anotherone
    36502721, 8540388,  27512881, 93275895, // 18 - anotherone
    41016834, 31277551, 57078975, 79548963, // 19 - anotherone
    11087034, 85422180, 57638331, 51155829, // 20 - anotherone
    5480399,  33101016, 25581793, 80034577, // 21 - anotherone
    68464626, 5562,     0,        0,        // 22 - anotherone
  };

  for(int k = 0; k < 1075; k++)
  {
  backwards:
    // 36 words * 32 bits = 1152 bits. perfectly fits k=1100 + 10^8 overflow.
    const unsigned P = std::floor(k * std::log10(2));

    const unsigned K_DIV_8 = static_cast<unsigned>(k) >> 3U;
    const unsigned K_DIV_64 = static_cast<unsigned>(k) >> 6U;
    const unsigned BASE_IDX = K_DIV_64 * 92U;

    const __m256i ZERO = _mm256_setzero_si256();

    // total digits in 5^k
    const uint32_t TOTAL_DIGITS = std::floor(k * std::log10(5)) + 1;
    constexpr unsigned NUM_WORDS = 96;
    std::array<uint32_t, NUM_WORDS> SIMD_PRE_WRITTEN = { 0 };
    std::array<uint32_t, NUM_WORDS> slow_pre_arr = { 0 };

    std::array<uint64_t, 96> SIMD_ARRAY;

    __m256i rrprime_1 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&POW_5_CACHE[BASE_IDX])));
    __m256i rrprime_2 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&POW_5_CACHE[BASE_IDX + 4U])));
    __m256i rrprime_3 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&POW_5_CACHE[BASE_IDX + 8U])));
    __m256i rrprime_4 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&POW_5_CACHE[BASE_IDX + 12U])));
    __m256i rrprime_5 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&POW_5_CACHE[BASE_IDX + 16U])));
    __m256i rrprime_6 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&POW_5_CACHE[BASE_IDX + 20U])));
    __m256i rrprime_7 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&POW_5_CACHE[BASE_IDX + 24U])));
    __m256i rrprime_8 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&POW_5_CACHE[BASE_IDX + 28U])));
    __m256i rrprime_9 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&POW_5_CACHE[BASE_IDX + 32U])));
    __m256i rrprime_10 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&POW_5_CACHE[BASE_IDX + 36U])));
    __m256i rrprime_11 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&POW_5_CACHE[BASE_IDX + 40U])));
    __m256i rrprime_12 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&POW_5_CACHE[BASE_IDX + 44U])));
    __m256i rrprime_13 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&POW_5_CACHE[BASE_IDX + 48U])));
    __m256i rrprime_14 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&POW_5_CACHE[BASE_IDX + 52U])));
    __m256i rrprime_15 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&POW_5_CACHE[BASE_IDX + 56U])));
    __m256i rrprime_16 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&POW_5_CACHE[BASE_IDX + 60U])));
    __m256i rrprime_17 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&POW_5_CACHE[BASE_IDX + 64U])));
    __m256i rrprime_18 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&POW_5_CACHE[BASE_IDX + 68U])));
    __m256i rrprime_19 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&POW_5_CACHE[BASE_IDX + 72U])));
    __m256i rrprime_20 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&POW_5_CACHE[BASE_IDX + 76U])));
    __m256i rrprime_21 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&POW_5_CACHE[BASE_IDX + 80U])));
    __m256i rrprime_22 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&POW_5_CACHE[BASE_IDX + 84U])));
    __m256i rrprime_23 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&POW_5_CACHE[BASE_IDX + 88U])));
    __m256i rrprime_24 = ZERO;

    const __m256i R_25 = _mm256_set1_epi64x(25U);
    const __m256i R_MAGIC_10E8 = _mm256_set1_epi64x(1441151881U);

    const unsigned e_o = POW_5_E[K_DIV_64];

    for(unsigned e = e_o; e < K_DIV_8; e++)
    {
      const __m256i u64_prod_1 = _mm256_mul_epu32(rrprime_1, R_25);
      const __m256i u64_prod_2 = _mm256_mul_epu32(rrprime_2, R_25);
      const __m256i u64_prod_3 = _mm256_mul_epu32(rrprime_3, R_25);
      const __m256i u64_prod_4 = _mm256_mul_epu32(rrprime_4, R_25);
      const __m256i u64_prod_5 = _mm256_mul_epu32(rrprime_5, R_25);
      const __m256i u64_prod_6 = _mm256_mul_epu32(rrprime_6, R_25);
      const __m256i u64_prod_7 = _mm256_mul_epu32(rrprime_7, R_25);
      const __m256i u64_prod_8 = _mm256_mul_epu32(rrprime_8, R_25);
      const __m256i u64_prod_9 = _mm256_mul_epu32(rrprime_9, R_25);
      const __m256i u64_prod_10 = _mm256_mul_epu32(rrprime_10, R_25);
      const __m256i u64_prod_11 = _mm256_mul_epu32(rrprime_11, R_25);
      const __m256i u64_prod_12 = _mm256_mul_epu32(rrprime_12, R_25);
      const __m256i u64_prod_13 = _mm256_mul_epu32(rrprime_13, R_25);
      const __m256i u64_prod_14 = _mm256_mul_epu32(rrprime_14, R_25);
      const __m256i u64_prod_15 = _mm256_mul_epu32(rrprime_15, R_25);
      const __m256i u64_prod_16 = _mm256_mul_epu32(rrprime_16, R_25);
      const __m256i u64_prod_17 = _mm256_mul_epu32(rrprime_17, R_25);
      const __m256i u64_prod_18 = _mm256_mul_epu32(rrprime_18, R_25);
      const __m256i u64_prod_19 = _mm256_mul_epu32(rrprime_19, R_25);
      const __m256i u64_prod_20 = _mm256_mul_epu32(rrprime_20, R_25);
      const __m256i u64_prod_21 = _mm256_mul_epu32(rrprime_21, R_25);
      const __m256i u64_prod_22 = _mm256_mul_epu32(rrprime_22, R_25);
      const __m256i u64_prod_23 = _mm256_mul_epu32(rrprime_23, R_25);
      const __m256i u64_prod_24 = _mm256_mul_epu32(rrprime_24, R_25);
      const __m256i u64_magic_prod_1 = _mm256_mul_epu32(u64_prod_1, R_MAGIC_10E8);
      const __m256i u64_magic_prod_2 = _mm256_mul_epu32(u64_prod_2, R_MAGIC_10E8);
      const __m256i u64_magic_prod_3 = _mm256_mul_epu32(u64_prod_3, R_MAGIC_10E8);
      const __m256i u64_magic_prod_4 = _mm256_mul_epu32(u64_prod_4, R_MAGIC_10E8);
      const __m256i u64_magic_prod_5 = _mm256_mul_epu32(u64_prod_5, R_MAGIC_10E8);
      const __m256i u64_magic_prod_6 = _mm256_mul_epu32(u64_prod_6, R_MAGIC_10E8);
      const __m256i u64_magic_prod_7 = _mm256_mul_epu32(u64_prod_7, R_MAGIC_10E8);
      const __m256i u64_magic_prod_8 = _mm256_mul_epu32(u64_prod_8, R_MAGIC_10E8);
      const __m256i u64_magic_prod_9 = _mm256_mul_epu32(u64_prod_9, R_MAGIC_10E8);
      const __m256i u64_magic_prod_10 = _mm256_mul_epu32(u64_prod_10, R_MAGIC_10E8);
      const __m256i u64_magic_prod_11 = _mm256_mul_epu32(u64_prod_11, R_MAGIC_10E8);
      const __m256i u64_magic_prod_12 = _mm256_mul_epu32(u64_prod_12, R_MAGIC_10E8);
      const __m256i u64_magic_prod_13 = _mm256_mul_epu32(u64_prod_13, R_MAGIC_10E8);
      const __m256i u64_magic_prod_14 = _mm256_mul_epu32(u64_prod_14, R_MAGIC_10E8);
      const __m256i u64_magic_prod_15 = _mm256_mul_epu32(u64_prod_15, R_MAGIC_10E8);
      const __m256i u64_magic_prod_16 = _mm256_mul_epu32(u64_prod_16, R_MAGIC_10E8);
      const __m256i u64_magic_prod_17 = _mm256_mul_epu32(u64_prod_17, R_MAGIC_10E8);
      const __m256i u64_magic_prod_18 = _mm256_mul_epu32(u64_prod_18, R_MAGIC_10E8);
      const __m256i u64_magic_prod_19 = _mm256_mul_epu32(u64_prod_19, R_MAGIC_10E8);
      const __m256i u64_magic_prod_20 = _mm256_mul_epu32(u64_prod_20, R_MAGIC_10E8);
      const __m256i u64_magic_prod_21 = _mm256_mul_epu32(u64_prod_21, R_MAGIC_10E8);
      const __m256i u64_magic_prod_22 = _mm256_mul_epu32(u64_prod_22, R_MAGIC_10E8);
      const __m256i u64_magic_prod_23 = _mm256_mul_epu32(u64_prod_23, R_MAGIC_10E8);
      const __m256i u64_magic_prod_24 = _mm256_mul_epu32(u64_prod_24, R_MAGIC_10E8);
      const __m256i u64_magic_res_1 = _mm256_srli_epi64(u64_magic_prod_1, 57U);
      const __m256i u64_magic_res_2 = _mm256_srli_epi64(u64_magic_prod_2, 57U);
      const __m256i u64_magic_res_3 = _mm256_srli_epi64(u64_magic_prod_3, 57U);
      const __m256i u64_magic_res_4 = _mm256_srli_epi64(u64_magic_prod_4, 57U);
      const __m256i u64_magic_res_5 = _mm256_srli_epi64(u64_magic_prod_5, 57U);
      const __m256i u64_magic_res_6 = _mm256_srli_epi64(u64_magic_prod_6, 57U);
      const __m256i u64_magic_res_7 = _mm256_srli_epi64(u64_magic_prod_7, 57U);
      const __m256i u64_magic_res_8 = _mm256_srli_epi64(u64_magic_prod_8, 57U);
      const __m256i u64_magic_res_9 = _mm256_srli_epi64(u64_magic_prod_9, 57U);
      const __m256i u64_magic_res_10 = _mm256_srli_epi64(u64_magic_prod_10, 57U);
      const __m256i u64_magic_res_11 = _mm256_srli_epi64(u64_magic_prod_11, 57U);
      const __m256i u64_magic_res_12 = _mm256_srli_epi64(u64_magic_prod_12, 57U);
      const __m256i u64_magic_res_13 = _mm256_srli_epi64(u64_magic_prod_13, 57U);
      const __m256i u64_magic_res_14 = _mm256_srli_epi64(u64_magic_prod_14, 57U);
      const __m256i u64_magic_res_15 = _mm256_srli_epi64(u64_magic_prod_15, 57U);
      const __m256i u64_magic_res_16 = _mm256_srli_epi64(u64_magic_prod_16, 57U);
      const __m256i u64_magic_res_17 = _mm256_srli_epi64(u64_magic_prod_17, 57U);
      const __m256i u64_magic_res_18 = _mm256_srli_epi64(u64_magic_prod_18, 57U);
      const __m256i u64_magic_res_19 = _mm256_srli_epi64(u64_magic_prod_19, 57U);
      const __m256i u64_magic_res_20 = _mm256_srli_epi64(u64_magic_prod_20, 57U);
      const __m256i u64_magic_res_21 = _mm256_srli_epi64(u64_magic_prod_21, 57U);
      const __m256i u64_magic_res_22 = _mm256_srli_epi64(u64_magic_prod_22, 57U);
      const __m256i u64_magic_res_23 = _mm256_srli_epi64(u64_magic_prod_23, 57U);
      const __m256i u64_magic_res_24 = _mm256_srli_epi64(u64_magic_prod_24, 57U);
      const __m256i u32_hi_prod_perm_1 = _mm256_permute4x64_epi64(u64_prod_1, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_2 = _mm256_permute4x64_epi64(u64_prod_2, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_3 = _mm256_permute4x64_epi64(u64_prod_3, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_4 = _mm256_permute4x64_epi64(u64_prod_4, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_5 = _mm256_permute4x64_epi64(u64_prod_5, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_6 = _mm256_permute4x64_epi64(u64_prod_6, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_7 = _mm256_permute4x64_epi64(u64_prod_7, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_8 = _mm256_permute4x64_epi64(u64_prod_8, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_9 = _mm256_permute4x64_epi64(u64_prod_9, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_10 = _mm256_permute4x64_epi64(u64_prod_10, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_11 = _mm256_permute4x64_epi64(u64_prod_11, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_12 = _mm256_permute4x64_epi64(u64_prod_12, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_13 = _mm256_permute4x64_epi64(u64_prod_13, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_14 = _mm256_permute4x64_epi64(u64_prod_14, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_15 = _mm256_permute4x64_epi64(u64_prod_15, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_16 = _mm256_permute4x64_epi64(u64_prod_16, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_17 = _mm256_permute4x64_epi64(u64_prod_17, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_18 = _mm256_permute4x64_epi64(u64_prod_18, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_19 = _mm256_permute4x64_epi64(u64_prod_19, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_20 = _mm256_permute4x64_epi64(u64_prod_20, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_21 = _mm256_permute4x64_epi64(u64_prod_21, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_22 = _mm256_permute4x64_epi64(u64_prod_22, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_23 = _mm256_permute4x64_epi64(u64_prod_23, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_24 = _mm256_permute4x64_epi64(u64_prod_24, 0b10'01'00'11);
      const __m256i u32_hi_prod_slide_1 = _mm256_blend_epi32(u32_hi_prod_perm_1, ZERO, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_2 = _mm256_blend_epi32(u32_hi_prod_perm_2, u32_hi_prod_perm_1, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_3 = _mm256_blend_epi32(u32_hi_prod_perm_3, u32_hi_prod_perm_2, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_4 = _mm256_blend_epi32(u32_hi_prod_perm_4, u32_hi_prod_perm_3, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_5 = _mm256_blend_epi32(u32_hi_prod_perm_5, u32_hi_prod_perm_4, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_6 = _mm256_blend_epi32(u32_hi_prod_perm_6, u32_hi_prod_perm_5, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_7 = _mm256_blend_epi32(u32_hi_prod_perm_7, u32_hi_prod_perm_6, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_8 = _mm256_blend_epi32(u32_hi_prod_perm_8, u32_hi_prod_perm_7, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_9 = _mm256_blend_epi32(u32_hi_prod_perm_9, u32_hi_prod_perm_8, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_10 = _mm256_blend_epi32(u32_hi_prod_perm_10, u32_hi_prod_perm_9, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_11 = _mm256_blend_epi32(u32_hi_prod_perm_11, u32_hi_prod_perm_10, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_12 = _mm256_blend_epi32(u32_hi_prod_perm_12, u32_hi_prod_perm_11, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_13 = _mm256_blend_epi32(u32_hi_prod_perm_13, u32_hi_prod_perm_12, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_14 = _mm256_blend_epi32(u32_hi_prod_perm_14, u32_hi_prod_perm_13, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_15 = _mm256_blend_epi32(u32_hi_prod_perm_15, u32_hi_prod_perm_14, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_16 = _mm256_blend_epi32(u32_hi_prod_perm_16, u32_hi_prod_perm_15, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_17 = _mm256_blend_epi32(u32_hi_prod_perm_17, u32_hi_prod_perm_16, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_18 = _mm256_blend_epi32(u32_hi_prod_perm_18, u32_hi_prod_perm_17, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_19 = _mm256_blend_epi32(u32_hi_prod_perm_19, u32_hi_prod_perm_18, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_20 = _mm256_blend_epi32(u32_hi_prod_perm_20, u32_hi_prod_perm_19, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_21 = _mm256_blend_epi32(u32_hi_prod_perm_21, u32_hi_prod_perm_20, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_22 = _mm256_blend_epi32(u32_hi_prod_perm_22, u32_hi_prod_perm_21, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_23 = _mm256_blend_epi32(u32_hi_prod_perm_23, u32_hi_prod_perm_22, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_24 = _mm256_blend_epi32(u32_hi_prod_perm_24, u32_hi_prod_perm_23, 0b00'00'00'11);
      rrprime_1 = _mm256_add_epi64(u64_magic_res_1, u32_hi_prod_slide_1);
      rrprime_2 = _mm256_add_epi64(u64_magic_res_2, u32_hi_prod_slide_2);
      rrprime_3 = _mm256_add_epi64(u64_magic_res_3, u32_hi_prod_slide_3);
      rrprime_4 = _mm256_add_epi64(u64_magic_res_4, u32_hi_prod_slide_4);
      rrprime_5 = _mm256_add_epi64(u64_magic_res_5, u32_hi_prod_slide_5);
      rrprime_6 = _mm256_add_epi64(u64_magic_res_6, u32_hi_prod_slide_6);
      rrprime_7 = _mm256_add_epi64(u64_magic_res_7, u32_hi_prod_slide_7);
      rrprime_8 = _mm256_add_epi64(u64_magic_res_8, u32_hi_prod_slide_8);
      rrprime_9 = _mm256_add_epi64(u64_magic_res_9, u32_hi_prod_slide_9);
      rrprime_10 = _mm256_add_epi64(u64_magic_res_10, u32_hi_prod_slide_10);
      rrprime_11 = _mm256_add_epi64(u64_magic_res_11, u32_hi_prod_slide_11);
      rrprime_12 = _mm256_add_epi64(u64_magic_res_12, u32_hi_prod_slide_12);
      rrprime_13 = _mm256_add_epi64(u64_magic_res_13, u32_hi_prod_slide_13);
      rrprime_14 = _mm256_add_epi64(u64_magic_res_14, u32_hi_prod_slide_14);
      rrprime_15 = _mm256_add_epi64(u64_magic_res_15, u32_hi_prod_slide_15);
      rrprime_16 = _mm256_add_epi64(u64_magic_res_16, u32_hi_prod_slide_16);
      rrprime_17 = _mm256_add_epi64(u64_magic_res_17, u32_hi_prod_slide_17);
      rrprime_18 = _mm256_add_epi64(u64_magic_res_18, u32_hi_prod_slide_18);
      rrprime_19 = _mm256_add_epi64(u64_magic_res_19, u32_hi_prod_slide_19);
      rrprime_20 = _mm256_add_epi64(u64_magic_res_20, u32_hi_prod_slide_20);
      rrprime_21 = _mm256_add_epi64(u64_magic_res_21, u32_hi_prod_slide_21);
      rrprime_22 = _mm256_add_epi64(u64_magic_res_22, u32_hi_prod_slide_22);
      rrprime_23 = _mm256_add_epi64(u64_magic_res_23, u32_hi_prod_slide_23);
      rrprime_24 = _mm256_add_epi64(u64_magic_res_24, u32_hi_prod_slide_24);
      const __m256i rrprime_perm_1 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_1, 0b10'01'00'11), 32U);
      const __m256i rrprime_perm_2 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_2, 0b10'01'00'11), 32U);
      const __m256i rrprime_perm_3 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_3, 0b10'01'00'11), 32U);
      const __m256i rrprime_perm_4 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_4, 0b10'01'00'11), 32U);
      const __m256i rrprime_perm_5 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_5, 0b10'01'00'11), 32U);
      const __m256i rrprime_perm_6 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_6, 0b10'01'00'11), 32U);
      const __m256i rrprime_perm_7 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_7, 0b10'01'00'11), 32U);
      const __m256i rrprime_perm_8 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_8, 0b10'01'00'11), 32U);
      const __m256i rrprime_perm_9 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_9, 0b10'01'00'11), 32U);
      const __m256i rrprime_perm_10 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_10, 0b10'01'00'11), 32U);
      const __m256i rrprime_perm_11 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_11, 0b10'01'00'11), 32U);
      const __m256i rrprime_perm_12 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_12, 0b10'01'00'11), 32U);
      const __m256i rrprime_perm_13 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_13, 0b10'01'00'11), 32U);
      const __m256i rrprime_perm_14 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_14, 0b10'01'00'11), 32U);
      const __m256i rrprime_perm_15 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_15, 0b10'01'00'11), 32U);
      const __m256i rrprime_perm_16 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_16, 0b10'01'00'11), 32U);
      const __m256i rrprime_perm_17 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_17, 0b10'01'00'11), 32U);
      const __m256i rrprime_perm_18 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_18, 0b10'01'00'11), 32U);
      const __m256i rrprime_perm_19 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_19, 0b10'01'00'11), 32U);
      const __m256i rrprime_perm_20 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_20, 0b10'01'00'11), 32U);
      const __m256i rrprime_perm_21 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_21, 0b10'01'00'11), 32U);
      const __m256i rrprime_perm_22 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_22, 0b10'01'00'11), 32U);
      const __m256i rrprime_perm_23 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_23, 0b10'01'00'11), 32U);
      const __m256i rrprime_perm_24 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_24, 0b10'01'00'11), 32U);
      const __m256i rrprime_slide_1 = _mm256_blend_epi32(rrprime_perm_1, ZERO, 0b00'00'00'11);
      const __m256i rrprime_slide_2 = _mm256_blend_epi32(rrprime_perm_2, rrprime_perm_1, 0b00'00'00'11);
      const __m256i rrprime_slide_3 = _mm256_blend_epi32(rrprime_perm_3, rrprime_perm_2, 0b00'00'00'11);
      const __m256i rrprime_slide_4 = _mm256_blend_epi32(rrprime_perm_4, rrprime_perm_3, 0b00'00'00'11);
      const __m256i rrprime_slide_5 = _mm256_blend_epi32(rrprime_perm_5, rrprime_perm_4, 0b00'00'00'11);
      const __m256i rrprime_slide_6 = _mm256_blend_epi32(rrprime_perm_6, rrprime_perm_5, 0b00'00'00'11);
      const __m256i rrprime_slide_7 = _mm256_blend_epi32(rrprime_perm_7, rrprime_perm_6, 0b00'00'00'11);
      const __m256i rrprime_slide_8 = _mm256_blend_epi32(rrprime_perm_8, rrprime_perm_7, 0b00'00'00'11);
      const __m256i rrprime_slide_9 = _mm256_blend_epi32(rrprime_perm_9, rrprime_perm_8, 0b00'00'00'11);
      const __m256i rrprime_slide_10 = _mm256_blend_epi32(rrprime_perm_10, rrprime_perm_9, 0b00'00'00'11);
      const __m256i rrprime_slide_11 = _mm256_blend_epi32(rrprime_perm_11, rrprime_perm_10, 0b00'00'00'11);
      const __m256i rrprime_slide_12 = _mm256_blend_epi32(rrprime_perm_12, rrprime_perm_11, 0b00'00'00'11);
      const __m256i rrprime_slide_13 = _mm256_blend_epi32(rrprime_perm_13, rrprime_perm_12, 0b00'00'00'11);
      const __m256i rrprime_slide_14 = _mm256_blend_epi32(rrprime_perm_14, rrprime_perm_13, 0b00'00'00'11);
      const __m256i rrprime_slide_15 = _mm256_blend_epi32(rrprime_perm_15, rrprime_perm_14, 0b00'00'00'11);
      const __m256i rrprime_slide_16 = _mm256_blend_epi32(rrprime_perm_16, rrprime_perm_15, 0b00'00'00'11);
      const __m256i rrprime_slide_17 = _mm256_blend_epi32(rrprime_perm_17, rrprime_perm_16, 0b00'00'00'11);
      const __m256i rrprime_slide_18 = _mm256_blend_epi32(rrprime_perm_18, rrprime_perm_17, 0b00'00'00'11);
      const __m256i rrprime_slide_19 = _mm256_blend_epi32(rrprime_perm_19, rrprime_perm_18, 0b00'00'00'11);
      const __m256i rrprime_slide_20 = _mm256_blend_epi32(rrprime_perm_20, rrprime_perm_19, 0b00'00'00'11);
      const __m256i rrprime_slide_21 = _mm256_blend_epi32(rrprime_perm_21, rrprime_perm_20, 0b00'00'00'11);
      const __m256i rrprime_slide_22 = _mm256_blend_epi32(rrprime_perm_22, rrprime_perm_21, 0b00'00'00'11);
      const __m256i rrprime_slide_23 = _mm256_blend_epi32(rrprime_perm_23, rrprime_perm_22, 0b00'00'00'11);
      const __m256i rrprime_slide_24 = _mm256_blend_epi32(rrprime_perm_24, rrprime_perm_23, 0b00'00'00'11);
      rrprime_1 = _mm256_add_epi32(rrprime_1, rrprime_slide_1);
      rrprime_2 = _mm256_add_epi32(rrprime_2, rrprime_slide_2);
      rrprime_3 = _mm256_add_epi32(rrprime_3, rrprime_slide_3);
      rrprime_4 = _mm256_add_epi32(rrprime_4, rrprime_slide_4);
      rrprime_5 = _mm256_add_epi32(rrprime_5, rrprime_slide_5);
      rrprime_6 = _mm256_add_epi32(rrprime_6, rrprime_slide_6);
      rrprime_7 = _mm256_add_epi32(rrprime_7, rrprime_slide_7);
      rrprime_8 = _mm256_add_epi32(rrprime_8, rrprime_slide_8);
      rrprime_9 = _mm256_add_epi32(rrprime_9, rrprime_slide_9);
      rrprime_10 = _mm256_add_epi32(rrprime_10, rrprime_slide_10);
      rrprime_11 = _mm256_add_epi32(rrprime_11, rrprime_slide_11);
      rrprime_12 = _mm256_add_epi32(rrprime_12, rrprime_slide_12);
      rrprime_13 = _mm256_add_epi32(rrprime_13, rrprime_slide_13);
      rrprime_14 = _mm256_add_epi32(rrprime_14, rrprime_slide_14);
      rrprime_15 = _mm256_add_epi32(rrprime_15, rrprime_slide_15);
      rrprime_16 = _mm256_add_epi32(rrprime_16, rrprime_slide_16);
      rrprime_17 = _mm256_add_epi32(rrprime_17, rrprime_slide_17);
      rrprime_18 = _mm256_add_epi32(rrprime_18, rrprime_slide_18);
      rrprime_19 = _mm256_add_epi32(rrprime_19, rrprime_slide_19);
      rrprime_20 = _mm256_add_epi32(rrprime_20, rrprime_slide_20);
      rrprime_21 = _mm256_add_epi32(rrprime_21, rrprime_slide_21);
      rrprime_22 = _mm256_add_epi32(rrprime_22, rrprime_slide_22);
      rrprime_23 = _mm256_add_epi32(rrprime_23, rrprime_slide_23);
      rrprime_24 = _mm256_add_epi32(rrprime_24, rrprime_slide_24);
    }

    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[0]), rrprime_1);
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[4]), rrprime_2);
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[8]), rrprime_3);
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[12]), rrprime_4);
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[16]), rrprime_5);
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[20]), rrprime_6);
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[24]), rrprime_7);
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[28]), rrprime_8);
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[32]), rrprime_9);
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[36]), rrprime_10);
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[40]), rrprime_11);
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[44]), rrprime_12);
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[48]), rrprime_13);
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[52]), rrprime_14);
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[56]), rrprime_15);
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[60]), rrprime_16);
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[64]), rrprime_17);
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[68]), rrprime_18);
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[72]), rrprime_19);
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[76]), rrprime_20);
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[80]), rrprime_21);
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[84]), rrprime_22);
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[88]), rrprime_23);
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[92]), rrprime_24);

    {
      const __m128i u32_pack_1 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_1, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));
      const __m128i u32_pack_2 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_2, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));
      const __m128i u32_pack_3 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_3, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));
      const __m128i u32_pack_4 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_4, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));
      const __m128i u32_pack_5 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_5, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));
      const __m128i u32_pack_6 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_6, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));
      const __m128i u32_pack_7 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_7, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));
      const __m128i u32_pack_8 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_8, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));
      const __m128i u32_pack_9 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_9, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));
      const __m128i u32_pack_10 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_10, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));
      const __m128i u32_pack_11 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_11, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));
      const __m128i u32_pack_12 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_12, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));
      const __m128i u32_pack_13 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_13, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));
      const __m128i u32_pack_14 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_14, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));
      const __m128i u32_pack_15 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_15, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));
      const __m128i u32_pack_16 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_16, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));
      const __m128i u32_pack_17 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_17, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));
      const __m128i u32_pack_18 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_18, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));
      const __m128i u32_pack_19 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_19, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));
      const __m128i u32_pack_20 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_20, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));
      const __m128i u32_pack_21 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_21, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));
      const __m128i u32_pack_22 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_22, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));
      const __m128i u32_pack_23 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_23, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));
      const __m128i u32_pack_24 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_24, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));

      _mm_storeu_si128(reinterpret_cast<__m128i *>(&SIMD_PRE_WRITTEN[0]), u32_pack_1);
      _mm_storeu_si128(reinterpret_cast<__m128i *>(&SIMD_PRE_WRITTEN[4]), u32_pack_2);
      _mm_storeu_si128(reinterpret_cast<__m128i *>(&SIMD_PRE_WRITTEN[8]), u32_pack_3);
      _mm_storeu_si128(reinterpret_cast<__m128i *>(&SIMD_PRE_WRITTEN[12]), u32_pack_4);
      _mm_storeu_si128(reinterpret_cast<__m128i *>(&SIMD_PRE_WRITTEN[16]), u32_pack_5);
      _mm_storeu_si128(reinterpret_cast<__m128i *>(&SIMD_PRE_WRITTEN[20]), u32_pack_6);
      _mm_storeu_si128(reinterpret_cast<__m128i *>(&SIMD_PRE_WRITTEN[24]), u32_pack_7);
      _mm_storeu_si128(reinterpret_cast<__m128i *>(&SIMD_PRE_WRITTEN[28]), u32_pack_8);
      _mm_storeu_si128(reinterpret_cast<__m128i *>(&SIMD_PRE_WRITTEN[32]), u32_pack_9);
      _mm_storeu_si128(reinterpret_cast<__m128i *>(&SIMD_PRE_WRITTEN[36]), u32_pack_10);
      _mm_storeu_si128(reinterpret_cast<__m128i *>(&SIMD_PRE_WRITTEN[40]), u32_pack_11);
      _mm_storeu_si128(reinterpret_cast<__m128i *>(&SIMD_PRE_WRITTEN[44]), u32_pack_12);
      _mm_storeu_si128(reinterpret_cast<__m128i *>(&SIMD_PRE_WRITTEN[48]), u32_pack_13);
      _mm_storeu_si128(reinterpret_cast<__m128i *>(&SIMD_PRE_WRITTEN[52]), u32_pack_14);
      _mm_storeu_si128(reinterpret_cast<__m128i *>(&SIMD_PRE_WRITTEN[56]), u32_pack_15);
      _mm_storeu_si128(reinterpret_cast<__m128i *>(&SIMD_PRE_WRITTEN[60]), u32_pack_16);
      _mm_storeu_si128(reinterpret_cast<__m128i *>(&SIMD_PRE_WRITTEN[64]), u32_pack_17);
      _mm_storeu_si128(reinterpret_cast<__m128i *>(&SIMD_PRE_WRITTEN[68]), u32_pack_18);
      _mm_storeu_si128(reinterpret_cast<__m128i *>(&SIMD_PRE_WRITTEN[72]), u32_pack_19);
      _mm_storeu_si128(reinterpret_cast<__m128i *>(&SIMD_PRE_WRITTEN[76]), u32_pack_20);
      _mm_storeu_si128(reinterpret_cast<__m128i *>(&SIMD_PRE_WRITTEN[80]), u32_pack_21);
      _mm_storeu_si128(reinterpret_cast<__m128i *>(&SIMD_PRE_WRITTEN[84]), u32_pack_22);
      _mm_storeu_si128(reinterpret_cast<__m128i *>(&SIMD_PRE_WRITTEN[88]), u32_pack_23);
      _mm_storeu_si128(reinterpret_cast<__m128i *>(&SIMD_PRE_WRITTEN[92]), u32_pack_24);
    }

    slow_pre_arr[0] = 1; // initialize r = 1

    for(uint32_t i = 0; i < P; ++i)
    {
      uint32_t carry = 0;
      for(unsigned int &w : slow_pre_arr)
      {
        uint64_t pp = (uint64_t)w * 10ull + carry;
        w = (uint32_t)pp;
        carry = (uint32_t)(pp >> 32);
      }
    }

    std::array<uint32_t, NUM_WORDS> x10byx10_arr = slow_pre_arr;

    size_t first = 0;

    while(first < slow_pre_arr.size() && slow_pre_arr[first] == 0)
    {
      ++first;
    }

    if(first != 0 && first != slow_pre_arr.size())
    {
      const size_t count = slow_pre_arr.size() - first;

      std::memmove(slow_pre_arr.data(), slow_pre_arr.data() + first, count * sizeof(uint32_t));

      std::memset(slow_pre_arr.data() + count, 0, first * sizeof(uint32_t));
    }

    if(SIMD_PRE_WRITTEN != slow_pre_arr)
    {
      std::cout << "k = " << k << "; p = " << P << '\n';
      goto backwards;
    }

    assert(SIMD_PRE_WRITTEN == slow_pre_arr);

    // unsigned lead_z = 24u - (k_div_32 - ((k * 1233u) >> 17u)) - (k < 32); //+ (k_div_32 & 0b0111);

    unsigned digits_computed = 0;
    unsigned word_idx = k / 32u;
    unsigned simdy_idx = (K_DIV_64 - ((k * 1233u) >> 17u));
    uint64_t chunk = 0;
    unsigned next_8;
    unsigned simdy_8;

    const unsigned bit_shift = k & ((1u << 5u) - 1);
    const unsigned mod_mask = (bit_shift == 0) ? 0 : ((1u << bit_shift) - 1);

    while(digits_computed < TOTAL_DIGITS)
    {
      // step a & b: multiply by 10^8 and cascade the carry
      uint32_t carry = 0; // carry max is 10^8, fits safely in uint32_t
      for(unsigned int &w : x10byx10_arr)
      {
        // max size: (2^32 - 1) * 10^8 + 10^8 = 2^32 * 10^8. fits comfortably in uint64_t.
        uint64_t p = (uint64_t)w * 100'000'000u + carry;
        w = (uint32_t)p;
        carry = (uint32_t)(p >> 32);
      }

      {
        __m256i rrrprime_1 = _mm256_loadu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[0]));
        __m256i rrrprime_2 = _mm256_loadu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[4]));
        __m256i rrrprime_3 = _mm256_loadu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[8]));
        __m256i rrrprime_4 = _mm256_loadu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[12]));
        __m256i rrrprime_5 = _mm256_loadu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[16]));
        __m256i rrrprime_6 = _mm256_loadu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[20]));
        __m256i rrrprime_7 = _mm256_loadu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[24]));
        const __m256i u64_prod_1 = _mm256_mul_epu32(rrrprime_1, R_25);
        const __m256i u64_prod_2 = _mm256_mul_epu32(rrrprime_2, R_25);
        const __m256i u64_prod_3 = _mm256_mul_epu32(rrrprime_3, R_25);
        const __m256i u64_prod_4 = _mm256_mul_epu32(rrrprime_4, R_25);
        const __m256i u64_prod_5 = _mm256_mul_epu32(rrrprime_5, R_25);
        const __m256i u64_prod_6 = _mm256_mul_epu32(rrrprime_6, R_25);
        const __m256i u64_prod_7 = _mm256_mul_epu32(rrrprime_7, R_25);
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
        const __m256i rrprime_perm_1 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrrprime_1, 0b10'01'00'11), 32u);
        const __m256i rrprime_perm_2 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrrprime_2, 0b10'01'00'11), 32u);
        const __m256i rrprime_perm_3 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrrprime_3, 0b10'01'00'11), 32u);
        const __m256i rrprime_perm_4 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrrprime_4, 0b10'01'00'11), 32u);
        const __m256i rrprime_perm_5 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrrprime_5, 0b10'01'00'11), 32u);
        const __m256i rrprime_perm_6 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrrprime_6, 0b10'01'00'11), 32u);
        const __m256i rrprime_perm_7 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrrprime_7, 0b10'01'00'11), 32u);
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

/*
int main()
{
  constexpr uint32_t pow_5_correction[] = { 1, 10, 100, 1'000, 10'000, 100'000, 1'000'000, 10'000'000, 100'000'000 };
  constexpr uint8_t pow_5_idx[] = { 0, 24, 48, 72, 96, 120, 144, 168, 192, 216, 240 };
  constexpr uint8_t pow_5_e[] = { 0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40 };

  constexpr uint32_t pow_5_cache[] = {
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
    const unsigned p = std::floor(k * std::log10(2));

    const unsigned p_div_8 = p >> 3u;

    const unsigned p_div_32 = p >> 5u;

    const unsigned k_div_32 = static_cast<unsigned>(k) >> 5u;

    const __m256i zero = _mm256_setzero_si256();

    // total digits in 5^k
    const uint32_t total_digits = std::floor(k * std::log10(5)) + 1;
    constexpr unsigned num_words = 40;
    std::array<uint32_t, num_words> simd_pre_written = { 0 };
    std::array<uint32_t, num_words> slow_pre_arr = { 0 };

    std::array<uint64_t, 28> simd_array;
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&simd_array[0]), zero);
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&simd_array[4]), zero);
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&simd_array[8]), zero);
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&simd_array[12]), zero);
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&simd_array[16]), zero);
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&simd_array[20]), zero);
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&simd_array[24]), zero);

    __m256i rrprime_1 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&pow_5_cache[pow_5_idx[p_div_32]])));
    __m256i rrprime_2 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&pow_5_cache[pow_5_idx[p_div_32] + 4u])));
    __m256i rrprime_3 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&pow_5_cache[pow_5_idx[p_div_32] + 8u])));
    __m256i rrprime_4 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&pow_5_cache[pow_5_idx[p_div_32] + 12u])));
    __m256i rrprime_5 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&pow_5_cache[pow_5_idx[p_div_32] + 16u])));
    __m256i rrprime_6 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&pow_5_cache[pow_5_idx[p_div_32] + 20u])));
    __m256i rrprime_7 = zero;
    const __m256i r1e8 = _mm256_set1_epi64x(pow_5_correction[8]);
    const __m256i r1remmis = _mm256_set1_epi64x(pow_5_correction[p - (p_div_8 << 3u)]);

    const unsigned e_o = pow_5_e[p_div_32];

    for(unsigned e = e_o; e < p_div_8; e++)
    {
      const __m256i u64_prod_1 = _mm256_mul_epu32(rrprime_1, r1e8);
      const __m256i u64_prod_2 = _mm256_mul_epu32(rrprime_2, r1e8);
      const __m256i u64_prod_3 = _mm256_mul_epu32(rrprime_3, r1e8);
      const __m256i u64_prod_4 = _mm256_mul_epu32(rrprime_4, r1e8);
      const __m256i u64_prod_5 = _mm256_mul_epu32(rrprime_5, r1e8);
      const __m256i u64_prod_6 = _mm256_mul_epu32(rrprime_6, r1e8);
      const __m256i u64_prod_7 = _mm256_mul_epu32(rrprime_7, r1e8);
      const __m256i u32_lo_prod_1 = _mm256_blend_epi32(u64_prod_1, zero, 0b1010'1010);
      const __m256i u32_lo_prod_2 = _mm256_blend_epi32(u64_prod_2, zero, 0b1010'1010);
      const __m256i u32_lo_prod_3 = _mm256_blend_epi32(u64_prod_3, zero, 0b1010'1010);
      const __m256i u32_lo_prod_4 = _mm256_blend_epi32(u64_prod_4, zero, 0b1010'1010);
      const __m256i u32_lo_prod_5 = _mm256_blend_epi32(u64_prod_5, zero, 0b1010'1010);
      const __m256i u32_lo_prod_6 = _mm256_blend_epi32(u64_prod_6, zero, 0b1010'1010);
      const __m256i u32_lo_prod_7 = _mm256_blend_epi32(u64_prod_7, zero, 0b1010'1010);
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
      const __m256i u32_hi_prod_slide_1 = _mm256_blend_epi32(u32_hi_prod_perm_1, zero, 0b00'00'00'11);
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
      const __m256i rrprime_perm_1 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_1, 0b10'01'00'11), 32u);
      const __m256i rrprime_perm_2 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_2, 0b10'01'00'11), 32u);
      const __m256i rrprime_perm_3 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_3, 0b10'01'00'11), 32u);
      const __m256i rrprime_perm_4 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_4, 0b10'01'00'11), 32u);
      const __m256i rrprime_perm_5 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_5, 0b10'01'00'11), 32u);
      const __m256i rrprime_perm_6 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_6, 0b10'01'00'11), 32u);
      const __m256i rrprime_perm_7 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_7, 0b10'01'00'11), 32u);
      const __m256i rrprime_slide_1 = _mm256_blend_epi32(rrprime_perm_1, zero, 0b00'00'00'11);
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
      const __m256i u64_prod_1 = _mm256_mul_epu32(rrprime_1, r1remmis);
      const __m256i u64_prod_2 = _mm256_mul_epu32(rrprime_2, r1remmis);
      const __m256i u64_prod_3 = _mm256_mul_epu32(rrprime_3, r1remmis);
      const __m256i u64_prod_4 = _mm256_mul_epu32(rrprime_4, r1remmis);
      const __m256i u64_prod_5 = _mm256_mul_epu32(rrprime_5, r1remmis);
      const __m256i u64_prod_6 = _mm256_mul_epu32(rrprime_6, r1remmis);
      const __m256i u64_prod_7 = _mm256_mul_epu32(rrprime_7, r1remmis);
      const __m256i u32_lo_prod_1 = _mm256_blend_epi32(u64_prod_1, zero, 0b1010'1010);
      const __m256i u32_lo_prod_2 = _mm256_blend_epi32(u64_prod_2, zero, 0b1010'1010);
      const __m256i u32_lo_prod_3 = _mm256_blend_epi32(u64_prod_3, zero, 0b1010'1010);
      const __m256i u32_lo_prod_4 = _mm256_blend_epi32(u64_prod_4, zero, 0b1010'1010);
      const __m256i u32_lo_prod_5 = _mm256_blend_epi32(u64_prod_5, zero, 0b1010'1010);
      const __m256i u32_lo_prod_6 = _mm256_blend_epi32(u64_prod_6, zero, 0b1010'1010);
      const __m256i u32_lo_prod_7 = _mm256_blend_epi32(u64_prod_7, zero, 0b1010'1010);
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
      const __m256i u32_hi_prod_slide_1 = _mm256_blend_epi32(u32_hi_prod_perm_1, zero, 0b00'00'00'11);
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
      const __m256i rrprime_perm_1 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_1, 0b10'01'00'11), 32u);
      const __m256i rrprime_perm_2 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_2, 0b10'01'00'11), 32u);
      const __m256i rrprime_perm_3 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_3, 0b10'01'00'11), 32u);
      const __m256i rrprime_perm_4 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_4, 0b10'01'00'11), 32u);
      const __m256i rrprime_perm_5 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_5, 0b10'01'00'11), 32u);
      const __m256i rrprime_perm_6 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_6, 0b10'01'00'11), 32u);
      const __m256i rrprime_perm_7 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_7, 0b10'01'00'11), 32u);
      const __m256i rrprime_slide_1 = _mm256_blend_epi32(rrprime_perm_1, zero, 0b00'00'00'11);
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

    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&simd_array[0]), rrprime_1);
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&simd_array[4]), rrprime_2);
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&simd_array[8]), rrprime_3);
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&simd_array[12]), rrprime_4);
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&simd_array[16]), rrprime_5);
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&simd_array[20]), rrprime_6);
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&simd_array[24]), rrprime_7);

    {
      const __m128i u32_pack_1 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_1, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));
      const __m128i u32_pack_2 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_2, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));
      const __m128i u32_pack_3 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_3, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));
      const __m128i u32_pack_4 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_4, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));
      const __m128i u32_pack_5 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_5, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));
      const __m128i u32_pack_6 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_6, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));
      const __m128i u32_pack_7 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_7, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));

      _mm_storeu_si128(reinterpret_cast<__m128i *>(&simd_pre_written[0]), u32_pack_1);
      _mm_storeu_si128(reinterpret_cast<__m128i *>(&simd_pre_written[4]), u32_pack_2);
      _mm_storeu_si128(reinterpret_cast<__m128i *>(&simd_pre_written[8]), u32_pack_3);
      _mm_storeu_si128(reinterpret_cast<__m128i *>(&simd_pre_written[12]), u32_pack_4);
      _mm_storeu_si128(reinterpret_cast<__m128i *>(&simd_pre_written[16]), u32_pack_5);
      _mm_storeu_si128(reinterpret_cast<__m128i *>(&simd_pre_written[20]), u32_pack_6);
      _mm_storeu_si128(reinterpret_cast<__m128i *>(&simd_pre_written[24]), u32_pack_7);
    }

    slow_pre_arr[0] = 1; // initialize r = 1

    for(uint32_t i = 0; i < p; ++i)
    {
      uint32_t carry = 0;
      for(unsigned int &w : slow_pre_arr)
      {
        uint64_t p = (uint64_t)w * 10ull + carry;
        w = (uint32_t)p;
        carry = (uint32_t)(p >> 32);
      }
    }

    std::array<uint32_t, num_words> x10byx10_arr = slow_pre_arr;

    size_t first = 0;

    while(first < slow_pre_arr.size() && slow_pre_arr[first] == 0)
    {
      ++first;
    }

    if(first != 0 && first != slow_pre_arr.size())
    {
      const size_t count = slow_pre_arr.size() - first;

      std::memmove(slow_pre_arr.data(), slow_pre_arr.data() + first, count * sizeof(uint32_t));

      std::memset(slow_pre_arr.data() + count, 0, first * sizeof(uint32_t));
    }

    if(simd_pre_written != slow_pre_arr)
    {
      std::cout << "k = " << k << "; p = " << p << '\n';
      goto backwards;
    }

    assert(simd_pre_written == slow_pre_arr);

    // unsigned lead_z = 24u - (k_div_32 - ((k * 1233u) >> 17u)) - (k < 32); //+ (k_div_32 & 0b0111);

    unsigned digits_computed = 0;
    unsigned word_idx = k / 32u;
    unsigned simdy_idx = (k_div_32 - ((k * 1233u) >> 17u));
    uint64_t chunk = 0;
    unsigned next_8;
    unsigned simdy_8;

    const unsigned bit_shift = k & ((1u << 5u) - 1);
    const unsigned mod_mask = (bit_shift == 0) ? 0 : ((1u << bit_shift) - 1);

    while(digits_computed < total_digits)
    {
      // step a & b: multiply by 10^8 and cascade the carry
      uint32_t carry = 0; // carry max is 10^8, fits safely in uint32_t
      for(unsigned int &w : x10byx10_arr)
      {
        // max size: (2^32 - 1) * 10^8 + 10^8 = 2^32 * 10^8. fits comfortably in uint64_t.
        uint64_t p = (uint64_t)w * 100'000'000u + carry;
        w = (uint32_t)p;
        carry = (uint32_t)(p >> 32);
      }

      {
        __m256i rrrprime_1 = _mm256_loadu_si256(reinterpret_cast<__m256i *>(&simd_array[0]));
        __m256i rrrprime_2 = _mm256_loadu_si256(reinterpret_cast<__m256i *>(&simd_array[4]));
        __m256i rrrprime_3 = _mm256_loadu_si256(reinterpret_cast<__m256i *>(&simd_array[8]));
        __m256i rrrprime_4 = _mm256_loadu_si256(reinterpret_cast<__m256i *>(&simd_array[12]));
        __m256i rrrprime_5 = _mm256_loadu_si256(reinterpret_cast<__m256i *>(&simd_array[16]));
        __m256i rrrprime_6 = _mm256_loadu_si256(reinterpret_cast<__m256i *>(&simd_array[20]));
        __m256i rrrprime_7 = _mm256_loadu_si256(reinterpret_cast<__m256i *>(&simd_array[24]));
        const __m256i u64_prod_1 = _mm256_mul_epu32(rrrprime_1, r1e8);
        const __m256i u64_prod_2 = _mm256_mul_epu32(rrrprime_2, r1e8);
        const __m256i u64_prod_3 = _mm256_mul_epu32(rrrprime_3, r1e8);
        const __m256i u64_prod_4 = _mm256_mul_epu32(rrrprime_4, r1e8);
        const __m256i u64_prod_5 = _mm256_mul_epu32(rrrprime_5, r1e8);
        const __m256i u64_prod_6 = _mm256_mul_epu32(rrrprime_6, r1e8);
        const __m256i u64_prod_7 = _mm256_mul_epu32(rrrprime_7, r1e8);
        const __m256i u32_lo_prod_1 = _mm256_blend_epi32(u64_prod_1, zero, 0b1010'1010);
        const __m256i u32_lo_prod_2 = _mm256_blend_epi32(u64_prod_2, zero, 0b1010'1010);
        const __m256i u32_lo_prod_3 = _mm256_blend_epi32(u64_prod_3, zero, 0b1010'1010);
        const __m256i u32_lo_prod_4 = _mm256_blend_epi32(u64_prod_4, zero, 0b1010'1010);
        const __m256i u32_lo_prod_5 = _mm256_blend_epi32(u64_prod_5, zero, 0b1010'1010);
        const __m256i u32_lo_prod_6 = _mm256_blend_epi32(u64_prod_6, zero, 0b1010'1010);
        const __m256i u32_lo_prod_7 = _mm256_blend_epi32(u64_prod_7, zero, 0b1010'1010);
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
        const __m256i u32_hi_prod_slide_1 = _mm256_blend_epi32(u32_hi_prod_perm_1, zero, 0b00'00'00'11);
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
        const __m256i rrprime_perm_1 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrrprime_1, 0b10'01'00'11), 32u);
        const __m256i rrprime_perm_2 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrrprime_2, 0b10'01'00'11), 32u);
        const __m256i rrprime_perm_3 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrrprime_3, 0b10'01'00'11), 32u);
        const __m256i rrprime_perm_4 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrrprime_4, 0b10'01'00'11), 32u);
        const __m256i rrprime_perm_5 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrrprime_5, 0b10'01'00'11), 32u);
        const __m256i rrprime_perm_6 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrrprime_6, 0b10'01'00'11), 32u);
        const __m256i rrprime_perm_7 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrrprime_7, 0b10'01'00'11), 32u);
        const __m256i rrprime_slide_1 = _mm256_blend_epi32(rrprime_perm_1, zero, 0b00'00'00'11);
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
        rrrprime_1 = _mm256_blend_epi32(rrrprime_1, zero, 0b1010'1010);
        rrrprime_2 = _mm256_blend_epi32(rrrprime_2, zero, 0b1010'1010);
        rrrprime_3 = _mm256_blend_epi32(rrrprime_3, zero, 0b1010'1010);
        rrrprime_4 = _mm256_blend_epi32(rrrprime_4, zero, 0b1010'1010);
        rrrprime_5 = _mm256_blend_epi32(rrrprime_5, zero, 0b1010'1010);
        rrrprime_6 = _mm256_blend_epi32(rrrprime_6, zero, 0b1010'1010);
        rrrprime_7 = _mm256_blend_epi32(rrrprime_7, zero, 0b1010'1010);
        _mm256_storeu_si256(reinterpret_cast<__m256i *>(&simd_array[0]), rrrprime_1);
        _mm256_storeu_si256(reinterpret_cast<__m256i *>(&simd_array[4]), rrrprime_2);
        _mm256_storeu_si256(reinterpret_cast<__m256i *>(&simd_array[8]), rrrprime_3);
        _mm256_storeu_si256(reinterpret_cast<__m256i *>(&simd_array[12]), rrrprime_4);
        _mm256_storeu_si256(reinterpret_cast<__m256i *>(&simd_array[16]), rrrprime_5);
        _mm256_storeu_si256(reinterpret_cast<__m256i *>(&simd_array[20]), rrrprime_6);
        _mm256_storeu_si256(reinterpret_cast<__m256i *>(&simd_array[24]), rrrprime_7);
      }

      // step c: extract next8 chunk (r >> k)
      chunk = x10byx10_arr[word_idx] >> bit_shift;
      // cast to uint64_t before shifting to prevent any 32-bit boundary overflow
      chunk |= ((uint64_t)x10byx10_arr[word_idx + 1] << (32 - bit_shift));
      next_8 = (uint32_t)chunk;

      x10byx10_arr[word_idx] &= mod_mask;
      std::memset(static_cast<void *>(&x10byx10_arr[word_idx + 1]), 0, (num_words - word_idx - 1) * 4);

      chunk = simd_array[simdy_idx] >> bit_shift;
      chunk |= (simd_array[simdy_idx + 1] << (32 - bit_shift));
      simdy_8 = (uint32_t)chunk;
      simd_array[simdy_idx] &= mod_mask;
      _mm256_storeu_si256(reinterpret_cast<__m256i *>(&simd_array[simdy_idx + 1]), zero);

      if(simdy_8 != next_8)
      {
        std::cout << "k = " << k << "; p = " << p << '\n';
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
*/

/*
int main()
{
  constexpr uint32_t pow_5_correction[] = { 1, 10, 100, 1'000, 10'000, 100'000, 1'000'000, 10'000'000, 100'000'000 };
  constexpr uint8_t pow_5_idx[] = { 0, 24, 48, 72, 96, 120, 144, 168, 192, 216, 240 };
  constexpr uint8_t pow_5_e[] = { 0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40 };

  constexpr uint32_t pow_5_cache[] = {
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
    const unsigned p = std::floor(k * std::log10(2));

    const unsigned p_div_8 = p >> 3u;

    const unsigned p_div_32 = p >> 5u;

    const unsigned k_div_32 = static_cast<unsigned>(k) >> 5u;

    const __m256i zero = _mm256_setzero_si256();

    __m256i rrprime_1 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&pow_5_cache[pow_5_idx[p_div_32]])));
    __m256i rrprime_2 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&pow_5_cache[pow_5_idx[p_div_32] + 4u])));
    __m256i rrprime_3 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&pow_5_cache[pow_5_idx[p_div_32] + 8u])));
    __m256i rrprime_4 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&pow_5_cache[pow_5_idx[p_div_32] + 12u])));
    __m256i rrprime_5 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&pow_5_cache[pow_5_idx[p_div_32] + 16u])));
    __m256i rrprime_6 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&pow_5_cache[pow_5_idx[p_div_32] + 20u])));
    __m256i rrprime_7 = zero;
    __m256i rrprime_8 = zero;
    const __m256i r1e8 = _mm256_set1_epi64x(pow_5_correction[8]);
    const __m256i r1remmis = _mm256_set1_epi64x(pow_5_correction[p - (p_div_8 << 3u)]);

    const unsigned e_o = pow_5_e[p_div_32];

    for(unsigned e = e_o; e < p_div_8; e++)
    {
      const __m256i u64_prod_1 = _mm256_mul_epu32(rrprime_1, r1e8);
      const __m256i u64_prod_2 = _mm256_mul_epu32(rrprime_2, r1e8);
      const __m256i u64_prod_3 = _mm256_mul_epu32(rrprime_3, r1e8);
      const __m256i u64_prod_4 = _mm256_mul_epu32(rrprime_4, r1e8);
      const __m256i u64_prod_5 = _mm256_mul_epu32(rrprime_5, r1e8);
      const __m256i u64_prod_6 = _mm256_mul_epu32(rrprime_6, r1e8);
      const __m256i u64_prod_7 = _mm256_mul_epu32(rrprime_7, r1e8);
      const __m256i u64_prod_8 = _mm256_mul_epu32(rrprime_8, r1e8);
      const __m256i u32_lo_prod_1 = _mm256_blend_epi32(u64_prod_1, zero, 0b1010'1010);
      const __m256i u32_lo_prod_2 = _mm256_blend_epi32(u64_prod_2, zero, 0b1010'1010);
      const __m256i u32_lo_prod_3 = _mm256_blend_epi32(u64_prod_3, zero, 0b1010'1010);
      const __m256i u32_lo_prod_4 = _mm256_blend_epi32(u64_prod_4, zero, 0b1010'1010);
      const __m256i u32_lo_prod_5 = _mm256_blend_epi32(u64_prod_5, zero, 0b1010'1010);
      const __m256i u32_lo_prod_6 = _mm256_blend_epi32(u64_prod_6, zero, 0b1010'1010);
      const __m256i u32_lo_prod_7 = _mm256_blend_epi32(u64_prod_7, zero, 0b1010'1010);
      const __m256i u32_lo_prod_8 = _mm256_blend_epi32(u64_prod_8, zero, 0b1010'1010);
      const __m256i u32_hi_prod_1 = _mm256_srli_epi64(u64_prod_1, 32);
      const __m256i u32_hi_prod_2 = _mm256_srli_epi64(u64_prod_2, 32);
      const __m256i u32_hi_prod_3 = _mm256_srli_epi64(u64_prod_3, 32);
      const __m256i u32_hi_prod_4 = _mm256_srli_epi64(u64_prod_4, 32);
      const __m256i u32_hi_prod_5 = _mm256_srli_epi64(u64_prod_5, 32);
      const __m256i u32_hi_prod_6 = _mm256_srli_epi64(u64_prod_6, 32);
      const __m256i u32_hi_prod_7 = _mm256_srli_epi64(u64_prod_7, 32);
      const __m256i u32_hi_prod_8 = _mm256_srli_epi64(u64_prod_8, 32);
      const __m256i u32_hi_prod_perm_1 = _mm256_permute4x64_epi64(u32_hi_prod_1, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_2 = _mm256_permute4x64_epi64(u32_hi_prod_2, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_3 = _mm256_permute4x64_epi64(u32_hi_prod_3, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_4 = _mm256_permute4x64_epi64(u32_hi_prod_4, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_5 = _mm256_permute4x64_epi64(u32_hi_prod_5, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_6 = _mm256_permute4x64_epi64(u32_hi_prod_6, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_7 = _mm256_permute4x64_epi64(u32_hi_prod_7, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_8 = _mm256_permute4x64_epi64(u32_hi_prod_8, 0b10'01'00'11);
      const __m256i u32_hi_prod_slide_1 = _mm256_blend_epi32(u32_hi_prod_perm_1, zero, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_2 = _mm256_blend_epi32(u32_hi_prod_perm_2, u32_hi_prod_perm_1, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_3 = _mm256_blend_epi32(u32_hi_prod_perm_3, u32_hi_prod_perm_2, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_4 = _mm256_blend_epi32(u32_hi_prod_perm_4, u32_hi_prod_perm_3, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_5 = _mm256_blend_epi32(u32_hi_prod_perm_5, u32_hi_prod_perm_4, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_6 = _mm256_blend_epi32(u32_hi_prod_perm_6, u32_hi_prod_perm_5, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_7 = _mm256_blend_epi32(u32_hi_prod_perm_7, u32_hi_prod_perm_6, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_8 = _mm256_blend_epi32(u32_hi_prod_perm_8, u32_hi_prod_perm_7, 0b00'00'00'11);
      rrprime_1 = _mm256_add_epi64(u32_lo_prod_1, u32_hi_prod_slide_1);
      rrprime_2 = _mm256_add_epi64(u32_lo_prod_2, u32_hi_prod_slide_2);
      rrprime_3 = _mm256_add_epi64(u32_lo_prod_3, u32_hi_prod_slide_3);
      rrprime_4 = _mm256_add_epi64(u32_lo_prod_4, u32_hi_prod_slide_4);
      rrprime_5 = _mm256_add_epi64(u32_lo_prod_5, u32_hi_prod_slide_5);
      rrprime_6 = _mm256_add_epi64(u32_lo_prod_6, u32_hi_prod_slide_6);
      rrprime_7 = _mm256_add_epi64(u32_lo_prod_7, u32_hi_prod_slide_7);
      rrprime_8 = _mm256_add_epi64(u32_lo_prod_8, u32_hi_prod_slide_8);
      const __m256i rrprime_perm_1 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_1, 0b10'01'00'11), 32u);
      const __m256i rrprime_perm_2 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_2, 0b10'01'00'11), 32u);
      const __m256i rrprime_perm_3 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_3, 0b10'01'00'11), 32u);
      const __m256i rrprime_perm_4 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_4, 0b10'01'00'11), 32u);
      const __m256i rrprime_perm_5 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_5, 0b10'01'00'11), 32u);
      const __m256i rrprime_perm_6 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_6, 0b10'01'00'11), 32u);
      const __m256i rrprime_perm_7 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_7, 0b10'01'00'11), 32u);
      const __m256i rrprime_perm_8 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_8, 0b10'01'00'11), 32u);
      const __m256i rrprime_slide_1 = _mm256_blend_epi32(rrprime_perm_1, zero, 0b00'00'00'11);
      const __m256i rrprime_slide_2 = _mm256_blend_epi32(rrprime_perm_2, rrprime_perm_1, 0b00'00'00'11);
      const __m256i rrprime_slide_3 = _mm256_blend_epi32(rrprime_perm_3, rrprime_perm_2, 0b00'00'00'11);
      const __m256i rrprime_slide_4 = _mm256_blend_epi32(rrprime_perm_4, rrprime_perm_3, 0b00'00'00'11);
      const __m256i rrprime_slide_5 = _mm256_blend_epi32(rrprime_perm_5, rrprime_perm_4, 0b00'00'00'11);
      const __m256i rrprime_slide_6 = _mm256_blend_epi32(rrprime_perm_6, rrprime_perm_5, 0b00'00'00'11);
      const __m256i rrprime_slide_7 = _mm256_blend_epi32(rrprime_perm_7, rrprime_perm_6, 0b00'00'00'11);
      const __m256i rrprime_slide_8 = _mm256_blend_epi32(rrprime_perm_8, rrprime_perm_7, 0b00'00'00'11);
      rrprime_1 = _mm256_add_epi32(rrprime_1, rrprime_slide_1);
      rrprime_2 = _mm256_add_epi32(rrprime_2, rrprime_slide_2);
      rrprime_3 = _mm256_add_epi32(rrprime_3, rrprime_slide_3);
      rrprime_4 = _mm256_add_epi32(rrprime_4, rrprime_slide_4);
      rrprime_5 = _mm256_add_epi32(rrprime_5, rrprime_slide_5);
      rrprime_6 = _mm256_add_epi32(rrprime_6, rrprime_slide_6);
      rrprime_7 = _mm256_add_epi32(rrprime_7, rrprime_slide_7);
      rrprime_8 = _mm256_add_epi32(rrprime_8, rrprime_slide_8);
    }
    {
      const __m256i u64_prod_1 = _mm256_mul_epu32(rrprime_1, r1remmis);
      const __m256i u64_prod_2 = _mm256_mul_epu32(rrprime_2, r1remmis);
      const __m256i u64_prod_3 = _mm256_mul_epu32(rrprime_3, r1remmis);
      const __m256i u64_prod_4 = _mm256_mul_epu32(rrprime_4, r1remmis);
      const __m256i u64_prod_5 = _mm256_mul_epu32(rrprime_5, r1remmis);
      const __m256i u64_prod_6 = _mm256_mul_epu32(rrprime_6, r1remmis);
      const __m256i u64_prod_7 = _mm256_mul_epu32(rrprime_7, r1remmis);
      const __m256i u64_prod_8 = _mm256_mul_epu32(rrprime_8, r1remmis);
      const __m256i u32_lo_prod_1 = _mm256_blend_epi32(u64_prod_1, zero, 0b1010'1010);
      const __m256i u32_lo_prod_2 = _mm256_blend_epi32(u64_prod_2, zero, 0b1010'1010);
      const __m256i u32_lo_prod_3 = _mm256_blend_epi32(u64_prod_3, zero, 0b1010'1010);
      const __m256i u32_lo_prod_4 = _mm256_blend_epi32(u64_prod_4, zero, 0b1010'1010);
      const __m256i u32_lo_prod_5 = _mm256_blend_epi32(u64_prod_5, zero, 0b1010'1010);
      const __m256i u32_lo_prod_6 = _mm256_blend_epi32(u64_prod_6, zero, 0b1010'1010);
      const __m256i u32_lo_prod_7 = _mm256_blend_epi32(u64_prod_7, zero, 0b1010'1010);
      const __m256i u32_lo_prod_8 = _mm256_blend_epi32(u64_prod_8, zero, 0b1010'1010);
      const __m256i u32_hi_prod_1 = _mm256_srli_epi64(u64_prod_1, 32);
      const __m256i u32_hi_prod_2 = _mm256_srli_epi64(u64_prod_2, 32);
      const __m256i u32_hi_prod_3 = _mm256_srli_epi64(u64_prod_3, 32);
      const __m256i u32_hi_prod_4 = _mm256_srli_epi64(u64_prod_4, 32);
      const __m256i u32_hi_prod_5 = _mm256_srli_epi64(u64_prod_5, 32);
      const __m256i u32_hi_prod_6 = _mm256_srli_epi64(u64_prod_6, 32);
      const __m256i u32_hi_prod_7 = _mm256_srli_epi64(u64_prod_7, 32);
      const __m256i u32_hi_prod_8 = _mm256_srli_epi64(u64_prod_8, 32);
      const __m256i u32_hi_prod_perm_1 = _mm256_permute4x64_epi64(u32_hi_prod_1, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_2 = _mm256_permute4x64_epi64(u32_hi_prod_2, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_3 = _mm256_permute4x64_epi64(u32_hi_prod_3, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_4 = _mm256_permute4x64_epi64(u32_hi_prod_4, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_5 = _mm256_permute4x64_epi64(u32_hi_prod_5, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_6 = _mm256_permute4x64_epi64(u32_hi_prod_6, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_7 = _mm256_permute4x64_epi64(u32_hi_prod_7, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_8 = _mm256_permute4x64_epi64(u32_hi_prod_8, 0b10'01'00'11);
      const __m256i u32_hi_prod_slide_1 = _mm256_blend_epi32(u32_hi_prod_perm_1, zero, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_2 = _mm256_blend_epi32(u32_hi_prod_perm_2, u32_hi_prod_perm_1, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_3 = _mm256_blend_epi32(u32_hi_prod_perm_3, u32_hi_prod_perm_2, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_4 = _mm256_blend_epi32(u32_hi_prod_perm_4, u32_hi_prod_perm_3, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_5 = _mm256_blend_epi32(u32_hi_prod_perm_5, u32_hi_prod_perm_4, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_6 = _mm256_blend_epi32(u32_hi_prod_perm_6, u32_hi_prod_perm_5, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_7 = _mm256_blend_epi32(u32_hi_prod_perm_7, u32_hi_prod_perm_6, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_8 = _mm256_blend_epi32(u32_hi_prod_perm_8, u32_hi_prod_perm_7, 0b00'00'00'11);
      rrprime_1 = _mm256_add_epi64(u32_lo_prod_1, u32_hi_prod_slide_1);
      rrprime_2 = _mm256_add_epi64(u32_lo_prod_2, u32_hi_prod_slide_2);
      rrprime_3 = _mm256_add_epi64(u32_lo_prod_3, u32_hi_prod_slide_3);
      rrprime_4 = _mm256_add_epi64(u32_lo_prod_4, u32_hi_prod_slide_4);
      rrprime_5 = _mm256_add_epi64(u32_lo_prod_5, u32_hi_prod_slide_5);
      rrprime_6 = _mm256_add_epi64(u32_lo_prod_6, u32_hi_prod_slide_6);
      rrprime_7 = _mm256_add_epi64(u32_lo_prod_7, u32_hi_prod_slide_7);
      rrprime_8 = _mm256_add_epi64(u32_lo_prod_8, u32_hi_prod_slide_8);
      const __m256i rrprime_perm_1 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_1, 0b10'01'00'11), 32u);
      const __m256i rrprime_perm_2 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_2, 0b10'01'00'11), 32u);
      const __m256i rrprime_perm_3 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_3, 0b10'01'00'11), 32u);
      const __m256i rrprime_perm_4 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_4, 0b10'01'00'11), 32u);
      const __m256i rrprime_perm_5 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_5, 0b10'01'00'11), 32u);
      const __m256i rrprime_perm_6 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_6, 0b10'01'00'11), 32u);
      const __m256i rrprime_perm_7 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_7, 0b10'01'00'11), 32u);
      const __m256i rrprime_perm_8 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_8, 0b10'01'00'11), 32u);
      const __m256i rrprime_slide_1 = _mm256_blend_epi32(rrprime_perm_1, zero, 0b00'00'00'11);
      const __m256i rrprime_slide_2 = _mm256_blend_epi32(rrprime_perm_2, rrprime_perm_1, 0b00'00'00'11);
      const __m256i rrprime_slide_3 = _mm256_blend_epi32(rrprime_perm_3, rrprime_perm_2, 0b00'00'00'11);
      const __m256i rrprime_slide_4 = _mm256_blend_epi32(rrprime_perm_4, rrprime_perm_3, 0b00'00'00'11);
      const __m256i rrprime_slide_5 = _mm256_blend_epi32(rrprime_perm_5, rrprime_perm_4, 0b00'00'00'11);
      const __m256i rrprime_slide_6 = _mm256_blend_epi32(rrprime_perm_6, rrprime_perm_5, 0b00'00'00'11);
      const __m256i rrprime_slide_7 = _mm256_blend_epi32(rrprime_perm_7, rrprime_perm_6, 0b00'00'00'11);
      const __m256i rrprime_slide_8 = _mm256_blend_epi32(rrprime_perm_8, rrprime_perm_7, 0b00'00'00'11);
      rrprime_1 = _mm256_add_epi32(rrprime_1, rrprime_slide_1);
      rrprime_2 = _mm256_add_epi32(rrprime_2, rrprime_slide_2);
      rrprime_3 = _mm256_add_epi32(rrprime_3, rrprime_slide_3);
      rrprime_4 = _mm256_add_epi32(rrprime_4, rrprime_slide_4);
      rrprime_5 = _mm256_add_epi32(rrprime_5, rrprime_slide_5);
      rrprime_6 = _mm256_add_epi32(rrprime_6, rrprime_slide_6);
      rrprime_7 = _mm256_add_epi32(rrprime_7, rrprime_slide_7);
      rrprime_8 = _mm256_add_epi32(rrprime_8, rrprime_slide_8);
    }

    // total digits in 5^k
    const uint32_t total_digits = std::floor(k * std::log10(5)) + 1;
    constexpr unsigned num_words = 40;
    std::array<uint32_t, num_words> written = { 0 };
    std::array<uint32_t, num_words> r = { 0 };

    {
      const __m128i u32_pack_1 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_1, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));
      const __m128i u32_pack_2 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_2, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));
      const __m128i u32_pack_3 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_3, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));
      const __m128i u32_pack_4 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_4, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));
      const __m128i u32_pack_5 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_5, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));
      const __m128i u32_pack_6 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_6, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));
      const __m128i u32_pack_7 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_7, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));
      const __m128i u32_pack_8 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_8, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));

      _mm_storeu_si128(reinterpret_cast<__m128i *>(&written[0]), u32_pack_1);
      _mm_storeu_si128(reinterpret_cast<__m128i *>(&written[4]), u32_pack_2);
      _mm_storeu_si128(reinterpret_cast<__m128i *>(&written[8]), u32_pack_3);
      _mm_storeu_si128(reinterpret_cast<__m128i *>(&written[12]), u32_pack_4);
      _mm_storeu_si128(reinterpret_cast<__m128i *>(&written[16]), u32_pack_5);
      _mm_storeu_si128(reinterpret_cast<__m128i *>(&written[20]), u32_pack_6);
      _mm_storeu_si128(reinterpret_cast<__m128i *>(&written[24]), u32_pack_7);
      _mm_storeu_si128(reinterpret_cast<__m128i *>(&written[28]), u32_pack_8);
    }

    r[0] = 1; // initialize r = 1

    for(uint32_t i = 0; i < p; ++i)
    {
      uint32_t carry = 0;
      for(unsigned int &w : r)
      {
        uint64_t p = (uint64_t)w * 10ull + carry;
        w = (uint32_t)p;
        carry = (uint32_t)(p >> 32);
      }
    }

    size_t first = 0;

    while(first < r.size() && r[first] == 0)
    {
      ++first;
    }

    if(first != 0 && first != r.size())
    {
      const size_t count = r.size() - first;

      std::memmove(r.data(), r.data() + first, count * sizeof(uint32_t));

      std::memset(r.data() + count, 0, first * sizeof(uint32_t));
    }

    if(written != r)
    {
      std::cout << "k = " << k << "; p = " << p << '\n';
      goto backwards;
    }

    assert(written == r);

    unsigned lead_z = 24u - (k_div_32 - ((k * 1233u) >> 17u)) - (k < 32); //+ (k_div_32 & 0b0111);

    while(lead_z != 0)
    {
      if(lead_z >= 16)
      {
        rrprime_8 = rrprime_4;
        rrprime_7 = rrprime_3;
        rrprime_6 = rrprime_2;
        rrprime_5 = rrprime_1;
        rrprime_4 = zero;
        rrprime_3 = zero;
        rrprime_2 = zero;
        rrprime_1 = zero;
        lead_z -= 16u;
      }
      else if(lead_z >= 8)
      {
        rrprime_8 = rrprime_6;
        rrprime_7 = rrprime_5;
        rrprime_6 = rrprime_4;
        rrprime_5 = rrprime_3;
        rrprime_4 = rrprime_2;
        rrprime_3 = rrprime_1;
        rrprime_2 = zero;
        rrprime_1 = zero;

        lead_z -= 8u;
      }
      else if(lead_z >= 4)
      {
        rrprime_8 = rrprime_7;
        rrprime_7 = rrprime_6;
        rrprime_6 = rrprime_5;
        rrprime_5 = rrprime_4;
        rrprime_4 = rrprime_3;
        rrprime_3 = rrprime_2;
        rrprime_2 = rrprime_1;
        rrprime_1 = zero;

        lead_z -= 4u;
      }
      else if(lead_z >= 2)
      {
        const __m256i perm_8 = _mm256_permute4x64_epi64(rrprime_8, _mm_shuffle(1, 0, 3, 2));
        const __m256i perm_7 = _mm256_permute4x64_epi64(rrprime_7, _mm_shuffle(1, 0, 3, 2));
        const __m256i perm_6 = _mm256_permute4x64_epi64(rrprime_6, _mm_shuffle(1, 0, 3, 2));
        const __m256i perm_5 = _mm256_permute4x64_epi64(rrprime_5, _mm_shuffle(1, 0, 3, 2));
        const __m256i perm_4 = _mm256_permute4x64_epi64(rrprime_4, _mm_shuffle(1, 0, 3, 2));
        const __m256i perm_3 = _mm256_permute4x64_epi64(rrprime_3, _mm_shuffle(1, 0, 3, 2));
        const __m256i perm_2 = _mm256_permute4x64_epi64(rrprime_2, _mm_shuffle(1, 0, 3, 2));
        const __m256i perm_1 = _mm256_permute4x64_epi64(rrprime_1, _mm_shuffle(1, 0, 3, 2));

        rrprime_1 = _mm256_blend_epi32(perm_1, zero, 0b00001111);
        rrprime_2 = _mm256_blend_epi32(perm_2, perm_1, 0b00001111);
        rrprime_3 = _mm256_blend_epi32(perm_3, perm_2, 0b00001111);
        rrprime_4 = _mm256_blend_epi32(perm_4, perm_3, 0b00001111);
        rrprime_5 = _mm256_blend_epi32(perm_5, perm_4, 0b00001111);
        rrprime_6 = _mm256_blend_epi32(perm_6, perm_5, 0b00001111);
        rrprime_7 = _mm256_blend_epi32(perm_7, perm_6, 0b00001111);
        rrprime_8 = _mm256_blend_epi32(perm_8, perm_7, 0b00001111);

        lead_z -= 2u;
      }
      else
      {
        const __m256i u32_hi_prod_perm_8 = _mm256_permute4x64_epi64(rrprime_8, 0b10'01'00'11);
        const __m256i u32_hi_prod_perm_7 = _mm256_permute4x64_epi64(rrprime_7, 0b10'01'00'11);
        const __m256i u32_hi_prod_perm_6 = _mm256_permute4x64_epi64(rrprime_6, 0b10'01'00'11);
        const __m256i u32_hi_prod_perm_5 = _mm256_permute4x64_epi64(rrprime_5, 0b10'01'00'11);
        const __m256i u32_hi_prod_perm_4 = _mm256_permute4x64_epi64(rrprime_4, 0b10'01'00'11);
        const __m256i u32_hi_prod_perm_3 = _mm256_permute4x64_epi64(rrprime_3, 0b10'01'00'11);
        const __m256i u32_hi_prod_perm_2 = _mm256_permute4x64_epi64(rrprime_2, 0b10'01'00'11);
        const __m256i u32_hi_prod_perm_1 = _mm256_permute4x64_epi64(rrprime_1, 0b10'01'00'11);
        rrprime_1 = _mm256_blend_epi32(u32_hi_prod_perm_1, zero, 0b00'00'00'11);
        rrprime_2 = _mm256_blend_epi32(u32_hi_prod_perm_2, u32_hi_prod_perm_1, 0b00'00'00'11);
        rrprime_3 = _mm256_blend_epi32(u32_hi_prod_perm_3, u32_hi_prod_perm_2, 0b00'00'00'11);
        rrprime_4 = _mm256_blend_epi32(u32_hi_prod_perm_4, u32_hi_prod_perm_3, 0b00'00'00'11);
        rrprime_5 = _mm256_blend_epi32(u32_hi_prod_perm_5, u32_hi_prod_perm_4, 0b00'00'00'11);
        rrprime_6 = _mm256_blend_epi32(u32_hi_prod_perm_6, u32_hi_prod_perm_5, 0b00'00'00'11);
        rrprime_7 = _mm256_blend_epi32(u32_hi_prod_perm_7, u32_hi_prod_perm_6, 0b00'00'00'11);
        rrprime_8 = _mm256_blend_epi32(u32_hi_prod_perm_8, u32_hi_prod_perm_7, 0b00'00'00'11);
        lead_z -= 1u;
      }
    }

    unsigned digits_computed = 0;
    unsigned word_idx = k / 32u;
    uint64_t chunk = 0;
    unsigned next_8;
    unsigned simdy_8;

    const unsigned bit_shift = k & ((1u << 6u) - 1);
    const unsigned mod_mask = (bit_shift == 0) ? 0 : ((1u << bit_shift) - 1);

    while(digits_computed < total_digits)
    {
      // step a & b: multiply by 10^8 and cascade the carry
      uint32_t carry = 0; // carry max is 10^8, fits safely in uint32_t
      for(unsigned int &w : r)
      {
        // max size: (2^32 - 1) * 10^8 + 10^8 = 2^32 * 10^8. fits comfortably in uint64_t.
        uint64_t p = (uint64_t)w * 100'000'000u + carry;
        w = (uint32_t)p;
        carry = (uint32_t)(p >> 32);
      }

      {
        const __m256i u64_prod_1 = _mm256_mul_epu32(rrprime_1, r1e8);
        const __m256i u64_prod_2 = _mm256_mul_epu32(rrprime_2, r1e8);
        const __m256i u64_prod_3 = _mm256_mul_epu32(rrprime_3, r1e8);
        const __m256i u64_prod_4 = _mm256_mul_epu32(rrprime_4, r1e8);
        const __m256i u64_prod_5 = _mm256_mul_epu32(rrprime_5, r1e8);
        const __m256i u64_prod_6 = _mm256_mul_epu32(rrprime_6, r1e8);
        const __m256i u64_prod_7 = _mm256_mul_epu32(rrprime_7, r1e8);
        const __m256i u64_prod_8 = _mm256_mul_epu32(rrprime_8, r1e8);
        const __m256i u32_lo_prod_1 = _mm256_blend_epi32(u64_prod_1, zero, 0b1010'1010);
        const __m256i u32_lo_prod_2 = _mm256_blend_epi32(u64_prod_2, zero, 0b1010'1010);
        const __m256i u32_lo_prod_3 = _mm256_blend_epi32(u64_prod_3, zero, 0b1010'1010);
        const __m256i u32_lo_prod_4 = _mm256_blend_epi32(u64_prod_4, zero, 0b1010'1010);
        const __m256i u32_lo_prod_5 = _mm256_blend_epi32(u64_prod_5, zero, 0b1010'1010);
        const __m256i u32_lo_prod_6 = _mm256_blend_epi32(u64_prod_6, zero, 0b1010'1010);
        const __m256i u32_lo_prod_7 = _mm256_blend_epi32(u64_prod_7, zero, 0b1010'1010);
        const __m256i u32_lo_prod_8 = _mm256_blend_epi32(u64_prod_8, zero, 0b1010'1010);
        const __m256i u32_hi_prod_1 = _mm256_srli_epi64(u64_prod_1, 32);
        const __m256i u32_hi_prod_2 = _mm256_srli_epi64(u64_prod_2, 32);
        const __m256i u32_hi_prod_3 = _mm256_srli_epi64(u64_prod_3, 32);
        const __m256i u32_hi_prod_4 = _mm256_srli_epi64(u64_prod_4, 32);
        const __m256i u32_hi_prod_5 = _mm256_srli_epi64(u64_prod_5, 32);
        const __m256i u32_hi_prod_6 = _mm256_srli_epi64(u64_prod_6, 32);
        const __m256i u32_hi_prod_7 = _mm256_srli_epi64(u64_prod_7, 32);
        const __m256i u32_hi_prod_8 = _mm256_srli_epi64(u64_prod_8, 32);
        const __m256i u32_hi_prod_perm_1 = _mm256_permute4x64_epi64(u32_hi_prod_1, 0b10'01'00'11);
        const __m256i u32_hi_prod_perm_2 = _mm256_permute4x64_epi64(u32_hi_prod_2, 0b10'01'00'11);
        const __m256i u32_hi_prod_perm_3 = _mm256_permute4x64_epi64(u32_hi_prod_3, 0b10'01'00'11);
        const __m256i u32_hi_prod_perm_4 = _mm256_permute4x64_epi64(u32_hi_prod_4, 0b10'01'00'11);
        const __m256i u32_hi_prod_perm_5 = _mm256_permute4x64_epi64(u32_hi_prod_5, 0b10'01'00'11);
        const __m256i u32_hi_prod_perm_6 = _mm256_permute4x64_epi64(u32_hi_prod_6, 0b10'01'00'11);
        const __m256i u32_hi_prod_perm_7 = _mm256_permute4x64_epi64(u32_hi_prod_7, 0b10'01'00'11);
        const __m256i u32_hi_prod_perm_8 = _mm256_permute4x64_epi64(u32_hi_prod_8, 0b10'01'00'11);
        const __m256i u32_hi_prod_slide_1 = _mm256_blend_epi32(u32_hi_prod_perm_1, zero, 0b00'00'00'11);
        const __m256i u32_hi_prod_slide_2 = _mm256_blend_epi32(u32_hi_prod_perm_2, u32_hi_prod_perm_1, 0b00'00'00'11);
        const __m256i u32_hi_prod_slide_3 = _mm256_blend_epi32(u32_hi_prod_perm_3, u32_hi_prod_perm_2, 0b00'00'00'11);
        const __m256i u32_hi_prod_slide_4 = _mm256_blend_epi32(u32_hi_prod_perm_4, u32_hi_prod_perm_3, 0b00'00'00'11);
        const __m256i u32_hi_prod_slide_5 = _mm256_blend_epi32(u32_hi_prod_perm_5, u32_hi_prod_perm_4, 0b00'00'00'11);
        const __m256i u32_hi_prod_slide_6 = _mm256_blend_epi32(u32_hi_prod_perm_6, u32_hi_prod_perm_5, 0b00'00'00'11);
        const __m256i u32_hi_prod_slide_7 = _mm256_blend_epi32(u32_hi_prod_perm_7, u32_hi_prod_perm_6, 0b00'00'00'11);
        const __m256i u32_hi_prod_slide_8 = _mm256_blend_epi32(u32_hi_prod_perm_8, u32_hi_prod_perm_7, 0b00'00'00'11);
        rrprime_1 = _mm256_add_epi64(u32_lo_prod_1, u32_hi_prod_slide_1);
        rrprime_2 = _mm256_add_epi64(u32_lo_prod_2, u32_hi_prod_slide_2);
        rrprime_3 = _mm256_add_epi64(u32_lo_prod_3, u32_hi_prod_slide_3);
        rrprime_4 = _mm256_add_epi64(u32_lo_prod_4, u32_hi_prod_slide_4);
        rrprime_5 = _mm256_add_epi64(u32_lo_prod_5, u32_hi_prod_slide_5);
        rrprime_6 = _mm256_add_epi64(u32_lo_prod_6, u32_hi_prod_slide_6);
        rrprime_7 = _mm256_add_epi64(u32_lo_prod_7, u32_hi_prod_slide_7);
        rrprime_8 = _mm256_add_epi64(u32_lo_prod_8, u32_hi_prod_slide_8);
        const __m256i rrprime_perm_1 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_1, 0b10'01'00'11), 32u);
        const __m256i rrprime_perm_2 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_2, 0b10'01'00'11), 32u);
        const __m256i rrprime_perm_3 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_3, 0b10'01'00'11), 32u);
        const __m256i rrprime_perm_4 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_4, 0b10'01'00'11), 32u);
        const __m256i rrprime_perm_5 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_5, 0b10'01'00'11), 32u);
        const __m256i rrprime_perm_6 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_6, 0b10'01'00'11), 32u);
        const __m256i rrprime_perm_7 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_7, 0b10'01'00'11), 32u);
        const __m256i rrprime_perm_8 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_8, 0b10'01'00'11), 32u);
        const __m256i rrprime_slide_1 = _mm256_blend_epi32(rrprime_perm_1, zero, 0b00'00'00'11);
        const __m256i rrprime_slide_2 = _mm256_blend_epi32(rrprime_perm_2, rrprime_perm_1, 0b00'00'00'11);
        const __m256i rrprime_slide_3 = _mm256_blend_epi32(rrprime_perm_3, rrprime_perm_2, 0b00'00'00'11);
        const __m256i rrprime_slide_4 = _mm256_blend_epi32(rrprime_perm_4, rrprime_perm_3, 0b00'00'00'11);
        const __m256i rrprime_slide_5 = _mm256_blend_epi32(rrprime_perm_5, rrprime_perm_4, 0b00'00'00'11);
        const __m256i rrprime_slide_6 = _mm256_blend_epi32(rrprime_perm_6, rrprime_perm_5, 0b00'00'00'11);
        const __m256i rrprime_slide_7 = _mm256_blend_epi32(rrprime_perm_7, rrprime_perm_6, 0b00'00'00'11);
        const __m256i rrprime_slide_8 = _mm256_blend_epi32(rrprime_perm_8, rrprime_perm_7, 0b00'00'00'11);
        rrprime_1 = _mm256_add_epi32(rrprime_1, rrprime_slide_1);
        rrprime_2 = _mm256_add_epi32(rrprime_2, rrprime_slide_2);
        rrprime_3 = _mm256_add_epi32(rrprime_3, rrprime_slide_3);
        rrprime_4 = _mm256_add_epi32(rrprime_4, rrprime_slide_4);
        rrprime_5 = _mm256_add_epi32(rrprime_5, rrprime_slide_5);
        rrprime_6 = _mm256_add_epi32(rrprime_6, rrprime_slide_6);
        rrprime_7 = _mm256_add_epi32(rrprime_7, rrprime_slide_7);
        rrprime_8 = _mm256_add_epi32(rrprime_8, rrprime_slide_8);
      }

      const __m128i rrprime_7_128 = _mm256_castsi256_si128(rrprime_7);

      const __m128i next_o = _mm_shuffle_epi32(rrprime_7_128, 0b01'00'11'10);

      const __m128i lo_o = _mm_srli_epi64(rrprime_7_128, bit_shift);
      const __m128i hi_o = _mm_slli_epi64(next_o, 32u - bit_shift);
      const __m128i chunks_o = _mm_or_si128(lo_o, hi_o);

      // step c: extract next8 chunk (r >> k)
      chunk = 0;
      if(word_idx < num_words)
      {
        chunk = r[word_idx] >> bit_shift;
      }
      if(word_idx + 1 < num_words && bit_shift != 0)
      {
        // cast to uint64_t before shifting to prevent any 32-bit boundary overflow
        chunk |= ((uint64_t)r[word_idx + 1] << (32 - bit_shift));
      }
      next_8 = (uint32_t)chunk;

      // step d: apply modulo 2^k instantly
      if(word_idx < num_words)
      {
        r[word_idx] &= mod_mask;
        std::memset(static_cast<void *>(&r[word_idx + 1]), 0, (num_words - word_idx - 1) * 4);
      }

      const unsigned r_wrd_idx = _mm256_extract_epi32(rrprime_6, 6);
      const unsigned r_wrd_idx_plus_1 = _mm256_extract_epi32(rrprime_7, 0);

      const unsigned r_pt_1 = ((r_wrd_idx) >> bit_shift);
      const unsigned r_pt_2 = (r_wrd_idx_plus_1 << (32 - bit_shift));

      simdy_8 = r_pt_1 | r_pt_2;

      const unsigned r_wrd_idx_masked = r_wrd_idx & mod_mask;

      const __m256i rrprime_7_modd = _mm256_set1_epi64x(r_wrd_idx_masked);
      rrprime_6 = _mm256_blend_epi32(rrprime_6, rrprime_7_modd, 0b11'00'00'00);
      rrprime_7 = zero;

      if(simdy_8 != next_8)
      {
        std::cout << "k = " << k << "; p = " << p << '\n';
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
*/

/*
int main()
{
  constexpr uint32_t pow_5_correction[] = { 1, 10, 100, 1'000, 10'000, 100'000, 1'000'000, 10'000'000, 100'000'000 };
  constexpr uint8_t pow_5_idx[] = { 0, 24, 48, 72, 96, 120, 144, 168, 192, 216, 240 };
  constexpr uint8_t pow_5_e[] = { 0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40 };

  constexpr uint32_t pow_5_cache[] = {
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
    const unsigned p = std::floor(k * std::log10(2));

    const unsigned p_div_8 = p >> 3u;

    const unsigned p_div_32 = p >> 5u;

    const unsigned k_div_32 = static_cast<unsigned>(k) >> 5u;

    const __m256i zero = _mm256_setzero_si256();

    __m256i rrprime_1 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&pow_5_cache[pow_5_idx[p_div_32]])));
    __m256i rrprime_2 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&pow_5_cache[pow_5_idx[p_div_32] + 4u])));
    __m256i rrprime_3 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&pow_5_cache[pow_5_idx[p_div_32] + 8u])));
    __m256i rrprime_4 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&pow_5_cache[pow_5_idx[p_div_32] + 12u])));
    __m256i rrprime_5 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&pow_5_cache[pow_5_idx[p_div_32] + 16u])));
    __m256i rrprime_6 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&pow_5_cache[pow_5_idx[p_div_32] + 20u])));
    __m256i rrprime_7 = zero;
    __m256i rrprime_8 = zero;
    const __m256i r1e8 = _mm256_set1_epi64x(pow_5_correction[8]);
    const __m256i r1remmis = _mm256_set1_epi64x(pow_5_correction[p - (p_div_8 << 3u)]);

    const unsigned e_o = pow_5_e[p_div_32];

    for(unsigned e = e_o; e < p_div_8; e++)
    {
      const __m256i u64_prod_1 = _mm256_mul_epu32(rrprime_1, r1e8);
      const __m256i u64_prod_2 = _mm256_mul_epu32(rrprime_2, r1e8);
      const __m256i u64_prod_3 = _mm256_mul_epu32(rrprime_3, r1e8);
      const __m256i u64_prod_4 = _mm256_mul_epu32(rrprime_4, r1e8);
      const __m256i u64_prod_5 = _mm256_mul_epu32(rrprime_5, r1e8);
      const __m256i u64_prod_6 = _mm256_mul_epu32(rrprime_6, r1e8);
      const __m256i u64_prod_7 = _mm256_mul_epu32(rrprime_7, r1e8);
      const __m256i u64_prod_8 = _mm256_mul_epu32(rrprime_8, r1e8);
      const __m256i u32_lo_prod_1 = _mm256_blend_epi32(u64_prod_1, zero, 0b1010'1010);
      const __m256i u32_lo_prod_2 = _mm256_blend_epi32(u64_prod_2, zero, 0b1010'1010);
      const __m256i u32_lo_prod_3 = _mm256_blend_epi32(u64_prod_3, zero, 0b1010'1010);
      const __m256i u32_lo_prod_4 = _mm256_blend_epi32(u64_prod_4, zero, 0b1010'1010);
      const __m256i u32_lo_prod_5 = _mm256_blend_epi32(u64_prod_5, zero, 0b1010'1010);
      const __m256i u32_lo_prod_6 = _mm256_blend_epi32(u64_prod_6, zero, 0b1010'1010);
      const __m256i u32_lo_prod_7 = _mm256_blend_epi32(u64_prod_7, zero, 0b1010'1010);
      const __m256i u32_lo_prod_8 = _mm256_blend_epi32(u64_prod_8, zero, 0b1010'1010);
      const __m256i u32_hi_prod_1 = _mm256_srli_epi64(u64_prod_1, 32);
      const __m256i u32_hi_prod_2 = _mm256_srli_epi64(u64_prod_2, 32);
      const __m256i u32_hi_prod_3 = _mm256_srli_epi64(u64_prod_3, 32);
      const __m256i u32_hi_prod_4 = _mm256_srli_epi64(u64_prod_4, 32);
      const __m256i u32_hi_prod_5 = _mm256_srli_epi64(u64_prod_5, 32);
      const __m256i u32_hi_prod_6 = _mm256_srli_epi64(u64_prod_6, 32);
      const __m256i u32_hi_prod_7 = _mm256_srli_epi64(u64_prod_7, 32);
      const __m256i u32_hi_prod_8 = _mm256_srli_epi64(u64_prod_8, 32);
      const __m256i u32_hi_prod_perm_1 = _mm256_permute4x64_epi64(u32_hi_prod_1, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_2 = _mm256_permute4x64_epi64(u32_hi_prod_2, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_3 = _mm256_permute4x64_epi64(u32_hi_prod_3, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_4 = _mm256_permute4x64_epi64(u32_hi_prod_4, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_5 = _mm256_permute4x64_epi64(u32_hi_prod_5, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_6 = _mm256_permute4x64_epi64(u32_hi_prod_6, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_7 = _mm256_permute4x64_epi64(u32_hi_prod_7, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_8 = _mm256_permute4x64_epi64(u32_hi_prod_8, 0b10'01'00'11);
      const __m256i u32_hi_prod_slide_1 = _mm256_blend_epi32(u32_hi_prod_perm_1, zero, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_2 = _mm256_blend_epi32(u32_hi_prod_perm_2, u32_hi_prod_perm_1, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_3 = _mm256_blend_epi32(u32_hi_prod_perm_3, u32_hi_prod_perm_2, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_4 = _mm256_blend_epi32(u32_hi_prod_perm_4, u32_hi_prod_perm_3, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_5 = _mm256_blend_epi32(u32_hi_prod_perm_5, u32_hi_prod_perm_4, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_6 = _mm256_blend_epi32(u32_hi_prod_perm_6, u32_hi_prod_perm_5, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_7 = _mm256_blend_epi32(u32_hi_prod_perm_7, u32_hi_prod_perm_6, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_8 = _mm256_blend_epi32(u32_hi_prod_perm_8, u32_hi_prod_perm_7, 0b00'00'00'11);
      rrprime_1 = _mm256_add_epi64(u32_lo_prod_1, u32_hi_prod_slide_1);
      rrprime_2 = _mm256_add_epi64(u32_lo_prod_2, u32_hi_prod_slide_2);
      rrprime_3 = _mm256_add_epi64(u32_lo_prod_3, u32_hi_prod_slide_3);
      rrprime_4 = _mm256_add_epi64(u32_lo_prod_4, u32_hi_prod_slide_4);
      rrprime_5 = _mm256_add_epi64(u32_lo_prod_5, u32_hi_prod_slide_5);
      rrprime_6 = _mm256_add_epi64(u32_lo_prod_6, u32_hi_prod_slide_6);
      rrprime_7 = _mm256_add_epi64(u32_lo_prod_7, u32_hi_prod_slide_7);
      rrprime_8 = _mm256_add_epi64(u32_lo_prod_8, u32_hi_prod_slide_8);
      const __m256i rrprime_perm_1 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_1, 0b10'01'00'11), 32u);
      const __m256i rrprime_perm_2 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_2, 0b10'01'00'11), 32u);
      const __m256i rrprime_perm_3 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_3, 0b10'01'00'11), 32u);
      const __m256i rrprime_perm_4 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_4, 0b10'01'00'11), 32u);
      const __m256i rrprime_perm_5 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_5, 0b10'01'00'11), 32u);
      const __m256i rrprime_perm_6 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_6, 0b10'01'00'11), 32u);
      const __m256i rrprime_perm_7 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_7, 0b10'01'00'11), 32u);
      const __m256i rrprime_perm_8 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_8, 0b10'01'00'11), 32u);
      const __m256i rrprime_slide_1 = _mm256_blend_epi32(rrprime_perm_1, zero, 0b00'00'00'11);
      const __m256i rrprime_slide_2 = _mm256_blend_epi32(rrprime_perm_2, rrprime_perm_1, 0b00'00'00'11);
      const __m256i rrprime_slide_3 = _mm256_blend_epi32(rrprime_perm_3, rrprime_perm_2, 0b00'00'00'11);
      const __m256i rrprime_slide_4 = _mm256_blend_epi32(rrprime_perm_4, rrprime_perm_3, 0b00'00'00'11);
      const __m256i rrprime_slide_5 = _mm256_blend_epi32(rrprime_perm_5, rrprime_perm_4, 0b00'00'00'11);
      const __m256i rrprime_slide_6 = _mm256_blend_epi32(rrprime_perm_6, rrprime_perm_5, 0b00'00'00'11);
      const __m256i rrprime_slide_7 = _mm256_blend_epi32(rrprime_perm_7, rrprime_perm_6, 0b00'00'00'11);
      const __m256i rrprime_slide_8 = _mm256_blend_epi32(rrprime_perm_8, rrprime_perm_7, 0b00'00'00'11);
      rrprime_1 = _mm256_add_epi32(rrprime_1, rrprime_slide_1);
      rrprime_2 = _mm256_add_epi32(rrprime_2, rrprime_slide_2);
      rrprime_3 = _mm256_add_epi32(rrprime_3, rrprime_slide_3);
      rrprime_4 = _mm256_add_epi32(rrprime_4, rrprime_slide_4);
      rrprime_5 = _mm256_add_epi32(rrprime_5, rrprime_slide_5);
      rrprime_6 = _mm256_add_epi32(rrprime_6, rrprime_slide_6);
      rrprime_7 = _mm256_add_epi32(rrprime_7, rrprime_slide_7);
      rrprime_8 = _mm256_add_epi32(rrprime_8, rrprime_slide_8);
    }
    {
      const __m256i u64_prod_1 = _mm256_mul_epu32(rrprime_1, r1remmis);
      const __m256i u64_prod_2 = _mm256_mul_epu32(rrprime_2, r1remmis);
      const __m256i u64_prod_3 = _mm256_mul_epu32(rrprime_3, r1remmis);
      const __m256i u64_prod_4 = _mm256_mul_epu32(rrprime_4, r1remmis);
      const __m256i u64_prod_5 = _mm256_mul_epu32(rrprime_5, r1remmis);
      const __m256i u64_prod_6 = _mm256_mul_epu32(rrprime_6, r1remmis);
      const __m256i u64_prod_7 = _mm256_mul_epu32(rrprime_7, r1remmis);
      const __m256i u64_prod_8 = _mm256_mul_epu32(rrprime_8, r1remmis);
      const __m256i u32_lo_prod_1 = _mm256_blend_epi32(u64_prod_1, zero, 0b1010'1010);
      const __m256i u32_lo_prod_2 = _mm256_blend_epi32(u64_prod_2, zero, 0b1010'1010);
      const __m256i u32_lo_prod_3 = _mm256_blend_epi32(u64_prod_3, zero, 0b1010'1010);
      const __m256i u32_lo_prod_4 = _mm256_blend_epi32(u64_prod_4, zero, 0b1010'1010);
      const __m256i u32_lo_prod_5 = _mm256_blend_epi32(u64_prod_5, zero, 0b1010'1010);
      const __m256i u32_lo_prod_6 = _mm256_blend_epi32(u64_prod_6, zero, 0b1010'1010);
      const __m256i u32_lo_prod_7 = _mm256_blend_epi32(u64_prod_7, zero, 0b1010'1010);
      const __m256i u32_lo_prod_8 = _mm256_blend_epi32(u64_prod_8, zero, 0b1010'1010);
      const __m256i u32_hi_prod_1 = _mm256_srli_epi64(u64_prod_1, 32);
      const __m256i u32_hi_prod_2 = _mm256_srli_epi64(u64_prod_2, 32);
      const __m256i u32_hi_prod_3 = _mm256_srli_epi64(u64_prod_3, 32);
      const __m256i u32_hi_prod_4 = _mm256_srli_epi64(u64_prod_4, 32);
      const __m256i u32_hi_prod_5 = _mm256_srli_epi64(u64_prod_5, 32);
      const __m256i u32_hi_prod_6 = _mm256_srli_epi64(u64_prod_6, 32);
      const __m256i u32_hi_prod_7 = _mm256_srli_epi64(u64_prod_7, 32);
      const __m256i u32_hi_prod_8 = _mm256_srli_epi64(u64_prod_8, 32);
      const __m256i u32_hi_prod_perm_1 = _mm256_permute4x64_epi64(u32_hi_prod_1, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_2 = _mm256_permute4x64_epi64(u32_hi_prod_2, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_3 = _mm256_permute4x64_epi64(u32_hi_prod_3, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_4 = _mm256_permute4x64_epi64(u32_hi_prod_4, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_5 = _mm256_permute4x64_epi64(u32_hi_prod_5, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_6 = _mm256_permute4x64_epi64(u32_hi_prod_6, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_7 = _mm256_permute4x64_epi64(u32_hi_prod_7, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_8 = _mm256_permute4x64_epi64(u32_hi_prod_8, 0b10'01'00'11);
      const __m256i u32_hi_prod_slide_1 = _mm256_blend_epi32(u32_hi_prod_perm_1, zero, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_2 = _mm256_blend_epi32(u32_hi_prod_perm_2, u32_hi_prod_perm_1, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_3 = _mm256_blend_epi32(u32_hi_prod_perm_3, u32_hi_prod_perm_2, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_4 = _mm256_blend_epi32(u32_hi_prod_perm_4, u32_hi_prod_perm_3, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_5 = _mm256_blend_epi32(u32_hi_prod_perm_5, u32_hi_prod_perm_4, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_6 = _mm256_blend_epi32(u32_hi_prod_perm_6, u32_hi_prod_perm_5, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_7 = _mm256_blend_epi32(u32_hi_prod_perm_7, u32_hi_prod_perm_6, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_8 = _mm256_blend_epi32(u32_hi_prod_perm_8, u32_hi_prod_perm_7, 0b00'00'00'11);
      rrprime_1 = _mm256_add_epi64(u32_lo_prod_1, u32_hi_prod_slide_1);
      rrprime_2 = _mm256_add_epi64(u32_lo_prod_2, u32_hi_prod_slide_2);
      rrprime_3 = _mm256_add_epi64(u32_lo_prod_3, u32_hi_prod_slide_3);
      rrprime_4 = _mm256_add_epi64(u32_lo_prod_4, u32_hi_prod_slide_4);
      rrprime_5 = _mm256_add_epi64(u32_lo_prod_5, u32_hi_prod_slide_5);
      rrprime_6 = _mm256_add_epi64(u32_lo_prod_6, u32_hi_prod_slide_6);
      rrprime_7 = _mm256_add_epi64(u32_lo_prod_7, u32_hi_prod_slide_7);
      rrprime_8 = _mm256_add_epi64(u32_lo_prod_8, u32_hi_prod_slide_8);
      const __m256i rrprime_perm_1 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_1, 0b10'01'00'11), 32u);
      const __m256i rrprime_perm_2 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_2, 0b10'01'00'11), 32u);
      const __m256i rrprime_perm_3 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_3, 0b10'01'00'11), 32u);
      const __m256i rrprime_perm_4 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_4, 0b10'01'00'11), 32u);
      const __m256i rrprime_perm_5 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_5, 0b10'01'00'11), 32u);
      const __m256i rrprime_perm_6 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_6, 0b10'01'00'11), 32u);
      const __m256i rrprime_perm_7 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_7, 0b10'01'00'11), 32u);
      const __m256i rrprime_perm_8 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_8, 0b10'01'00'11), 32u);
      const __m256i rrprime_slide_1 = _mm256_blend_epi32(rrprime_perm_1, zero, 0b00'00'00'11);
      const __m256i rrprime_slide_2 = _mm256_blend_epi32(rrprime_perm_2, rrprime_perm_1, 0b00'00'00'11);
      const __m256i rrprime_slide_3 = _mm256_blend_epi32(rrprime_perm_3, rrprime_perm_2, 0b00'00'00'11);
      const __m256i rrprime_slide_4 = _mm256_blend_epi32(rrprime_perm_4, rrprime_perm_3, 0b00'00'00'11);
      const __m256i rrprime_slide_5 = _mm256_blend_epi32(rrprime_perm_5, rrprime_perm_4, 0b00'00'00'11);
      const __m256i rrprime_slide_6 = _mm256_blend_epi32(rrprime_perm_6, rrprime_perm_5, 0b00'00'00'11);
      const __m256i rrprime_slide_7 = _mm256_blend_epi32(rrprime_perm_7, rrprime_perm_6, 0b00'00'00'11);
      const __m256i rrprime_slide_8 = _mm256_blend_epi32(rrprime_perm_8, rrprime_perm_7, 0b00'00'00'11);
      rrprime_1 = _mm256_add_epi32(rrprime_1, rrprime_slide_1);
      rrprime_2 = _mm256_add_epi32(rrprime_2, rrprime_slide_2);
      rrprime_3 = _mm256_add_epi32(rrprime_3, rrprime_slide_3);
      rrprime_4 = _mm256_add_epi32(rrprime_4, rrprime_slide_4);
      rrprime_5 = _mm256_add_epi32(rrprime_5, rrprime_slide_5);
      rrprime_6 = _mm256_add_epi32(rrprime_6, rrprime_slide_6);
      rrprime_7 = _mm256_add_epi32(rrprime_7, rrprime_slide_7);
      rrprime_8 = _mm256_add_epi32(rrprime_8, rrprime_slide_8);
    }

    // total digits in 5^k
    const uint32_t total_digits = std::floor(k * std::log10(5)) + 1;
    constexpr unsigned num_words = 40;
    std::array<uint32_t, num_words> written = { 0 };
    std::array<uint32_t, num_words> r = { 0 };

    const __m128i u32_pack_1 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_1, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));
    const __m128i u32_pack_2 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_2, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));
    const __m128i u32_pack_3 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_3, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));
    const __m128i u32_pack_4 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_4, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));
    const __m128i u32_pack_5 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_5, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));
    const __m128i u32_pack_6 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_6, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));
    const __m128i u32_pack_7 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_7, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));
    const __m128i u32_pack_8 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_8, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));

    _mm_storeu_si128(reinterpret_cast<__m128i *>(&written[0]), u32_pack_1);
    _mm_storeu_si128(reinterpret_cast<__m128i *>(&written[4]), u32_pack_2);
    _mm_storeu_si128(reinterpret_cast<__m128i *>(&written[8]), u32_pack_3);
    _mm_storeu_si128(reinterpret_cast<__m128i *>(&written[12]), u32_pack_4);
    _mm_storeu_si128(reinterpret_cast<__m128i *>(&written[16]), u32_pack_5);
    _mm_storeu_si128(reinterpret_cast<__m128i *>(&written[20]), u32_pack_6);
    _mm_storeu_si128(reinterpret_cast<__m128i *>(&written[24]), u32_pack_7);
    _mm_storeu_si128(reinterpret_cast<__m128i *>(&written[28]), u32_pack_8);

    r[0] = 1; // initialize r = 1

    for(uint32_t i = 0; i < p; ++i)
    {
      uint32_t carry = 0;
      for(unsigned int &w : r)
      {
        uint64_t p = (uint64_t)w * 10ull + carry;
        w = (uint32_t)p;
        carry = (uint32_t)(p >> 32);
      }
    }

    size_t first = 0;

    while(first < r.size() && r[first] == 0)
    {
      ++first;
    }

    if(first != 0 && first != r.size())
    {
      const size_t count = r.size() - first;

      std::memmove(r.data(), r.data() + first, count * sizeof(uint32_t));

      std::memset(r.data() + count, 0, first * sizeof(uint32_t));
    }

    if(written != r)
    {
      std::cout << "k = " << k << "; p = " << p << '\n';
      goto backwards;
    }

    assert(written == r);

    // std::cout << "\n";
  }

  return 0;
}
*/

/*
int main()
{
  constexpr uint32_t pow_5_correction[] = { 1, 10, 100, 1'000, 10'000, 100'000, 1'000'000, 10'000'000, 100'000'000 };
  constexpr uint8_t pow_5_idx[] = { 0, 24, 48, 72, 96, 120, 144, 168, 192, 216, 240 };
  constexpr uint8_t pow_5_e[] = { 0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40 };

  constexpr uint32_t pow_5_cache[] = {
=======
>>>>>>> parent of 737812c (wow AVX2; 5^k full precision algo is donegit add . passes all tests; now its improvement timeeeegit add .!)
    1,          0,          0,          0,          0,          0,          0,          0, // 0 - _mm256i loaded and 0 extended to a _mm512i
    0,          0,          0,          0,          0,          0,          0,          0, // 1 - again
    0,          0,          0,          0,          0,          0,          0,          0, // 2 - again

    2242703233, 762134875,  1262,       0,          0,          0,          0,          0, // 0 - _mm256i loaded and 0 extended to a _mm512i
    0,          0,          0,          0,          0,          0,          0,          0, // 1 - again
    0,          0,          0,          0,          0,          0,          0,          0, // 2 - again

    3211403009, 1849224548, 3668416493, 3913284084, 1593091,    0,          0,          0, // 0 - _mm256i loaded and 0 extended to a _mm512i
    0,          0,          0,          0,          0,          0,          0,          0, // 1 - again
    0,          0,          0,          0,          0,          0,          0,          0, // 2 - again

    3776417409, 3833115195, 474402842,  2046101519, 1659368615, 1657637457, 2010764683, 0, // 0 - _mm256i loaded and 0 extended to a _mm512i
    0,          0,          0,          0,          0,          0,          0,          0, // 1 - again
    0,          0,          0,          0,          0,          0,          0,          0, // 2 - again

    781532673,  64985353,   253049085,  594863151,  3553621484, 3288652808, 3167596762, 2788392729, // 0 - _mm256i loaded and 0 extended to a _mm512i
    3911132675, 590,        0,          0,          0,          0,          0,          0,          // 1 - again -
    0,          0,          0,          0,          0,          0,          0,          0,          // 2 - again

    4223872385, 1378013428, 3071223888, 1460925171, 3695318035, 3594083709, 4166588825, 9582620, // 0 - _mm256i loaded and 0 extended to a _mm512i
    2259883222, 2057664441, 314048097,  745834,     0,          0,          0,          0,       // 1 - again -
    0,          0,          0,          0,          0,          0,          0,          0,       // 2 - again

    2894159105, 2850127391, 1678958885, 1990048638, 4055408710, 4272372864, 3869429580, 1940770817, // 0 - _mm256i loaded and 0 extended to a _mm512i
    3594030316, 3284427401, 4246652165, 2708404372, 1313461594, 941374947,  0,          0,          // 1 - again -
    0,          0,          0,          0,          0,          0,          0,          0,          // 2 - again

    3031420033, 1112378521, 855339782,  4191278750, 248038208,  1136169807, 2230652081, 3378214371, // 0 - _mm256i loaded and 0 extended to a _mm512i
    1653863564, 613940998,  1392057263, 562793371,  894754680,  2060902835, 2039635717, 2771255238, // 1 - again -
    276,        0,          0,          0,          0,          0,          0,          0,          // 2 - again

    2553183233, 3201533787, 3638140786, 303378311,  1809731782, 3477761648, 3583367183, 649228654,  // 0 - _mm256i loaded and 0 extended to a _mm512i
    2915460784, 487929380,  1011012442, 1677677582, 3428152256, 1710878487, 1438394610, 2161952759, // 1 - again -
    4100910556, 1608314830, 349175,     0,          0,          0,          0,          0,          // 2 - again

    3940379521, 184513341,  2872588323, 2214530454, 38258512,   2980860351, 114267010,  2188874685, // 0 - _mm256i loaded and 0 extended to a _mm512i
    234079247,  2101059099, 1948702207, 947446250,  864457656,  507589568,  1321007357, 3911984176, // 1 - again -
    1011110295, 2382358050, 2389730781, 730678769,  440721283,  0,          0,          0,          // 2 - again

    1352440577, 3353128433, 943566124,  1942375735, 3933038544, 1369155590, 3288593144, 2689786274, // 0 - _mm256i loaded and 0 extended to a _mm512i
    3608159034, 1149596585, 3474119098, 1887901699, 1390164635, 3853669096, 119012174,  3192759855, // 1 - again -
    179034081,  2355293519, 3468291195, 3563470881, 2702541568, 3437458918, 2217683442, 129,        // 2 - again
  };

  for(int k = 0; k < 1075; k++)
  {
    // 36 words * 32 bits = 1152 bits. perfectly fits k=1100 + 10^8 overflow.
    const unsigned p = std::floor(k * std::log10(2));

    const unsigned p_div_8 = p >> 3u;

    const unsigned p_div_32 = p >> 5u;

    const unsigned k_div_32 = static_cast<unsigned>(k) >> 5u;

    const __m512i zero = _mm512_setzero_si512();

    __m512i rrprime_1 = _mm512_cvtepu32_epi64(_mm256_loadu_epi32(&pow_5_cache[pow_5_idx[p_div_32]]));
    __m512i rrprime_2 = _mm512_cvtepu32_epi64(_mm256_loadu_epi32(&pow_5_cache[pow_5_idx[p_div_32] + 8u]));
    __m512i rrprime_3 = _mm512_cvtepu32_epi64(_mm256_loadu_epi32(&pow_5_cache[pow_5_idx[p_div_32] + 16u]));
    __m512i rrprime_4 = zero;
    __m512i r1e8 = _mm512_set1_epi64(pow_5_correction[8]);
    __m512i r1emis = _mm512_set1_epi64(pow_5_correction[p - (p_div_8 << 3u)]);

    const unsigned e_o = pow_5_e[p_div_32];

    for(unsigned e = e_o; e < p_div_8; e++)
    {
      const __m512i u64_prod_1 = _mm512_mul_epu32(rrprime_1, r1e8);
      const __m512i u64_prod_2 = _mm512_mul_epu32(rrprime_2, r1e8);
      const __m512i u64_prod_3 = _mm512_mul_epu32(rrprime_3, r1e8);
      const __m512i u64_prod_4 = _mm512_mul_epu32(rrprime_4, r1e8);
      const __m512i u32_lo_prod_1 = _mm512_mask_blend_epi32(0b1010'1010'1010'1010, u64_prod_1, zero);
      const __m512i u32_lo_prod_2 = _mm512_mask_blend_epi32(0b1010'1010'1010'1010, u64_prod_2, zero);
      const __m512i u32_lo_prod_3 = _mm512_mask_blend_epi32(0b1010'1010'1010'1010, u64_prod_3, zero);
      const __m512i u32_lo_prod_4 = _mm512_mask_blend_epi32(0b1010'1010'1010'1010, u64_prod_4, zero);
      const __m512i u32_hi_prod_1 = _mm512_srli_epi64(u64_prod_1, 32);
      const __m512i u32_hi_prod_2 = _mm512_srli_epi64(u64_prod_2, 32);
      const __m512i u32_hi_prod_3 = _mm512_srli_epi64(u64_prod_3, 32);
      const __m512i u32_hi_prod_4 = _mm512_srli_epi64(u64_prod_4, 32);
      const __m512i u32_hi_prod_slide_1 = _mm512_alignr_epi64(u32_hi_prod_1, zero, 7);
      const __m512i u32_hi_prod_slide_2 = _mm512_alignr_epi64(u32_hi_prod_2, u32_hi_prod_1, 7);
      const __m512i u32_hi_prod_slide_3 = _mm512_alignr_epi64(u32_hi_prod_3, u32_hi_prod_2, 7);
      const __m512i u32_hi_prod_slide_4 = _mm512_alignr_epi64(u32_hi_prod_4, u32_hi_prod_3, 7);
      rrprime_1 = _mm512_add_epi64(u32_lo_prod_1, u32_hi_prod_slide_1);
      rrprime_2 = _mm512_add_epi64(u32_lo_prod_2, u32_hi_prod_slide_2);
      rrprime_3 = _mm512_add_epi64(u32_lo_prod_3, u32_hi_prod_slide_3);
      rrprime_4 = _mm512_add_epi64(u32_lo_prod_4, u32_hi_prod_slide_4);
      const __m512i rrprime_slide_1 = _mm512_srli_epi64(_mm512_alignr_epi64(rrprime_1, zero, 7), 32);
      const __m512i rrprime_slide_2 = _mm512_srli_epi64(_mm512_alignr_epi64(rrprime_2, rrprime_1, 7), 32);
      const __m512i rrprime_slide_3 = _mm512_srli_epi64(_mm512_alignr_epi64(rrprime_3, rrprime_2, 7), 32);
      const __m512i rrprime_slide_4 = _mm512_srli_epi64(_mm512_alignr_epi64(rrprime_4, rrprime_3, 7), 32);
      rrprime_1 = _mm512_add_epi32(rrprime_1, rrprime_slide_1);
      rrprime_2 = _mm512_add_epi32(rrprime_2, rrprime_slide_2);
      rrprime_3 = _mm512_add_epi32(rrprime_3, rrprime_slide_3);
      rrprime_4 = _mm512_add_epi32(rrprime_4, rrprime_slide_4);
    }
    {

      const __m512i u64_prod_1 = _mm512_mul_epu32(rrprime_1, r1emis);
      const __m512i u64_prod_2 = _mm512_mul_epu32(rrprime_2, r1emis);
      const __m512i u64_prod_3 = _mm512_mul_epu32(rrprime_3, r1emis);
      const __m512i u64_prod_4 = _mm512_mul_epu32(rrprime_4, r1emis);
      const __m512i u32_lo_prod_1 = _mm512_mask_blend_epi32(0b1010'1010'1010'1010, u64_prod_1, zero);
      const __m512i u32_lo_prod_2 = _mm512_mask_blend_epi32(0b1010'1010'1010'1010, u64_prod_2, zero);
      const __m512i u32_lo_prod_3 = _mm512_mask_blend_epi32(0b1010'1010'1010'1010, u64_prod_3, zero);
      const __m512i u32_lo_prod_4 = _mm512_mask_blend_epi32(0b1010'1010'1010'1010, u64_prod_4, zero);
      const __m512i u32_hi_prod_1 = _mm512_srli_epi64(u64_prod_1, 32);
      const __m512i u32_hi_prod_2 = _mm512_srli_epi64(u64_prod_2, 32);
      const __m512i u32_hi_prod_3 = _mm512_srli_epi64(u64_prod_3, 32);
      const __m512i u32_hi_prod_4 = _mm512_srli_epi64(u64_prod_4, 32);
      const __m512i u32_hi_prod_slide_1 = _mm512_alignr_epi64(u32_hi_prod_1, zero, 7);
      const __m512i u32_hi_prod_slide_2 = _mm512_alignr_epi64(u32_hi_prod_2, u32_hi_prod_1, 7);
      const __m512i u32_hi_prod_slide_3 = _mm512_alignr_epi64(u32_hi_prod_3, u32_hi_prod_2, 7);
      const __m512i u32_hi_prod_slide_4 = _mm512_alignr_epi64(u32_hi_prod_4, u32_hi_prod_3, 7);
      rrprime_1 = _mm512_add_epi32(u32_lo_prod_1, u32_hi_prod_slide_1);
      rrprime_2 = _mm512_add_epi32(u32_lo_prod_2, u32_hi_prod_slide_2);
      rrprime_3 = _mm512_add_epi32(u32_lo_prod_3, u32_hi_prod_slide_3);
      rrprime_4 = _mm512_add_epi32(u32_lo_prod_4, u32_hi_prod_slide_4);
      const __m512i rrprime_slide_1 = _mm512_srli_epi64(_mm512_alignr_epi64(rrprime_1, zero, 7), 32);
      const __m512i rrprime_slide_2 = _mm512_srli_epi64(_mm512_alignr_epi64(rrprime_2, rrprime_1, 7), 32);
      const __m512i rrprime_slide_3 = _mm512_srli_epi64(_mm512_alignr_epi64(rrprime_3, rrprime_2, 7), 32);
      const __m512i rrprime_slide_4 = _mm512_srli_epi64(_mm512_alignr_epi64(rrprime_4, rrprime_3, 7), 32);
      rrprime_1 = _mm512_add_epi32(rrprime_1, rrprime_slide_1);
      rrprime_2 = _mm512_add_epi32(rrprime_2, rrprime_slide_2);
      rrprime_3 = _mm512_add_epi32(rrprime_3, rrprime_slide_3);
      rrprime_4 = _mm512_add_epi32(rrprime_4, rrprime_slide_4);
    }

    // total digits in 5^k
    const uint32_t total_digits = std::floor(k * std::log10(5)) + 1;
    constexpr unsigned num_words = 40;
    unsigned digits_computed = 0;

    std::cout << "k = " << k << std::endl;
    uint32_t r[num_words] = { 0 };
    r[0] = 1; // initialize r = 1

    for(uint32_t i = 0; i < p; ++i)
    {
      uint32_t carry = 0;
      for(unsigned int &w : r)
      {
        uint64_t p = (uint64_t)w * 10ull + carry;
        w = (uint32_t)p;
        carry = (uint32_t)(p >> 32);
      }
    }

    const unsigned bit_shift = k & ((1u << 6u) - 1);
    const unsigned mod_mask = (bit_shift == 0) ? 0 : ((1u << bit_shift) - 1);

    const __m512i mod_vec = _mm512_set1_epi64(mod_mask);

    const unsigned rrprime_1_zero_mask = _mm512_cmpneq_epi64_mask(rrprime_1, zero);
    const unsigned rrprime_2_zero_mask = _mm512_cmpneq_epi64_mask(rrprime_2, zero);
    const unsigned rrprime_3_zero_mask = _mm512_cmpneq_epi64_mask(rrprime_3, zero);
    const unsigned rrprime_4_zero_mask = _mm512_cmpneq_epi64_mask(rrprime_4, zero);
    unsigned comb = rrprime_4_zero_mask << 24u | rrprime_3_zero_mask << 16u | rrprime_2_zero_mask << 8u | rrprime_1_zero_mask;
    unsigned lead_z = __builtin_clz(comb) - 8u;
    while(lead_z != 0)
    {
      if(lead_z >= 16)
      {
        rrprime_4 = rrprime_2;
        rrprime_3 = rrprime_1;
        rrprime_2 = zero;
        rrprime_1 = zero;
        comb <<= 16u;
      }
      else if(lead_z >= 8)
      {
        rrprime_4 = rrprime_3;
        rrprime_3 = rrprime_2;
        rrprime_2 = rrprime_1;
        rrprime_1 = zero;
        comb <<= 8u;
      }
      else if(lead_z >= 4)
      {
        rrprime_4 = _mm512_alignr_epi64(rrprime_4, rrprime_3, 4);
        rrprime_3 = _mm512_alignr_epi64(rrprime_3, rrprime_2, 4);
        rrprime_2 = _mm512_alignr_epi64(rrprime_2, rrprime_1, 4);
        rrprime_1 = _mm512_alignr_epi64(rrprime_1, zero, 4);
        comb <<= 4u;
      }
      else if(lead_z >= 2)
      {
        rrprime_4 = _mm512_alignr_epi64(rrprime_4, rrprime_3, 6);
        rrprime_3 = _mm512_alignr_epi64(rrprime_3, rrprime_2, 6);
        rrprime_2 = _mm512_alignr_epi64(rrprime_2, rrprime_1, 6);
        rrprime_1 = _mm512_alignr_epi64(rrprime_1, zero, 6);
        comb <<= 2u;
      }
      else
      {
        rrprime_4 = _mm512_alignr_epi64(rrprime_4, rrprime_3, 7);
        rrprime_3 = _mm512_alignr_epi64(rrprime_3, rrprime_2, 7);
        rrprime_2 = _mm512_alignr_epi64(rrprime_2, rrprime_1, 7);
        rrprime_1 = _mm512_alignr_epi64(rrprime_1, zero, 7);
        comb <<= 1u;
      }
      lead_z = __builtin_clz(comb) - 8u;
    }

    uint32_t word_idx = k / 32;
    uint64_t chunk = 0;
    unsigned next_8;
    unsigned simdy_8;

    while(digits_computed < total_digits)
    {

      // step a & b: multiply by 10^8 and cascade the carry
      uint32_t carry = 0; // carry max is 10^8, fits safely in uint32_t
      for(unsigned int &w : r)
      {
        // max size: (2^32 - 1) * 10^8 + 10^8 = 2^32 * 10^8. fits comfortably in uint64_t.
        uint64_t p = (uint64_t)w * 100'000'000u + carry;
        w = (uint32_t)p;
        carry = (uint32_t)(p >> 32);
      }

      const __m512i u64_prod_1 = _mm512_mul_epu32(rrprime_1, r1e8);
      const __m512i u64_prod_2 = _mm512_mul_epu32(rrprime_2, r1e8);
      const __m512i u64_prod_3 = _mm512_mul_epu32(rrprime_3, r1e8);
      const __m512i u64_prod_4 = _mm512_mul_epu32(rrprime_4, r1e8);
      const __m512i u32_lo_prod_1 = _mm512_mask_blend_epi32(0b1010'1010'1010'1010, u64_prod_1, zero);
      const __m512i u32_lo_prod_2 = _mm512_mask_blend_epi32(0b1010'1010'1010'1010, u64_prod_2, zero);
      const __m512i u32_lo_prod_3 = _mm512_mask_blend_epi32(0b1010'1010'1010'1010, u64_prod_3, zero);
      const __m512i u32_lo_prod_4 = _mm512_mask_blend_epi32(0b1010'1010'1010'1010, u64_prod_4, zero);
      const __m512i u32_hi_prod_1 = _mm512_srli_epi64(u64_prod_1, 32);
      const __m512i u32_hi_prod_2 = _mm512_srli_epi64(u64_prod_2, 32);
      const __m512i u32_hi_prod_3 = _mm512_srli_epi64(u64_prod_3, 32);
      const __m512i u32_hi_prod_4 = _mm512_srli_epi64(u64_prod_4, 32);
      const __m512i u32_hi_prod_slide_1 = _mm512_alignr_epi64(u32_hi_prod_1, zero, 7);
      const __m512i u32_hi_prod_slide_2 = _mm512_alignr_epi64(u32_hi_prod_2, u32_hi_prod_1, 7);
      const __m512i u32_hi_prod_slide_3 = _mm512_alignr_epi64(u32_hi_prod_3, u32_hi_prod_2, 7);
      const __m512i u32_hi_prod_slide_4 = _mm512_alignr_epi64(u32_hi_prod_4, u32_hi_prod_3, 7);
      rrprime_1 = _mm512_add_epi64(u32_lo_prod_1, u32_hi_prod_slide_1);
      rrprime_2 = _mm512_add_epi64(u32_lo_prod_2, u32_hi_prod_slide_2);
      rrprime_3 = _mm512_add_epi64(u32_lo_prod_3, u32_hi_prod_slide_3);
      rrprime_4 = _mm512_add_epi64(u32_lo_prod_4, u32_hi_prod_slide_4);
      const __m512i rrprime_slide_1 = _mm512_srli_epi64(_mm512_alignr_epi64(rrprime_1, zero, 7), 32);
      const __m512i rrprime_slide_2 = _mm512_srli_epi64(_mm512_alignr_epi64(rrprime_2, rrprime_1, 7), 32);
      const __m512i rrprime_slide_3 = _mm512_srli_epi64(_mm512_alignr_epi64(rrprime_3, rrprime_2, 7), 32);
      const __m512i rrprime_slide_4 = _mm512_srli_epi64(_mm512_alignr_epi64(rrprime_4, rrprime_3, 7), 32);
      rrprime_1 = _mm512_add_epi32(rrprime_1, rrprime_slide_1);
      rrprime_2 = _mm512_add_epi32(rrprime_2, rrprime_slide_2);
      rrprime_3 = _mm512_add_epi32(rrprime_3, rrprime_slide_3);
      rrprime_4 = _mm512_add_epi32(rrprime_4, rrprime_slide_4);

      const __m512i next_o = _mm512_alignr_epi64(rrprime_3, zero, 7);

      const __m512i last_lsb = _mm512_alignr_epi64(zero, rrprime_3, 7);

      const __m512i lo_o = _mm512_srli_epi64(rrprime_4, bit_shift);
      const __m512i hi_o = _mm512_slli_epi64(next_o, 32u - bit_shift);

      const __m512i lo_n = _mm512_srli_epi64(last_lsb, bit_shift);
      const __m512i hi_n = _mm512_slli_epi64(rrprime_4, 32u - bit_shift);

      const __m512i chunks_o = _mm512_or_si512(lo_o, hi_o);
      const __m512i chunks_n = _mm512_or_si512(lo_n, hi_n);

      // step c: extract next8 chunk (r >> k)
      chunk = 0;
      if(word_idx < num_words)
      {
        chunk = r[word_idx] >> bit_shift;
      }
      if(word_idx + 1 < num_words && bit_shift != 0)
      {
        // cast to uint64_t before shifting to prevent any 32-bit boundary overflow
        chunk |= ((uint64_t)r[word_idx + 1] << (32 - bit_shift));
      }
      next_8 = (uint32_t)chunk;

      // step d: apply modulo 2^k instantly
      if(word_idx < num_words)
      {
        r[word_idx] &= mod_mask;
        std::memset(static_cast<void *>(&r[word_idx + 1]), 0, (num_words - word_idx - 1) * 4);
      }

      rrprime_3 = _mm512_mask_and_epi64(rrprime_3, 0b1000'0000, rrprime_3, mod_vec);

      rrprime_4 = zero;

      simdy_8 = _mm_extract_epi32(_mm512_castsi512_si128(chunks_n), 0);

      assert(simdy_8 == next_8);

      // step e: print immediately
      // std::cout << std::setfill('0') << std::setw(8) << simdy_8;

      digits_computed += 8;
    }

    // std::cout << "\n";
  }

  return 0;
}

/*
int main()
{
  constexpr uint32_t pow_5_correction[] = { 1, 10, 100, 1'000, 10'000, 100'000, 1'000'000, 10'000'000, 100'000'000 };
  constexpr uint8_t pow_5_idx[] = { 0, 24, 48, 72, 96, 120, 144, 168, 192, 216, 240 };
  constexpr uint8_t pow_5_e[] = { 0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40 };

  constexpr uint32_t pow_5_cache[] = {
    1,          0,          0,          0,          0,          0,          0,          0, // 0 - _mm256i loaded and 0 extended to a _mm512i
    0,          0,          0,          0,          0,          0,          0,          0, // 1 - again
    0,          0,          0,          0,          0,          0,          0,          0, // 2 - again

    2242703233, 762134875,  1262,       0,          0,          0,          0,          0, // 0 - _mm256i loaded and 0 extended to a _mm512i
    0,          0,          0,          0,          0,          0,          0,          0, // 1 - again
    0,          0,          0,          0,          0,          0,          0,          0, // 2 - again

    3211403009, 1849224548, 3668416493, 3913284084, 1593091,    0,          0,          0, // 0 - _mm256i loaded and 0 extended to a _mm512i
    0,          0,          0,          0,          0,          0,          0,          0, // 1 - again
    0,          0,          0,          0,          0,          0,          0,          0, // 2 - again

    3776417409, 3833115195, 474402842,  2046101519, 1659368615, 1657637457, 2010764683, 0, // 0 - _mm256i loaded and 0 extended to a _mm512i
    0,          0,          0,          0,          0,          0,          0,          0, // 1 - again
    0,          0,          0,          0,          0,          0,          0,          0, // 2 - again

    781532673,  64985353,   253049085,  594863151,  3553621484, 3288652808, 3167596762, 2788392729, // 0 - _mm256i loaded and 0 extended to a _mm512i
    3911132675, 590,        0,          0,          0,          0,          0,          0,          // 1 - again -
    0,          0,          0,          0,          0,          0,          0,          0,          // 2 - again

    4223872385, 1378013428, 3071223888, 1460925171, 3695318035, 3594083709, 4166588825, 9582620, // 0 - _mm256i loaded and 0 extended to a _mm512i
    2259883222, 2057664441, 314048097,  745834,     0,          0,          0,          0,       // 1 - again -
    0,          0,          0,          0,          0,          0,          0,          0,       // 2 - again

    2894159105, 2850127391, 1678958885, 1990048638, 4055408710, 4272372864, 3869429580, 1940770817, // 0 - _mm256i loaded and 0 extended to a _mm512i
    3594030316, 3284427401, 4246652165, 2708404372, 1313461594, 941374947,  0,          0,          // 1 - again -
    0,          0,          0,          0,          0,          0,          0,          0,          // 2 - again

    3031420033, 1112378521, 855339782,  4191278750, 248038208,  1136169807, 2230652081, 3378214371, // 0 - _mm256i loaded and 0 extended to a _mm512i
    1653863564, 613940998,  1392057263, 562793371,  894754680,  2060902835, 2039635717, 2771255238, // 1 - again -
    276,        0,          0,          0,          0,          0,          0,          0,          // 2 - again

    2553183233, 3201533787, 3638140786, 303378311,  1809731782, 3477761648, 3583367183, 649228654,  // 0 - _mm256i loaded and 0 extended to a _mm512i
    2915460784, 487929380,  1011012442, 1677677582, 3428152256, 1710878487, 1438394610, 2161952759, // 1 - again -
    4100910556, 1608314830, 349175,     0,          0,          0,          0,          0,          // 2 - again

    3940379521, 184513341,  2872588323, 2214530454, 38258512,   2980860351, 114267010,  2188874685, // 0 - _mm256i loaded and 0 extended to a _mm512i
    234079247,  2101059099, 1948702207, 947446250,  864457656,  507589568,  1321007357, 3911984176, // 1 - again -
    1011110295, 2382358050, 2389730781, 730678769,  440721283,  0,          0,          0,          // 2 - again

    1352440577, 3353128433, 943566124,  1942375735, 3933038544, 1369155590, 3288593144, 2689786274, // 0 - _mm256i loaded and 0 extended to a _mm512i
    3608159034, 1149596585, 3474119098, 1887901699, 1390164635, 3853669096, 119012174,  3192759855, // 1 - again -
    179034081,  2355293519, 3468291195, 3563470881, 2702541568, 3437458918, 2217683442, 129,        // 2 - again
  };

  for(int k = 1050; k < 1075; k++)
  {
    // 36 words * 32 bits = 1152 bits. perfectly fits k=1100 + 10^8 overflow.
    const uint32_t p = std::floor(k * std::log10(2));

    const uint32_t p_3 = p >> 3u;

    const uint32_t p_3_4 = p_3 >> 2u;

    const __m512i zero = _mm512_setzero_si512();

    __m512i rrprime_1 = _mm512_cvtepu32_epi64(_mm256_loadu_epi32(&pow_5_cache[pow_5_idx[p_3_4]]));
    __m512i rrprime_2 = _mm512_cvtepu32_epi64(_mm256_loadu_epi32(&pow_5_cache[pow_5_idx[p_3_4] + 8u]));
    __m512i rrprime_3 = _mm512_cvtepu32_epi64(_mm256_loadu_epi32(&pow_5_cache[pow_5_idx[p_3_4] + 16u]));
    __m512i rrprime_4 = zero;
    __m512i r1e8 = _mm512_set1_epi64(pow_5_correction[8]);
    __m512i r1emis = _mm512_set1_epi64(pow_5_correction[p - (p_3 << 3u)]);

    uint32_t e = pow_5_e[p_3_4];

    for(; e < p_3; e++)
    {
      const __m512i u64_prod_1 = _mm512_mul_epu32(rrprime_1, r1e8);
      const __m512i u64_prod_2 = _mm512_mul_epu32(rrprime_2, r1e8);
      const __m512i u64_prod_3 = _mm512_mul_epu32(rrprime_3, r1e8);
      const __m512i u64_prod_4 = _mm512_mul_epu32(rrprime_4, r1e8);
      const __m512i u32_lo_prod_1 = _mm512_mask_blend_epi32(0b1010'1010'1010'1010, u64_prod_1, zero);
      const __m512i u32_lo_prod_2 = _mm512_mask_blend_epi32(0b1010'1010'1010'1010, u64_prod_2, zero);
      const __m512i u32_lo_prod_3 = _mm512_mask_blend_epi32(0b1010'1010'1010'1010, u64_prod_3, zero);
      const __m512i u32_lo_prod_4 = _mm512_mask_blend_epi32(0b1010'1010'1010'1010, u64_prod_4, zero);
      const __m512i u32_hi_prod_1 = _mm512_srli_epi64(u64_prod_1, 32);
      const __m512i u32_hi_prod_2 = _mm512_srli_epi64(u64_prod_2, 32);
      const __m512i u32_hi_prod_3 = _mm512_srli_epi64(u64_prod_3, 32);
      const __m512i u32_hi_prod_4 = _mm512_srli_epi64(u64_prod_4, 32);
      const __m512i u32_hi_prod_slide_1 = _mm512_alignr_epi64(u32_hi_prod_1, zero, 7);
      const __m512i u32_hi_prod_slide_2 = _mm512_alignr_epi64(u32_hi_prod_2, u32_hi_prod_1, 7);
      const __m512i u32_hi_prod_slide_3 = _mm512_alignr_epi64(u32_hi_prod_3, u32_hi_prod_2, 7);
      const __m512i u32_hi_prod_slide_4 = _mm512_alignr_epi64(u32_hi_prod_4, u32_hi_prod_3, 7);
      rrprime_1 = _mm512_add_epi64(u32_lo_prod_1, u32_hi_prod_slide_1);
      rrprime_2 = _mm512_add_epi64(u32_lo_prod_2, u32_hi_prod_slide_2);
      rrprime_3 = _mm512_add_epi64(u32_lo_prod_3, u32_hi_prod_slide_3);
      rrprime_4 = _mm512_add_epi64(u32_lo_prod_4, u32_hi_prod_slide_4);
      const __m512i rrprime_slide_1 = _mm512_srli_epi64(_mm512_alignr_epi64(rrprime_1, zero, 7), 32);
      const __m512i rrprime_slide_2 = _mm512_srli_epi64(_mm512_alignr_epi64(rrprime_2, rrprime_1, 7), 32);
      const __m512i rrprime_slide_3 = _mm512_srli_epi64(_mm512_alignr_epi64(rrprime_3, rrprime_2, 7), 32);
      const __m512i rrprime_slide_4 = _mm512_srli_epi64(_mm512_alignr_epi64(rrprime_4, rrprime_3, 7), 32);
      rrprime_1 = _mm512_add_epi32(rrprime_1, rrprime_slide_1);
      rrprime_2 = _mm512_add_epi32(rrprime_2, rrprime_slide_2);
      rrprime_3 = _mm512_add_epi32(rrprime_3, rrprime_slide_3);
      rrprime_4 = _mm512_add_epi32(rrprime_4, rrprime_slide_4);
    }
    {

      const __m512i u64_prod_1 = _mm512_mul_epu32(rrprime_1, r1emis);
      const __m512i u64_prod_2 = _mm512_mul_epu32(rrprime_2, r1emis);
      const __m512i u64_prod_3 = _mm512_mul_epu32(rrprime_3, r1emis);
      const __m512i u64_prod_4 = _mm512_mul_epu32(rrprime_4, r1emis);
      const __m512i u32_lo_prod_1 = _mm512_mask_blend_epi32(0b1010'1010'1010'1010, u64_prod_1, zero);
      const __m512i u32_lo_prod_2 = _mm512_mask_blend_epi32(0b1010'1010'1010'1010, u64_prod_2, zero);
      const __m512i u32_lo_prod_3 = _mm512_mask_blend_epi32(0b1010'1010'1010'1010, u64_prod_3, zero);
      const __m512i u32_lo_prod_4 = _mm512_mask_blend_epi32(0b1010'1010'1010'1010, u64_prod_4, zero);
      const __m512i u32_hi_prod_1 = _mm512_srli_epi64(u64_prod_1, 32);
      const __m512i u32_hi_prod_2 = _mm512_srli_epi64(u64_prod_2, 32);
      const __m512i u32_hi_prod_3 = _mm512_srli_epi64(u64_prod_3, 32);
      const __m512i u32_hi_prod_4 = _mm512_srli_epi64(u64_prod_4, 32);
      const __m512i u32_hi_prod_slide_1 = _mm512_alignr_epi64(u32_hi_prod_1, zero, 7);
      const __m512i u32_hi_prod_slide_2 = _mm512_alignr_epi64(u32_hi_prod_2, u32_hi_prod_1, 7);
      const __m512i u32_hi_prod_slide_3 = _mm512_alignr_epi64(u32_hi_prod_3, u32_hi_prod_2, 7);
      const __m512i u32_hi_prod_slide_4 = _mm512_alignr_epi64(u32_hi_prod_4, u32_hi_prod_3, 7);
      rrprime_1 = _mm512_add_epi32(u32_lo_prod_1, u32_hi_prod_slide_1);
      rrprime_2 = _mm512_add_epi32(u32_lo_prod_2, u32_hi_prod_slide_2);
      rrprime_3 = _mm512_add_epi32(u32_lo_prod_3, u32_hi_prod_slide_3);
      rrprime_4 = _mm512_add_epi32(u32_lo_prod_4, u32_hi_prod_slide_4);
      const __m512i rrprime_slide_1 = _mm512_srli_epi64(_mm512_alignr_epi64(rrprime_1, zero, 7), 32);
      const __m512i rrprime_slide_2 = _mm512_srli_epi64(_mm512_alignr_epi64(rrprime_2, rrprime_1, 7), 32);
      const __m512i rrprime_slide_3 = _mm512_srli_epi64(_mm512_alignr_epi64(rrprime_3, rrprime_2, 7), 32);
      const __m512i rrprime_slide_4 = _mm512_srli_epi64(_mm512_alignr_epi64(rrprime_4, rrprime_3, 7), 32);
      rrprime_1 = _mm512_add_epi32(rrprime_1, rrprime_slide_1);
      rrprime_2 = _mm512_add_epi32(rrprime_2, rrprime_slide_2);
      rrprime_3 = _mm512_add_epi32(rrprime_3, rrprime_slide_3);
      rrprime_4 = _mm512_add_epi32(rrprime_4, rrprime_slide_4);
    }

    constexpr unsigned num_words = 40;
    std::array<uint32_t, num_words> written = { 0 };
    std::array<uint32_t, num_words> r = { 0 };

    const __m256i u32_pack_1 = _mm512_cvtepi64_epi32(rrprime_1);
    const __m256i u32_pack_2 = _mm512_cvtepi64_epi32(rrprime_2);
    const __m256i u32_pack_3 = _mm512_cvtepi64_epi32(rrprime_3);
    const __m256i u32_pack_4 = _mm512_cvtepi64_epi32(rrprime_4);

    _mm256_storeu_epi32(static_cast<void *>(&written[0]), u32_pack_1);
    _mm256_storeu_epi32(static_cast<void *>(&written[8]), u32_pack_2);
    _mm256_storeu_epi32(static_cast<void *>(&written[16]), u32_pack_3);
    _mm256_storeu_epi32(static_cast<void *>(&written[24]), u32_pack_4);

    r[0] = 1; // initialize r = 1

    for(uint32_t i = 0; i < p; ++i)
    {
      uint32_t carry = 0;
      for(int w = 0; w < num_words; ++w)
      {
        uint64_t p = (uint64_t)r[w] * 10ull + carry;
        r[w] = (uint32_t)p;
        carry = (uint32_t)(p >> 32);
      }
    }

    size_t first = 0;

    while(first < r.size() && r[first] == 0)
    {
      ++first;
    }

    if(first != 0 && first != r.size())
    {
      const size_t count = r.size() - first;

      std::memmove(r.data(), r.data() + first, count * sizeof(uint32_t));

      std::memset(r.data() + count, 0, first * sizeof(uint32_t));
    }

    assert(written == r);
  }

  return 0;
}
*/

/*
int main()
{
  constexpr uint32_t pow_5_cache[] = {
    1,          0,          0,          0,          0,          0,          0,          0, // 0 - _mm256i loaded and 0 extended to a _mm512i

    2242703233, 762134875,  1262,       0,          0,          0,          0,          0, // 0 - _mm256i loaded and 0 extended to a _mm512i

    3211403009, 1849224548, 3668416493, 3913284084, 1593091,    0,          0,          0, // 0 - _mm256i loaded and 0 extended to a _mm512i

    3776417409, 3833115195, 474402842,  2046101519, 1659368615, 1657637457, 2010764683, 0, // 0 - _mm256i loaded and 0 extended to a _mm512i

    781532673,  64985353,   253049085,  594863151,  3553621484, 3288652808, 3167596762, 2788392729, // 0 - _mm256i loaded and 0 extended to a _mm512i
    3911132675, 590,        0,          0,          0,          0,          0,          0,          // 1 - again -

    4223872385, 1378013428, 3071223888, 1460925171, 3695318035, 3594083709, 4166588825, 9582620, // 0 - _mm256i loaded and 0 extended to a _mm512i
    2259883221, 2057664441, 314048097,  745834,     0,          0,          0,          0,       // 1 - again -

    2894159105, 2850127391, 1678958885, 1990048638, 4055408710, 4272372864, 3869429580, 1940770817, // 0 - _mm256i loaded and 0 extended to a _mm512i
    1351327083, 2522292526, 4246650903, 2708404372, 1313461594, 941374947,  0,          0,          // 1 - again -

    3031420033, 1112378521, 855339782,  4191278750, 248038208,  1136169807, 2230652081,    // 0 - _mm256i loaded and 0 extended to a _mm512i
    3378214371, 2737427851, 3059683745, 2018608065, 944476582,  893161588,  2060902835,    // 1 - again -
    2039635717, 2771255238, 276,        0,          0,          0,          0,          0, // 2 - again -

    2553183233, 3201533787, 3638140786, 303378311,  1809731782, 3477761648,                // 0 - _mm256i loaded and 0 extended to a _mm512i
    3583367183, 649228654,  3434010671, 949781480,  536609599,  3926543359,                // 1 -  again  -
    1768783640, 53241030,   3722597222, 2161952758, 4100910556, 1608314830, 349175,     0, // 2 -  again  -

    3940379521, 184513341,  2872588323, 2214530454, 38258512,   2980860350, 114267010,  2188874685, // 0 - _mm256i loaded and 0 extended to a _mm512i
    3647513870, 2036073745, 1695653122, 352583099,  1605803468, 1513904055, 205674657,  361456571,  // 1 - again  -
    1394943654, 2382357459, 2389730781, 730678769,  440721283,  0,          0,          0,          // 2 - again  -

    1352440577, 651996657,  477028415,  1942321525, 3933038544, 3421419653, 2526458268, 2689785012, // 0 - _mm256i loaded and 0 extended to a _mm512i
    559387833,  2512249609, 3034169254, 426976498,  1989813896, 259585386,  1330954932, 1333952685, // 1 - again  -
    2840668958, 679312288,  3152650006, 3562725047, 1409792,    2970921208, 2217629232, 129,        // 2 - again  -
  };

  std::cout << "{";

  std::vector<bool> done(41, false);

  for(uint32_t k = 1; k < 1074; k++)
  {
    // 36 words * 32 bits = 1152 bits. perfectly fits k=1100 + 10^8 overflow.
    const uint32_t p = std::floor(k * std::log10(2));
    // initialize our base as a bigint equivalent to 10^8
    uint32_t e = 0;

    const __m512i zero = _mm512_setzero_si512();
    __m512i rrprime_1 = _mm512_setr_epi64(1, 0, 0, 0, 0, 0, 0, 0);
    __m512i rrprime_2 = zero;
    __m512i rrprime_3 = zero;
    __m512i rrprime_4 = zero;
    __m512i rrprime_5 = zero;
    __m512i r1e8 = _mm512_set1_epi64(100'000'000u);

    const uint32_t p_3 = p >> 3u;

    uint32_t written[40];

    for(; e < p_3; e++)
    {
      const __m512i u64_prod_1 = _mm512_mul_epu32(rrprime_1, r1e8);
      const __m512i u64_prod_2 = _mm512_mul_epu32(rrprime_2, r1e8);
      const __m512i u64_prod_3 = _mm512_mul_epu32(rrprime_3, r1e8);
      const __m512i u64_prod_4 = _mm512_mul_epu32(rrprime_4, r1e8);
      const __m512i u64_prod_5 = _mm512_mul_epu32(rrprime_5, r1e8);
      const __m512i u32_lo_prod_1 = _mm512_mask_blend_epi32(0b1010'1010'1010'1010, u64_prod_1, zero);
      const __m512i u32_lo_prod_2 = _mm512_mask_blend_epi32(0b1010'1010'1010'1010, u64_prod_2, zero);
      const __m512i u32_lo_prod_3 = _mm512_mask_blend_epi32(0b1010'1010'1010'1010, u64_prod_3, zero);
      const __m512i u32_lo_prod_4 = _mm512_mask_blend_epi32(0b1010'1010'1010'1010, u64_prod_4, zero);
      const __m512i u32_lo_prod_5 = _mm512_mask_blend_epi32(0b1010'1010'1010'1010, u64_prod_5, zero);
      const __m512i u32_hi_prod_1 = _mm512_srli_epi64(u64_prod_1, 32);
      const __m512i u32_hi_prod_2 = _mm512_srli_epi64(u64_prod_2, 32);
      const __m512i u32_hi_prod_3 = _mm512_srli_epi64(u64_prod_3, 32);
      const __m512i u32_hi_prod_4 = _mm512_srli_epi64(u64_prod_4, 32);
      const __m512i u32_hi_prod_5 = _mm512_srli_epi64(u64_prod_5, 32);
      const __m512i u32_hi_prod_slide_1 = _mm512_alignr_epi64(u32_hi_prod_1, zero, 7);
      const __m512i u32_hi_prod_slide_2 = _mm512_alignr_epi64(u32_hi_prod_2, u32_hi_prod_1, 7);
      const __m512i u32_hi_prod_slide_3 = _mm512_alignr_epi64(u32_hi_prod_3, u32_hi_prod_2, 7);
      const __m512i u32_hi_prod_slide_4 = _mm512_alignr_epi64(u32_hi_prod_4, u32_hi_prod_3, 7);
      const __m512i u32_hi_prod_slide_5 = _mm512_alignr_epi64(u32_hi_prod_5, u32_hi_prod_4, 7);
      rrprime_1 = _mm512_add_epi64(u32_lo_prod_1, u32_hi_prod_slide_1);
      rrprime_2 = _mm512_add_epi64(u32_lo_prod_2, u32_hi_prod_slide_2);
      rrprime_3 = _mm512_add_epi64(u32_lo_prod_3, u32_hi_prod_slide_3);
      rrprime_4 = _mm512_add_epi64(u32_lo_prod_4, u32_hi_prod_slide_4);
      rrprime_5 = _mm512_add_epi64(u32_lo_prod_5, u32_hi_prod_slide_5);
    }

    const __m256i u32_pack_1 = _mm512_cvtepi64_epi32(rrprime_1);
    const __m256i u32_pack_2 = _mm512_cvtepi64_epi32(rrprime_2);
    const __m256i u32_pack_3 = _mm512_cvtepi64_epi32(rrprime_3);
    const __m256i u32_pack_4 = _mm512_cvtepi64_epi32(rrprime_4);
    const __m256i u32_pack_5 = _mm512_cvtepi64_epi32(rrprime_5);

    _mm256_storeu_epi32(static_cast<void *>(&written[0]), u32_pack_1);
    _mm256_storeu_epi32(static_cast<void *>(&written[8]), u32_pack_2);
    _mm256_storeu_epi32(static_cast<void *>(&written[16]), u32_pack_3);
    _mm256_storeu_epi32(static_cast<void *>(&written[24]), u32_pack_4);
    _mm256_storeu_epi32(static_cast<void *>(&written[32]), u32_pack_5);

    if(done[p_3] == false && p_3 % 4 == 0)
    {
      done[p_3] = true;
      std::cout << "{ ";
      for(const auto &i : written)
      {
        std::cout << i << ",";
      }
      std::cout << "}, ";
    }
  }

  std::cout << "} \n";

  return 0;
}
*/

/*
int main()
{
  constexpr uint32_t pow_5_cache[] = {
    2701131776, 466537709, 54210, 0,                // 0 - 256 bit (8  uint32_t's)
    0, 0, 0, 0,                                     // 1 -
                                                    //
    553648128, 1943292173, 4261398408, 2484038180,  // 0 - 256 bit (8 uint32_t's)
    68422776, 0, 0, 0,                              // 1 -
                                                    //
    2701131776, 807615852, 3882706566, 3057181734,  // 0 - 256 bit (8 uint32_t's)
    745289159, 4056365773, 462339630, 20,           // 1 -
                                                    //
    1581318144, 3996984702, 317337323, 3761442594,  // 0 - 512 bit (16 uint32_t's) // will be loaded as 2 separate (_mm256i) and 0 extended
    3054408957, 3984113819, 1405123673, 3088262246, // 1 -
    1090037, 0, 0, 0,                               // 2 -
    0, 0, 0, 0,                                     // 3 -
                                                    //
    767623168, 4241160024, 1260959332, 2541775228,  // 0 - 512 bit (16 uint32_t's) // will be loaded as 2 separate (_mm256i) and 0 extended
    2965753944, 1796720685, 484800439, 1311835347,  // 1 -
    2945126454, 3563705203, 1375821026, 0,          // 2 -
    0, 0, 0, 0,                                     // 3 -
                                                    //
    1027670016, 584120530, 2817733607, 1418065558,  // 0 - 512 bit (16 uint32_t's) // will be loaded as 2 separate (_mm256i) and 0 extended
    2125434793, 2583803420, 1379713046, 3308183724, // 1 -
    3479371296, 2677438127, 4145998343, 931079930,  // 2 -
    1363485451, 404, 0, 0,                          // 3 -
                                                    //
    2361458688, 1372100044, 4019207324, 1803970613, // 0 - 512 bit (16 uint32_t's) // will be loaded as 2 separate (_mm256i) and 0 extended
    3054822823, 2484332018, 886717433, 979406226,   // 1 -
    178043906, 3605602663, 1325973018, 2766841440,  // 2 -
    3535169275, 170736516, 1638513683, 510320,      // 3 -
                                                    //
    3207749888, 3090445067, 878803004, 3926310579,  // 0 - 768 bit (24 uint32_t's) // will be loaded as 3 separate (_mm256i) and 0 extended
    2511096468, 3211496945, 4229969674, 2801008543, // 1 -
    3731249614, 2522876060, 2243995037, 263064917,  // 2 -
    3209265061, 1558881170, 2972226309, 387950025,  // 3 -
    1894719538, 6, 0, 0,                            // 4 -
    0, 0, 0, 0,                                     // 5 -
                                                    //
    2364662016, 1050217897, 464427981, 3271896570,  // 0 - 768 bit (24 uint32_t's) // will be loaded as 3 separate (_mm256i) and 0 extended
    2431110680, 1321427493, 3373513620, 2398142296, // 1 -
    2095248896, 1505421683, 3598109983, 3281306459, // 2 -
    1545540873, 1451518959, 3120402494, 1029160510, // 3 -
    1905946741, 3130719652, 3748297313, 8129,       // 4 -
    0, 0, 0, 0,                                     // 5 -
                                                    //
    49373440, 69305454, 3524743267, 494625378,      // 0 - 768 bit (24 uint32_t's) // will be loaded as 3 separate (_mm256i) and 0 extended
    1758466655, 1690015127, 707937707, 769832326,   // 1 -
    947974910, 331926644, 3401326036, 4283614656,   // 2 -
    2171178814, 2423608825, 1226070469, 2615635200, // 3 -
    3797307464, 286984290, 2589368102, 3530478015,  // 4 -
    13941208, 10261341, 0, 0                        // 5 -
                                                    //

  };

  std::cout << "{ ";
  for(uint32_t k = 0; k < 1100; k += 100)
  {
    // 36 words * 32 bits = 1152 bits. perfectly fits k=1100 + 10^8 overflow.
    const uint32_t p = std::floor(k * std::log10(2));
    // initialize our base as a bigint equivalent to 10^8
    uint32_t e = 0;

    const __m512i zero = _mm512_setzero_si512();
    __m512i rrprime_1 = _mm512_setr_epi64(1, 0, 0, 0, 0, 0, 0, 0);
    __m512i rrprime_2 = zero;
    __m512i rrprime_3 = zero;
    __m512i rrprime_4 = zero;
    __m512i rrprime_5 = zero;
    __m512i r1e8 = _mm512_set1_epi64(100'000'000u);

    uint32_t written[40];

    for(; e + 8 < p; e += 8)
    {
      const __m512i u64_prod_1 = _mm512_mul_epu32(rrprime_1, r1e8);
      const __m512i u64_prod_2 = _mm512_mul_epu32(rrprime_2, r1e8);
      const __m512i u64_prod_3 = _mm512_mul_epu32(rrprime_3, r1e8);
      const __m512i u64_prod_4 = _mm512_mul_epu32(rrprime_4, r1e8);
      const __m512i u64_prod_5 = _mm512_mul_epu32(rrprime_5, r1e8);
      const __m512i u32_lo_prod_1 = _mm512_mask_blend_epi32(0b1010'1010'1010'1010, u64_prod_1, zero);
      const __m512i u32_lo_prod_2 = _mm512_mask_blend_epi32(0b1010'1010'1010'1010, u64_prod_2, zero);
      const __m512i u32_lo_prod_3 = _mm512_mask_blend_epi32(0b1010'1010'1010'1010, u64_prod_3, zero);
      const __m512i u32_lo_prod_4 = _mm512_mask_blend_epi32(0b1010'1010'1010'1010, u64_prod_4, zero);
      const __m512i u32_lo_prod_5 = _mm512_mask_blend_epi32(0b1010'1010'1010'1010, u64_prod_5, zero);
      const __m512i u32_hi_prod_1 = _mm512_srli_epi64(u64_prod_1, 32);
      const __m512i u32_hi_prod_2 = _mm512_srli_epi64(u64_prod_2, 32);
      const __m512i u32_hi_prod_3 = _mm512_srli_epi64(u64_prod_3, 32);
      const __m512i u32_hi_prod_4 = _mm512_srli_epi64(u64_prod_4, 32);
      const __m512i u32_hi_prod_5 = _mm512_srli_epi64(u64_prod_5, 32);
      const __m512i u32_hi_prod_slide_1 = _mm512_alignr_epi64(u32_hi_prod_1, zero, 7);
      const __m512i u32_hi_prod_slide_2 = _mm512_alignr_epi64(u32_hi_prod_2, u32_hi_prod_1, 7);
      const __m512i u32_hi_prod_slide_3 = _mm512_alignr_epi64(u32_hi_prod_3, u32_hi_prod_2, 7);
      const __m512i u32_hi_prod_slide_4 = _mm512_alignr_epi64(u32_hi_prod_4, u32_hi_prod_3, 7);
      const __m512i u32_hi_prod_slide_5 = _mm512_alignr_epi64(u32_hi_prod_5, u32_hi_prod_4, 7);
      rrprime_1 = _mm512_add_epi64(u32_lo_prod_1, u32_hi_prod_slide_1);
      rrprime_2 = _mm512_add_epi64(u32_lo_prod_2, u32_hi_prod_slide_2);
      rrprime_3 = _mm512_add_epi64(u32_lo_prod_3, u32_hi_prod_slide_3);
      rrprime_4 = _mm512_add_epi64(u32_lo_prod_4, u32_hi_prod_slide_4);
      rrprime_5 = _mm512_add_epi64(u32_lo_prod_5, u32_hi_prod_slide_5);
    }

    const __m256i u32_pack_1 = _mm512_cvtepi64_epi32(rrprime_1);
    const __m256i u32_pack_2 = _mm512_cvtepi64_epi32(rrprime_2);
    const __m256i u32_pack_3 = _mm512_cvtepi64_epi32(rrprime_3);
    const __m256i u32_pack_4 = _mm512_cvtepi64_epi32(rrprime_4);
    const __m256i u32_pack_5 = _mm512_cvtepi64_epi32(rrprime_5);

    _mm256_storeu_epi32(static_cast<void *>(&written[0]), u32_pack_1);
    _mm256_storeu_epi32(static_cast<void *>(&written[8]), u32_pack_2);
    _mm256_storeu_epi32(static_cast<void *>(&written[16]), u32_pack_3);
    _mm256_storeu_epi32(static_cast<void *>(&written[24]), u32_pack_4);
    _mm256_storeu_epi32(static_cast<void *>(&written[32]), u32_pack_5);

    std::cout << "{ ";
    for(const auto &i : written)
    {
      std::cout << i << ",";
    }
    std::cout << "}, ";

    int alsjdfl = 1;
  }

  std::cout << "}\n";

  return 0;
}
*/

// real algo
/*
int main()
{
  for(uint32_t k = 0; k < 1100; k += 100)
  {
    // 36 words * 32 bits = 1152 bits. perfectly fits k=1100 + 10^8 overflow.
    const uint32_t p = std::floor(k * std::log10(2));
    // initialize our base as a bigint equivalent to 10^8
    uint32_t e = 0;

    const __m512i zero = _mm512_setzero_si512();
    __m512i rrprime_1 = _mm512_setr_epi64(1, 0, 0, 0, 0, 0, 0, 0);
    __m512i rrprime_2 = zero;
    __m512i rrprime_3 = zero;
    __m512i rrprime_4 = zero;
    __m512i rrprime_5 = zero;
    __m512i r1e8 = _mm512_set1_epi64(100'000'000u);

    for(; e + 8 < p; e += 8)
    {
      const __m512i u64_prod_1 = _mm512_mul_epu32(rrprime_1, r1e8);
      const __m512i u64_prod_2 = _mm512_mul_epu32(rrprime_2, r1e8);
      const __m512i u64_prod_3 = _mm512_mul_epu32(rrprime_3, r1e8);
      const __m512i u64_prod_4 = _mm512_mul_epu32(rrprime_4, r1e8);
      const __m512i u64_prod_5 = _mm512_mul_epu32(rrprime_5, r1e8);
      const __m512i u32_lo_prod_1 = _mm512_mask_blend_epi32(0b1010'1010'1010'1010, u64_prod_1, zero);
      const __m512i u32_lo_prod_2 = _mm512_mask_blend_epi32(0b1010'1010'1010'1010, u64_prod_2, zero);
      const __m512i u32_lo_prod_3 = _mm512_mask_blend_epi32(0b1010'1010'1010'1010, u64_prod_3, zero);
      const __m512i u32_lo_prod_4 = _mm512_mask_blend_epi32(0b1010'1010'1010'1010, u64_prod_4, zero);
      const __m512i u32_lo_prod_5 = _mm512_mask_blend_epi32(0b1010'1010'1010'1010, u64_prod_5, zero);
      const __m512i u32_hi_prod_1 = _mm512_srli_epi64(u64_prod_1, 32);
      const __m512i u32_hi_prod_2 = _mm512_srli_epi64(u64_prod_2, 32);
      const __m512i u32_hi_prod_3 = _mm512_srli_epi64(u64_prod_3, 32);
      const __m512i u32_hi_prod_4 = _mm512_srli_epi64(u64_prod_4, 32);
      const __m512i u32_hi_prod_5 = _mm512_srli_epi64(u64_prod_5, 32);
      const __m512i u32_hi_prod_slide_1 = _mm512_alignr_epi64(u32_hi_prod_1, ZERO, 7);
      const __m512i u32_hi_prod_slide_2 = _mm512_alignr_epi64(u32_hi_prod_2, u32_hi_prod_1, 7);
      const __m512i u32_hi_prod_slide_3 = _mm512_alignr_epi64(u32_hi_prod_3, u32_hi_prod_2, 7);
      const __m512i u32_hi_prod_slide_4 = _mm512_alignr_epi64(u32_hi_prod_4, u32_hi_prod_3, 7);
      const __m512i u32_hi_prod_slide_5 = _mm512_alignr_epi64(u32_hi_prod_5, u32_hi_prod_4, 7);
      rrprime_1 = _mm512_add_epi64(u32_lo_prod_1, u32_hi_prod_slide_1);
      rrprime_2 = _mm512_add_epi64(u32_lo_prod_2, u32_hi_prod_slide_2);
      rrprime_3 = _mm512_add_epi64(u32_lo_prod_3, u32_hi_prod_slide_3);
      rrprime_4 = _mm512_add_epi64(u32_lo_prod_4, u32_hi_prod_slide_4);
      rrprime_5 = _mm512_add_epi64(u32_lo_prod_5, u32_hi_prod_slide_5);
    }

    int alsjdfl = 1;
  }

  return 0;
}
*/

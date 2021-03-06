/*
 *  Copyright (c) 2012 The LibYuv project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "libyuv/row.h"

#ifdef __cplusplus
namespace libyuv {
extern "C" {
#endif

#if !defined(YUV_DISABLE_ASM) && defined(__mips__)
#if defined HAS_COPYROW_MIPS
extern "C" void  memcpy_MIPS(uint8* dst, const uint8* src, int count);
void CopyRow_MIPS(const uint8* src, uint8* dst, int count) {
  memcpy_MIPS(dst, src, count);
}
#endif

#ifdef HAS_SPLITUV_MIPS_DSPR2
void SplitUV_MIPS_DSPR2(const uint8* src_uv, uint8* dst_u, uint8* dst_v,
                        int width) {
  __asm__ __volatile__ (
    ".set push                                     \n"
    ".set noreorder                                \n"
    "srl             $t4, %[width], 4              \n"  // multiplies of 16
    "blez            $t4, 2f                       \n"
    " andi           %[width], %[width], 0xf       \n"  // residual

  "1:                                              \n"
    "addiu           $t4, $t4, -1                  \n"
    "lw              $t0, 0(%[src_uv])             \n"  // V1 | U1 | V0 | U0
    "lw              $t1, 4(%[src_uv])             \n"  // V3 | U3 | V2 | U2
    "lw              $t2, 8(%[src_uv])             \n"  // V5 | U5 | V4 | U4
    "lw              $t3, 12(%[src_uv])            \n"  // V7 | U7 | V6 | U6
    "lw              $t5, 16(%[src_uv])            \n"  // V9 | U9 | V8 | U8
    "lw              $t6, 20(%[src_uv])            \n"  // V11 | U11 | V10 | U10
    "lw              $t7, 24(%[src_uv])            \n"  // V13 | U13 | V12 | U12
    "lw              $t8, 28(%[src_uv])            \n"  // V15 | U15 | V14 | U14
    "addiu           %[src_uv], %[src_uv], 32      \n"
    "precrq.qb.ph    $t9, $t1, $t0                 \n"  // V3 | V2 | V1 | V0
    "precr.qb.ph     $t0, $t1, $t0                 \n"  // U3 | U2 | U1 | U0
    "precrq.qb.ph    $t1, $t3, $t2                 \n"  // V7 | V6 | V5 | V4
    "precr.qb.ph     $t2, $t3, $t2                 \n"  // U7 | U6 | U5 | U4
    "precrq.qb.ph    $t3, $t6, $t5                 \n"  // V11 | V10 | V9 | V8
    "precr.qb.ph     $t5, $t6, $t5                 \n"  // U11 | U10 | U9 | U8
    "precrq.qb.ph    $t6, $t8, $t7                 \n"  // V15 | V14 | V13 | V12
    "precr.qb.ph     $t7, $t8, $t7                 \n"  // U15 | U14 | U13 | U12
    "sw              $t9, 0(%[dst_v])              \n"
    "sw              $t0, 0(%[dst_u])              \n"
    "sw              $t1, 4(%[dst_v])              \n"
    "sw              $t2, 4(%[dst_u])              \n"
    "sw              $t3, 8(%[dst_v])              \n"
    "sw              $t5, 8(%[dst_u])              \n"
    "sw              $t6, 12(%[dst_v])             \n"
    "sw              $t7, 12(%[dst_u])             \n"
    "addiu           %[dst_v], %[dst_v], 16        \n"
    "bgtz            $t4, 1b                       \n"
    " addiu          %[dst_u], %[dst_u], 16        \n"

    "beqz            %[width], 3f                  \n"
    " nop                                          \n"

  "2:                                              \n"
    "lbu             $t0, 0(%[src_uv])             \n"
    "lbu             $t1, 1(%[src_uv])             \n"
    "addiu           %[src_uv], %[src_uv], 2       \n"
    "addiu           %[width], %[width], -1        \n"
    "sb              $t0, 0(%[dst_u])              \n"
    "sb              $t1, 0(%[dst_v])              \n"
    "addiu           %[dst_u], %[dst_u], 1         \n"
    "bgtz            %[width], 2b                  \n"
    " addiu          %[dst_v], %[dst_v], 1         \n"

  "3:                                              \n"
    ".set pop                                      \n"
     : [src_uv] "+r" (src_uv),
       [width] "+r" (width),
       [dst_u] "+r" (dst_u),
       [dst_v] "+r" (dst_v)
     :
     : "t0", "t1", "t2", "t3",
       "t4", "t5", "t6", "t7", "t8", "t9"
  );
}

void SplitUV_Unaligned_MIPS_DSPR2(const uint8* src_uv, uint8* dst_u,
                                  uint8* dst_v, int width) {
  __asm__ __volatile__ (
    ".set push                                     \n"
    ".set noreorder                                \n"
    "srl             $t4, %[width], 4              \n"  // multiplies of 16
    "blez            $t4, 2f                       \n"
    " andi           %[width], %[width], 0xf       \n"  // residual

  "1:                                              \n"
    "addiu           $t4, $t4, -1                  \n"
    "lwr             $t0, 0(%[src_uv])             \n"
    "lwl             $t0, 3(%[src_uv])             \n"  // V1 | U1 | V0 | U0
    "lwr             $t1, 4(%[src_uv])             \n"
    "lwl             $t1, 7(%[src_uv])             \n"  // V3 | U3 | V2 | U2
    "lwr             $t2, 8(%[src_uv])             \n"
    "lwl             $t2, 11(%[src_uv])            \n"  // V5 | U5 | V4 | U4
    "lwr             $t3, 12(%[src_uv])            \n"
    "lwl             $t3, 15(%[src_uv])            \n"  // V7 | U7 | V6 | U6
    "lwr             $t5, 16(%[src_uv])            \n"
    "lwl             $t5, 19(%[src_uv])            \n"  // V9 | U9 | V8 | U8
    "lwr             $t6, 20(%[src_uv])            \n"
    "lwl             $t6, 23(%[src_uv])            \n"  // V11 | U11 | V10 | U10
    "lwr             $t7, 24(%[src_uv])            \n"
    "lwl             $t7, 27(%[src_uv])            \n"  // V13 | U13 | V12 | U12
    "lwr             $t8, 28(%[src_uv])            \n"
    "lwl             $t8, 31(%[src_uv])            \n"  // V15 | U15 | V14 | U14
    "precrq.qb.ph    $t9, $t1, $t0                 \n"  // V3 | V2 | V1 | V0
    "precr.qb.ph     $t0, $t1, $t0                 \n"  // U3 | U2 | U1 | U0
    "precrq.qb.ph    $t1, $t3, $t2                 \n"  // V7 | V6 | V5 | V4
    "precr.qb.ph     $t2, $t3, $t2                 \n"  // U7 | U6 | U5 | U4
    "precrq.qb.ph    $t3, $t6, $t5                 \n"  // V11 | V10 | V9 | V8
    "precr.qb.ph     $t5, $t6, $t5                 \n"  // U11 | U10 | U9 | U8
    "precrq.qb.ph    $t6, $t8, $t7                 \n"  // V15 | V14 | V13 | V12
    "precr.qb.ph     $t7, $t8, $t7                 \n"  // U15 | U14 | U13 | U12
    "addiu           %[src_uv], %[src_uv], 32      \n"
    "swr             $t9, 0(%[dst_v])              \n"
    "swl             $t9, 3(%[dst_v])              \n"
    "swr             $t0, 0(%[dst_u])              \n"
    "swl             $t0, 3(%[dst_u])              \n"
    "swr             $t1, 4(%[dst_v])              \n"
    "swl             $t1, 7(%[dst_v])              \n"
    "swr             $t2, 4(%[dst_u])              \n"
    "swl             $t2, 7(%[dst_u])              \n"
    "swr             $t3, 8(%[dst_v])              \n"
    "swl             $t3, 11(%[dst_v])             \n"
    "swr             $t5, 8(%[dst_u])              \n"
    "swl             $t5, 11(%[dst_u])             \n"
    "swr             $t6, 12(%[dst_v])             \n"
    "swl             $t6, 15(%[dst_v])             \n"
    "swr             $t7, 12(%[dst_u])             \n"
    "swl             $t7, 15(%[dst_u])             \n"
    "addiu           %[dst_u], %[dst_u], 16        \n"
    "bgtz            $t4, 1b                       \n"
    " addiu          %[dst_v], %[dst_v], 16        \n"

    "beqz            %[width], 3f                  \n"
    " nop                                          \n"

  "2:                                              \n"
    "lbu             $t0, 0(%[src_uv])             \n"
    "lbu             $t1, 1(%[src_uv])             \n"
    "addiu           %[src_uv], %[src_uv], 2       \n"
    "addiu           %[width], %[width], -1        \n"
    "sb              $t0, 0(%[dst_u])              \n"
    "sb              $t1, 0(%[dst_v])              \n"
    "addiu           %[dst_u], %[dst_u], 1         \n"
    "bgtz            %[width], 2b                  \n"
    " addiu          %[dst_v], %[dst_v], 1         \n"

  "3:                                              \n"
    ".set pop                                      \n"
     : [src_uv] "+r" (src_uv),
       [width] "+r" (width),
       [dst_u] "+r" (dst_u),
       [dst_v] "+r" (dst_v)
     :
     : "t0", "t1", "t2", "t3",
       "t4", "t5", "t6", "t7", "t8", "t9"
  );
}
#endif  // HAS_SPLITUV_MIPS_DSPR2

#ifdef HAS_MIRRORROW_MIPS_DSPR2
void MirrorRow_MIPS_DSPR2(const uint8* src, uint8* dst, int width) {
  __asm__ __volatile__ (
      ".set push                             \n"
      ".set noreorder                        \n"

      "srl       $t4, %[width], 4            \n"  // multiplies of 16
      "andi      $t5, %[width], 0xf          \n"
      "blez      $t4, 2f                     \n"
      " addu     %[src], %[src], %[width]    \n"  // src += width

    "1:                                      \n"
      "lw        $t0, -16(%[src])            \n"  // |3|2|1|0|
      "lw        $t1, -12(%[src])            \n"  // |7|6|5|4|
      "lw        $t2, -8(%[src])             \n"  // |11|10|9|8|
      "lw        $t3, -4(%[src])             \n"  // |15|14|13|12|
      "wsbh      $t0, $t0                    \n"  // |2|3|0|1|
      "wsbh      $t1, $t1                    \n"  // |6|7|4|5|
      "wsbh      $t2, $t2                    \n"  // |10|11|8|9|
      "wsbh      $t3, $t3                    \n"  // |14|15|12|13|
      "rotr      $t0, $t0, 16                \n"  // |0|1|2|3|
      "rotr      $t1, $t1, 16                \n"  // |4|5|6|7|
      "rotr      $t2, $t2, 16                \n"  // |8|9|10|11|
      "rotr      $t3, $t3, 16                \n"  // |12|13|14|15|
      "addiu     %[src], %[src], -16         \n"
      "addiu     $t4, $t4, -1                \n"
      "sw        $t3, 0(%[dst])              \n"  // |15|14|13|12|
      "sw        $t2, 4(%[dst])              \n"  // |11|10|9|8|
      "sw        $t1, 8(%[dst])              \n"  // |7|6|5|4|
      "sw        $t0, 12(%[dst])             \n"  // |3|2|1|0|
      "bgtz      $t4, 1b                     \n"
      " addiu    %[dst], %[dst], 16          \n"
      "beqz      $t5, 3f                     \n"
      " nop                                  \n"

    "2:                                      \n"
      "lbu       $t0, -1(%[src])             \n"
      "addiu     $t5, $t5, -1                \n"
      "addiu     %[src], %[src], -1          \n"
      "sb        $t0, 0(%[dst])              \n"
      "bgez      $t5, 2b                     \n"
      " addiu    %[dst], %[dst], 1           \n"

    "3:                                      \n"
      ".set pop                              \n"
      : [src] "+r" (src), [dst] "+r" (dst)
      : [width] "r" (width)
      : "t0", "t1", "t2", "t3", "t4", "t5"
  );
}
#endif  // HAS_MIRRORROW_MIPS_DSPR2

#ifdef HAS_MirrorUVRow_MIPS_DSPR2
void MirrorUVRow_MIPS_DSPR2(const uint8* src_uv, uint8* dst_u, uint8* dst_v,
                            int width) {
  int x = 0;
  int y = 0;
  __asm__ __volatile__ (
      ".set push                                    \n"
      ".set noreorder                               \n"

      "addu            $t4, %[width], %[width]      \n"
      "srl             %[x], %[width], 4            \n"
      "andi            %[y], %[width], 0xf          \n"
      "blez            %[x], 2f                     \n"
      " addu           %[src_uv], %[src_uv], $t4    \n"

    "1:                                             \n"
      "lw              $t0, -32(%[src_uv])          \n"  // |3|2|1|0|
      "lw              $t1, -28(%[src_uv])          \n"  // |7|6|5|4|
      "lw              $t2, -24(%[src_uv])          \n"  // |11|10|9|8|
      "lw              $t3, -20(%[src_uv])          \n"  // |15|14|13|12|
      "lw              $t4, -16(%[src_uv])          \n"  // |19|18|17|16|
      "lw              $t6, -12(%[src_uv])          \n"  // |23|22|21|20|
      "lw              $t7, -8(%[src_uv])           \n"  // |27|26|25|24|
      "lw              $t8, -4(%[src_uv])           \n"  // |31|30|29|28|

      "rotr            $t0, $t0, 16                 \n"  // |1|0|3|2|
      "rotr            $t1, $t1, 16                 \n"  // |5|4|7|6|
      "rotr            $t2, $t2, 16                 \n"  // |9|8|11|10|
      "rotr            $t3, $t3, 16                 \n"  // |13|12|15|14|
      "rotr            $t4, $t4, 16                 \n"  // |17|16|19|18|
      "rotr            $t6, $t6, 16                 \n"  // |21|20|23|22|
      "rotr            $t7, $t7, 16                 \n"  // |25|24|27|26|
      "rotr            $t8, $t8, 16                 \n"  // |29|28|31|30|
      "precr.qb.ph     $t9, $t0, $t1                \n"  // |0|2|4|6|
      "precrq.qb.ph    $t5, $t0, $t1                \n"  // |1|3|5|7|
      "precr.qb.ph     $t0, $t2, $t3                \n"  // |8|10|12|14|
      "precrq.qb.ph    $t1, $t2, $t3                \n"  // |9|11|13|15|
      "precr.qb.ph     $t2, $t4, $t6                \n"  // |16|18|20|22|
      "precrq.qb.ph    $t3, $t4, $t6                \n"  // |17|19|21|23|
      "precr.qb.ph     $t4, $t7, $t8                \n"  // |24|26|28|30|
      "precrq.qb.ph    $t6, $t7, $t8                \n"  // |25|27|29|31|
      "addiu           %[src_uv], %[src_uv], -32    \n"
      "addiu           %[x], %[x], -1               \n"
      "swr             $t4, 0(%[dst_u])             \n"
      "swl             $t4, 3(%[dst_u])             \n"  // |30|28|26|24|
      "swr             $t6, 0(%[dst_v])             \n"
      "swl             $t6, 3(%[dst_v])             \n"  // |31|29|27|25|
      "swr             $t2, 4(%[dst_u])             \n"
      "swl             $t2, 7(%[dst_u])             \n"  // |22|20|18|16|
      "swr             $t3, 4(%[dst_v])             \n"
      "swl             $t3, 7(%[dst_v])             \n"  // |23|21|19|17|
      "swr             $t0, 8(%[dst_u])             \n"
      "swl             $t0, 11(%[dst_u])            \n"  // |14|12|10|8|
      "swr             $t1, 8(%[dst_v])             \n"
      "swl             $t1, 11(%[dst_v])            \n"  // |15|13|11|9|
      "swr             $t9, 12(%[dst_u])            \n"
      "swl             $t9, 15(%[dst_u])            \n"  // |6|4|2|0|
      "swr             $t5, 12(%[dst_v])            \n"
      "swl             $t5, 15(%[dst_v])            \n"  // |7|5|3|1|
      "addiu           %[dst_v], %[dst_v], 16       \n"
      "bgtz            %[x], 1b                     \n"
      " addiu          %[dst_u], %[dst_u], 16       \n"
      "beqz            %[y], 3f                     \n"
      " nop                                         \n"
      "b               2f                           \n"
      " nop                                         \n"

    "2:                                             \n"
      "lbu             $t0, -2(%[src_uv])           \n"
      "lbu             $t1, -1(%[src_uv])           \n"
      "addiu           %[src_uv], %[src_uv], -2     \n"
      "addiu           %[y], %[y], -1               \n"
      "sb              $t0, 0(%[dst_u])             \n"
      "sb              $t1, 0(%[dst_v])             \n"
      "addiu           %[dst_u], %[dst_u], 1        \n"
      "bgtz            %[y], 2b                     \n"
      " addiu          %[dst_v], %[dst_v], 1        \n"

    "3:                                             \n"
      ".set pop                                     \n"
      : [src_uv] "+r" (src_uv),
        [dst_u] "+r" (dst_u),
        [dst_v] "+r" (dst_v),
        [x] "=&r" (x),
        [y] "+r" (y)
      : [width] "r" (width)
      : "t0", "t1", "t2", "t3", "t4",
        "t5", "t7", "t8", "t9"
  );
}
#endif  // HAS_MirrorUVRow_MIPS_DSPR2



// Convert (4 Y and 2 VU) I422 and arrange RGB values into
// t5 = | 0 | B0 | 0 | b0 |
// t4 = | 0 | B1 | 0 | b1 |
// t9 = | 0 | G0 | 0 | g0 |
// t8 = | 0 | G1 | 0 | g1 |
// t2 = | 0 | R0 | 0 | r0 |
// t1 = | 0 | R1 | 0 | r1 |
#define I422ToTransientMipsRGB                                                 \
        "lw                $t0, 0(%[y_buf])       \n"                          \
        "lhu               $t1, 0(%[u_buf])       \n"                          \
        "lhu               $t2, 0(%[v_buf])       \n"                          \
        "preceu.ph.qbr     $t1, $t1               \n"                          \
        "preceu.ph.qbr     $t2, $t2               \n"                          \
        "preceu.ph.qbra    $t3, $t0               \n"                          \
        "preceu.ph.qbla    $t0, $t0               \n"                          \
        "subu.ph           $t1, $t1, $s5          \n"                          \
        "subu.ph           $t2, $t2, $s5          \n"                          \
        "subu.ph           $t3, $t3, $s4          \n"                          \
        "subu.ph           $t0, $t0, $s4          \n"                          \
        "mul.ph            $t3, $t3, $s0          \n"                          \
        "mul.ph            $t0, $t0, $s0          \n"                          \
        "shll.ph           $t4, $t1, 0x7          \n"                          \
        "subu.ph           $t4, $t4, $t1          \n"                          \
        "mul.ph            $t6, $t1, $s1          \n"                          \
        "mul.ph            $t1, $t2, $s2          \n"                          \
        "addq_s.ph         $t5, $t4, $t3          \n"                          \
        "addq_s.ph         $t4, $t4, $t0          \n"                          \
        "shra.ph           $t5, $t5, 6            \n"                          \
        "shra.ph           $t4, $t4, 6            \n"                          \
        "addiu             %[u_buf], 2            \n"                          \
        "addiu             %[v_buf], 2            \n"                          \
        "addu.ph           $t6, $t6, $t1          \n"                          \
        "mul.ph            $t1, $t2, $s3          \n"                          \
        "addu.ph           $t9, $t6, $t3          \n"                          \
        "addu.ph           $t8, $t6, $t0          \n"                          \
        "shra.ph           $t9, $t9, 6            \n"                          \
        "shra.ph           $t8, $t8, 6            \n"                          \
        "addu.ph           $t2, $t1, $t3          \n"                          \
        "addu.ph           $t1, $t1, $t0          \n"                          \
        "shra.ph           $t2, $t2, 6            \n"                          \
        "shra.ph           $t1, $t1, 6            \n"                          \
        "subu.ph           $t5, $t5, $s5          \n"                          \
        "subu.ph           $t4, $t4, $s5          \n"                          \
        "subu.ph           $t9, $t9, $s5          \n"                          \
        "subu.ph           $t8, $t8, $s5          \n"                          \
        "subu.ph           $t2, $t2, $s5          \n"                          \
        "subu.ph           $t1, $t1, $s5          \n"                          \
        "shll_s.ph         $t5, $t5, 8            \n"                          \
        "shll_s.ph         $t4, $t4, 8            \n"                          \
        "shll_s.ph         $t9, $t9, 8            \n"                          \
        "shll_s.ph         $t8, $t8, 8            \n"                          \
        "shll_s.ph         $t2, $t2, 8            \n"                          \
        "shll_s.ph         $t1, $t1, 8            \n"                          \
        "shra.ph           $t5, $t5, 8            \n"                          \
        "shra.ph           $t4, $t4, 8            \n"                          \
        "shra.ph           $t9, $t9, 8            \n"                          \
        "shra.ph           $t8, $t8, 8            \n"                          \
        "shra.ph           $t2, $t2, 8            \n"                          \
        "shra.ph           $t1, $t1, 8            \n"                          \
        "addu.ph           $t5, $t5, $s5          \n"                          \
        "addu.ph           $t4, $t4, $s5          \n"                          \
        "addu.ph           $t9, $t9, $s5          \n"                          \
        "addu.ph           $t8, $t8, $s5          \n"                          \
        "addu.ph           $t2, $t2, $s5          \n"                          \
        "addu.ph           $t1, $t1, $s5          \n"

void I422ToARGBRow_MIPS_DSPR2(const uint8* y_buf,
                              const uint8* u_buf,
                              const uint8* v_buf,
                              uint8* rgb_buf,
                              int width) {
  __asm__ __volatile__ (
      ".set push                                \n"
      ".set noreorder                           \n"
      "beqz              %[width], 2f           \n"
      " repl.ph          $s0, 74                \n"  // |YG|YG| = |74|74|
      "repl.ph           $s1, -25               \n"  // |UG|UG| = |-25|-25|
      "repl.ph           $s2, -52               \n"  // |VG|VG| = |-52|-52|
      "repl.ph           $s3, 102               \n"  // |VR|VR| = |102|102|
      "repl.ph           $s4, 16                \n"  // |0|16|0|16|
      "repl.ph           $s5, 128               \n"  // |128|128| // clipping
      "lui               $s6, 0xff00            \n"
      "ori               $s6, 0xff00            \n"  // |ff|00|ff|00|ff|
    "1:                                         \n"
      I422ToTransientMipsRGB
// Arranging into argb format
      "precr.qb.ph       $t4, $t8, $t4          \n"  // |G1|g1|B1|b1|
      "precr.qb.ph       $t5, $t9, $t5          \n"  // |G0|g0|B0|b0|
      "addiu             %[width], -4           \n"
      "precrq.qb.ph      $t8, $t4, $t5          \n"  // |G1|B1|G0|B0|
      "precr.qb.ph       $t9, $t4, $t5          \n"  // |g1|b1|g0|b0|
      "precr.qb.ph       $t2, $t1, $t2          \n"  // |R1|r1|R0|r0|

      "addiu             %[y_buf], 4            \n"
      "preceu.ph.qbla    $t1, $t2               \n"  // |0 |R1|0 |R0|
      "preceu.ph.qbra    $t2, $t2               \n"  // |0 |r1|0 |r0|
      "or                $t1, $t1, $s6          \n"  // |ff|R1|ff|R0|
      "or                $t2, $t2, $s6          \n"  // |ff|r1|ff|r0|
      "precrq.ph.w       $t0, $t2, $t9          \n"  // |ff|r1|g1|b1|
      "precrq.ph.w       $t3, $t1, $t8          \n"  // |ff|R1|G1|B1|
      "sll               $t9, $t9, 16           \n"
      "sll               $t8, $t8, 16           \n"
      "packrl.ph         $t2, $t2, $t9          \n"  // |ff|r0|g0|b0|
      "packrl.ph         $t1, $t1, $t8          \n"  // |ff|R0|G0|B0|
// Store results.
      "sw                $t2, 0(%[rgb_buf])     \n"
      "sw                $t0, 4(%[rgb_buf])     \n"
      "sw                $t1, 8(%[rgb_buf])     \n"
      "sw                $t3, 12(%[rgb_buf])    \n"
      "bnez              %[width], 1b           \n"
      " addiu            %[rgb_buf], 16         \n"
    "2:                                         \n"
      ".set pop                                 \n"
      :[y_buf] "+r" (y_buf),
       [u_buf] "+r" (u_buf),
       [v_buf] "+r" (v_buf),
       [width] "+r" (width),
       [rgb_buf] "+r" (rgb_buf)
      :
      : "t0", "t1",  "t2", "t3",  "t4", "t5",
        "t6", "t7", "t8", "t9",
        "s0", "s1", "s2", "s3",
        "s4", "s5", "s6"
  );
}

void I422ToABGRRow_MIPS_DSPR2(const uint8* y_buf,
                              const uint8* u_buf,
                              const uint8* v_buf,
                              uint8* rgb_buf,
                              int width) {
  __asm__ __volatile__ (
      ".set push                                \n\t"
      ".set noreorder                           \n\t"
      "beqz              %[width], 2f           \n\t"
      " repl.ph          $s0, 74                \n\t"  // |YG|YG| = |74|74|
      "repl.ph           $s1, -25               \n\t"  // |UG|UG| = |-25|-25|
      "repl.ph           $s2, -52               \n\t"  // |VG|VG| = |-52|-52|
      "repl.ph           $s3, 102               \n\t"  // |VR|VR| = |102|102|
      "repl.ph           $s4, 16                \n\t"  // |0|16|0|16|
      "repl.ph           $s5, 128               \n\t"  // |128|128|
      "lui               $s6, 0xff00            \n\t"
      "ori               $s6, 0xff00            \n\t"  // |ff|00|ff|00|
    "1:                                         \n"
      I422ToTransientMipsRGB
// Arranging into abgr format
      "precr.qb.ph      $t0, $t8, $t1           \n\t"  // |G1|g1|R1|r1|
      "precr.qb.ph      $t3, $t9, $t2           \n\t"  // |G0|g0|R0|r0|
      "precrq.qb.ph     $t8, $t0, $t3           \n\t"  // |G1|R1|G0|R0|
      "precr.qb.ph      $t9, $t0, $t3           \n\t"  // |g1|r1|g0|r0|

      "precr.qb.ph       $t2, $t4, $t5          \n\t"  // |B1|b1|B0|b0|
      "addiu             %[width], -4           \n\t"
      "addiu             %[y_buf], 4            \n\t"
      "preceu.ph.qbla    $t1, $t2               \n\t"  // |0 |B1|0 |B0|
      "preceu.ph.qbra    $t2, $t2               \n\t"  // |0 |b1|0 |b0|
      "or                $t1, $t1, $s6          \n\t"  // |ff|B1|ff|B0|
      "or                $t2, $t2, $s6          \n\t"  // |ff|b1|ff|b0|
      "precrq.ph.w       $t0, $t2, $t9          \n\t"  // |ff|b1|g1|r1|
      "precrq.ph.w       $t3, $t1, $t8          \n\t"  // |ff|B1|G1|R1|
      "sll               $t9, $t9, 16           \n\t"
      "sll               $t8, $t8, 16           \n\t"
      "packrl.ph         $t2, $t2, $t9          \n\t"  // |ff|b0|g0|r0|
      "packrl.ph         $t1, $t1, $t8          \n\t"  // |ff|B0|G0|R0|
// Store results.
      "sw                $t2, 0(%[rgb_buf])     \n\t"
      "sw                $t0, 4(%[rgb_buf])     \n\t"
      "sw                $t1, 8(%[rgb_buf])     \n\t"
      "sw                $t3, 12(%[rgb_buf])    \n\t"
      "bnez              %[width], 1b           \n\t"
      " addiu            %[rgb_buf], 16         \n\t"
    "2:                                         \n\t"
      ".set pop                                 \n\t"
      :[y_buf] "+r" (y_buf),
       [u_buf] "+r" (u_buf),
       [v_buf] "+r" (v_buf),
       [width] "+r" (width),
       [rgb_buf] "+r" (rgb_buf)
      :
      : "t0", "t1",  "t2", "t3",  "t4", "t5",
        "t6", "t7", "t8", "t9",
        "s0", "s1", "s2", "s3",
        "s4", "s5", "s6"
  );
}

void I422ToBGRARow_MIPS_DSPR2(const uint8* y_buf,
                              const uint8* u_buf,
                              const uint8* v_buf,
                              uint8* rgb_buf,
                              int width) {
  __asm__ __volatile__ (
      ".set push                                \n"
      ".set noreorder                           \n"
      "beqz              %[width], 2f           \n"
      " repl.ph          $s0, 74                \n"  // |YG|YG| = |74 |74 |
      "repl.ph           $s1, -25               \n"  // |UG|UG| = |-25|-25|
      "repl.ph           $s2, -52               \n"  // |VG|VG| = |-52|-52|
      "repl.ph           $s3, 102               \n"  // |VR|VR| = |102|102|
      "repl.ph           $s4, 16                \n"  // |0|16|0|16|
      "repl.ph           $s5, 128               \n"  // |128|128|
      "lui               $s6, 0xff              \n"
      "ori               $s6, 0xff              \n"  // |00|ff|00|ff|
    "1:                                         \n"
      I422ToTransientMipsRGB
      // Arranging into bgra format
      "precr.qb.ph      $t4, $t4, $t8           \n"  // |B1|b1|G1|g1|
      "precr.qb.ph      $t5, $t5, $t9           \n"  // |B0|b0|G0|g0|
      "precrq.qb.ph     $t8, $t4, $t5           \n"  // |B1|G1|B0|G0|
      "precr.qb.ph      $t9, $t4, $t5           \n"  // |b1|g1|b0|g0|

      "precr.qb.ph       $t2, $t1, $t2          \n"  // |R1|r1|R0|r0|
      "addiu             %[width], -4           \n"
      "addiu             %[y_buf], 4            \n"
      "preceu.ph.qbla    $t1, $t2               \n"  // |0 |R1|0 |R0|
      "preceu.ph.qbra    $t2, $t2               \n"  // |0 |r1|0 |r0|
      "sll               $t1, $t1, 8            \n"  // |R1|0 |R0|0 |
      "sll               $t2, $t2, 8            \n"  // |r1|0 |r0|0 |
      "or                $t1, $t1, $s6          \n"  // |R1|ff|R0|ff|
      "or                $t2, $t2, $s6          \n"  // |r1|ff|r0|ff|
      "precrq.ph.w       $t0, $t9, $t2          \n"  // |b1|g1|r1|ff|
      "precrq.ph.w       $t3, $t8, $t1          \n"  // |B1|G1|R1|ff|
      "sll               $t1, $t1, 16           \n"
      "sll               $t2, $t2, 16           \n"
      "packrl.ph         $t2, $t9, $t2          \n"  // |b0|g0|r0|ff|
      "packrl.ph         $t1, $t8, $t1          \n"  // |B0|G0|R0|ff|
// Store results.
      "sw                $t2, 0(%[rgb_buf])     \n"
      "sw                $t0, 4(%[rgb_buf])     \n"
      "sw                $t1, 8(%[rgb_buf])     \n"
      "sw                $t3, 12(%[rgb_buf])    \n"
      "bnez              %[width], 1b           \n"
      " addiu            %[rgb_buf], 16         \n"
    "2:                                         \n"
      ".set pop                                 \n"
      :[y_buf] "+r" (y_buf),
       [u_buf] "+r" (u_buf),
       [v_buf] "+r" (v_buf),
       [width] "+r" (width),
       [rgb_buf] "+r" (rgb_buf)
      :
      : "t0", "t1",  "t2", "t3",  "t4", "t5",
        "t6", "t7", "t8", "t9",
        "s0", "s1", "s2", "s3",
        "s4", "s5", "s6"
  );
}

#endif  // __mips__

#ifdef __cplusplus
}  // extern "C"
}  // namespace libyuv
#endif

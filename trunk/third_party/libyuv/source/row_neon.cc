/*
 *  Copyright 2011 The LibYuv Project Authors. All rights reserved.
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

// This module is for GCC Neon
#if !defined(YUV_DISABLE_ASM) && defined(__ARM_NEON__)

// Read 8 Y, 4 U and 4 V from 422
#define READYUV422                                                             \
    "vld1.u8    {d0}, [%0]!                    \n"                             \
    "vld1.u32   {d2[0]}, [%1]!                 \n"                             \
    "vld1.u32   {d2[1]}, [%2]!                 \n"

// Read 8 Y, 2 U and 2 V from 422
#define READYUV411                                                             \
    "vld1.u8    {d0}, [%0]!                    \n"                             \
    "vld1.u16   {d2[0]}, [%1]!                 \n"                             \
    "vld1.u16   {d2[1]}, [%2]!                 \n"                             \
    "vmov.u8    d3, d2                         \n"                             \
    "vzip.u8    d2, d3                         \n"

// Read 8 Y, 8 U and 8 V from 444
#define READYUV444                                                             \
    "vld1.u8    {d0}, [%0]!                    \n"                             \
    "vld1.u8    {d2}, [%1]!                    \n"                             \
    "vld1.u8    {d3}, [%2]!                    \n"                             \
    "vpaddl.u8  q1, q1                         \n"                             \
    "vrshrn.u16 d2, q1, #1                     \n"

// Read 8 Y, and set 4 U and 4 V to 128
#define READYUV400                                                             \
    "vld1.u8    {d0}, [%0]!                    \n"                             \
    "vmov.u8    d2, #128                       \n"

// Read 8 Y and 4 UV from NV12
#define READNV12                                                               \
    "vld1.u8    {d0}, [%0]!                    \n"                             \
    "vld1.u8    {d2}, [%1]!                    \n"                             \
    "vmov.u8    d3, d2                         \n"/* split odd/even uv apart */\
    "vuzp.u8    d2, d3                         \n"                             \
    "vtrn.u32   d2, d3                         \n"

// Read 8 Y and 4 VU from NV21
#define READNV21                                                               \
    "vld1.u8    {d0}, [%0]!                    \n"                             \
    "vld1.u8    {d2}, [%1]!                    \n"                             \
    "vmov.u8    d3, d2                         \n"/* split odd/even uv apart */\
    "vuzp.u8    d3, d2                         \n"                             \
    "vtrn.u32   d2, d3                         \n"

// Read 8 YUY2
#define READYUY2                                                               \
    "vld2.u8    {d0, d2}, [%0]!                \n"                             \
    "vmov.u8    d3, d2                         \n"                             \
    "vuzp.u8    d2, d3                         \n"                             \
    "vtrn.u32   d2, d3                         \n"

// Read 8 UYVY
#define READUYVY                                                               \
    "vld2.u8    {d2, d3}, [%0]!                \n"                             \
    "vmov.u8    d0, d3                         \n"                             \
    "vmov.u8    d3, d2                         \n"                             \
    "vuzp.u8    d2, d3                         \n"                             \
    "vtrn.u32   d2, d3                         \n"

#define YUV422TORGB                                                            \
    "veor.u8    d2, d26                        \n"/*subtract 128 from u and v*/\
    "vmull.s8   q8, d2, d24                    \n"/*  u/v B/R component      */\
    "vmull.s8   q9, d2, d25                    \n"/*  u/v G component        */\
    "vmov.u8    d1, #0                         \n"/*  split odd/even y apart */\
    "vtrn.u8    d0, d1                         \n"                             \
    "vsub.s16   q0, q0, q15                    \n"/*  offset y               */\
    "vmul.s16   q0, q0, q14                    \n"                             \
    "vadd.s16   d18, d19                       \n"                             \
    "vqadd.s16  d20, d0, d16                   \n" /* B */                     \
    "vqadd.s16  d21, d1, d16                   \n"                             \
    "vqadd.s16  d22, d0, d17                   \n" /* R */                     \
    "vqadd.s16  d23, d1, d17                   \n"                             \
    "vqadd.s16  d16, d0, d18                   \n" /* G */                     \
    "vqadd.s16  d17, d1, d18                   \n"                             \
    "vqshrun.s16 d0, q10, #6                   \n" /* B */                     \
    "vqshrun.s16 d1, q11, #6                   \n" /* G */                     \
    "vqshrun.s16 d2, q8, #6                    \n" /* R */                     \
    "vmovl.u8   q10, d0                        \n"/*  set up for reinterleave*/\
    "vmovl.u8   q11, d1                        \n"                             \
    "vmovl.u8   q8, d2                         \n"                             \
    "vtrn.u8    d20, d21                       \n"                             \
    "vtrn.u8    d22, d23                       \n"                             \
    "vtrn.u8    d16, d17                       \n"                             \
    "vmov.u8    d21, d16                       \n"

#if defined(HAS_I422TOARGBROW_NEON) || defined(HAS_I422TOBGRAROW_NEON) ||      \
    defined(HAS_I422TOABGRROW_NEON) || defined(HAS_I422TORGBAROW_NEON)
static const vec8 kUVToRB  = { 127, 127, 127, 127, 102, 102, 102, 102,
                               0, 0, 0, 0, 0, 0, 0, 0 };
static const vec8 kUVToG = { -25, -25, -25, -25, -52, -52, -52, -52,
                             0, 0, 0, 0, 0, 0, 0, 0 };
#endif

#ifdef HAS_I444TOARGBROW_NEON
void I444ToARGBRow_NEON(const uint8* src_y,
                        const uint8* src_u,
                        const uint8* src_v,
                        uint8* dst_argb,
                        int width) {
  asm volatile (
    "vld1.u8    {d24}, [%5]                    \n"
    "vld1.u8    {d25}, [%6]                    \n"
    "vmov.u8    d26, #128                      \n"
    "vmov.u16   q14, #74                       \n"
    "vmov.u16   q15, #16                       \n"
    ".p2align  2                               \n"
  "1:                                          \n"
    READYUV444
    YUV422TORGB
    "subs       %4, %4, #8                     \n"
    "vmov.u8    d23, #255                      \n"
    "vst4.8     {d20, d21, d22, d23}, [%3]!    \n"
    "bgt        1b                             \n"
    : "+r"(src_y),     // %0
      "+r"(src_u),     // %1
      "+r"(src_v),     // %2
      "+r"(dst_argb),  // %3
      "+r"(width)      // %4
    : "r"(&kUVToRB),   // %5
      "r"(&kUVToG)     // %6
    : "cc", "memory", "q0", "q1", "q2", "q3",
      "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
  );
}
#endif  // HAS_I444TOARGBROW_NEON

#ifdef HAS_I422TOARGBROW_NEON
void I422ToARGBRow_NEON(const uint8* src_y,
                        const uint8* src_u,
                        const uint8* src_v,
                        uint8* dst_argb,
                        int width) {
  asm volatile (
    "vld1.u8    {d24}, [%5]                    \n"
    "vld1.u8    {d25}, [%6]                    \n"
    "vmov.u8    d26, #128                      \n"
    "vmov.u16   q14, #74                       \n"
    "vmov.u16   q15, #16                       \n"
    ".p2align  2                               \n"
  "1:                                          \n"
    READYUV422
    YUV422TORGB
    "subs       %4, %4, #8                     \n"
    "vmov.u8    d23, #255                      \n"
    "vst4.8     {d20, d21, d22, d23}, [%3]!    \n"
    "bgt        1b                             \n"
    : "+r"(src_y),     // %0
      "+r"(src_u),     // %1
      "+r"(src_v),     // %2
      "+r"(dst_argb),  // %3
      "+r"(width)      // %4
    : "r"(&kUVToRB),   // %5
      "r"(&kUVToG)     // %6
    : "cc", "memory", "q0", "q1", "q2", "q3",
      "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
  );
}
#endif  // HAS_I422TOARGBROW_NEON

#ifdef HAS_I411TOARGBROW_NEON
void I411ToARGBRow_NEON(const uint8* src_y,
                        const uint8* src_u,
                        const uint8* src_v,
                        uint8* dst_argb,
                        int width) {
  asm volatile (
    "vld1.u8    {d24}, [%5]                    \n"
    "vld1.u8    {d25}, [%6]                    \n"
    "vmov.u8    d26, #128                      \n"
    "vmov.u16   q14, #74                       \n"
    "vmov.u16   q15, #16                       \n"
    ".p2align  2                               \n"
  "1:                                          \n"
    READYUV411
    YUV422TORGB
    "subs       %4, %4, #8                     \n"
    "vmov.u8    d23, #255                      \n"
    "vst4.8     {d20, d21, d22, d23}, [%3]!    \n"
    "bgt        1b                             \n"
    : "+r"(src_y),     // %0
      "+r"(src_u),     // %1
      "+r"(src_v),     // %2
      "+r"(dst_argb),  // %3
      "+r"(width)      // %4
    : "r"(&kUVToRB),   // %5
      "r"(&kUVToG)     // %6
    : "cc", "memory", "q0", "q1", "q2", "q3",
      "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
  );
}
#endif  // HAS_I411TOARGBROW_NEON

#ifdef HAS_I422TOBGRAROW_NEON
void I422ToBGRARow_NEON(const uint8* src_y,
                        const uint8* src_u,
                        const uint8* src_v,
                        uint8* dst_bgra,
                        int width) {
  asm volatile (
    "vld1.u8    {d24}, [%5]                    \n"
    "vld1.u8    {d25}, [%6]                    \n"
    "vmov.u8    d26, #128                      \n"
    "vmov.u16   q14, #74                       \n"
    "vmov.u16   q15, #16                       \n"
    ".p2align  2                               \n"
  "1:                                          \n"
    READYUV422
    YUV422TORGB
    "subs       %4, %4, #8                     \n"
    "vswp.u8    d20, d22                       \n"
    "vmov.u8    d19, #255                      \n"
    "vst4.8     {d19, d20, d21, d22}, [%3]!    \n"
    "bgt        1b                             \n"
    : "+r"(src_y),     // %0
      "+r"(src_u),     // %1
      "+r"(src_v),     // %2
      "+r"(dst_bgra),  // %3
      "+r"(width)      // %4
    : "r"(&kUVToRB),   // %5
      "r"(&kUVToG)     // %6
    : "cc", "memory", "q0", "q1", "q2", "q3",
      "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
  );
}
#endif  // HAS_I422TOBGRAROW_NEON

#ifdef HAS_I422TOABGRROW_NEON
void I422ToABGRRow_NEON(const uint8* src_y,
                        const uint8* src_u,
                        const uint8* src_v,
                        uint8* dst_abgr,
                        int width) {
  asm volatile (
    "vld1.u8    {d24}, [%5]                    \n"
    "vld1.u8    {d25}, [%6]                    \n"
    "vmov.u8    d26, #128                      \n"
    "vmov.u16   q14, #74                       \n"
    "vmov.u16   q15, #16                       \n"
    ".p2align  2                               \n"
  "1:                                          \n"
    READYUV422
    YUV422TORGB
    "subs       %4, %4, #8                     \n"
    "vswp.u8    d20, d22                       \n"
    "vmov.u8    d23, #255                      \n"
    "vst4.8     {d20, d21, d22, d23}, [%3]!    \n"
    "bgt        1b                             \n"
    : "+r"(src_y),     // %0
      "+r"(src_u),     // %1
      "+r"(src_v),     // %2
      "+r"(dst_abgr),  // %3
      "+r"(width)      // %4
    : "r"(&kUVToRB),   // %5
      "r"(&kUVToG)     // %6
    : "cc", "memory", "q0", "q1", "q2", "q3",
      "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
  );
}
#endif  // HAS_I422TOABGRROW_NEON

#ifdef HAS_I422TORGBAROW_NEON
void I422ToRGBARow_NEON(const uint8* src_y,
                        const uint8* src_u,
                        const uint8* src_v,
                        uint8* dst_rgba,
                        int width) {
  asm volatile (
    "vld1.u8    {d24}, [%5]                    \n"
    "vld1.u8    {d25}, [%6]                    \n"
    "vmov.u8    d26, #128                      \n"
    "vmov.u16   q14, #74                       \n"
    "vmov.u16   q15, #16                       \n"
    ".p2align  2                               \n"
  "1:                                          \n"
    READYUV422
    YUV422TORGB
    "subs       %4, %4, #8                     \n"
    "vmov.u8    d19, #255                      \n"
    "vst4.8     {d19, d20, d21, d22}, [%3]!    \n"
    "bgt        1b                             \n"
    : "+r"(src_y),     // %0
      "+r"(src_u),     // %1
      "+r"(src_v),     // %2
      "+r"(dst_rgba),  // %3
      "+r"(width)      // %4
    : "r"(&kUVToRB),   // %5
      "r"(&kUVToG)     // %6
    : "cc", "memory", "q0", "q1", "q2", "q3",
      "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
  );
}
#endif  // HAS_I422TORGBAROW_NEON

#ifdef HAS_I422TORGB24ROW_NEON
void I422ToRGB24Row_NEON(const uint8* src_y,
                        const uint8* src_u,
                        const uint8* src_v,
                        uint8* dst_rgb24,
                        int width) {
  asm volatile (
    "vld1.u8    {d24}, [%5]                    \n"
    "vld1.u8    {d25}, [%6]                    \n"
    "vmov.u8    d26, #128                      \n"
    "vmov.u16   q14, #74                       \n"
    "vmov.u16   q15, #16                       \n"
    ".p2align  2                               \n"
  "1:                                          \n"
    READYUV422
    YUV422TORGB
    "subs       %4, %4, #8                     \n"
    "vst3.8     {d20, d21, d22}, [%3]!         \n"
    "bgt        1b                             \n"
    : "+r"(src_y),      // %0
      "+r"(src_u),      // %1
      "+r"(src_v),      // %2
      "+r"(dst_rgb24),  // %3
      "+r"(width)       // %4
    : "r"(&kUVToRB),    // %5
      "r"(&kUVToG)      // %6
    : "cc", "memory", "q0", "q1", "q2", "q3",
      "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
  );
}
#endif  // HAS_I422TORGB24ROW_NEON

#ifdef HAS_I422TORAWROW_NEON
void I422ToRAWRow_NEON(const uint8* src_y,
                       const uint8* src_u,
                       const uint8* src_v,
                       uint8* dst_raw,
                       int width) {
  asm volatile (
    "vld1.u8    {d24}, [%5]                    \n"
    "vld1.u8    {d25}, [%6]                    \n"
    "vmov.u8    d26, #128                      \n"
    "vmov.u16   q14, #74                       \n"
    "vmov.u16   q15, #16                       \n"
    ".p2align  2                               \n"
  "1:                                          \n"
    READYUV422
    YUV422TORGB
    "subs       %4, %4, #8                     \n"
    "vswp.u8    d20, d22                       \n"
    "vst3.8     {d20, d21, d22}, [%3]!         \n"
    "bgt        1b                             \n"
    : "+r"(src_y),    // %0
      "+r"(src_u),    // %1
      "+r"(src_v),    // %2
      "+r"(dst_raw),  // %3
      "+r"(width)     // %4
    : "r"(&kUVToRB),  // %5
      "r"(&kUVToG)    // %6
    : "cc", "memory", "q0", "q1", "q2", "q3",
      "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
  );
}
#endif  // HAS_I422TORAWROW_NEON

#ifdef HAS_I422TORGB565ROW_NEON
#define ARGBTORGB565                                                           \
    "vshr.u8    d20, d20, #3                   \n"  /* B                    */ \
    "vshr.u8    d21, d21, #2                   \n"  /* G                    */ \
    "vshr.u8    d22, d22, #3                   \n"  /* R                    */ \
    "vmovl.u8   q8, d20                        \n"  /* B                    */ \
    "vmovl.u8   q9, d21                        \n"  /* G                    */ \
    "vmovl.u8   q10, d22                       \n"  /* R                    */ \
    "vshl.u16   q9, q9, #5                     \n"  /* G                    */ \
    "vshl.u16   q10, q10, #11                  \n"  /* R                    */ \
    "vorr       q0, q8, q9                     \n"  /* BG                   */ \
    "vorr       q0, q0, q10                    \n"  /* BGR                  */

void I422ToRGB565Row_NEON(const uint8* src_y,
                       const uint8* src_u,
                       const uint8* src_v,
                       uint8* dst_rgb565,
                       int width) {
  asm volatile (
    "vld1.u8    {d24}, [%5]                    \n"
    "vld1.u8    {d25}, [%6]                    \n"
    "vmov.u8    d26, #128                      \n"
    "vmov.u16   q14, #74                       \n"
    "vmov.u16   q15, #16                       \n"
    ".p2align  2                               \n"
  "1:                                          \n"
    READYUV422
    YUV422TORGB
    "subs       %4, %4, #8                     \n"
    ARGBTORGB565
    "vst1.8     {q0}, [%3]!                    \n"  // store 8 pixels RGB565.
    "bgt        1b                             \n"
    : "+r"(src_y),    // %0
      "+r"(src_u),    // %1
      "+r"(src_v),    // %2
      "+r"(dst_rgb565),  // %3
      "+r"(width)     // %4
    : "r"(&kUVToRB),  // %5
      "r"(&kUVToG)    // %6
    : "cc", "memory", "q0", "q1", "q2", "q3",
      "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
  );
}
#endif  // HAS_I422TORGB565ROW_NEON

#ifdef HAS_I422TOARGB1555ROW_NEON
#define ARGBTOARGB1555                                                         \
    "vshr.u8    q10, q10, #3                   \n"  /* B                    */ \
    "vshr.u8    d22, d22, #3                   \n"  /* R                    */ \
    "vshr.u8    d23, d23, #7                   \n"  /* A                    */ \
    "vmovl.u8   q8, d20                        \n"  /* B                    */ \
    "vmovl.u8   q9, d21                        \n"  /* G                    */ \
    "vmovl.u8   q10, d22                       \n"  /* R                    */ \
    "vmovl.u8   q11, d23                       \n"  /* A                    */ \
    "vshl.u16   q9, q9, #5                     \n"  /* G                    */ \
    "vshl.u16   q10, q10, #10                  \n"  /* R                    */ \
    "vshl.u16   q11, q11, #15                  \n"  /* A                    */ \
    "vorr       q0, q8, q9                     \n"  /* BG                   */ \
    "vorr       q1, q10, q11                   \n"  /* RA                   */ \
    "vorr       q0, q0, q1                     \n"  /* BGRA                 */

void I422ToARGB1555Row_NEON(const uint8* src_y,
                       const uint8* src_u,
                       const uint8* src_v,
                       uint8* dst_argb1555,
                       int width) {
  asm volatile (
    "vld1.u8    {d24}, [%5]                    \n"
    "vld1.u8    {d25}, [%6]                    \n"
    "vmov.u8    d26, #128                      \n"
    "vmov.u16   q14, #74                       \n"
    "vmov.u16   q15, #16                       \n"
    ".p2align  2                               \n"
  "1:                                          \n"
    READYUV422
    YUV422TORGB
    "subs       %4, %4, #8                     \n"
    "vmov.u8    d23, #255                      \n"
    ARGBTOARGB1555
    "vst1.8     {q0}, [%3]!                    \n"  // store 8 pixels ARGB1555.
    "bgt        1b                             \n"
    : "+r"(src_y),    // %0
      "+r"(src_u),    // %1
      "+r"(src_v),    // %2
      "+r"(dst_argb1555),  // %3
      "+r"(width)     // %4
    : "r"(&kUVToRB),  // %5
      "r"(&kUVToG)    // %6
    : "cc", "memory", "q0", "q1", "q2", "q3",
      "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
  );
}
#endif  // HAS_I422TOARGB1555ROW_NEON

#ifdef HAS_I422TOARGB4444ROW_NEON
#define ARGBTOARGB4444                                                         \
    "vshr.u8    d20, d20, #4                   \n"  /* B                    */ \
    "vbic.32    d21, d21, d4                   \n"  /* G                    */ \
    "vshr.u8    d22, d22, #4                   \n"  /* R                    */ \
    "vbic.32    d23, d23, d4                   \n"  /* A                    */ \
    "vorr       d0, d20, d21                   \n"  /* BG                   */ \
    "vorr       d1, d22, d23                   \n"  /* RA                   */ \
    "vzip.u8    d0, d1                         \n"  /* BGRA                 */

void I422ToARGB4444Row_NEON(const uint8* src_y,
                       const uint8* src_u,
                       const uint8* src_v,
                       uint8* dst_argb4444,
                       int width) {
  asm volatile (
    "vld1.u8    {d24}, [%5]                    \n"
    "vld1.u8    {d25}, [%6]                    \n"
    "vmov.u8    d26, #128                      \n"
    "vmov.u16   q14, #74                       \n"
    "vmov.u16   q15, #16                       \n"
    "vmov.u8    d4, #0x0f                      \n"  // bits to clear with vbic.
    ".p2align  2                               \n"
  "1:                                          \n"
    READYUV422
    YUV422TORGB
    "subs       %4, %4, #8                     \n"
    "vmov.u8    d23, #255                      \n"
    ARGBTOARGB4444
    "vst1.8     {q0}, [%3]!                    \n"  // store 8 pixels ARGB4444.
    "bgt        1b                             \n"
    : "+r"(src_y),    // %0
      "+r"(src_u),    // %1
      "+r"(src_v),    // %2
      "+r"(dst_argb4444),  // %3
      "+r"(width)     // %4
    : "r"(&kUVToRB),  // %5
      "r"(&kUVToG)    // %6
    : "cc", "memory", "q0", "q1", "q2", "q3",
      "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
  );
}
#endif  // HAS_I422TOARGB4444ROW_NEON

#ifdef HAS_YTOARGBROW_NEON
void YToARGBRow_NEON(const uint8* src_y,
                     uint8* dst_argb,
                     int width) {
  asm volatile (
    "vld1.u8    {d24}, [%3]                    \n"
    "vld1.u8    {d25}, [%4]                    \n"
    "vmov.u8    d26, #128                      \n"
    "vmov.u16   q14, #74                       \n"
    "vmov.u16   q15, #16                       \n"
    ".p2align  2                               \n"
  "1:                                          \n"
    READYUV400
    YUV422TORGB
    "subs       %2, %2, #8                     \n"
    "vmov.u8    d23, #255                      \n"
    "vst4.8     {d20, d21, d22, d23}, [%1]!    \n"
    "bgt        1b                             \n"
    : "+r"(src_y),     // %0
      "+r"(dst_argb),  // %1
      "+r"(width)      // %2
    : "r"(&kUVToRB),   // %3
      "r"(&kUVToG)     // %4
    : "cc", "memory", "q0", "q1", "q2", "q3",
      "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
  );
}
#endif  // HAS_YTOARGBROW_NEON

#ifdef HAS_I400TOARGBROW_NEON
void I400ToARGBRow_NEON(const uint8* src_y,
                        uint8* dst_argb,
                        int width) {
  asm volatile (
    ".p2align  2                               \n"
    "vmov.u8    d23, #255                      \n"
  "1:                                          \n"
    "vld1.u8    {d20}, [%0]!                   \n"
    "vmov       d21, d20                       \n"
    "vmov       d22, d20                       \n"
    "subs       %2, %2, #8                     \n"
    "vst4.8     {d20, d21, d22, d23}, [%1]!    \n"
    "bgt        1b                             \n"
    : "+r"(src_y),     // %0
      "+r"(dst_argb),  // %1
      "+r"(width)      // %2
    :
    : "cc", "memory", "d20", "d21", "d22", "d23"
  );
}
#endif  // HAS_I400TOARGBROW_NEON

#ifdef HAS_NV12TOARGBROW_NEON
void NV12ToARGBRow_NEON(const uint8* src_y,
                        const uint8* src_uv,
                        uint8* dst_argb,
                        int width) {
  asm volatile (
    "vld1.u8    {d24}, [%4]                    \n"
    "vld1.u8    {d25}, [%5]                    \n"
    "vmov.u8    d26, #128                      \n"
    "vmov.u16   q14, #74                       \n"
    "vmov.u16   q15, #16                       \n"
    ".p2align  2                               \n"
  "1:                                          \n"
    READNV12
    YUV422TORGB
    "subs       %3, %3, #8                     \n"
    "vmov.u8    d23, #255                      \n"
    "vst4.8     {d20, d21, d22, d23}, [%2]!    \n"
    "bgt        1b                             \n"
    : "+r"(src_y),     // %0
      "+r"(src_uv),    // %1
      "+r"(dst_argb),  // %2
      "+r"(width)      // %3
    : "r"(&kUVToRB),   // %4
      "r"(&kUVToG)     // %5
    : "cc", "memory", "q0", "q1", "q2", "q3",
      "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
  );
}
#endif  // HAS_NV12TOARGBROW_NEON

#ifdef HAS_NV21TOARGBROW_NEON
void NV21ToARGBRow_NEON(const uint8* src_y,
                        const uint8* src_uv,
                        uint8* dst_argb,
                        int width) {
  asm volatile (
    "vld1.u8    {d24}, [%4]                    \n"
    "vld1.u8    {d25}, [%5]                    \n"
    "vmov.u8    d26, #128                      \n"
    "vmov.u16   q14, #74                       \n"
    "vmov.u16   q15, #16                       \n"
    ".p2align  2                               \n"
  "1:                                          \n"
    READNV21
    YUV422TORGB
    "subs       %3, %3, #8                     \n"
    "vmov.u8    d23, #255                      \n"
    "vst4.8     {d20, d21, d22, d23}, [%2]!    \n"
    "bgt        1b                             \n"
    : "+r"(src_y),     // %0
      "+r"(src_uv),    // %1
      "+r"(dst_argb),  // %2
      "+r"(width)      // %3
    : "r"(&kUVToRB),   // %4
      "r"(&kUVToG)     // %5
    : "cc", "memory", "q0", "q1", "q2", "q3",
      "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
  );
}
#endif  // HAS_NV21TOARGBROW_NEON

#ifdef HAS_NV12TORGB565ROW_NEON
void NV12ToRGB565Row_NEON(const uint8* src_y,
                          const uint8* src_uv,
                          uint8* dst_rgb565,
                          int width) {
  asm volatile (
    "vld1.u8    {d24}, [%4]                    \n"
    "vld1.u8    {d25}, [%5]                    \n"
    "vmov.u8    d26, #128                      \n"
    "vmov.u16   q14, #74                       \n"
    "vmov.u16   q15, #16                       \n"
    ".p2align  2                               \n"
  "1:                                          \n"
    READNV12
    YUV422TORGB
    "subs       %3, %3, #8                     \n"
    ARGBTORGB565
    "vst1.8     {q0}, [%2]!                    \n"  // store 8 pixels RGB565.
    "bgt        1b                             \n"
    : "+r"(src_y),     // %0
      "+r"(src_uv),    // %1
      "+r"(dst_rgb565),  // %2
      "+r"(width)      // %3
    : "r"(&kUVToRB),   // %4
      "r"(&kUVToG)     // %5
    : "cc", "memory", "q0", "q1", "q2", "q3",
      "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
  );
}
#endif  // HAS_NV12TORGB565ROW_NEON

#ifdef HAS_NV21TORGB565ROW_NEON
void NV21ToRGB565Row_NEON(const uint8* src_y,
                          const uint8* src_uv,
                          uint8* dst_rgb565,
                          int width) {
  asm volatile (
    "vld1.u8    {d24}, [%4]                    \n"
    "vld1.u8    {d25}, [%5]                    \n"
    "vmov.u8    d26, #128                      \n"
    "vmov.u16   q14, #74                       \n"
    "vmov.u16   q15, #16                       \n"
    ".p2align  2                               \n"
  "1:                                          \n"
    READNV21
    YUV422TORGB
    "subs       %3, %3, #8                     \n"
    ARGBTORGB565
    "vst1.8     {q0}, [%2]!                    \n"  // store 8 pixels RGB565.
    "bgt        1b                             \n"
    : "+r"(src_y),     // %0
      "+r"(src_uv),    // %1
      "+r"(dst_rgb565),  // %2
      "+r"(width)      // %3
    : "r"(&kUVToRB),   // %4
      "r"(&kUVToG)     // %5
    : "cc", "memory", "q0", "q1", "q2", "q3",
      "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
  );
}
#endif  // HAS_NV21TORGB565ROW_NEON

#ifdef HAS_YUY2TOARGBROW_NEON
void YUY2ToARGBRow_NEON(const uint8* src_yuy2,
                        uint8* dst_argb,
                        int width) {
  asm volatile (
    "vld1.u8    {d24}, [%3]                    \n"
    "vld1.u8    {d25}, [%4]                    \n"
    "vmov.u8    d26, #128                      \n"
    "vmov.u16   q14, #74                       \n"
    "vmov.u16   q15, #16                       \n"
    ".p2align  2                               \n"
  "1:                                          \n"
    READYUY2
    YUV422TORGB
    "subs       %2, %2, #8                     \n"
    "vmov.u8    d23, #255                      \n"
    "vst4.8     {d20, d21, d22, d23}, [%1]!    \n"
    "bgt        1b                             \n"
    : "+r"(src_yuy2),  // %0
      "+r"(dst_argb),  // %1
      "+r"(width)      // %2
    : "r"(&kUVToRB),   // %3
      "r"(&kUVToG)     // %4
    : "cc", "memory", "q0", "q1", "q2", "q3",
      "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
  );
}
#endif  // HAS_YUY2TOARGBROW_NEON

#ifdef HAS_UYVYTOARGBROW_NEON
void UYVYToARGBRow_NEON(const uint8* src_uyvy,
                        uint8* dst_argb,
                        int width) {
  asm volatile (
    "vld1.u8    {d24}, [%3]                    \n"
    "vld1.u8    {d25}, [%4]                    \n"
    "vmov.u8    d26, #128                      \n"
    "vmov.u16   q14, #74                       \n"
    "vmov.u16   q15, #16                       \n"
    ".p2align  2                               \n"
  "1:                                          \n"
    READUYVY
    YUV422TORGB
    "subs       %2, %2, #8                     \n"
    "vmov.u8    d23, #255                      \n"
    "vst4.8     {d20, d21, d22, d23}, [%1]!    \n"
    "bgt        1b                             \n"
    : "+r"(src_uyvy),  // %0
      "+r"(dst_argb),  // %1
      "+r"(width)      // %2
    : "r"(&kUVToRB),   // %3
      "r"(&kUVToG)     // %4
    : "cc", "memory", "q0", "q1", "q2", "q3",
      "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
  );
}
#endif  // HAS_UYVYTOARGBROW_NEON

#ifdef HAS_SPLITUV_NEON
// Reads 16 pairs of UV and write even values to dst_u and odd to dst_v
// Alignment requirement: 16 bytes for pointers, and multiple of 16 pixels.
void SplitUV_NEON(const uint8* src_uv, uint8* dst_u, uint8* dst_v, int width) {
  asm volatile (
    ".p2align  2                               \n"
  "1:                                          \n"
    "vld2.u8    {q0, q1}, [%0:128]!            \n"  // load 16 pairs of UV
    "subs       %3, %3, #16                    \n"  // 16 processed per loop
    "vst1.u8    {q0}, [%1:128]!                \n"  // store U
    "vst1.u8    {q1}, [%2:128]!                \n"  // store V
    "bgt        1b                             \n"
    : "+r"(src_uv),  // %0
      "+r"(dst_u),   // %1
      "+r"(dst_v),   // %2
      "+r"(width)    // %3  // Output registers
    :                       // Input registers
    : "memory", "cc", "q0", "q1"  // Clobber List
  );
}

// Reads 16 pairs of UV and write even values to dst_u and odd to dst_v
// Alignment requirement: Multiple of 16 pixels, pointers unaligned.
void SplitUV_Unaligned_NEON(const uint8* src_uv, uint8* dst_u, uint8* dst_v,
                            int width) {
  asm volatile (
    ".p2align  2                               \n"
  "1:                                          \n"
    "vld2.u8    {q0, q1}, [%0]!                \n"  // load 16 pairs of UV
    "subs       %3, %3, #16                    \n"  // 16 processed per loop
    "vst1.u8    {q0}, [%1]!                    \n"  // store U
    "vst1.u8    {q1}, [%2]!                    \n"  // store V
    "bgt        1b                             \n"
    : "+r"(src_uv),  // %0
      "+r"(dst_u),   // %1
      "+r"(dst_v),   // %2
      "+r"(width)    // %3  // Output registers
    :                       // Input registers
    : "memory", "cc", "q0", "q1"  // Clobber List
  );
}
#endif  // HAS_SPLITUV_NEON

#ifdef HAS_MERGEUV_NEON
// Reads 16 U's and V's and writes out 16 pairs of UV.
// Alignment requirement: 16 bytes for pointers, and multiple of 16 pixels.
void MergeUV_NEON(const uint8* src_u, const uint8* src_v, uint8* dst_uv,
                  int width) {
  asm volatile (
    ".p2align  2                               \n"
  "1:                                          \n"
    "vld1.u8    {q0}, [%0:128]!                \n"  // load U
    "vld1.u8    {q1}, [%1:128]!                \n"  // load V
    "subs       %3, %3, #16                    \n"  // 16 processed per loop
    "vst2.u8    {q0, q1}, [%2:128]!            \n"  // store 16 pairs of UV
    "bgt        1b                             \n"
    :
      "+r"(src_u),   // %0
      "+r"(src_v),   // %1
      "+r"(dst_uv),  // %2
      "+r"(width)    // %3  // Output registers
    :                       // Input registers
    : "memory", "cc", "q0", "q1"  // Clobber List
  );
}

// Reads 16 U's and V's and writes out 16 pairs of UV.
void MergeUV_Unaligned_NEON(const uint8* src_u, const uint8* src_v,
                            uint8* dst_uv, int width) {
  asm volatile (
    ".p2align  2                               \n"
  "1:                                          \n"
    "vld1.u8    {q0}, [%0]!                    \n"  // load U
    "vld1.u8    {q1}, [%1]!                    \n"  // load V
    "subs       %3, %3, #16                    \n"  // 16 processed per loop
    "vst2.u8    {q0, q1}, [%2]!                \n"  // store 16 pairs of UV
    "bgt        1b                             \n"
    :
      "+r"(src_u),   // %0
      "+r"(src_v),   // %1
      "+r"(dst_uv),  // %2
      "+r"(width)    // %3  // Output registers
    :                       // Input registers
    : "memory", "cc", "q0", "q1"  // Clobber List
  );
}
#endif  // HAS_MERGEUV_NEON
#ifdef HAS_COPYROW_NEON
// Copy multiple of 32.  vld4.u8 allow unaligned and is fastest on a15.
void CopyRow_NEON(const uint8* src, uint8* dst, int count) {
  asm volatile (
    ".p2align  2                               \n"
  "1:                                          \n"
    "vld4.u8    {d0, d1, d2, d3}, [%0]!        \n"  // load 32
    "subs       %2, %2, #32                    \n"  // 32 processed per loop
    "vst4.u8    {d0, d1, d2, d3}, [%1]!        \n"  // store 32
    "bgt        1b                             \n"
    : "+r"(src),   // %0
      "+r"(dst),   // %1
      "+r"(count)  // %2  // Output registers
    :                     // Input registers
    : "memory", "cc", "q0", "q1"  // Clobber List
  );
}
#endif  // HAS_COPYROW_NEON

#ifdef HAS_SETROW_NEON
// SetRow8 writes 'count' bytes using a 32 bit value repeated.
void SetRow8_NEON(uint8* dst, uint32 v32, int count) {
  asm volatile (  // NOLINT
    "vdup.u32  q0, %2                          \n"  // duplicate 4 ints
    "1:                                        \n"
    "subs      %1, %1, #16                     \n"  // 16 bytes per loop
    "vst1.u8   {q0}, [%0]!                     \n"  // store
    "bgt       1b                              \n"
    : "+r"(dst),   // %0
      "+r"(count)  // %1
    : "r"(v32)     // %2
    : "q0", "memory", "cc");
}

// TODO(fbarchard): Make fully assembler
// SetRow32 writes 'count' words using a 32 bit value repeated.
void SetRows32_NEON(uint8* dst, uint32 v32, int width,
                    int dst_stride, int height) {
  for (int y = 0; y < height; ++y) {
    SetRow8_NEON(dst, v32, width << 2);
    dst += dst_stride;
  }
}
#endif  // HAS_SETROW_NEON

#ifdef HAS_MIRRORROW_NEON
void MirrorRow_NEON(const uint8* src, uint8* dst, int width) {
  asm volatile (
    // compute where to start writing destination
    "add         %1, %2                        \n"
    // work on segments that are multiples of 16
    "lsrs        r3, %2, #4                    \n"
    // the output is written in two block. 8 bytes followed
    // by another 8. reading is done sequentially, from left to
    // right. writing is done from right to left in block sizes
    // %1, the destination pointer is incremented after writing
    // the first of the two blocks. need to subtract that 8 off
    // along with 16 to get the next location.
    "mov         r3, #-24                      \n"
    "beq         2f                            \n"

    // back of destination by the size of the register that is
    // going to be mirrored
    "sub         %1, #16                       \n"
    // the loop needs to run on blocks of 16. what will be left
    // over is either a negative number, the residuals that need
    // to be done, or 0. If this isn't subtracted off here the
    // loop will run one extra time.
    "sub         %2, #16                       \n"

    // mirror the bytes in the 64 bit segments. unable to mirror
    // the bytes in the entire 128 bits in one go.
    // because of the inability to mirror the entire 128 bits
    // mirror the writing out of the two 64 bit segments.
    ".p2align  2                               \n"
  "1:                                          \n"
    "vld1.8      {q0}, [%0]!                   \n"  // src += 16
    "subs        %2, #16                       \n"
    "vrev64.8    q0, q0                        \n"
    "vst1.8      {d1}, [%1]!                   \n"
    "vst1.8      {d0}, [%1], r3                \n"  // dst -= 16
    "bge         1b                            \n"

    // add 16 back to the counter. if the result is 0 there is no
    // residuals so jump past
    "adds        %2, #16                       \n"
    "beq         5f                            \n"
    "add         %1, #16                       \n"
  "2:                                          \n"
    "mov         r3, #-3                       \n"
    "sub         %1, #2                        \n"
    "subs        %2, #2                        \n"
    // check for 16*n+1 scenarios where segments_of_2 should not
    // be run, but there is something left over.
    "blt         4f                            \n"

// do this in neon registers as per
// http://blogs.arm.com/software-enablement/196-coding-for-neon-part-2-dealing-with-leftovers/
  "3:                                          \n"
    "vld2.8      {d0[0], d1[0]}, [%0]!         \n"  // src += 2
    "subs        %2, #2                        \n"
    "vst1.8      {d1[0]}, [%1]!                \n"
    "vst1.8      {d0[0]}, [%1], r3             \n"  // dst -= 2
    "bge         3b                            \n"

    "adds        %2, #2                        \n"
    "beq         5f                            \n"
  "4:                                          \n"
    "add         %1, #1                        \n"
    "vld1.8      {d0[0]}, [%0]                 \n"
    "vst1.8      {d0[0]}, [%1]                 \n"
  "5:                                          \n"
    : "+r"(src),   // %0
      "+r"(dst),   // %1
      "+r"(width)  // %2
    :
    : "memory", "cc", "r3", "q0"
  );
}
#endif  // HAS_MIRRORROW_NEON

#ifdef HAS_MirrorUVRow_NEON
void MirrorUVRow_NEON(const uint8* src, uint8* dst_a, uint8* dst_b, int width) {
  asm volatile (
    // compute where to start writing destination
    "add         %1, %3                        \n"  // dst_a + width
    "add         %2, %3                        \n"  // dst_b + width
    // work on input segments that are multiples of 16, but
    // width that has been passed is output segments, half
    // the size of input.
    "lsrs        r12, %3, #3                   \n"
    "beq         2f                            \n"
    // the output is written in to two blocks.
    "mov         r12, #-8                      \n"
    // back of destination by the size of the register that is
    // going to be mirrord
    "sub         %1, #8                        \n"
    "sub         %2, #8                        \n"
    // the loop needs to run on blocks of 8. what will be left
    // over is either a negative number, the residuals that need
    // to be done, or 0. if this isn't subtracted off here the
    // loop will run one extra time.
    "sub         %3, #8                        \n"

    // mirror the bytes in the 64 bit segments
    ".p2align  2                               \n"
  "1:                                          \n"
    "vld2.8      {d0, d1}, [%0]!               \n"  // src += 16
    "subs        %3, #8                        \n"
    "vrev64.8    q0, q0                        \n"
    "vst1.8      {d0}, [%1], r12               \n"  // dst_a -= 8
    "vst1.8      {d1}, [%2], r12               \n"  // dst_b -= 8
    "bge         1b                            \n"

    // add 8 back to the counter. if the result is 0 there is no
    // residuals so return
    "adds        %3, #8                        \n"
    "beq         4f                            \n"
    "add         %1, #8                        \n"
    "add         %2, #8                        \n"
  "2:                                          \n"
    "mov         r12, #-1                      \n"
    "sub         %1, #1                        \n"
    "sub         %2, #1                        \n"
  "3:                                          \n"
      "vld2.8      {d0[0], d1[0]}, [%0]!       \n"  // src += 2
      "subs        %3, %3, #1                  \n"
      "vst1.8      {d0[0]}, [%1], r12          \n"  // dst_a -= 1
      "vst1.8      {d1[0]}, [%2], r12          \n"  // dst_b -= 1
      "bgt         3b                          \n"
  "4:                                          \n"
    : "+r"(src),    // %0
      "+r"(dst_a),  // %1
      "+r"(dst_b),  // %2
      "+r"(width)   // %3
    :
    : "memory", "cc", "r12", "q0"
  );
}
#endif  // HAS_MirrorUVRow_NEON

#ifdef HAS_BGRATOARGBROW_NEON
void BGRAToARGBRow_NEON(const uint8* src_bgra, uint8* dst_argb, int pix) {
  asm volatile (
    ".p2align  2                               \n"
  "1:                                          \n"
    "vld4.8     {d0, d1, d2, d3}, [%0]!        \n"  // load 8 pixels of BGRA.
    "subs       %2, %2, #8                     \n"  // 8 processed per loop.
    "vswp.u8    d1, d2                         \n"  // swap G, R
    "vswp.u8    d0, d3                         \n"  // swap B, A
    "vst4.8     {d0, d1, d2, d3}, [%1]!        \n"  // store 8 pixels of ARGB.
    "bgt        1b                             \n"
  : "+r"(src_bgra),  // %0
    "+r"(dst_argb),  // %1
    "+r"(pix)        // %2
  :
  : "memory", "cc", "d0", "d1", "d2", "d3"  // Clobber List
  );
}
#endif  // HAS_BGRATOARGBROW_NEON

#ifdef HAS_ABGRTOARGBROW_NEON
void ABGRToARGBRow_NEON(const uint8* src_abgr, uint8* dst_argb, int pix) {
  asm volatile (
    ".p2align  2                               \n"
  "1:                                          \n"
    "vld4.8     {d0, d1, d2, d3}, [%0]!        \n"  // load 8 pixels of ABGR.
    "subs       %2, %2, #8                     \n"  // 8 processed per loop.
    "vswp.u8    d0, d2                         \n"  // swap R, B
    "vst4.8     {d0, d1, d2, d3}, [%1]!        \n"  // store 8 pixels of ARGB.
    "bgt        1b                             \n"
  : "+r"(src_abgr),  // %0
    "+r"(dst_argb),  // %1
    "+r"(pix)        // %2
  :
  : "memory", "cc", "d0", "d1", "d2", "d3"  // Clobber List
  );
}
#endif  // HAS_ABGRTOARGBROW_NEON

#ifdef HAS_RGBATOARGBROW_NEON
void RGBAToARGBRow_NEON(const uint8* src_rgba, uint8* dst_argb, int pix) {
  asm volatile (
    ".p2align  2                                \n"
  "1:                                           \n"
    "vld4.8     {d0, d1, d2, d3}, [%0]!         \n"  // load 8 pixels of RGBA.
    "subs       %2, %2, #8                      \n"  // 8 processed per loop.
    "vmov.u8    d4, d0                          \n"  // move A after RGB
    "vst4.8     {d1, d2, d3, d4}, [%1]!         \n"  // store 8 pixels of ARGB.
    "bgt        1b                              \n"
  : "+r"(src_rgba),  // %0
    "+r"(dst_argb),  // %1
    "+r"(pix)        // %2
  :
  : "memory", "cc", "d0", "d1", "d2", "d3", "d4"  // Clobber List
  );
}
#endif  // HAS_RGBATOARGBROW_NEON

#ifdef HAS_RGB24TOARGBROW_NEON
void RGB24ToARGBRow_NEON(const uint8* src_rgb24, uint8* dst_argb, int pix) {
  asm volatile (
    "vmov.u8    d4, #255                       \n"  // Alpha
    ".p2align  2                               \n"
  "1:                                          \n"
    "vld3.8     {d1, d2, d3}, [%0]!            \n"  // load 8 pixels of RGB24.
    "subs       %2, %2, #8                     \n"  // 8 processed per loop.
    "vst4.8     {d1, d2, d3, d4}, [%1]!        \n"  // store 8 pixels of ARGB.
    "bgt        1b                             \n"
  : "+r"(src_rgb24),  // %0
    "+r"(dst_argb),   // %1
    "+r"(pix)         // %2
  :
  : "memory", "cc", "d1", "d2", "d3", "d4"  // Clobber List
  );
}
#endif  // HAS_RGB24TOARGBROW_NEON

#ifdef HAS_RAWTOARGBROW_NEON
void RAWToARGBRow_NEON(const uint8* src_raw, uint8* dst_argb, int pix) {
  asm volatile (
    "vmov.u8    d4, #255                       \n"  // Alpha
    ".p2align  2                               \n"
  "1:                                          \n"
    "vld3.8     {d1, d2, d3}, [%0]!            \n"  // load 8 pixels of RAW.
    "subs       %2, %2, #8                     \n"  // 8 processed per loop.
    "vswp.u8    d1, d3                         \n"  // swap R, B
    "vst4.8     {d1, d2, d3, d4}, [%1]!        \n"  // store 8 pixels of ARGB.
    "bgt        1b                             \n"
  : "+r"(src_raw),   // %0
    "+r"(dst_argb),  // %1
    "+r"(pix)        // %2
  :
  : "memory", "cc", "d1", "d2", "d3", "d4"  // Clobber List
  );
}
#endif  // HAS_RAWTOARGBROW_NEON

#ifdef HAS_RGB565TOARGBROW_NEON
#define RGB565TOARGB                                                           \
    "vshrn.u16  d6, q0, #5                     \n"  /* G xxGGGGGG           */ \
    "vuzp.u8    d0, d1                         \n"  /* d0 xxxBBBBB RRRRRxxx */ \
    "vshl.u8    d6, d6, #2                     \n"  /* G GGGGGG00 upper 6   */ \
    "vshr.u8    d1, d1, #3                     \n"  /* R 000RRRRR lower 5   */ \
    "vshl.u8    q0, q0, #3                     \n"  /* B,R BBBBB000 upper 5 */ \
    "vshr.u8    q2, q0, #5                     \n"  /* B,R 00000BBB lower 3 */ \
    "vorr.u8    d0, d0, d4                     \n"  /* B                    */ \
    "vshr.u8    d4, d6, #6                     \n"  /* G 000000GG lower 2   */ \
    "vorr.u8    d2, d1, d5                     \n"  /* R                    */ \
    "vorr.u8    d1, d4, d6                     \n"  /* G                    */

void RGB565ToARGBRow_NEON(const uint8* src_rgb565, uint8* dst_argb, int pix) {
  asm volatile (
    "vmov.u8    d3, #255                       \n"  // Alpha
    ".p2align  2                               \n"
  "1:                                          \n"
    "vld1.8     {q0}, [%0]!                    \n"  // load 8 RGB565 pixels.
    "subs       %2, %2, #8                     \n"  // 8 processed per loop.
    RGB565TOARGB
    "vst4.8     {d0, d1, d2, d3}, [%1]!        \n"  // store 8 pixels of ARGB.
    "bgt        1b                             \n"
  : "+r"(src_rgb565),  // %0
    "+r"(dst_argb),    // %1
    "+r"(pix)          // %2
  :
  : "memory", "cc", "q0", "q1", "q2", "q3"  // Clobber List
  );
}
#endif  // HAS_RGB565TOARGBROW_NEON

#ifdef HAS_ARGB1555TOARGBROW_NEON
#define ARGB1555TOARGB                                                         \
    "vshrn.u16  d7, q0, #8                     \n"  /* A Arrrrrxx           */ \
    "vshr.u8    d6, d7, #2                     \n"  /* R xxxRRRRR           */ \
    "vshrn.u16  d5, q0, #5                     \n"  /* G xxxGGGGG           */ \
    "vmovn.u16  d4, q0                         \n"  /* B xxxBBBBB           */ \
    "vshr.u8    d7, d7, #7                     \n"  /* A 0000000A           */ \
    "vneg.s8    d7, d7                         \n"  /* A AAAAAAAA upper 8   */ \
    "vshl.u8    d6, d6, #3                     \n"  /* R RRRRR000 upper 5   */ \
    "vshr.u8    q1, q3, #5                     \n"  /* R,A 00000RRR lower 3 */ \
    "vshl.u8    q0, q2, #3                     \n"  /* B,G BBBBB000 upper 5 */ \
    "vshr.u8    q2, q0, #5                     \n"  /* B,G 00000BBB lower 3 */ \
    "vorr.u8    q1, q1, q3                     \n"  /* R,A                  */ \
    "vorr.u8    q0, q0, q2                     \n"  /* B,G                  */ \

// RGB555TOARGB is same as ARGB1555TOARGB but ignores alpha.
#define RGB555TOARGB                                                           \
    "vshrn.u16  d6, q0, #5                     \n"  /* G xxxGGGGG           */ \
    "vuzp.u8    d0, d1                         \n"  /* d0 xxxBBBBB xRRRRRxx */ \
    "vshl.u8    d6, d6, #3                     \n"  /* G GGGGG000 upper 5   */ \
    "vshr.u8    d1, d1, #2                     \n"  /* R 00xRRRRR lower 5   */ \
    "vshl.u8    q0, q0, #3                     \n"  /* B,R BBBBB000 upper 5 */ \
    "vshr.u8    q2, q0, #5                     \n"  /* B,R 00000BBB lower 3 */ \
    "vorr.u8    d0, d0, d4                     \n"  /* B                    */ \
    "vshr.u8    d4, d6, #5                     \n"  /* G 00000GGG lower 3   */ \
    "vorr.u8    d2, d1, d5                     \n"  /* R                    */ \
    "vorr.u8    d1, d4, d6                     \n"  /* G                    */

void ARGB1555ToARGBRow_NEON(const uint8* src_argb1555, uint8* dst_argb,
                            int pix) {
  asm volatile (
    "vmov.u8    d3, #255                       \n"  // Alpha
    ".p2align  2                               \n"
  "1:                                          \n"
    "vld1.8     {q0}, [%0]!                    \n"  // load 8 ARGB1555 pixels.
    "subs       %2, %2, #8                     \n"  // 8 processed per loop.
    ARGB1555TOARGB
    "vst4.8     {d0, d1, d2, d3}, [%1]!        \n"  // store 8 pixels of ARGB.
    "bgt        1b                             \n"
  : "+r"(src_argb1555),  // %0
    "+r"(dst_argb),    // %1
    "+r"(pix)          // %2
  :
  : "memory", "cc", "q0", "q1", "q2", "q3"  // Clobber List
  );
}
#endif  // HAS_ARGB1555TOARGBROW_NEON

#ifdef HAS_ARGB4444TOARGBROW_NEON
#define ARGB4444TOARGB                                                         \
    "vuzp.u8    d0, d1                         \n"  /* d0 BG, d1 RA         */ \
    "vshl.u8    q2, q0, #4                     \n"  /* B,R BBBB0000         */ \
    "vshr.u8    q1, q0, #4                     \n"  /* G,A 0000GGGG         */ \
    "vshr.u8    q0, q2, #4                     \n"  /* B,R 0000BBBB         */ \
    "vorr.u8    q0, q0, q2                     \n"  /* B,R BBBBBBBB         */ \
    "vshl.u8    q2, q1, #4                     \n"  /* G,A GGGG0000         */ \
    "vorr.u8    q1, q1, q2                     \n"  /* G,A GGGGGGGG         */ \
    "vswp.u8    d1, d2                         \n"  /* B,R,G,A -> B,G,R,A   */

void ARGB4444ToARGBRow_NEON(const uint8* src_argb4444, uint8* dst_argb,
                            int pix) {
  asm volatile (
    "vmov.u8    d3, #255                       \n"  // Alpha
    ".p2align  2                               \n"
  "1:                                          \n"
    "vld1.8     {q0}, [%0]!                    \n"  // load 8 ARGB4444 pixels.
    "subs       %2, %2, #8                     \n"  // 8 processed per loop.
    ARGB4444TOARGB
    "vst4.8     {d0, d1, d2, d3}, [%1]!        \n"  // store 8 pixels of ARGB.
    "bgt        1b                             \n"
  : "+r"(src_argb4444),  // %0
    "+r"(dst_argb),    // %1
    "+r"(pix)          // %2
  :
  : "memory", "cc", "q0", "q1", "q2"  // Clobber List
  );
}
#endif  // HAS_ARGB4444TOARGBROW_NEON

#ifdef HAS_ARGBTORGBAROW_NEON
void ARGBToRGBARow_NEON(const uint8* src_argb, uint8* dst_rgba, int pix) {
  asm volatile (
    ".p2align  2                               \n"
  "1:                                          \n"
    "vld4.8     {d1, d2, d3, d4}, [%0]!        \n"  // load 8 pixels of ARGB.
    "subs       %2, %2, #8                     \n"  // 8 processed per loop.
    "vmov.u8    d0, d4                         \n"  // move A before RGB.
    "vst4.8     {d0, d1, d2, d3}, [%1]!        \n"  // store 8 pixels of RGBA.
    "bgt        1b                             \n"
  : "+r"(src_argb),  // %0
    "+r"(dst_rgba),  // %1
    "+r"(pix)        // %2
  :
  : "memory", "cc", "d0", "d1", "d2", "d3", "d4"  // Clobber List
  );
}
#endif  // HAS_ARGBTORGBAROW_NEON

#ifdef HAS_ARGBTORGB24ROW_NEON
void ARGBToRGB24Row_NEON(const uint8* src_argb, uint8* dst_rgb24, int pix) {
  asm volatile (
    ".p2align  2                               \n"
  "1:                                          \n"
    "vld4.8     {d1, d2, d3, d4}, [%0]!        \n"  // load 8 pixels of ARGB.
    "subs       %2, %2, #8                     \n"  // 8 processed per loop.
    "vst3.8     {d1, d2, d3}, [%1]!            \n"  // store 8 pixels of RGB24.
    "bgt        1b                             \n"
  : "+r"(src_argb),   // %0
    "+r"(dst_rgb24),  // %1
    "+r"(pix)         // %2
  :
  : "memory", "cc", "d1", "d2", "d3", "d4"  // Clobber List
  );
}
#endif  // HAS_ARGBTORGB24ROW_NEON

#ifdef HAS_ARGBTORAWROW_NEON
void ARGBToRAWRow_NEON(const uint8* src_argb, uint8* dst_raw, int pix) {
  asm volatile (
    ".p2align  2                               \n"
  "1:                                          \n"
    "vld4.8     {d1, d2, d3, d4}, [%0]!        \n"  // load 8 pixels of ARGB.
    "subs       %2, %2, #8                     \n"  // 8 processed per loop.
    "vswp.u8    d1, d3                         \n"  // swap R, B
    "vst3.8     {d1, d2, d3}, [%1]!            \n"  // store 8 pixels of RAW.
    "bgt        1b                             \n"
  : "+r"(src_argb),  // %0
    "+r"(dst_raw),   // %1
    "+r"(pix)        // %2
  :
  : "memory", "cc", "d1", "d2", "d3", "d4"  // Clobber List
  );
}
#endif  // HAS_ARGBTORAWROW_NEON

#ifdef HAS_YUY2TOYROW_NEON
void YUY2ToYRow_NEON(const uint8* src_yuy2, uint8* dst_y, int pix) {
  asm volatile (
    ".p2align  2                               \n"
  "1:                                          \n"
    "vld2.u8    {q0, q1}, [%0]!                \n"  // load 16 pixels of YUY2.
    "subs       %2, %2, #16                    \n"  // 16 processed per loop.
    "vst1.u8    {q0}, [%1]!                    \n"  // store 16 pixels of Y.
    "bgt        1b                             \n"
  : "+r"(src_yuy2),  // %0
    "+r"(dst_y),     // %1
    "+r"(pix)        // %2
  :
  : "memory", "cc", "q0", "q1"  // Clobber List
  );
}
#endif  // HAS_YUY2TOYROW_NEON

#ifdef HAS_UYVYTOYROW_NEON
void UYVYToYRow_NEON(const uint8* src_uyvy, uint8* dst_y, int pix) {
  asm volatile (
    ".p2align  2                               \n"
  "1:                                          \n"
    "vld2.u8    {q0, q1}, [%0]!                \n"  // load 16 pixels of UYVY.
    "subs       %2, %2, #16                    \n"  // 16 processed per loop.
    "vst1.u8    {q1}, [%1]!                    \n"  // store 16 pixels of Y.
    "bgt        1b                             \n"
  : "+r"(src_uyvy),  // %0
    "+r"(dst_y),     // %1
    "+r"(pix)        // %2
  :
  : "memory", "cc", "q0", "q1"  // Clobber List
  );
}
#endif  // HAS_UYVYTOYROW_NEON

#ifdef HAS_YUY2TOYROW_NEON
void YUY2ToUV422Row_NEON(const uint8* src_yuy2, uint8* dst_u, uint8* dst_v,
                         int pix) {
  asm volatile (
    ".p2align  2                               \n"
  "1:                                          \n"
    "vld4.8     {d0, d1, d2, d3}, [%0]!        \n"  // load 16 pixels of YUY2.
    "subs       %3, %3, #16                    \n"  // 16 pixels = 8 UVs.
    "vst1.u8    {d1}, [%1]!                    \n"  // store 8 U.
    "vst1.u8    {d3}, [%2]!                    \n"  // store 8 V.
    "bgt        1b                             \n"
  : "+r"(src_yuy2),  // %0
    "+r"(dst_u),     // %1
    "+r"(dst_v),     // %2
    "+r"(pix)        // %3
  :
  : "memory", "cc", "d0", "d1", "d2", "d3"  // Clobber List
  );
}
#endif  // HAS_YUY2TOYROW_NEON

#ifdef HAS_UYVYTOYROW_NEON
void UYVYToUV422Row_NEON(const uint8* src_uyvy, uint8* dst_u, uint8* dst_v,
                         int pix) {
  asm volatile (
    ".p2align  2                               \n"
  "1:                                          \n"
    "vld4.8     {d0, d1, d2, d3}, [%0]!        \n"  // load 16 pixels of UYVY.
    "subs       %3, %3, #16                    \n"  // 16 pixels = 8 UVs.
    "vst1.u8    {d0}, [%1]!                    \n"  // store 8 U.
    "vst1.u8    {d2}, [%2]!                    \n"  // store 8 V.
    "bgt        1b                             \n"
  : "+r"(src_uyvy),  // %0
    "+r"(dst_u),     // %1
    "+r"(dst_v),     // %2
    "+r"(pix)        // %3
  :
  : "memory", "cc", "d0", "d1", "d2", "d3"  // Clobber List
  );
}
#endif  // HAS_UYVYTOYROW_NEON

#ifdef HAS_YUY2TOYROW_NEON
void YUY2ToUVRow_NEON(const uint8* src_yuy2, int stride_yuy2,
                      uint8* dst_u, uint8* dst_v, int pix) {
  asm volatile (
    "add        %1, %0, %1                     \n"  // stride + src_yuy2
    ".p2align  2                               \n"
  "1:                                          \n"
    "vld4.8     {d0, d1, d2, d3}, [%0]!        \n"  // load 16 pixels of YUY2.
    "subs       %4, %4, #16                    \n"  // 16 pixels = 8 UVs.
    "vld4.8     {d4, d5, d6, d7}, [%1]!        \n"  // load next row YUY2.
    "vrhadd.u8  d1, d1, d5                     \n"  // average rows of U
    "vrhadd.u8  d3, d3, d7                     \n"  // average rows of V
    "vst1.u8    {d1}, [%2]!                    \n"  // store 8 U.
    "vst1.u8    {d3}, [%3]!                    \n"  // store 8 V.
    "bgt        1b                             \n"
  : "+r"(src_yuy2),     // %0
    "+r"(stride_yuy2),  // %1
    "+r"(dst_u),        // %2
    "+r"(dst_v),        // %3
    "+r"(pix)           // %4
  :
  : "memory", "cc", "d0", "d1", "d2", "d3", "d4", "d5", "d6", "d7"  // Clobber List
  );
}
#endif  // HAS_YUY2TOYROW_NEON

#ifdef HAS_UYVYTOYROW_NEON
void UYVYToUVRow_NEON(const uint8* src_uyvy, int stride_uyvy,
                      uint8* dst_u, uint8* dst_v, int pix) {
  asm volatile (
    "add        %1, %0, %1                     \n"  // stride + src_uyvy
    ".p2align  2                               \n"
  "1:                                          \n"
    "vld4.8     {d0, d1, d2, d3}, [%0]!        \n"  // load 16 pixels of UYVY.
    "subs       %4, %4, #16                    \n"  // 16 pixels = 8 UVs.
    "vld4.8     {d4, d5, d6, d7}, [%1]!        \n"  // load next row UYVY.
    "vrhadd.u8  d0, d0, d4                     \n"  // average rows of U
    "vrhadd.u8  d2, d2, d6                     \n"  // average rows of V
    "vst1.u8    {d0}, [%2]!                    \n"  // store 8 U.
    "vst1.u8    {d2}, [%3]!                    \n"  // store 8 V.
    "bgt        1b                             \n"
  : "+r"(src_uyvy),     // %0
    "+r"(stride_uyvy),  // %1
    "+r"(dst_u),        // %2
    "+r"(dst_v),        // %3
    "+r"(pix)           // %4
  :
  : "memory", "cc", "d0", "d1", "d2", "d3", "d4", "d5", "d6", "d7"  // Clobber List
  );
}
#endif  // HAS_UYVYTOYROW_NEON

void HalfRow_NEON(const uint8* src_uv, int src_uv_stride,
                  uint8* dst_uv, int pix) {
  asm volatile (
    // change the stride to row 2 pointer
    "add        %1, %0                         \n"
  "1:                                          \n"
    "vld1.u8    {q0}, [%0]!                    \n"  // load row 1 16 pixels.
    "subs       %3, %3, #16                    \n"  // 16 processed per loop
    "vld1.u8    {q1}, [%1]!                    \n"  // load row 2 16 pixels.
    "vrhadd.u8  q0, q1                         \n"  // average row 1 and 2
    "vst1.u8    {q0}, [%2]!                    \n"
    "bgt        1b                             \n"
    : "+r"(src_uv),         // %0
      "+r"(src_uv_stride),  // %1
      "+r"(dst_uv),         // %2
      "+r"(pix)             // %3
    :
    : "memory", "cc", "q0", "q1"  // Clobber List
   );
}

// Select 2 channels from ARGB on alternating pixels.  e.g.  BGBGBGBG
void ARGBToBayerRow_NEON(const uint8* src_argb,
                         uint8* dst_bayer, uint32 selector, int pix) {
  asm volatile (
    "vmov.u32   d2[0], %2                      \n"  // selector
  "1:                                          \n"
    "vld1.u8    {q0}, [%0]!                    \n"  // load row 4 pixels.
    "subs       %3, %3, #4                     \n"  // 4 processed per loop
    "vtbl.8     d3, {d0, d1}, d2               \n"  // look up 4 pixels
    "vst1.u32   {d3[0]}, [%1]!                 \n"  // store 4.
    "bgt        1b                             \n"
    : "+r"(src_argb),         // %0
      "+r"(dst_bayer),        // %1
      "+r"(selector),         // %2
      "+r"(pix)               // %3
    :
    : "memory", "cc", "q0", "q1"  // Clobber List
   );
}

void I422ToYUY2Row_NEON(const uint8* src_y,
                        const uint8* src_u,
                        const uint8* src_v,
                        uint8* dst_yuy2, int width) {
  asm volatile (
    ".p2align  2                               \n"
  "1:                                          \n"
    "vld2.8     {d0, d2}, [%0]!                \n"  // load 16 Ys
    "vld1.8     {d1}, [%1]!                    \n"  // load 8 Us
    "vld1.8     {d3}, [%2]!                    \n"  // load 8 Vs
    "subs       %4, %4, #16                    \n"  // 16 pixels
    "vst4.u8    {d0, d1, d2, d3}, [%3]!        \n"  // Store 8 YUY2/16 pixels.
    "bgt        1b                             \n"
    : "+r"(src_y),     // %0
      "+r"(src_u),     // %1
      "+r"(src_v),     // %2
      "+r"(dst_yuy2),  // %3
      "+r"(width)      // %4
    :
    : "cc", "memory", "d0", "d1", "d2", "d3"
  );
}

void I422ToUYVYRow_NEON(const uint8* src_y,
                        const uint8* src_u,
                        const uint8* src_v,
                        uint8* dst_uyvy, int width) {
  asm volatile (
    ".p2align  2                               \n"
  "1:                                          \n"
    "vld2.8     {d1, d3}, [%0]!                \n"  // load 16 Ys
    "vld1.8     {d0}, [%1]!                    \n"  // load 8 Us
    "vld1.8     {d2}, [%2]!                    \n"  // load 8 Vs
    "subs       %4, %4, #16                    \n"  // 16 pixels
    "vst4.u8    {d0, d1, d2, d3}, [%3]!        \n"  // Store 8 UYVY/16 pixels.
    "bgt        1b                             \n"
    : "+r"(src_y),     // %0
      "+r"(src_u),     // %1
      "+r"(src_v),     // %2
      "+r"(dst_uyvy),  // %3
      "+r"(width)      // %4
    :
    : "cc", "memory", "d0", "d1", "d2", "d3"
  );
}

#ifdef HAS_ARGBTORGB565ROW_NEON
void ARGBToRGB565Row_NEON(const uint8* src_argb, uint8* dst_rgb565, int pix) {
  asm volatile (
    ".p2align  2                               \n"
  "1:                                          \n"
    "vld4.8     {d20, d21, d22, d23}, [%0]!    \n"  // load 8 pixels of ARGB.
    "subs       %2, %2, #8                     \n"  // 8 processed per loop.
    ARGBTORGB565
    "vst1.8     {q0}, [%1]!                    \n"  // store 8 pixels RGB565.
    "bgt        1b                             \n"
  : "+r"(src_argb),  // %0
    "+r"(dst_rgb565),  // %1
    "+r"(pix)        // %2
  :
  : "memory", "cc", "q0", "q8", "q9", "q10", "q11"
  );
}
#endif  // HAS_ARGBTORGB565ROW_NEON

#ifdef HAS_ARGBTOARGB1555ROW_NEON
void ARGBToARGB1555Row_NEON(const uint8* src_argb, uint8* dst_argb1555,
                            int pix) {
  asm volatile (
    ".p2align  2                               \n"
  "1:                                          \n"
    "vld4.8     {d20, d21, d22, d23}, [%0]!    \n"  // load 8 pixels of ARGB.
    "subs       %2, %2, #8                     \n"  // 8 processed per loop.
    ARGBTOARGB1555
    "vst1.8     {q0}, [%1]!                    \n"  // store 8 pixels ARGB1555.
    "bgt        1b                             \n"
  : "+r"(src_argb),  // %0
    "+r"(dst_argb1555),  // %1
    "+r"(pix)        // %2
  :
  : "memory", "cc", "q0", "q8", "q9", "q10", "q11"
  );
}
#endif  // HAS_ARGBTOARGB1555ROW_NEON

#ifdef HAS_ARGBTOARGB4444ROW_NEON
void ARGBToARGB4444Row_NEON(const uint8* src_argb, uint8* dst_argb4444,
                            int pix) {
  asm volatile (
    "vmov.u8    d4, #0x0f                      \n"  // bits to clear with vbic.
    ".p2align  2                               \n"
  "1:                                          \n"
    "vld4.8     {d20, d21, d22, d23}, [%0]!    \n"  // load 8 pixels of ARGB.
    "subs       %2, %2, #8                     \n"  // 8 processed per loop.
    ARGBTOARGB4444
    "vst1.8     {q0}, [%1]!                    \n"  // store 8 pixels ARGB4444.
    "bgt        1b                             \n"
  : "+r"(src_argb),      // %0
    "+r"(dst_argb4444),  // %1
    "+r"(pix)            // %2
  :
  : "memory", "cc", "q0", "q8", "q9", "q10", "q11"
  );
}
#endif  // HAS_ARGBTOARGB4444ROW_NEON

#ifdef HAS_ARGBTOYROW_NEON
void ARGBToYRow_NEON(const uint8* src_argb, uint8* dst_y, int pix) {
  asm volatile (
    "vmov.u8    d24, #13                       \n"  // B * 0.1016 coefficient
    "vmov.u8    d25, #65                       \n"  // G * 0.5078 coefficient
    "vmov.u8    d26, #33                       \n"  // R * 0.2578 coefficient
    "vmov.u8    d27, #16                       \n"  // Add 16 constant
    ".p2align  2                               \n"
  "1:                                          \n"
    "vld4.8     {d0, d1, d2, d3}, [%0]!        \n"  // load 8 ARGB pixels.
    "subs       %2, %2, #8                     \n"  // 8 processed per loop.
    "vmull.u8   q2, d0, d24                    \n"  // B
    "vmlal.u8   q2, d1, d25                    \n"  // G
    "vmlal.u8   q2, d2, d26                    \n"  // R
    "vqrshrun.s16 d0, q2, #7                   \n"  // 16 bit to 8 bit Y
    "vqadd.u8   d0, d27                        \n"
    "vst1.8     {d0}, [%1]!                    \n"  // store 8 pixels Y.
    "bgt        1b                             \n"
  : "+r"(src_argb),  // %0
    "+r"(dst_y),     // %1
    "+r"(pix)        // %2
  :
  : "memory", "cc", "q0", "q1", "q2", "q12", "q13"
  );
}
#endif  // HAS_ARGBTOYROW_NEON

// 8x1 pixels.
#ifdef HAS_ARGBTOUV444ROW_NEON
void ARGBToUV444Row_NEON(const uint8* src_argb, uint8* dst_u, uint8* dst_v,
                         int pix) {
  asm volatile (
    "vmov.u8    d24, #112                      \n"  // UB / VR 0.875 coefficient
    "vmov.u8    d25, #74                       \n"  // UG -0.5781 coefficient
    "vmov.u8    d26, #38                       \n"  // UR -0.2969 coefficient
    "vmov.u8    d27, #18                       \n"  // VB -0.1406 coefficient
    "vmov.u8    d28, #94                       \n"  // VG -0.7344 coefficient
    "vmov.u16   q15, #0x8080                   \n"  // 128.5
    ".p2align  2                               \n"
  "1:                                          \n"
    "vld4.8     {d0, d1, d2, d3}, [%0]!        \n"  // load 8 ARGB pixels.
    "subs       %3, %3, #8                     \n"  // 8 processed per loop.
    "vmull.u8   q2, d0, d24                    \n"  // B
    "vmlsl.u8   q2, d1, d25                    \n"  // G
    "vmlsl.u8   q2, d2, d26                    \n"  // R
    "vadd.u16   q2, q2, q15                    \n"  // +128 -> unsigned

    "vmull.u8   q3, d2, d24                    \n"  // R
    "vmlsl.u8   q3, d1, d28                    \n"  // G
    "vmlsl.u8   q3, d0, d27                    \n"  // B
    "vadd.u16   q3, q3, q15                    \n"  // +128 -> unsigned

    "vqshrn.u16  d0, q2, #8                    \n"  // 16 bit to 8 bit U
    "vqshrn.u16  d1, q3, #8                    \n"  // 16 bit to 8 bit V

    "vst1.8     {d0}, [%1]!                    \n"  // store 8 pixels U.
    "vst1.8     {d1}, [%2]!                    \n"  // store 8 pixels V.
    "bgt        1b                             \n"
  : "+r"(src_argb),  // %0
    "+r"(dst_u),     // %1
    "+r"(dst_v),     // %2
    "+r"(pix)        // %3
  :
  : "memory", "cc", "q0", "q1", "q2", "q3", "q4", "q12", "q13", "q14", "q15"
  );
}
#endif  // HAS_ARGBTOUV444ROW_NEON

// 16x1 pixels -> 8x1.  pix is number of argb pixels. e.g. 16.
#ifdef HAS_ARGBTOUV422ROW_NEON
void ARGBToUV422Row_NEON(const uint8* src_argb, uint8* dst_u, uint8* dst_v,
                         int pix) {
  asm volatile (
    "vmov.s16   q10, #112 / 2                  \n"  // UB / VR 0.875 coefficient
    "vmov.s16   q11, #74 / 2                   \n"  // UG -0.5781 coefficient
    "vmov.s16   q12, #38 / 2                   \n"  // UR -0.2969 coefficient
    "vmov.s16   q13, #18 / 2                   \n"  // VB -0.1406 coefficient
    "vmov.s16   q14, #94 / 2                   \n"  // VG -0.7344 coefficient
    "vmov.u16   q15, #0x8080                   \n"  // 128.5
    ".p2align  2                               \n"
  "1:                                          \n"
    "vld4.8     {d0, d2, d4, d6}, [%0]!        \n"  // load 8 ARGB pixels.
    "vld4.8     {d1, d3, d5, d7}, [%0]!        \n"  // load next 8 ARGB pixels.

    "vpaddl.u8  q0, q0                         \n"  // B 16 bytes -> 8 shorts.
    "vpaddl.u8  q1, q1                         \n"  // G 16 bytes -> 8 shorts.
    "vpaddl.u8  q2, q2                         \n"  // R 16 bytes -> 8 shorts.

    "subs       %3, %3, #16                    \n"  // 16 processed per loop.
    "vmul.s16   q8, q0, q10                    \n"  // B
    "vmls.s16   q8, q1, q11                    \n"  // G
    "vmls.s16   q8, q2, q12                    \n"  // R
    "vadd.u16   q8, q8, q15                    \n"  // +128 -> unsigned

    "vmul.s16   q9, q2, q10                    \n"  // R
    "vmls.s16   q9, q1, q14                    \n"  // G
    "vmls.s16   q9, q0, q13                    \n"  // B
    "vadd.u16   q9, q9, q15                    \n"  // +128 -> unsigned

    "vqshrn.u16  d0, q8, #8                    \n"  // 16 bit to 8 bit U
    "vqshrn.u16  d1, q9, #8                    \n"  // 16 bit to 8 bit V

    "vst1.8     {d0}, [%1]!                    \n"  // store 8 pixels U.
    "vst1.8     {d1}, [%2]!                    \n"  // store 8 pixels V.
    "bgt        1b                             \n"
  : "+r"(src_argb),  // %0
    "+r"(dst_u),     // %1
    "+r"(dst_v),     // %2
    "+r"(pix)        // %3
  :
  : "memory", "cc", "q0", "q1", "q2", "q3",
    "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
  );
}
#endif  // HAS_ARGBTOUV422ROW_NEON

// 32x1 pixels -> 8x1.  pix is number of argb pixels. e.g. 32.
#ifdef HAS_ARGBTOUV411ROW_NEON
void ARGBToUV411Row_NEON(const uint8* src_argb, uint8* dst_u, uint8* dst_v,
                         int pix) {
  asm volatile (
    "vmov.s16   q10, #112 / 4                  \n"  // UB / VR 0.875 coefficient
    "vmov.s16   q11, #74 / 4                   \n"  // UG -0.5781 coefficient
    "vmov.s16   q12, #38 / 4                   \n"  // UR -0.2969 coefficient
    "vmov.s16   q13, #18 / 4                   \n"  // VB -0.1406 coefficient
    "vmov.s16   q14, #94 / 4                   \n"  // VG -0.7344 coefficient
    "vmov.u16   q15, #0x8080                   \n"  // 128.5
    ".p2align  2                               \n"
  "1:                                          \n"
    "vld4.8     {d0, d2, d4, d6}, [%0]!        \n"  // load 8 ARGB pixels.
    "vld4.8     {d1, d3, d5, d7}, [%0]!        \n"  // load next 8 ARGB pixels.
    "vpaddl.u8  q0, q0                         \n"  // B 16 bytes -> 8 shorts.
    "vpaddl.u8  q1, q1                         \n"  // G 16 bytes -> 8 shorts.
    "vpaddl.u8  q2, q2                         \n"  // R 16 bytes -> 8 shorts.
    "vld4.8     {d8, d10, d12, d14}, [%0]!     \n"  // load 8 more ARGB pixels.
    "vld4.8     {d9, d11, d13, d15}, [%0]!     \n"  // load last 8 ARGB pixels.
    "vpaddl.u8  q4, q4                         \n"  // B 16 bytes -> 8 shorts.
    "vpaddl.u8  q5, q5                         \n"  // G 16 bytes -> 8 shorts.
    "vpaddl.u8  q6, q6                         \n"  // R 16 bytes -> 8 shorts.
    "vpadd.u16  d0, d0, d1                     \n"  // B 16 shorts -> 8 shorts.
    "vpadd.u16  d1, d8, d9                     \n"  // B
    "vpadd.u16  d2, d2, d3                     \n"  // G 16 shorts -> 8 shorts.
    "vpadd.u16  d3, d10, d11                   \n"  // G
    "vpadd.u16  d4, d4, d5                     \n"  // R 16 shorts -> 8 shorts.
    "vpadd.u16  d5, d12, d13                   \n"  // R
    "subs       %3, %3, #32                    \n"  // 32 processed per loop.
    "vmul.s16   q8, q0, q10                    \n"  // B
    "vmls.s16   q8, q1, q11                    \n"  // G
    "vmls.s16   q8, q2, q12                    \n"  // R
    "vadd.u16   q8, q8, q15                    \n"  // +128 -> unsigned
    "vmul.s16   q9, q2, q10                    \n"  // R
    "vmls.s16   q9, q1, q14                    \n"  // G
    "vmls.s16   q9, q0, q13                    \n"  // B
    "vadd.u16   q9, q9, q15                    \n"  // +128 -> unsigned
    "vqshrn.u16  d0, q8, #8                    \n"  // 16 bit to 8 bit U
    "vqshrn.u16  d1, q9, #8                    \n"  // 16 bit to 8 bit V
    "vst1.8     {d0}, [%1]!                    \n"  // store 8 pixels U.
    "vst1.8     {d1}, [%2]!                    \n"  // store 8 pixels V.
    "bgt        1b                             \n"
  : "+r"(src_argb),  // %0
    "+r"(dst_u),     // %1
    "+r"(dst_v),     // %2
    "+r"(pix)        // %3
  :
  : "memory", "cc", "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7",
    "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
  );
}
#endif  // HAS_ARGBTOUV411ROW_NEON

// 16x2 pixels -> 8x1.  pix is number of argb pixels. e.g. 16.
#ifdef HAS_ARGBTOUVROW_NEON

#define RGBTOUV(QB, QG, QR) \
    "vmul.s16   q8, " #QB ", q10               \n"  /* B                    */ \
    "vmls.s16   q8, " #QG ", q11               \n"  /* G                    */ \
    "vmls.s16   q8, " #QR ", q12               \n"  /* R                    */ \
    "vadd.u16   q8, q8, q15                    \n"  /* +128 -> unsigned     */ \
    "vmul.s16   q9, " #QR ", q10               \n"  /* R                    */ \
    "vmls.s16   q9, " #QG ", q14               \n"  /* G                    */ \
    "vmls.s16   q9, " #QB ", q13               \n"  /* B                    */ \
    "vadd.u16   q9, q9, q15                    \n"  /* +128 -> unsigned     */ \
    "vqshrn.u16  d0, q8, #8                    \n"  /* 16 bit to 8 bit U    */ \
    "vqshrn.u16  d1, q9, #8                    \n"  /* 16 bit to 8 bit V    */

void ARGBToUVRow_NEON(const uint8* src_argb, int src_stride_argb,
                      uint8* dst_u, uint8* dst_v, int pix) {
  asm volatile (
    "add        %1, %0, %1                     \n"  // src_stride + src_argb
    "vmov.s16   q10, #112 / 4                  \n"  // UB / VR 0.875 coefficient
    "vmov.s16   q11, #74 / 4                   \n"  // UG -0.5781 coefficient
    "vmov.s16   q12, #38 / 4                   \n"  // UR -0.2969 coefficient
    "vmov.s16   q13, #18 / 4                   \n"  // VB -0.1406 coefficient
    "vmov.s16   q14, #94 / 4                   \n"  // VG -0.7344 coefficient
    "vmov.u16   q15, #0x8080                   \n"  // 128.5
    ".p2align  2                               \n"
  "1:                                          \n"
    "vld4.8     {d0, d2, d4, d6}, [%0]!        \n"  // load 8 ARGB pixels.
    "vld4.8     {d1, d3, d5, d7}, [%0]!        \n"  // load next 8 ARGB pixels.
    "vpaddl.u8  q0, q0                         \n"  // B 16 bytes -> 8 shorts.
    "vpaddl.u8  q1, q1                         \n"  // G 16 bytes -> 8 shorts.
    "vpaddl.u8  q2, q2                         \n"  // R 16 bytes -> 8 shorts.
    "vld4.8     {d8, d10, d12, d14}, [%1]!     \n"  // load 8 more ARGB pixels.
    "vld4.8     {d9, d11, d13, d15}, [%1]!     \n"  // load last 8 ARGB pixels.
    "vpadal.u8  q0, q4                         \n"  // B 16 bytes -> 8 shorts.
    "vpadal.u8  q1, q5                         \n"  // G 16 bytes -> 8 shorts.
    "vpadal.u8  q2, q6                         \n"  // R 16 bytes -> 8 shorts.
    "subs       %4, %4, #16                    \n"  // 32 processed per loop.
    RGBTOUV(q0, q1, q2)
    "vst1.8     {d0}, [%2]!                    \n"  // store 8 pixels U.
    "vst1.8     {d1}, [%3]!                    \n"  // store 8 pixels V.
    "bgt        1b                             \n"
  : "+r"(src_argb),  // %0
    "+r"(src_stride_argb),  // %1
    "+r"(dst_u),     // %2
    "+r"(dst_v),     // %3
    "+r"(pix)        // %4
  :
  : "memory", "cc", "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7",
    "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
  );
}

void BGRAToUVRow_NEON(const uint8* src_bgra, int src_stride_bgra,
                      uint8* dst_u, uint8* dst_v, int pix) {
  asm volatile (
    "add        %1, %0, %1                     \n"  // src_stride + src_bgra
    "vmov.s16   q10, #112 / 4                  \n"  // UB / VR 0.875 coefficient
    "vmov.s16   q11, #74 / 4                   \n"  // UG -0.5781 coefficient
    "vmov.s16   q12, #38 / 4                   \n"  // UR -0.2969 coefficient
    "vmov.s16   q13, #18 / 4                   \n"  // VB -0.1406 coefficient
    "vmov.s16   q14, #94 / 4                   \n"  // VG -0.7344 coefficient
    "vmov.u16   q15, #0x8080                   \n"  // 128.5
    ".p2align  2                               \n"
  "1:                                          \n"
    "vld4.8     {d0, d2, d4, d6}, [%0]!        \n"  // load 8 BGRA pixels.
    "vld4.8     {d1, d3, d5, d7}, [%0]!        \n"  // load next 8 BGRA pixels.
    "vpaddl.u8  q3, q3                         \n"  // B 16 bytes -> 8 shorts.
    "vpaddl.u8  q2, q2                         \n"  // G 16 bytes -> 8 shorts.
    "vpaddl.u8  q1, q1                         \n"  // R 16 bytes -> 8 shorts.
    "vld4.8     {d8, d10, d12, d14}, [%1]!     \n"  // load 8 more BGRA pixels.
    "vld4.8     {d9, d11, d13, d15}, [%1]!     \n"  // load last 8 BGRA pixels.
    "vpadal.u8  q3, q7                         \n"  // B 16 bytes -> 8 shorts.
    "vpadal.u8  q2, q6                         \n"  // G 16 bytes -> 8 shorts.
    "vpadal.u8  q1, q5                         \n"  // R 16 bytes -> 8 shorts.
    "subs       %4, %4, #16                    \n"  // 32 processed per loop.
    RGBTOUV(q3, q2, q1)
    "vst1.8     {d0}, [%2]!                    \n"  // store 8 pixels U.
    "vst1.8     {d1}, [%3]!                    \n"  // store 8 pixels V.
    "bgt        1b                             \n"
  : "+r"(src_bgra),  // %0
    "+r"(src_stride_bgra),  // %1
    "+r"(dst_u),     // %2
    "+r"(dst_v),     // %3
    "+r"(pix)        // %4
  :
  : "memory", "cc", "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7",
    "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
  );
}

void ABGRToUVRow_NEON(const uint8* src_abgr, int src_stride_abgr,
                      uint8* dst_u, uint8* dst_v, int pix) {
  asm volatile (
    "add        %1, %0, %1                     \n"  // src_stride + src_abgr
    "vmov.s16   q10, #112 / 4                  \n"  // UB / VR 0.875 coefficient
    "vmov.s16   q11, #74 / 4                   \n"  // UG -0.5781 coefficient
    "vmov.s16   q12, #38 / 4                   \n"  // UR -0.2969 coefficient
    "vmov.s16   q13, #18 / 4                   \n"  // VB -0.1406 coefficient
    "vmov.s16   q14, #94 / 4                   \n"  // VG -0.7344 coefficient
    "vmov.u16   q15, #0x8080                   \n"  // 128.5
    ".p2align  2                               \n"
  "1:                                          \n"
    "vld4.8     {d0, d2, d4, d6}, [%0]!        \n"  // load 8 ABGR pixels.
    "vld4.8     {d1, d3, d5, d7}, [%0]!        \n"  // load next 8 ABGR pixels.
    "vpaddl.u8  q2, q2                         \n"  // B 16 bytes -> 8 shorts.
    "vpaddl.u8  q1, q1                         \n"  // G 16 bytes -> 8 shorts.
    "vpaddl.u8  q0, q0                         \n"  // R 16 bytes -> 8 shorts.
    "vld4.8     {d8, d10, d12, d14}, [%1]!     \n"  // load 8 more ABGR pixels.
    "vld4.8     {d9, d11, d13, d15}, [%1]!     \n"  // load last 8 ABGR pixels.
    "vpadal.u8  q2, q6                         \n"  // B 16 bytes -> 8 shorts.
    "vpadal.u8  q1, q5                         \n"  // G 16 bytes -> 8 shorts.
    "vpadal.u8  q0, q4                         \n"  // R 16 bytes -> 8 shorts.
    "subs       %4, %4, #16                    \n"  // 32 processed per loop.
    RGBTOUV(q2, q1, q0)
    "vst1.8     {d0}, [%2]!                    \n"  // store 8 pixels U.
    "vst1.8     {d1}, [%3]!                    \n"  // store 8 pixels V.
    "bgt        1b                             \n"
  : "+r"(src_abgr),  // %0
    "+r"(src_stride_abgr),  // %1
    "+r"(dst_u),     // %2
    "+r"(dst_v),     // %3
    "+r"(pix)        // %4
  :
  : "memory", "cc", "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7",
    "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
  );
}

void RGBAToUVRow_NEON(const uint8* src_rgba, int src_stride_rgba,
                      uint8* dst_u, uint8* dst_v, int pix) {
  asm volatile (
    "add        %1, %0, %1                     \n"  // src_stride + src_rgba
    "vmov.s16   q10, #112 / 4                  \n"  // UB / VR 0.875 coefficient
    "vmov.s16   q11, #74 / 4                   \n"  // UG -0.5781 coefficient
    "vmov.s16   q12, #38 / 4                   \n"  // UR -0.2969 coefficient
    "vmov.s16   q13, #18 / 4                   \n"  // VB -0.1406 coefficient
    "vmov.s16   q14, #94 / 4                   \n"  // VG -0.7344 coefficient
    "vmov.u16   q15, #0x8080                   \n"  // 128.5
    ".p2align  2                               \n"
  "1:                                          \n"
    "vld4.8     {d0, d2, d4, d6}, [%0]!        \n"  // load 8 RGBA pixels.
    "vld4.8     {d1, d3, d5, d7}, [%0]!        \n"  // load next 8 RGBA pixels.
    "vpaddl.u8  q0, q1                         \n"  // B 16 bytes -> 8 shorts.
    "vpaddl.u8  q1, q2                         \n"  // G 16 bytes -> 8 shorts.
    "vpaddl.u8  q2, q3                         \n"  // R 16 bytes -> 8 shorts.
    "vld4.8     {d8, d10, d12, d14}, [%1]!     \n"  // load 8 more RGBA pixels.
    "vld4.8     {d9, d11, d13, d15}, [%1]!     \n"  // load last 8 RGBA pixels.
    "vpadal.u8  q0, q5                         \n"  // B 16 bytes -> 8 shorts.
    "vpadal.u8  q1, q6                         \n"  // G 16 bytes -> 8 shorts.
    "vpadal.u8  q2, q7                         \n"  // R 16 bytes -> 8 shorts.
    "subs       %4, %4, #16                    \n"  // 32 processed per loop.
    RGBTOUV(q0, q1, q2)
    "vst1.8     {d0}, [%2]!                    \n"  // store 8 pixels U.
    "vst1.8     {d1}, [%3]!                    \n"  // store 8 pixels V.
    "bgt        1b                             \n"
  : "+r"(src_rgba),  // %0
    "+r"(src_stride_rgba),  // %1
    "+r"(dst_u),     // %2
    "+r"(dst_v),     // %3
    "+r"(pix)        // %4
  :
  : "memory", "cc", "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7",
    "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
  );
}

void RGB24ToUVRow_NEON(const uint8* src_rgb24, int src_stride_rgb24,
                       uint8* dst_u, uint8* dst_v, int pix) {
  asm volatile (
    "add        %1, %0, %1                     \n"  // src_stride + src_rgb24
    "vmov.s16   q10, #112 / 4                  \n"  // UB / VR 0.875 coefficient
    "vmov.s16   q11, #74 / 4                   \n"  // UG -0.5781 coefficient
    "vmov.s16   q12, #38 / 4                   \n"  // UR -0.2969 coefficient
    "vmov.s16   q13, #18 / 4                   \n"  // VB -0.1406 coefficient
    "vmov.s16   q14, #94 / 4                   \n"  // VG -0.7344 coefficient
    "vmov.u16   q15, #0x8080                   \n"  // 128.5
    ".p2align  2                               \n"
  "1:                                          \n"
    "vld3.8     {d0, d2, d4}, [%0]!            \n"  // load 8 RGB24 pixels.
    "vld3.8     {d1, d3, d5}, [%0]!            \n"  // load next 8 RGB24 pixels.
    "vpaddl.u8  q0, q0                         \n"  // B 16 bytes -> 8 shorts.
    "vpaddl.u8  q1, q1                         \n"  // G 16 bytes -> 8 shorts.
    "vpaddl.u8  q2, q2                         \n"  // R 16 bytes -> 8 shorts.
    "vld3.8     {d8, d10, d12}, [%1]!          \n"  // load 8 more RGB24 pixels.
    "vld3.8     {d9, d11, d13}, [%1]!          \n"  // load last 8 RGB24 pixels.
    "vpadal.u8  q0, q4                         \n"  // B 16 bytes -> 8 shorts.
    "vpadal.u8  q1, q5                         \n"  // G 16 bytes -> 8 shorts.
    "vpadal.u8  q2, q6                         \n"  // R 16 bytes -> 8 shorts.
    "subs       %4, %4, #16                    \n"  // 32 processed per loop.
    RGBTOUV(q0, q1, q2)
    "vst1.8     {d0}, [%2]!                    \n"  // store 8 pixels U.
    "vst1.8     {d1}, [%3]!                    \n"  // store 8 pixels V.
    "bgt        1b                             \n"
  : "+r"(src_rgb24),  // %0
    "+r"(src_stride_rgb24),  // %1
    "+r"(dst_u),     // %2
    "+r"(dst_v),     // %3
    "+r"(pix)        // %4
  :
  : "memory", "cc", "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7",
    "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
  );
}

void RAWToUVRow_NEON(const uint8* src_raw, int src_stride_raw,
                     uint8* dst_u, uint8* dst_v, int pix) {
  asm volatile (
    "add        %1, %0, %1                     \n"  // src_stride + src_raw
    "vmov.s16   q10, #112 / 4                  \n"  // UB / VR 0.875 coefficient
    "vmov.s16   q11, #74 / 4                   \n"  // UG -0.5781 coefficient
    "vmov.s16   q12, #38 / 4                   \n"  // UR -0.2969 coefficient
    "vmov.s16   q13, #18 / 4                   \n"  // VB -0.1406 coefficient
    "vmov.s16   q14, #94 / 4                   \n"  // VG -0.7344 coefficient
    "vmov.u16   q15, #0x8080                   \n"  // 128.5
    ".p2align  2                               \n"
  "1:                                          \n"
    "vld3.8     {d0, d2, d4}, [%0]!            \n"  // load 8 RAW pixels.
    "vld3.8     {d1, d3, d5}, [%0]!            \n"  // load next 8 RAW pixels.
    "vpaddl.u8  q2, q2                         \n"  // B 16 bytes -> 8 shorts.
    "vpaddl.u8  q1, q1                         \n"  // G 16 bytes -> 8 shorts.
    "vpaddl.u8  q0, q0                         \n"  // R 16 bytes -> 8 shorts.
    "vld3.8     {d8, d10, d12}, [%1]!          \n"  // load 8 more RAW pixels.
    "vld3.8     {d9, d11, d13}, [%1]!          \n"  // load last 8 RAW pixels.
    "vpadal.u8  q2, q6                         \n"  // B 16 bytes -> 8 shorts.
    "vpadal.u8  q1, q5                         \n"  // G 16 bytes -> 8 shorts.
    "vpadal.u8  q0, q4                         \n"  // R 16 bytes -> 8 shorts.
    "subs       %4, %4, #16                    \n"  // 32 processed per loop.
    RGBTOUV(q2, q1, q0)
    "vst1.8     {d0}, [%2]!                    \n"  // store 8 pixels U.
    "vst1.8     {d1}, [%3]!                    \n"  // store 8 pixels V.
    "bgt        1b                             \n"
  : "+r"(src_raw),  // %0
    "+r"(src_stride_raw),  // %1
    "+r"(dst_u),     // %2
    "+r"(dst_v),     // %3
    "+r"(pix)        // %4
  :
  : "memory", "cc", "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7",
    "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
  );
}

#endif  // HAS_ARGBTOUVROW_NEON

// 16x2 pixels -> 8x1.  pix is number of argb pixels. e.g. 16.
#ifdef HAS_RGB565TOUVROW_NEON
void RGB565ToUVRow_NEON(const uint8* src_rgb565, int src_stride_rgb565,
                        uint8* dst_u, uint8* dst_v, int pix) {
  asm volatile (
    "add        %1, %0, %1                     \n"  // src_stride + src_argb
    "vmov.s16   q10, #112 / 4                  \n"  // UB / VR 0.875 coefficient
    "vmov.s16   q11, #74 / 4                   \n"  // UG -0.5781 coefficient
    "vmov.s16   q12, #38 / 4                   \n"  // UR -0.2969 coefficient
    "vmov.s16   q13, #18 / 4                   \n"  // VB -0.1406 coefficient
    "vmov.s16   q14, #94 / 4                   \n"  // VG -0.7344 coefficient
    "vmov.u16   q15, #0x8080                   \n"  // 128.5
    ".p2align  2                               \n"
  "1:                                          \n"
    "vld1.8     {q0}, [%0]!                    \n"  // load 8 RGB565 pixels.
    RGB565TOARGB
    "vpaddl.u8  d8, d0                         \n"  // B 8 bytes -> 4 shorts.
    "vpaddl.u8  d10, d1                        \n"  // G 8 bytes -> 4 shorts.
    "vpaddl.u8  d12, d2                        \n"  // R 8 bytes -> 4 shorts.
    "vld1.8     {q0}, [%0]!                    \n"  // next 8 RGB565 pixels.
    RGB565TOARGB
    "vpaddl.u8  d9, d0                         \n"  // B 8 bytes -> 4 shorts.
    "vpaddl.u8  d11, d1                        \n"  // G 8 bytes -> 4 shorts.
    "vpaddl.u8  d13, d2                        \n"  // R 8 bytes -> 4 shorts.

    "vld1.8     {q0}, [%1]!                    \n"  // load 8 RGB565 pixels.
    RGB565TOARGB
    "vpadal.u8  d8, d0                         \n"  // B 8 bytes -> 4 shorts.
    "vpadal.u8  d10, d1                        \n"  // G 8 bytes -> 4 shorts.
    "vpadal.u8  d12, d2                        \n"  // R 8 bytes -> 4 shorts.
    "vld1.8     {q0}, [%1]!                    \n"  // next 8 RGB565 pixels.
    RGB565TOARGB
    "vpadal.u8  d9, d0                         \n"  // B 8 bytes -> 4 shorts.
    "vpadal.u8  d11, d1                        \n"  // G 8 bytes -> 4 shorts.
    "vpadal.u8  d13, d2                        \n"  // R 8 bytes -> 4 shorts.

    "subs       %4, %4, #16                    \n"  // 16 processed per loop.
    "vmul.s16   q8, q4, q10                    \n"  // B
    "vmls.s16   q8, q5, q11                    \n"  // G
    "vmls.s16   q8, q6, q12                    \n"  // R
    "vadd.u16   q8, q8, q15                    \n"  // +128 -> unsigned
    "vmul.s16   q9, q6, q10                    \n"  // R
    "vmls.s16   q9, q5, q14                    \n"  // G
    "vmls.s16   q9, q4, q13                    \n"  // B
    "vadd.u16   q9, q9, q15                    \n"  // +128 -> unsigned
    "vqshrn.u16  d0, q8, #8                    \n"  // 16 bit to 8 bit U
    "vqshrn.u16  d1, q9, #8                    \n"  // 16 bit to 8 bit V
    "vst1.8     {d0}, [%2]!                    \n"  // store 8 pixels U.
    "vst1.8     {d1}, [%3]!                    \n"  // store 8 pixels V.
    "bgt        1b                             \n"
  : "+r"(src_rgb565),  // %0
    "+r"(src_stride_rgb565),  // %1
    "+r"(dst_u),     // %2
    "+r"(dst_v),     // %3
    "+r"(pix)        // %4
  :
  : "memory", "cc", "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7",
    "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
  );
}
#endif  // HAS_RGB565TOUVROW_NEON

// 16x2 pixels -> 8x1.  pix is number of argb pixels. e.g. 16.
#ifdef HAS_ARGB1555TOUVROW_NEON
void ARGB1555ToUVRow_NEON(const uint8* src_argb1555, int src_stride_argb1555,
                        uint8* dst_u, uint8* dst_v, int pix) {
  asm volatile (
    "add        %1, %0, %1                     \n"  // src_stride + src_argb
    "vmov.s16   q10, #112 / 4                  \n"  // UB / VR 0.875 coefficient
    "vmov.s16   q11, #74 / 4                   \n"  // UG -0.5781 coefficient
    "vmov.s16   q12, #38 / 4                   \n"  // UR -0.2969 coefficient
    "vmov.s16   q13, #18 / 4                   \n"  // VB -0.1406 coefficient
    "vmov.s16   q14, #94 / 4                   \n"  // VG -0.7344 coefficient
    "vmov.u16   q15, #0x8080                   \n"  // 128.5
    ".p2align  2                               \n"
  "1:                                          \n"
    "vld1.8     {q0}, [%0]!                    \n"  // load 8 ARGB1555 pixels.
    RGB555TOARGB
    "vpaddl.u8  d8, d0                         \n"  // B 8 bytes -> 4 shorts.
    "vpaddl.u8  d10, d1                        \n"  // G 8 bytes -> 4 shorts.
    "vpaddl.u8  d12, d2                        \n"  // R 8 bytes -> 4 shorts.
    "vld1.8     {q0}, [%0]!                    \n"  // next 8 ARGB1555 pixels.
    RGB555TOARGB
    "vpaddl.u8  d9, d0                         \n"  // B 8 bytes -> 4 shorts.
    "vpaddl.u8  d11, d1                        \n"  // G 8 bytes -> 4 shorts.
    "vpaddl.u8  d13, d2                        \n"  // R 8 bytes -> 4 shorts.

    "vld1.8     {q0}, [%1]!                    \n"  // load 8 ARGB1555 pixels.
    RGB555TOARGB
    "vpadal.u8  d8, d0                         \n"  // B 8 bytes -> 4 shorts.
    "vpadal.u8  d10, d1                        \n"  // G 8 bytes -> 4 shorts.
    "vpadal.u8  d12, d2                        \n"  // R 8 bytes -> 4 shorts.
    "vld1.8     {q0}, [%1]!                    \n"  // next 8 ARGB1555 pixels.
    RGB555TOARGB
    "vpadal.u8  d9, d0                         \n"  // B 8 bytes -> 4 shorts.
    "vpadal.u8  d11, d1                        \n"  // G 8 bytes -> 4 shorts.
    "vpadal.u8  d13, d2                        \n"  // R 8 bytes -> 4 shorts.

    "subs       %4, %4, #16                    \n"  // 16 processed per loop.
    "vmul.s16   q8, q4, q10                    \n"  // B
    "vmls.s16   q8, q5, q11                    \n"  // G
    "vmls.s16   q8, q6, q12                    \n"  // R
    "vadd.u16   q8, q8, q15                    \n"  // +128 -> unsigned
    "vmul.s16   q9, q6, q10                    \n"  // R
    "vmls.s16   q9, q5, q14                    \n"  // G
    "vmls.s16   q9, q4, q13                    \n"  // B
    "vadd.u16   q9, q9, q15                    \n"  // +128 -> unsigned
    "vqshrn.u16  d0, q8, #8                    \n"  // 16 bit to 8 bit U
    "vqshrn.u16  d1, q9, #8                    \n"  // 16 bit to 8 bit V
    "vst1.8     {d0}, [%2]!                    \n"  // store 8 pixels U.
    "vst1.8     {d1}, [%3]!                    \n"  // store 8 pixels V.
    "bgt        1b                             \n"
  : "+r"(src_argb1555),  // %0
    "+r"(src_stride_argb1555),  // %1
    "+r"(dst_u),     // %2
    "+r"(dst_v),     // %3
    "+r"(pix)        // %4
  :
  : "memory", "cc", "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7",
    "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
  );
}
#endif  // HAS_ARGB1555TOUVROW_NEON

// 16x2 pixels -> 8x1.  pix is number of argb pixels. e.g. 16.
#ifdef HAS_ARGB4444TOUVROW_NEON
void ARGB4444ToUVRow_NEON(const uint8* src_argb4444, int src_stride_argb4444,
                          uint8* dst_u, uint8* dst_v, int pix) {
  asm volatile (
    "add        %1, %0, %1                     \n"  // src_stride + src_argb
    "vmov.s16   q10, #112 / 4                  \n"  // UB / VR 0.875 coefficient
    "vmov.s16   q11, #74 / 4                   \n"  // UG -0.5781 coefficient
    "vmov.s16   q12, #38 / 4                   \n"  // UR -0.2969 coefficient
    "vmov.s16   q13, #18 / 4                   \n"  // VB -0.1406 coefficient
    "vmov.s16   q14, #94 / 4                   \n"  // VG -0.7344 coefficient
    "vmov.u16   q15, #0x8080                   \n"  // 128.5
    ".p2align  2                               \n"
  "1:                                          \n"
    "vld1.8     {q0}, [%0]!                    \n"  // load 8 ARGB4444 pixels.
    ARGB4444TOARGB
    "vpaddl.u8  d8, d0                         \n"  // B 8 bytes -> 4 shorts.
    "vpaddl.u8  d10, d1                        \n"  // G 8 bytes -> 4 shorts.
    "vpaddl.u8  d12, d2                        \n"  // R 8 bytes -> 4 shorts.
    "vld1.8     {q0}, [%0]!                    \n"  // next 8 ARGB4444 pixels.
    ARGB4444TOARGB
    "vpaddl.u8  d9, d0                         \n"  // B 8 bytes -> 4 shorts.
    "vpaddl.u8  d11, d1                        \n"  // G 8 bytes -> 4 shorts.
    "vpaddl.u8  d13, d2                        \n"  // R 8 bytes -> 4 shorts.

    "vld1.8     {q0}, [%1]!                    \n"  // load 8 ARGB4444 pixels.
    ARGB4444TOARGB
    "vpadal.u8  d8, d0                         \n"  // B 8 bytes -> 4 shorts.
    "vpadal.u8  d10, d1                        \n"  // G 8 bytes -> 4 shorts.
    "vpadal.u8  d12, d2                        \n"  // R 8 bytes -> 4 shorts.
    "vld1.8     {q0}, [%1]!                    \n"  // next 8 ARGB4444 pixels.
    ARGB4444TOARGB
    "vpadal.u8  d9, d0                         \n"  // B 8 bytes -> 4 shorts.
    "vpadal.u8  d11, d1                        \n"  // G 8 bytes -> 4 shorts.
    "vpadal.u8  d13, d2                        \n"  // R 8 bytes -> 4 shorts.

    "subs       %4, %4, #16                    \n"  // 16 processed per loop.
    "vmul.s16   q8, q4, q10                    \n"  // B
    "vmls.s16   q8, q5, q11                    \n"  // G
    "vmls.s16   q8, q6, q12                    \n"  // R
    "vadd.u16   q8, q8, q15                    \n"  // +128 -> unsigned
    "vmul.s16   q9, q6, q10                    \n"  // R
    "vmls.s16   q9, q5, q14                    \n"  // G
    "vmls.s16   q9, q4, q13                    \n"  // B
    "vadd.u16   q9, q9, q15                    \n"  // +128 -> unsigned
    "vqshrn.u16  d0, q8, #8                    \n"  // 16 bit to 8 bit U
    "vqshrn.u16  d1, q9, #8                    \n"  // 16 bit to 8 bit V
    "vst1.8     {d0}, [%2]!                    \n"  // store 8 pixels U.
    "vst1.8     {d1}, [%3]!                    \n"  // store 8 pixels V.
    "bgt        1b                             \n"
  : "+r"(src_argb4444),  // %0
    "+r"(src_stride_argb4444),  // %1
    "+r"(dst_u),     // %2
    "+r"(dst_v),     // %3
    "+r"(pix)        // %4
  :
  : "memory", "cc", "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7",
    "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
  );
}
#endif  // HAS_ARGB4444TOUVROW_NEON

#ifdef HAS_RGB565TOYROW_NEON
void RGB565ToYRow_NEON(const uint8* src_rgb565, uint8* dst_y, int pix) {
  asm volatile (
    "vmov.u8    d24, #13                       \n"  // B * 0.1016 coefficient
    "vmov.u8    d25, #65                       \n"  // G * 0.5078 coefficient
    "vmov.u8    d26, #33                       \n"  // R * 0.2578 coefficient
    "vmov.u8    d27, #16                       \n"  // Add 16 constant
    ".p2align  2                               \n"
  "1:                                          \n"
    "vld1.8     {q0}, [%0]!                    \n"  // load 8 RGB565 pixels.
    "subs       %2, %2, #8                     \n"  // 8 processed per loop.
    RGB565TOARGB
    "vmull.u8   q2, d0, d24                    \n"  // B
    "vmlal.u8   q2, d1, d25                    \n"  // G
    "vmlal.u8   q2, d2, d26                    \n"  // R
    "vqrshrun.s16 d0, q2, #7                   \n"  // 16 bit to 8 bit Y
    "vqadd.u8   d0, d27                        \n"
    "vst1.8     {d0}, [%1]!                    \n"  // store 8 pixels Y.
    "bgt        1b                             \n"
  : "+r"(src_rgb565),  // %0
    "+r"(dst_y),       // %1
    "+r"(pix)          // %2
  :
  : "memory", "cc", "q0", "q1", "q2", "q3", "q12", "q13"
  );
}
#endif  // HAS_RGB565TOYROW_NEON

#ifdef HAS_ARGB1555TOYROW_NEON
void ARGB1555ToYRow_NEON(const uint8* src_argb1555, uint8* dst_y, int pix) {
  asm volatile (
    "vmov.u8    d24, #13                       \n"  // B * 0.1016 coefficient
    "vmov.u8    d25, #65                       \n"  // G * 0.5078 coefficient
    "vmov.u8    d26, #33                       \n"  // R * 0.2578 coefficient
    "vmov.u8    d27, #16                       \n"  // Add 16 constant
    ".p2align  2                               \n"
  "1:                                          \n"
    "vld1.8     {q0}, [%0]!                    \n"  // load 8 ARGB1555 pixels.
    "subs       %2, %2, #8                     \n"  // 8 processed per loop.
    ARGB1555TOARGB
    "vmull.u8   q2, d0, d24                    \n"  // B
    "vmlal.u8   q2, d1, d25                    \n"  // G
    "vmlal.u8   q2, d2, d26                    \n"  // R
    "vqrshrun.s16 d0, q2, #7                   \n"  // 16 bit to 8 bit Y
    "vqadd.u8   d0, d27                        \n"
    "vst1.8     {d0}, [%1]!                    \n"  // store 8 pixels Y.
    "bgt        1b                             \n"
  : "+r"(src_argb1555),  // %0
    "+r"(dst_y),         // %1
    "+r"(pix)            // %2
  :
  : "memory", "cc", "q0", "q1", "q2", "q3", "q12", "q13"
  );
}
#endif  // HAS_ARGB1555TOYROW_NEON

#ifdef HAS_ARGB4444TOYROW_NEON
void ARGB4444ToYRow_NEON(const uint8* src_argb4444, uint8* dst_y, int pix) {
  asm volatile (
    "vmov.u8    d24, #13                       \n"  // B * 0.1016 coefficient
    "vmov.u8    d25, #65                       \n"  // G * 0.5078 coefficient
    "vmov.u8    d26, #33                       \n"  // R * 0.2578 coefficient
    "vmov.u8    d27, #16                       \n"  // Add 16 constant
    ".p2align  2                               \n"
  "1:                                          \n"
    "vld1.8     {q0}, [%0]!                    \n"  // load 8 ARGB4444 pixels.
    "subs       %2, %2, #8                     \n"  // 8 processed per loop.
    ARGB4444TOARGB
    "vmull.u8   q2, d0, d24                    \n"  // B
    "vmlal.u8   q2, d1, d25                    \n"  // G
    "vmlal.u8   q2, d2, d26                    \n"  // R
    "vqrshrun.s16 d0, q2, #7                   \n"  // 16 bit to 8 bit Y
    "vqadd.u8   d0, d27                        \n"
    "vst1.8     {d0}, [%1]!                    \n"  // store 8 pixels Y.
    "bgt        1b                             \n"
  : "+r"(src_argb4444),  // %0
    "+r"(dst_y),         // %1
    "+r"(pix)            // %2
  :
  : "memory", "cc", "q0", "q1", "q2", "q3", "q12", "q13"
  );
}
#endif  // HAS_ARGB4444TOYROW_NEON

#ifdef HAS_BGRATOYROW_NEON
void BGRAToYRow_NEON(const uint8* src_bgra, uint8* dst_y, int pix) {
  asm volatile (
    "vmov.u8    d4, #33                        \n"  // R * 0.2578 coefficient
    "vmov.u8    d5, #65                        \n"  // G * 0.5078 coefficient
    "vmov.u8    d6, #13                        \n"  // B * 0.1016 coefficient
    "vmov.u8    d7, #16                        \n"  // Add 16 constant
    ".p2align  2                               \n"
  "1:                                          \n"
    "vld4.8     {d0, d1, d2, d3}, [%0]!        \n"  // load 8 pixels of BGRA.
    "subs       %2, %2, #8                     \n"  // 8 processed per loop.
    "vmull.u8   q8, d1, d4                     \n"  // R
    "vmlal.u8   q8, d2, d5                     \n"  // G
    "vmlal.u8   q8, d3, d6                     \n"  // B
    "vqrshrun.s16 d0, q8, #7                   \n"  // 16 bit to 8 bit Y
    "vqadd.u8   d0, d7                         \n"
    "vst1.8     {d0}, [%1]!                    \n"  // store 8 pixels Y.
    "bgt        1b                             \n"
  : "+r"(src_bgra),  // %0
    "+r"(dst_y),     // %1
    "+r"(pix)        // %2
  :
  : "memory", "cc", "d0", "d1", "d2", "d3", "d4", "d5", "d6", "d7", "q8"
  );
}
#endif  // HAS_BGRATOYROW_NEON

#ifdef HAS_ABGRTOYROW_NEON
void ABGRToYRow_NEON(const uint8* src_abgr, uint8* dst_y, int pix) {
  asm volatile (
    "vmov.u8    d4, #33                        \n"  // R * 0.2578 coefficient
    "vmov.u8    d5, #65                        \n"  // G * 0.5078 coefficient
    "vmov.u8    d6, #13                        \n"  // B * 0.1016 coefficient
    "vmov.u8    d7, #16                        \n"  // Add 16 constant
    ".p2align  2                               \n"
  "1:                                          \n"
    "vld4.8     {d0, d1, d2, d3}, [%0]!        \n"  // load 8 pixels of ABGR.
    "subs       %2, %2, #8                     \n"  // 8 processed per loop.
    "vmull.u8   q8, d0, d4                     \n"  // R
    "vmlal.u8   q8, d1, d5                     \n"  // G
    "vmlal.u8   q8, d2, d6                     \n"  // B
    "vqrshrun.s16 d0, q8, #7                   \n"  // 16 bit to 8 bit Y
    "vqadd.u8   d0, d7                         \n"
    "vst1.8     {d0}, [%1]!                    \n"  // store 8 pixels Y.
    "bgt        1b                             \n"
  : "+r"(src_abgr),  // %0
    "+r"(dst_y),  // %1
    "+r"(pix)        // %2
  :
  : "memory", "cc", "d0", "d1", "d2", "d3", "d4", "d5", "d6", "d7", "q8"
  );
}
#endif  // HAS_ABGRTOYROW_NEON

#ifdef HAS_RGBATOYROW_NEON
void RGBAToYRow_NEON(const uint8* src_rgba, uint8* dst_y, int pix) {
  asm volatile (
    "vmov.u8    d4, #13                        \n"  // B * 0.1016 coefficient
    "vmov.u8    d5, #65                        \n"  // G * 0.5078 coefficient
    "vmov.u8    d6, #33                        \n"  // R * 0.2578 coefficient
    "vmov.u8    d7, #16                        \n"  // Add 16 constant
    ".p2align  2                               \n"
  "1:                                          \n"
    "vld4.8     {d0, d1, d2, d3}, [%0]!        \n"  // load 8 pixels of RGBA.
    "subs       %2, %2, #8                     \n"  // 8 processed per loop.
    "vmull.u8   q8, d1, d4                     \n"  // B
    "vmlal.u8   q8, d2, d5                     \n"  // G
    "vmlal.u8   q8, d3, d6                     \n"  // R
    "vqrshrun.s16 d0, q8, #7                   \n"  // 16 bit to 8 bit Y
    "vqadd.u8   d0, d7                         \n"
    "vst1.8     {d0}, [%1]!                    \n"  // store 8 pixels Y.
    "bgt        1b                             \n"
  : "+r"(src_rgba),  // %0
    "+r"(dst_y),  // %1
    "+r"(pix)        // %2
  :
  : "memory", "cc", "d0", "d1", "d2", "d3", "d4", "d5", "d6", "d7", "q8"
  );
}
#endif  // HAS_RGBATOYROW_NEON

#ifdef HAS_RGB24TOYROW_NEON
void RGB24ToYRow_NEON(const uint8* src_rgb24, uint8* dst_y, int pix) {
  asm volatile (
    "vmov.u8    d4, #13                        \n"  // B * 0.1016 coefficient
    "vmov.u8    d5, #65                        \n"  // G * 0.5078 coefficient
    "vmov.u8    d6, #33                        \n"  // R * 0.2578 coefficient
    "vmov.u8    d7, #16                        \n"  // Add 16 constant
    ".p2align  2                               \n"
  "1:                                          \n"
    "vld3.8     {d0, d1, d2}, [%0]!            \n"  // load 8 pixels of RGB24.
    "subs       %2, %2, #8                     \n"  // 8 processed per loop.
    "vmull.u8   q8, d0, d4                     \n"  // B
    "vmlal.u8   q8, d1, d5                     \n"  // G
    "vmlal.u8   q8, d2, d6                     \n"  // R
    "vqrshrun.s16 d0, q8, #7                   \n"  // 16 bit to 8 bit Y
    "vqadd.u8   d0, d7                         \n"
    "vst1.8     {d0}, [%1]!                    \n"  // store 8 pixels Y.
    "bgt        1b                             \n"
  : "+r"(src_rgb24),  // %0
    "+r"(dst_y),  // %1
    "+r"(pix)        // %2
  :
  : "memory", "cc", "d0", "d1", "d2", "d3", "d4", "d5", "d6", "d7", "q8"
  );
}
#endif  // HAS_RGB24TOYROW_NEON

#ifdef HAS_RAWTOYROW_NEON
void RAWToYRow_NEON(const uint8* src_raw, uint8* dst_y, int pix) {
  asm volatile (
    "vmov.u8    d4, #33                        \n"  // R * 0.2578 coefficient
    "vmov.u8    d5, #65                        \n"  // G * 0.5078 coefficient
    "vmov.u8    d6, #13                        \n"  // B * 0.1016 coefficient
    "vmov.u8    d7, #16                        \n"  // Add 16 constant
    ".p2align  2                               \n"
  "1:                                          \n"
    "vld3.8     {d0, d1, d2}, [%0]!            \n"  // load 8 pixels of RAW.
    "subs       %2, %2, #8                     \n"  // 8 processed per loop.
    "vmull.u8   q8, d0, d4                     \n"  // B
    "vmlal.u8   q8, d1, d5                     \n"  // G
    "vmlal.u8   q8, d2, d6                     \n"  // R
    "vqrshrun.s16 d0, q8, #7                   \n"  // 16 bit to 8 bit Y
    "vqadd.u8   d0, d7                         \n"
    "vst1.8     {d0}, [%1]!                    \n"  // store 8 pixels Y.
    "bgt        1b                             \n"
  : "+r"(src_raw),  // %0
    "+r"(dst_y),  // %1
    "+r"(pix)        // %2
  :
  : "memory", "cc", "d0", "d1", "d2", "d3", "d4", "d5", "d6", "d7", "q8"
  );
}
#endif  // HAS_RAWTOYROW_NEON

#endif  // __ARM_NEON__

#ifdef __cplusplus
}  // extern "C"
}  // namespace libyuv
#endif

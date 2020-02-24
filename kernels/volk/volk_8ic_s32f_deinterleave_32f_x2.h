/* -*- c++ -*- */
/*
 * Copyright 2012, 2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

/*!
 * \page volk_8ic_s32f_deinterleave_32f_x2
 *
 * \b Overview
 *
 * Deinterleaves the complex 8-bit char vector into I & Q vector data,
 * converts them to floats, and divides the results by the scalar
 * factor.
 *
 * <b>Dispatcher Prototype</b>
 * \code
 * void volk_8ic_s32f_deinterleave_32f_x2(float* iBuffer, float* qBuffer, const lv_8sc_t* complexVector, const float scalar, unsigned int num_points)
 * \endcode
 *
 * \b Inputs
 * \li complexVector: The complex input vector.
 * \li scalar: The scalar value used to divide the floating point results.
 * \li num_points: The number of complex data values to be deinterleaved.
 *
 * \b Outputs
 * \li iBuffer: The I buffer output data.
 * \li qBuffer: The Q buffer output data.
 *
 * \b Example
 * \code
 * int N = 10000;
 *
 * volk_8ic_s32f_deinterleave_32f_x2();
 *
 * volk_free(x);
 * \endcode
 */

#ifndef INCLUDED_volk_8ic_s32f_deinterleave_32f_x2_a_H
#define INCLUDED_volk_8ic_s32f_deinterleave_32f_x2_a_H

#include <volk/volk_common.h>
#include <inttypes.h>
#include <stdio.h>


#ifdef LV_HAVE_SSE4_1
#include <smmintrin.h>

static inline void
volk_8ic_s32f_deinterleave_32f_x2_a_sse4_1(float* iBuffer, float* qBuffer, const lv_8sc_t* complexVector,
                                           const float scalar, unsigned int num_points)
{
  float* iBufferPtr = iBuffer;
  float* qBufferPtr = qBuffer;

  unsigned int number = 0;
  const unsigned int eighthPoints = num_points / 8;
  __m128 iFloatValue, qFloatValue;

  const float iScalar= 1.0 / scalar;
  __m128 invScalar = _mm_set_ps1(iScalar);
  __m128i complexVal, iIntVal, qIntVal, iComplexVal, qComplexVal;
  int8_t* complexVectorPtr = (int8_t*)complexVector;

  __m128i iMoveMask = _mm_set_epi8(0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 14, 12, 10, 8, 6, 4, 2, 0);
  __m128i qMoveMask = _mm_set_epi8(0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 15, 13, 11, 9, 7, 5, 3, 1);

  for(;number < eighthPoints; number++){
    complexVal = _mm_load_si128((__m128i*)complexVectorPtr); complexVectorPtr += 16;
    iComplexVal = _mm_shuffle_epi8(complexVal, iMoveMask);
    qComplexVal = _mm_shuffle_epi8(complexVal, qMoveMask);

    iIntVal = _mm_cvtepi8_epi32(iComplexVal);
    iFloatValue = _mm_cvtepi32_ps(iIntVal);
    iFloatValue = _mm_mul_ps(iFloatValue, invScalar);
    _mm_store_ps(iBufferPtr, iFloatValue);
    iBufferPtr += 4;

    iComplexVal = _mm_srli_si128(iComplexVal, 4);

    iIntVal = _mm_cvtepi8_epi32(iComplexVal);
    iFloatValue = _mm_cvtepi32_ps(iIntVal);
    iFloatValue = _mm_mul_ps(iFloatValue, invScalar);
    _mm_store_ps(iBufferPtr, iFloatValue);
    iBufferPtr += 4;

    qIntVal = _mm_cvtepi8_epi32(qComplexVal);
    qFloatValue = _mm_cvtepi32_ps(qIntVal);
    qFloatValue = _mm_mul_ps(qFloatValue, invScalar);
    _mm_store_ps(qBufferPtr, qFloatValue);
    qBufferPtr += 4;

    qComplexVal = _mm_srli_si128(qComplexVal, 4);

    qIntVal = _mm_cvtepi8_epi32(qComplexVal);
    qFloatValue = _mm_cvtepi32_ps(qIntVal);
    qFloatValue = _mm_mul_ps(qFloatValue, invScalar);
    _mm_store_ps(qBufferPtr, qFloatValue);

    qBufferPtr += 4;
  }

  number = eighthPoints * 8;
  for(; number < num_points; number++){
    *iBufferPtr++ = (float)(*complexVectorPtr++) * iScalar;
    *qBufferPtr++ = (float)(*complexVectorPtr++) * iScalar;
  }

}
#endif /* LV_HAVE_SSE4_1 */


#ifdef LV_HAVE_SSE
#include <xmmintrin.h>

static inline void
volk_8ic_s32f_deinterleave_32f_x2_a_sse(float* iBuffer, float* qBuffer,
                                        const lv_8sc_t* complexVector,
                                        const float scalar, unsigned int num_points)
{
  float* iBufferPtr = iBuffer;
  float* qBufferPtr = qBuffer;

  unsigned int number = 0;
  const unsigned int quarterPoints = num_points / 4;
  __m128 cplxValue1, cplxValue2, iValue, qValue;

  __m128 invScalar = _mm_set_ps1(1.0/scalar);
  int8_t* complexVectorPtr = (int8_t*)complexVector;

  __VOLK_ATTR_ALIGNED(16) float floatBuffer[8];

  for(;number < quarterPoints; number++){
    floatBuffer[0] = (float)(complexVectorPtr[0]);
    floatBuffer[1] = (float)(complexVectorPtr[1]);
    floatBuffer[2] = (float)(complexVectorPtr[2]);
    floatBuffer[3] = (float)(complexVectorPtr[3]);

    floatBuffer[4] = (float)(complexVectorPtr[4]);
    floatBuffer[5] = (float)(complexVectorPtr[5]);
    floatBuffer[6] = (float)(complexVectorPtr[6]);
    floatBuffer[7] = (float)(complexVectorPtr[7]);

    cplxValue1 = _mm_load_ps(&floatBuffer[0]);
    cplxValue2 = _mm_load_ps(&floatBuffer[4]);

    complexVectorPtr += 8;

    cplxValue1 = _mm_mul_ps(cplxValue1, invScalar);
    cplxValue2 = _mm_mul_ps(cplxValue2, invScalar);

    // Arrange in i1i2i3i4 format
    iValue = _mm_shuffle_ps(cplxValue1, cplxValue2, _MM_SHUFFLE(2,0,2,0));
    qValue = _mm_shuffle_ps(cplxValue1, cplxValue2, _MM_SHUFFLE(3,1,3,1));

    _mm_store_ps(iBufferPtr, iValue);
    _mm_store_ps(qBufferPtr, qValue);

    iBufferPtr += 4;
    qBufferPtr += 4;
  }

  number = quarterPoints * 4;
  complexVectorPtr = (int8_t*)&complexVector[number];
  for(; number < num_points; number++){
    *iBufferPtr++ = (float)(*complexVectorPtr++) / scalar;
    *qBufferPtr++ = (float)(*complexVectorPtr++) / scalar;
  }
}
#endif /* LV_HAVE_SSE */


#ifdef LV_HAVE_AVX2
#include <immintrin.h>

static inline void
volk_8ic_s32f_deinterleave_32f_x2_a_avx2(float* iBuffer, float* qBuffer, const lv_8sc_t* complexVector,
                                           const float scalar, unsigned int num_points)
{
  float* iBufferPtr = iBuffer;
  float* qBufferPtr = qBuffer;

  unsigned int number = 0;
  const unsigned int sixteenthPoints = num_points / 16;
  __m256 iFloatValue, qFloatValue;

  const float iScalar= 1.0 / scalar;
  __m256 invScalar = _mm256_set1_ps(iScalar);
  __m256i complexVal, iIntVal, qIntVal, iComplexVal, qComplexVal;
  int8_t* complexVectorPtr = (int8_t*)complexVector;

  __m256i iMoveMask = _mm256_set_epi8(0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
                                      14, 12, 10, 8, 6, 4, 2, 0,
                                      0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
                                      14, 12, 10, 8, 6, 4, 2, 0);
  __m256i qMoveMask = _mm256_set_epi8(0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
                                      15, 13, 11, 9, 7, 5, 3, 1,
                                      0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
                                      15, 13, 11, 9, 7, 5, 3, 1);

  for(;number < sixteenthPoints; number++){
    complexVal = _mm256_load_si256((__m256i*)complexVectorPtr);
    complexVectorPtr += 32;
    iComplexVal = _mm256_shuffle_epi8(complexVal, iMoveMask);
    qComplexVal = _mm256_shuffle_epi8(complexVal, qMoveMask);

    iIntVal     = _mm256_cvtepi8_epi32(_mm256_castsi256_si128(iComplexVal));
    iFloatValue = _mm256_cvtepi32_ps(iIntVal);
    iFloatValue = _mm256_mul_ps(iFloatValue, invScalar);
    _mm256_store_ps(iBufferPtr, iFloatValue);
    iBufferPtr += 8;

    iComplexVal = _mm256_permute4x64_epi64(iComplexVal, 0b11000110);
    iIntVal     = _mm256_cvtepi8_epi32(_mm256_castsi256_si128(iComplexVal));
    iFloatValue = _mm256_cvtepi32_ps(iIntVal);
    iFloatValue = _mm256_mul_ps(iFloatValue, invScalar);
    _mm256_store_ps(iBufferPtr, iFloatValue);
    iBufferPtr += 8;

    qIntVal     = _mm256_cvtepi8_epi32(_mm256_castsi256_si128(qComplexVal));
    qFloatValue = _mm256_cvtepi32_ps(qIntVal);
    qFloatValue = _mm256_mul_ps(qFloatValue, invScalar);
    _mm256_store_ps(qBufferPtr, qFloatValue);
    qBufferPtr += 8;

    qComplexVal = _mm256_permute4x64_epi64(qComplexVal, 0b11000110);
    qIntVal     = _mm256_cvtepi8_epi32(_mm256_castsi256_si128(qComplexVal));
    qFloatValue = _mm256_cvtepi32_ps(qIntVal);
    qFloatValue = _mm256_mul_ps(qFloatValue, invScalar);
    _mm256_store_ps(qBufferPtr, qFloatValue);
    qBufferPtr += 8;
  }

  number = sixteenthPoints * 16;
  for(; number < num_points; number++){
    *iBufferPtr++ = (float)(*complexVectorPtr++) * iScalar;
    *qBufferPtr++ = (float)(*complexVectorPtr++) * iScalar;
  }

}
#endif /* LV_HAVE_AVX2 */


#ifdef LV_HAVE_GENERIC

static inline void
volk_8ic_s32f_deinterleave_32f_x2_generic(float* iBuffer, float* qBuffer,
                                          const lv_8sc_t* complexVector,
                                          const float scalar, unsigned int num_points)
{
  const int8_t* complexVectorPtr = (const int8_t*)complexVector;
  float* iBufferPtr = iBuffer;
  float* qBufferPtr = qBuffer;
  unsigned int number;
  const float invScalar = 1.0 / scalar;
  for(number = 0; number < num_points; number++){
    *iBufferPtr++ = (float)(*complexVectorPtr++)*invScalar;
    *qBufferPtr++ = (float)(*complexVectorPtr++)*invScalar;
  }
}
#endif /* LV_HAVE_GENERIC */


#endif /* INCLUDED_volk_8ic_s32f_deinterleave_32f_x2_a_H */


#ifndef INCLUDED_volk_8ic_s32f_deinterleave_32f_x2_u_H
#define INCLUDED_volk_8ic_s32f_deinterleave_32f_x2_u_H

#include <volk/volk_common.h>
#include <inttypes.h>
#include <stdio.h>

#ifdef LV_HAVE_AVX2
#include <immintrin.h>

static inline void
volk_8ic_s32f_deinterleave_32f_x2_u_avx2(float* iBuffer, float* qBuffer, const lv_8sc_t* complexVector,
                                           const float scalar, unsigned int num_points)
{
  float* iBufferPtr = iBuffer;
  float* qBufferPtr = qBuffer;

  unsigned int number = 0;
  const unsigned int sixteenthPoints = num_points / 16;
  __m256 iFloatValue, qFloatValue;

  const float iScalar= 1.0 / scalar;
  __m256 invScalar = _mm256_set1_ps(iScalar);
  __m256i complexVal, iIntVal, qIntVal;
  __m128i iComplexVal, qComplexVal;
  int8_t* complexVectorPtr = (int8_t*)complexVector;

  __m256i MoveMask = _mm256_set_epi8(15, 13, 11, 9, 7, 5, 3, 1, 14, 12, 10, 8,
      6, 4, 2, 0,15, 13, 11, 9, 7, 5, 3, 1, 14, 12, 10, 8, 6, 4, 2, 0);

  for(;number < sixteenthPoints; number++){
    complexVal = _mm256_loadu_si256((__m256i*)complexVectorPtr); complexVectorPtr += 32;
    complexVal = _mm256_shuffle_epi8(complexVal, MoveMask);
    complexVal = _mm256_permute4x64_epi64(complexVal,0xd8);
    iComplexVal = _mm256_extractf128_si256(complexVal,0);
    qComplexVal = _mm256_extractf128_si256(complexVal,1);

    iIntVal = _mm256_cvtepi8_epi32(iComplexVal);
    iFloatValue = _mm256_cvtepi32_ps(iIntVal);
    iFloatValue = _mm256_mul_ps(iFloatValue, invScalar);
    _mm256_storeu_ps(iBufferPtr, iFloatValue);
    iBufferPtr += 8;

    qIntVal = _mm256_cvtepi8_epi32(qComplexVal);
    qFloatValue = _mm256_cvtepi32_ps(qIntVal);
    qFloatValue = _mm256_mul_ps(qFloatValue, invScalar);
    _mm256_storeu_ps(qBufferPtr, qFloatValue);
    qBufferPtr += 8;

    complexVal = _mm256_srli_si256(complexVal, 8);
    iComplexVal = _mm256_extractf128_si256(complexVal,0);
    qComplexVal = _mm256_extractf128_si256(complexVal,1);

    iIntVal = _mm256_cvtepi8_epi32(iComplexVal);
    iFloatValue = _mm256_cvtepi32_ps(iIntVal);
    iFloatValue = _mm256_mul_ps(iFloatValue, invScalar);
    _mm256_storeu_ps(iBufferPtr, iFloatValue);
    iBufferPtr += 8;

    qIntVal = _mm256_cvtepi8_epi32(qComplexVal);
    qFloatValue = _mm256_cvtepi32_ps(qIntVal);
    qFloatValue = _mm256_mul_ps(qFloatValue, invScalar);
    _mm256_storeu_ps(qBufferPtr, qFloatValue);
    qBufferPtr += 8;
  }

  number = sixteenthPoints * 16;
  for(; number < num_points; number++){
    *iBufferPtr++ = (float)(*complexVectorPtr++) * iScalar;
    *qBufferPtr++ = (float)(*complexVectorPtr++) * iScalar;
  }

}
#endif /* LV_HAVE_AVX2 */

#endif /* INCLUDED_volk_8ic_s32f_deinterleave_32f_x2_u_H */

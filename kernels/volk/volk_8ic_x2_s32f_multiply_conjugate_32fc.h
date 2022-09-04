/* -*- c++ -*- */
/*
 * Copyright 2012, 2014 Free Software Foundation, Inc.
 *
 * This file is part of VOLK
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

/*!
 * \page volk_8ic_x2_s32f_multiply_conjugate_32fc
 *
 * \b Overview
 *
 * Multiplys the one complex vector with the complex conjugate of the
 * second complex vector and stores their results in the third vector
 *
 * <b>Dispatcher Prototype</b>
 * \code
 * void volk_8ic_x2_s32f_multiply_conjugate_32fc(lv_32fc_t* cVector, const lv_8sc_t*
 * aVector, const lv_8sc_t* bVector, const float scalar, unsigned int num_points) \endcode
 *
 * \b Inputs
 * \li aVector: One of the complex vectors to be multiplied.
 * \li bVector: The complex vector which will be converted to complex conjugate and
 * multiplied. \li scalar: each output value is scaled by 1/scalar. \li num_points: The
 * number of complex values in aVector and bVector to be multiplied together and stored
 * into cVector.
 *
 * \b Outputs
 * \li cVector: The complex vector where the results will be stored.
 *
 * \b Example
 * \code
 * int N = 10000;
 *
 * <FIXME>
 *
 * volk_8ic_x2_s32f_multiply_conjugate_32fc();
 *
 * \endcode
 */

#ifndef INCLUDED_volk_8ic_x2_s32f_multiply_conjugate_32fc_a_H
#define INCLUDED_volk_8ic_x2_s32f_multiply_conjugate_32fc_a_H

#include <inttypes.h>
#include <stdio.h>
#include <volk/volk_complex.h>

#ifdef LV_HAVE_AVX2
#include <immintrin.h>

static inline void
volk_8ic_x2_s32f_multiply_conjugate_32fc_a_avx2(lv_32fc_t* cVector,
                                                const lv_8sc_t* aVector,
                                                const lv_8sc_t* bVector,
                                                const float scalar,
                                                unsigned int num_points)
{
    unsigned int number = 0;
    const unsigned int oneEigthPoints = num_points / 8;

    __m256i x, y, realz, imagz;
    __m256 ret, retlo, rethi;
    lv_32fc_t* c = cVector;
    const lv_8sc_t* a = aVector;
    const lv_8sc_t* b = bVector;
    __m256i conjugateSign =
        _mm256_set_epi16(-1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1);

    __m256 invScalar = _mm256_set1_ps(1.0 / scalar);

    for (; number < oneEigthPoints; number++) {
        // Convert  8 bit values into 16 bit values
        x = _mm256_cvtepi8_epi16(_mm_load_si128((__m128i*)a));
        y = _mm256_cvtepi8_epi16(_mm_load_si128((__m128i*)b));

        // Calculate the ar*cr - ai*(-ci) portions
        realz = _mm256_madd_epi16(x, y);

        // Calculate the complex conjugate of the cr + ci j values
        y = _mm256_sign_epi16(y, conjugateSign);

        // Shift the order of the cr and ci values
        y = _mm256_shufflehi_epi16(_mm256_shufflelo_epi16(y, _MM_SHUFFLE(2, 3, 0, 1)),
                                   _MM_SHUFFLE(2, 3, 0, 1));

        // Calculate the ar*(-ci) + cr*(ai)
        imagz = _mm256_madd_epi16(x, y);

        // Interleave real and imaginary and then convert to float values
        retlo = _mm256_cvtepi32_ps(_mm256_unpacklo_epi32(realz, imagz));

        // Normalize the floating point values
        retlo = _mm256_mul_ps(retlo, invScalar);

        // Interleave real and imaginary and then convert to float values
        rethi = _mm256_cvtepi32_ps(_mm256_unpackhi_epi32(realz, imagz));

        // Normalize the floating point values
        rethi = _mm256_mul_ps(rethi, invScalar);

        ret = _mm256_permute2f128_ps(retlo, rethi, 0b00100000);
        _mm256_store_ps((float*)c, ret);
        c += 4;

        ret = _mm256_permute2f128_ps(retlo, rethi, 0b00110001);
        _mm256_store_ps((float*)c, ret);
        c += 4;

        a += 8;
        b += 8;
    }

    number = oneEigthPoints * 8;
    float* cFloatPtr = (float*)&cVector[number];
    int8_t* a8Ptr = (int8_t*)&aVector[number];
    int8_t* b8Ptr = (int8_t*)&bVector[number];
    for (; number < num_points; number++) {
        float aReal = (float)*a8Ptr++;
        float aImag = (float)*a8Ptr++;
        lv_32fc_t aVal = lv_cmake(aReal, aImag);
        float bReal = (float)*b8Ptr++;
        float bImag = (float)*b8Ptr++;
        lv_32fc_t bVal = lv_cmake(bReal, -bImag);
        lv_32fc_t temp = aVal * bVal;

        *cFloatPtr++ = lv_creal(temp) / scalar;
        *cFloatPtr++ = lv_cimag(temp) / scalar;
    }
}
#endif /* LV_HAVE_AVX2*/


#ifdef LV_HAVE_SSE4_1
#include <smmintrin.h>

static inline void
volk_8ic_x2_s32f_multiply_conjugate_32fc_a_sse4_1(lv_32fc_t* cVector,
                                                  const lv_8sc_t* aVector,
                                                  const lv_8sc_t* bVector,
                                                  const float scalar,
                                                  unsigned int num_points)
{
    unsigned int number = 0;
    const unsigned int quarterPoints = num_points / 4;

    __m128i x, y, realz, imagz;
    __m128 ret;
    lv_32fc_t* c = cVector;
    const lv_8sc_t* a = aVector;
    const lv_8sc_t* b = bVector;
    __m128i conjugateSign = _mm_set_epi16(-1, 1, -1, 1, -1, 1, -1, 1);

    __m128 invScalar = _mm_set_ps1(1.0 / scalar);

    for (; number < quarterPoints; number++) {
        // Convert into 8 bit values into 16 bit values
        x = _mm_cvtepi8_epi16(_mm_loadl_epi64((__m128i*)a));
        y = _mm_cvtepi8_epi16(_mm_loadl_epi64((__m128i*)b));

        // Calculate the ar*cr - ai*(-ci) portions
        realz = _mm_madd_epi16(x, y);

        // Calculate the complex conjugate of the cr + ci j values
        y = _mm_sign_epi16(y, conjugateSign);

        // Shift the order of the cr and ci values
        y = _mm_shufflehi_epi16(_mm_shufflelo_epi16(y, _MM_SHUFFLE(2, 3, 0, 1)),
                                _MM_SHUFFLE(2, 3, 0, 1));

        // Calculate the ar*(-ci) + cr*(ai)
        imagz = _mm_madd_epi16(x, y);

        // Interleave real and imaginary and then convert to float values
        ret = _mm_cvtepi32_ps(_mm_unpacklo_epi32(realz, imagz));

        // Normalize the floating point values
        ret = _mm_mul_ps(ret, invScalar);

        // Store the floating point values
        _mm_store_ps((float*)c, ret);
        c += 2;

        // Interleave real and imaginary and then convert to float values
        ret = _mm_cvtepi32_ps(_mm_unpackhi_epi32(realz, imagz));

        // Normalize the floating point values
        ret = _mm_mul_ps(ret, invScalar);

        // Store the floating point values
        _mm_store_ps((float*)c, ret);
        c += 2;

        a += 4;
        b += 4;
    }

    number = quarterPoints * 4;
    float* cFloatPtr = (float*)&cVector[number];
    int8_t* a8Ptr = (int8_t*)&aVector[number];
    int8_t* b8Ptr = (int8_t*)&bVector[number];
    for (; number < num_points; number++) {
        float aReal = (float)*a8Ptr++;
        float aImag = (float)*a8Ptr++;
        lv_32fc_t aVal = lv_cmake(aReal, aImag);
        float bReal = (float)*b8Ptr++;
        float bImag = (float)*b8Ptr++;
        lv_32fc_t bVal = lv_cmake(bReal, -bImag);
        lv_32fc_t temp = aVal * bVal;

        *cFloatPtr++ = lv_creal(temp) / scalar;
        *cFloatPtr++ = lv_cimag(temp) / scalar;
    }
}
#endif /* LV_HAVE_SSE4_1 */


#ifdef LV_HAVE_GENERIC

static inline void
volk_8ic_x2_s32f_multiply_conjugate_32fc_generic(lv_32fc_t* cVector,
                                                 const lv_8sc_t* aVector,
                                                 const lv_8sc_t* bVector,
                                                 const float scalar,
                                                 unsigned int num_points)
{
    unsigned int number = 0;
    float* cPtr = (float*)cVector;
    const float invScalar = 1.0 / scalar;
    int8_t* a8Ptr = (int8_t*)aVector;
    int8_t* b8Ptr = (int8_t*)bVector;
    for (number = 0; number < num_points; number++) {
        float aReal = (float)*a8Ptr++;
        float aImag = (float)*a8Ptr++;
        lv_32fc_t aVal = lv_cmake(aReal, aImag);
        float bReal = (float)*b8Ptr++;
        float bImag = (float)*b8Ptr++;
        lv_32fc_t bVal = lv_cmake(bReal, -bImag);
        lv_32fc_t temp = aVal * bVal;

        *cPtr++ = (lv_creal(temp) * invScalar);
        *cPtr++ = (lv_cimag(temp) * invScalar);
    }
}
#endif /* LV_HAVE_GENERIC */


#endif /* INCLUDED_volk_8ic_x2_s32f_multiply_conjugate_32fc_a_H */

#ifndef INCLUDED_volk_8ic_x2_s32f_multiply_conjugate_32fc_u_H
#define INCLUDED_volk_8ic_x2_s32f_multiply_conjugate_32fc_u_H

#include <inttypes.h>
#include <stdio.h>
#include <volk/volk_complex.h>

#ifdef LV_HAVE_AVX2
#include <immintrin.h>

static inline void
volk_8ic_x2_s32f_multiply_conjugate_32fc_u_avx2(lv_32fc_t* cVector,
                                                const lv_8sc_t* aVector,
                                                const lv_8sc_t* bVector,
                                                const float scalar,
                                                unsigned int num_points)
{
    unsigned int number = 0;
    const unsigned int oneEigthPoints = num_points / 8;

    __m256i x, y, realz, imagz;
    __m256 ret, retlo, rethi;
    lv_32fc_t* c = cVector;
    const lv_8sc_t* a = aVector;
    const lv_8sc_t* b = bVector;
    __m256i conjugateSign =
        _mm256_set_epi16(-1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1);

    __m256 invScalar = _mm256_set1_ps(1.0 / scalar);

    for (; number < oneEigthPoints; number++) {
        // Convert  8 bit values into 16 bit values
        x = _mm256_cvtepi8_epi16(_mm_loadu_si128((__m128i*)a));
        y = _mm256_cvtepi8_epi16(_mm_loadu_si128((__m128i*)b));

        // Calculate the ar*cr - ai*(-ci) portions
        realz = _mm256_madd_epi16(x, y);

        // Calculate the complex conjugate of the cr + ci j values
        y = _mm256_sign_epi16(y, conjugateSign);

        // Shift the order of the cr and ci values
        y = _mm256_shufflehi_epi16(_mm256_shufflelo_epi16(y, _MM_SHUFFLE(2, 3, 0, 1)),
                                   _MM_SHUFFLE(2, 3, 0, 1));

        // Calculate the ar*(-ci) + cr*(ai)
        imagz = _mm256_madd_epi16(x, y);

        // Interleave real and imaginary and then convert to float values
        retlo = _mm256_cvtepi32_ps(_mm256_unpacklo_epi32(realz, imagz));

        // Normalize the floating point values
        retlo = _mm256_mul_ps(retlo, invScalar);

        // Interleave real and imaginary and then convert to float values
        rethi = _mm256_cvtepi32_ps(_mm256_unpackhi_epi32(realz, imagz));

        // Normalize the floating point values
        rethi = _mm256_mul_ps(rethi, invScalar);

        ret = _mm256_permute2f128_ps(retlo, rethi, 0b00100000);
        _mm256_storeu_ps((float*)c, ret);
        c += 4;

        ret = _mm256_permute2f128_ps(retlo, rethi, 0b00110001);
        _mm256_storeu_ps((float*)c, ret);
        c += 4;

        a += 8;
        b += 8;
    }

    number = oneEigthPoints * 8;
    float* cFloatPtr = (float*)&cVector[number];
    int8_t* a8Ptr = (int8_t*)&aVector[number];
    int8_t* b8Ptr = (int8_t*)&bVector[number];
    for (; number < num_points; number++) {
        float aReal = (float)*a8Ptr++;
        float aImag = (float)*a8Ptr++;
        lv_32fc_t aVal = lv_cmake(aReal, aImag);
        float bReal = (float)*b8Ptr++;
        float bImag = (float)*b8Ptr++;
        lv_32fc_t bVal = lv_cmake(bReal, -bImag);
        lv_32fc_t temp = aVal * bVal;

        *cFloatPtr++ = lv_creal(temp) / scalar;
        *cFloatPtr++ = lv_cimag(temp) / scalar;
    }
}
#endif /* LV_HAVE_AVX2*/


#endif /* INCLUDED_volk_8ic_x2_s32f_multiply_conjugate_32fc_u_H */

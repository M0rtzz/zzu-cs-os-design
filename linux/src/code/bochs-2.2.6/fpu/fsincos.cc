/*============================================================================
This source file is an extension to the SoftFloat IEC/IEEE Floating-point
Arithmetic Package, Release 2b, written for Bochs (x86 achitecture simulator)
floating point emulation.

THIS SOFTWARE IS DISTRIBUTED AS IS, FOR FREE.  Although reasonable effort has
been made to avoid it, THIS SOFTWARE MAY CONTAIN FAULTS THAT WILL AT TIMES
RESULT IN INCORRECT BEHAVIOR.  USE OF THIS SOFTWARE IS RESTRICTED TO PERSONS
AND ORGANIZATIONS WHO CAN AND WILL TAKE FULL RESPONSIBILITY FOR ALL LOSSES,
COSTS, OR OTHER PROBLEMS THEY INCUR DUE TO THE SOFTWARE, AND WHO FURTHERMORE
EFFECTIVELY INDEMNIFY JOHN HAUSER AND THE INTERNATIONAL COMPUTER SCIENCE
INSTITUTE (possibly via similar legal warning) AGAINST ALL LOSSES, COSTS, OR
OTHER PROBLEMS INCURRED BY THEIR CUSTOMERS AND CLIENTS DUE TO THE SOFTWARE.

Derivative works are acceptable, even for commercial purposes, so long as
(1) the source code for the derivative work includes prominent notice that
the work is derivative, and (2) the source code includes prominent notice with
these four paragraphs for those parts of this code that are retained.
=============================================================================*/

/*============================================================================
 * Written for Bochs (x86 achitecture simulator) by
 *            Stanislav Shwartsman (stl at fidonet.org.il)
 * ==========================================================================*/ 

#define FLOAT128

#include "softfloatx80.h"
#include "softfloat-round-pack.h"
#include "fpu_constant.h"

static const floatx80 floatx80_one = packFloatx80(0, 0x3fff, BX_CONST64(0x8000000000000000));

/* reduce trigonometric function argument using 128-bit precision 
   M_PI approximation */
static Bit64u argument_reduction_kernel(Bit64u aSig0, int Exp, Bit64u *zSig0, Bit64u *zSig1)
{
    Bit64u term0, term1, term2;
    Bit64u aSig1 = 0;

    shortShift128Left(aSig1, aSig0, Exp, &aSig1, &aSig0);
    Bit64u q = estimateDiv128To64(aSig1, aSig0, FLOAT_PI_HI);
    mul128By64To192(FLOAT_PI_HI, FLOAT_PI_LO, q, &term0, &term1, &term2);
    sub128(aSig1, aSig0, term0, term1, zSig1, zSig0);
    while ((Bit64s)(*zSig1) < 0) {
        --q;
        add192(*zSig1, *zSig0, term2, 0, FLOAT_PI_HI, FLOAT_PI_LO, zSig1, zSig0, &term2);
    }
    *zSig1 = term2;
    return q;
}

static int reduce_trig_arg(int expDiff, int &zSign, Bit64u &aSig0, Bit64u &aSig1)
{
    Bit64u term0, term1, q = 0;

    if (expDiff < 0) {
        shift128Right(aSig0, 0, 1, &aSig0, &aSig1);
        expDiff = 0;
    }
    if (expDiff > 0) {
        q = argument_reduction_kernel(aSig0, expDiff, &aSig0, &aSig1);
    }
    else {
        if (FLOAT_PI_HI <= aSig0) {
            aSig0 -= FLOAT_PI_HI;
            q = 1;
        }
    }

    shift128Right(FLOAT_PI_HI, FLOAT_PI_LO, 1, &term0, &term1);
    if (! lt128(aSig0, aSig1, term0, term1))
    {
        int lt = lt128(term0, term1, aSig0, aSig1);
        int eq = eq128(aSig0, aSig1, term0, term1);
              
        if ((eq && (q & 1)) || lt) {
            zSign = !zSign;
            ++q;
        }
        if (lt) sub128(FLOAT_PI_HI, FLOAT_PI_LO, aSig0, aSig1, &aSig0, &aSig1);
    }

    return (int)(q & 3);
}

#define SIN_ARR_SIZE 9
#define COS_ARR_SIZE 9

static float128 sin_arr[SIN_ARR_SIZE] =
{
    PACK_FLOAT_128(0x3fff000000000000, 0x0000000000000000), /*  1 */
    PACK_FLOAT_128(0xbffc555555555555, 0x5555555555555555), /*  3 */
    PACK_FLOAT_128(0x3ff8111111111111, 0x1111111111111111), /*  5 */
    PACK_FLOAT_128(0xbff2a01a01a01a01, 0xa01a01a01a01a01a), /*  7 */
    PACK_FLOAT_128(0x3fec71de3a556c73, 0x38faac1c88e50017), /*  9 */
    PACK_FLOAT_128(0xbfe5ae64567f544e, 0x38fe747e4b837dc7), /* 11 */
    PACK_FLOAT_128(0x3fde6124613a86d0, 0x97ca38331d23af68), /* 13 */
    PACK_FLOAT_128(0xbfd6ae7f3e733b81, 0xf11d8656b0ee8cb0), /* 15 */
    PACK_FLOAT_128(0x3fce952c77030ad4, 0xa6b2605197771b00)  /* 17 */
};

static float128 cos_arr[COS_ARR_SIZE] =
{
    PACK_FLOAT_128(0x3fff000000000000, 0x0000000000000000), /*  0 */
    PACK_FLOAT_128(0xbffe000000000000, 0x0000000000000000), /*  2 */
    PACK_FLOAT_128(0x3ffa555555555555, 0x5555555555555555), /*  4 */
    PACK_FLOAT_128(0xbff56c16c16c16c1, 0x6c16c16c16c16c17), /*  6 */
    PACK_FLOAT_128(0x3fefa01a01a01a01, 0xa01a01a01a01a01a), /*  8 */
    PACK_FLOAT_128(0xbfe927e4fb7789f5, 0xc72ef016d3ea6679), /* 10 */
    PACK_FLOAT_128(0x3fe21eed8eff8d89, 0x7b544da987acfe85), /* 12 */
    PACK_FLOAT_128(0xbfda93974a8c07c9, 0xd20badf145dfa3e5), /* 14 */
    PACK_FLOAT_128(0x3fd2ae7f3e733b81, 0xf11d8656b0ee8cb0)  /* 16 */
};

extern float128 OddPoly (float128 x, float128 *arr, unsigned n, float_status_t &status);

/* 0 <= x <= pi/4 */
BX_CPP_INLINE float128 poly_sin(float128 x, float_status_t &status)
{
    //                 3     5     7     9     11     13     15
    //                x     x     x     x     x      x      x
    // sin (x) ~ x - --- + --- - --- + --- - ---- + ---- - ---- =
    //                3!    5!    7!    9!    11!    13!    15!
    //
    //                 2     4     6     8     10     12     14
    //                x     x     x     x     x      x      x
    //   = x * [ 1 - --- + --- - --- + --- - ---- + ---- - ---- ] =
    //                3!    5!    7!    9!    11!    13!    15!
    //
    //           3                          3
    //          --       4k                --        4k+2
    //   p(x) = >  C  * x   > 0     q(x) = >  C   * x     < 0
    //          --  2k                     --  2k+1
    //          k=0                        k=0
    //
    //                          2
    //   sin(x) ~ x * [ p(x) + x * q(x) ]
    //

    return OddPoly(x, sin_arr, SIN_ARR_SIZE, status);
}

extern float128 EvenPoly(float128 x, float128 *arr, unsigned n, float_status_t &status);

/* 0 <= x <= pi/4 */
BX_CPP_INLINE float128 poly_cos(float128 x, float_status_t &status)
{
    //                 2     4     6     8     10     12     14
    //                x     x     x     x     x      x      x
    // cos (x) ~ 1 - --- + --- - --- + --- - ---- + ---- - ----
    //                2!    4!    6!    8!    10!    12!    14!
    //
    //           3                          3
    //          --       4k                --        4k+2
    //   p(x) = >  C  * x   > 0     q(x) = >  C   * x     < 0
    //          --  2k                     --  2k+1
    //          k=0                        k=0
    //
    //                      2
    //   cos(x) ~ [ p(x) + x * q(x) ]
    //

    return EvenPoly(x, cos_arr, COS_ARR_SIZE, status);
}

BX_CPP_INLINE void sincos_invalid(floatx80 *sin_a, floatx80 *cos_a, floatx80 a)
{
    if (sin_a) *sin_a = a;
    if (cos_a) *cos_a = a;
}

BX_CPP_INLINE void sincos_tiny_argument(floatx80 *sin_a, floatx80 *cos_a, floatx80 a)
{
    if (sin_a) *sin_a = a;
    if (cos_a) *cos_a = floatx80_one;
}

static floatx80 sincos_approximation(int neg, float128 r, Bit64u quotient, float_status_t &status)
{
    if (quotient & 0x1) {
        r = poly_cos(r, status);
        neg = 0;
    } else  {
        r = poly_sin(r, status);
    }

    floatx80 result = float128_to_floatx80(r, status);
    if (quotient & 0x2) 
        neg = ! neg;

    if (neg) 
        floatx80_chs(result);

    return result;
}

// =================================================
// FSINCOS               Compute sin(x) and cos(x)
// =================================================

//
// Uses the following identities:
// ----------------------------------------------------------
//
//  sin(-x) = -sin(x)
//  cos(-x) =  cos(x)
//
//  sin(x+y) = sin(x)*cos(y)+cos(x)*sin(y)
//  cos(x+y) = sin(x)*sin(y)+cos(x)*cos(y)
//
//  sin(x+ pi/2)  =  cos(x)
//  sin(x+ pi)    = -sin(x)
//  sin(x+3pi/2)  = -cos(x)
//  sin(x+2pi)    =  sin(x)
//

int fsincos(floatx80 a, floatx80 *sin_a, floatx80 *cos_a, float_status_t &status)
{
    Bit64u aSig0, aSig1 = 0;
    Bit32s aExp, zExp, expDiff;
    int aSign, zSign;
    int q = 0;

    // handle unsupported extended double-precision floating encodings
    if (floatx80_is_unsupported(a)) 
    {
        goto invalid;
    }

    aSig0 = extractFloatx80Frac(a);
    aExp = extractFloatx80Exp(a);
    aSign = extractFloatx80Sign(a);
     
    /* invalid argument */
    if (aExp == 0x7FFF) {
        if ((Bit64u) (aSig0<<1)) {
            sincos_invalid(sin_a, cos_a, propagateFloatx80NaN(a, status));
            return 0;
        }

    invalid:
        float_raise(status, float_flag_invalid);
        sincos_invalid(sin_a, cos_a, floatx80_default_nan);
        return 0;
    }

    if (aExp == 0) {
        if (aSig0 == 0) {
            sincos_tiny_argument(sin_a, cos_a, a);
            return 0;
        }

        float_raise(status, float_flag_denormal);

        /* handle pseudo denormals */
        if (! (aSig0 & BX_CONST64(0x8000000000000000)))
        {
            float_raise(status, float_flag_inexact);
            if (sin_a)
                float_raise(status, float_flag_underflow);
            sincos_tiny_argument(sin_a, cos_a, a);
            return 0;
        }

        normalizeFloatx80Subnormal(aSig0, &aExp, &aSig0);
    }
    
    zSign = aSign;
    zExp = EXP_BIAS;
    expDiff = aExp - zExp;

    /* argument is out-of-range */
    if (expDiff >= 63) 
        return -1;

    float_raise(status, float_flag_inexact);

    if (expDiff < -1) {    // doesn't require reduction
        if (expDiff <= -68) {
            a = packFloatx80(aSign, aExp, aSig0);
            sincos_tiny_argument(sin_a, cos_a, a);
            return 0;
        }
        zExp = aExp;
    }
    else {
        q = reduce_trig_arg(expDiff, zSign, aSig0, aSig1);
    }

    /* **************************** */
    /* argument reduction completed */
    /* **************************** */

    /* using float128 for approximation */
    float128 r = normalizeRoundAndPackFloat128(0, zExp-0x10, aSig0, aSig1, status);

    if (aSign) q = -q;
    if (sin_a) *sin_a = sincos_approximation(zSign, r,   q, status);
    if (cos_a) *cos_a = sincos_approximation(zSign, r, q+1, status);

    return 0;
}

int fsin(floatx80 &a, float_status_t &status)
{
    return fsincos(a, &a, 0, status);
}

int fcos(floatx80 &a, float_status_t &status)
{
    return fsincos(a, 0, &a, status);
}

// =================================================
// FPTAN                 Compute tan(x)
// =================================================

//
// Uses the following identities:
//
// 1. ----------------------------------------------------------
//
//  sin(-x) = -sin(x)
//  cos(-x) =  cos(x)
//
//  sin(x+y) = sin(x)*cos(y)+cos(x)*sin(y)
//  cos(x+y) = sin(x)*sin(y)+cos(x)*cos(y)
//
//  sin(x+ pi/2)  =  cos(x)
//  sin(x+ pi)    = -sin(x)
//  sin(x+3pi/2)  = -cos(x)
//  sin(x+2pi)    =  sin(x)
//
// 2. ----------------------------------------------------------
//
//           sin(x)
//  tan(x) = ------
//           cos(x)
//

int ftan(floatx80 &a, float_status_t &status)
{
    Bit64u aSig0, aSig1 = 0;
    Bit32s aExp, zExp, expDiff;
    int aSign, zSign; 
    int q = 0;

    // handle unsupported extended double-precision floating encodings
    if (floatx80_is_unsupported(a)) 
    {
        goto invalid;
    }

    aSig0 = extractFloatx80Frac(a);
    aExp = extractFloatx80Exp(a);
    aSign = extractFloatx80Sign(a);
     
    /* invalid argument */
    if (aExp == 0x7FFF) {
        if ((Bit64u) (aSig0<<1))
        {
            a = propagateFloatx80NaN(a, status);
            return 0;
        }

    invalid:
        float_raise(status, float_flag_invalid);
        a = floatx80_default_nan;
        return 0;
    }

    if (aExp == 0) {
        if (aSig0 == 0) return 0;
        float_raise(status, float_flag_denormal);
        /* handle pseudo denormals */
        if (! (aSig0 & BX_CONST64(0x8000000000000000)))
        {
            float_raise(status, float_flag_inexact | float_flag_underflow);
            return 0;
        }
        normalizeFloatx80Subnormal(aSig0, &aExp, &aSig0);
    }
    
    zSign = aSign;
    zExp = EXP_BIAS;
    expDiff = aExp - zExp;

    /* argument is out-of-range */
    if (expDiff >= 63) 
        return -1;

    float_raise(status, float_flag_inexact);

    if (expDiff < -1) {    // doesn't require reduction
        if (expDiff <= -68) {
            a = packFloatx80(aSign, aExp, aSig0);
            return 0;
        }
        zExp = aExp;
    }
    else {
        q = reduce_trig_arg(expDiff, zSign, aSig0, aSig1);
    }

    /* **************************** */
    /* argument reduction completed */
    /* **************************** */

    /* using float128 for approximation */
    float128 r = normalizeRoundAndPackFloat128(0, zExp-0x10, aSig0, aSig1, status);

    float128 sin_r = poly_sin(r, status);
    float128 cos_r = poly_cos(r, status);

    if (q & 0x1) {
        r = float128_div(cos_r, sin_r, status);
        zSign = ! zSign;
    } else {
        r = float128_div(sin_r, cos_r, status);
    }

    a = float128_to_floatx80(r, status);
    if (zSign)
        floatx80_chs(a);

    return 0;
}

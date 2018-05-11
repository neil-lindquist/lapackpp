/*****************************************************************************
  Copyright (c) 2010, Intel Corp.
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of Intel Corporation nor the names of its contributors
      may be used to endorse or promote products derived from this software
      without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
  THE POSSIBILITY OF SUCH DAMAGE.
******************************************************************************
* Contents: Native C interface to LAPACK
* Author: Intel Corporation
* Generated May, 2011
*****************************************************************************/

#ifndef LAPACK_CONFIG_H
#define LAPACK_CONFIG_H

#include <stdlib.h>

#ifndef lapack_int
    #if defined(LAPACK_ILP64)
        #define lapack_int              long long  /* or int64_t */
    #else
        #define lapack_int              int
    #endif
#endif

#ifndef lapack_logical
    #define lapack_logical          lapack_int
#endif

/* f2c, hence MacOS Accelerate, returns double instead of float
 * for sdot, slange, clange, etc. */
#if defined(HAVE_MACOS_ACCELERATE) || defined(HAVE_F2C)
    typedef double lapack_float_return;
#else
    typedef float lapack_float_return;
#endif

/* -----------------------------------------------------------------------------
 * If user defines LAPACK_COMPLEX_CUSTOM, then use the user's definitions for:
 *     lapack_complex_float
 *     lapack_complex_double
 *     lapack_complex_float_real(z)
 *     lapack_complex_float_imag(z)
 *     lapack_complex_double_real(z)
 *     lapack_complex_double_imag(z)
 * Else if user defines LAPACK_COMPLEX_STRUCTURE, then use a struct;
 * Else if user defines LAPACK_COMPLEX_CPP, then use C++ std::complex;
 * Otherwise, the default is to use C99 _Complex.
 */
#ifndef LAPACK_COMPLEX_CUSTOM

#if defined(LAPACK_COMPLEX_STRUCTURE)

typedef struct { float  real, imag; } lapack_complex_float;
typedef struct { double real, imag; } lapack_complex_double;
#define lapack_complex_float  lapack_complex_float
#define lapack_complex_double lapack_complex_double
#define lapack_complex_float_real(z)  ((z).real)
#define lapack_complex_float_imag(z)  ((z).imag)
#define lapack_complex_double_real(z)  ((z).real)
#define lapack_complex_double_imag(z)  ((z).imag)

#elif defined(__cplusplus) && defined(LAPACK_COMPLEX_CPP)

#include <complex>
#define lapack_complex_float std::complex<float>
#define lapack_complex_double std::complex<double>
#define lapack_complex_float_real(z)       ((z).real())
#define lapack_complex_float_imag(z)       ((z).imag())
#define lapack_complex_double_real(z)       ((z).real())
#define lapack_complex_double_imag(z)       ((z).imag())

#else

/* default is LAPACK_COMPLEX_C99 */
#include <complex.h>
#define lapack_complex_float    float _Complex
#define lapack_complex_double   double _Complex
#define lapack_complex_float_real(z)       (creal(z))
#define lapack_complex_float_imag(z)       (cimag(z))
#define lapack_complex_double_real(z)       (creal(z))
#define lapack_complex_double_imag(z)       (cimag(z))

#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

lapack_complex_float lapack_make_complex_float( float re, float im );
lapack_complex_double lapack_make_complex_double( double re, double im );

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* not LAPACK_COMPLEX_CUSTOM */

#ifndef LAPACK_malloc
#define LAPACK_malloc( size )   malloc( size )
#endif

#ifndef LAPACK_free
#define LAPACK_free( p )        free( p )
#endif

#endif /* LAPACK_CONFIG_H */

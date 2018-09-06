#include "lapack.hh"
#include "lapack_fortran.h"

#include <vector>

namespace lapack {

using blas::max;
using blas::min;
using blas::real;

// -----------------------------------------------------------------------------
int64_t sbgvx(
    lapack::Job jobz, lapack::Range range, lapack::Uplo uplo, int64_t n, int64_t ka, int64_t kb,
    float* AB, int64_t ldab,
    float* BB, int64_t ldbb,
    float* Q, int64_t ldq, float vl, float vu, int64_t il, int64_t iu, float abstol,
    int64_t* m,
    float* W,
    float* Z, int64_t ldz,
    int64_t* ifail )
{
    // check for overflow
    if (sizeof(int64_t) > sizeof(lapack_int)) {
        lapack_error_if( std::abs(n) > std::numeric_limits<lapack_int>::max() );
        lapack_error_if( std::abs(ka) > std::numeric_limits<lapack_int>::max() );
        lapack_error_if( std::abs(kb) > std::numeric_limits<lapack_int>::max() );
        lapack_error_if( std::abs(ldab) > std::numeric_limits<lapack_int>::max() );
        lapack_error_if( std::abs(ldbb) > std::numeric_limits<lapack_int>::max() );
        lapack_error_if( std::abs(ldq) > std::numeric_limits<lapack_int>::max() );
        lapack_error_if( std::abs(il) > std::numeric_limits<lapack_int>::max() );
        lapack_error_if( std::abs(iu) > std::numeric_limits<lapack_int>::max() );
        lapack_error_if( std::abs(ldz) > std::numeric_limits<lapack_int>::max() );
    }
    char jobz_ = job2char( jobz );
    char range_ = range2char( range );
    char uplo_ = uplo2char( uplo );
    lapack_int n_ = (lapack_int) n;
    lapack_int ka_ = (lapack_int) ka;
    lapack_int kb_ = (lapack_int) kb;
    lapack_int ldab_ = (lapack_int) ldab;
    lapack_int ldbb_ = (lapack_int) ldbb;
    lapack_int ldq_ = (lapack_int) ldq;
    lapack_int il_ = (lapack_int) il;
    lapack_int iu_ = (lapack_int) iu;
    lapack_int m_ = (lapack_int) *m;
    lapack_int ldz_ = (lapack_int) ldz;
    #ifndef LAPACK_ILP64
        // 32-bit copy
        std::vector< lapack_int > ifail_( n );  // was m; n >= m
        lapack_int* ifail_ptr = &ifail_[0];
    #else
        lapack_int* ifail_ptr = ifail;
    #endif
    lapack_int info_ = 0;

    // allocate workspace
    std::vector< float > work( (7*n) );
    std::vector< lapack_int > iwork( (5*n) );

    LAPACK_ssbgvx(
        &jobz_, &range_, &uplo_, &n_, &ka_, &kb_,
        AB, &ldab_,
        BB, &ldbb_,
        Q, &ldq_, &vl, &vu, &il_, &iu_, &abstol, &m_,
        W,
        Z, &ldz_,
        &work[0],
        &iwork[0],
        ifail_ptr, &info_ );
    if (info_ < 0) {
        throw Error();
    }
    *m = m_;
    #ifndef LAPACK_ILP64
        std::copy( &ifail_[0], &ifail_[m_], ifail );  // was begin to end
    #endif
    return info_;
}

// -----------------------------------------------------------------------------
int64_t sbgvx(
    lapack::Job jobz, lapack::Range range, lapack::Uplo uplo, int64_t n, int64_t ka, int64_t kb,
    double* AB, int64_t ldab,
    double* BB, int64_t ldbb,
    double* Q, int64_t ldq, double vl, double vu, int64_t il, int64_t iu, double abstol,
    int64_t* m,
    double* W,
    double* Z, int64_t ldz,
    int64_t* ifail )
{
    // check for overflow
    if (sizeof(int64_t) > sizeof(lapack_int)) {
        lapack_error_if( std::abs(n) > std::numeric_limits<lapack_int>::max() );
        lapack_error_if( std::abs(ka) > std::numeric_limits<lapack_int>::max() );
        lapack_error_if( std::abs(kb) > std::numeric_limits<lapack_int>::max() );
        lapack_error_if( std::abs(ldab) > std::numeric_limits<lapack_int>::max() );
        lapack_error_if( std::abs(ldbb) > std::numeric_limits<lapack_int>::max() );
        lapack_error_if( std::abs(ldq) > std::numeric_limits<lapack_int>::max() );
        lapack_error_if( std::abs(il) > std::numeric_limits<lapack_int>::max() );
        lapack_error_if( std::abs(iu) > std::numeric_limits<lapack_int>::max() );
        lapack_error_if( std::abs(ldz) > std::numeric_limits<lapack_int>::max() );
    }
    char jobz_ = job2char( jobz );
    char range_ = range2char( range );
    char uplo_ = uplo2char( uplo );
    lapack_int n_ = (lapack_int) n;
    lapack_int ka_ = (lapack_int) ka;
    lapack_int kb_ = (lapack_int) kb;
    lapack_int ldab_ = (lapack_int) ldab;
    lapack_int ldbb_ = (lapack_int) ldbb;
    lapack_int ldq_ = (lapack_int) ldq;
    lapack_int il_ = (lapack_int) il;
    lapack_int iu_ = (lapack_int) iu;
    lapack_int m_ = (lapack_int) *m;
    lapack_int ldz_ = (lapack_int) ldz;
    #ifndef LAPACK_ILP64
        // 32-bit copy
        std::vector< lapack_int > ifail_( n );  // was m; n >= m
        lapack_int* ifail_ptr = &ifail_[0];
    #else
        lapack_int* ifail_ptr = ifail;
    #endif
    lapack_int info_ = 0;

    // allocate workspace
    std::vector< double > work( (7*n) );
    std::vector< lapack_int > iwork( (5*n) );

    LAPACK_dsbgvx(
        &jobz_, &range_, &uplo_, &n_, &ka_, &kb_,
        AB, &ldab_,
        BB, &ldbb_,
        Q, &ldq_, &vl, &vu, &il_, &iu_, &abstol, &m_,
        W,
        Z, &ldz_,
        &work[0],
        &iwork[0],
        ifail_ptr, &info_ );
    if (info_ < 0) {
        throw Error();
    }
    *m = m_;
    #ifndef LAPACK_ILP64
        std::copy( &ifail_[0], &ifail_[m_], ifail );  // was begin to end
    #endif
    return info_;
}

}  // namespace lapack

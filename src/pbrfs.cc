#include "lapack.hh"
#include "lapack_fortran.h"

#include <vector>

namespace lapack {

using blas::max;
using blas::min;
using blas::real;

// -----------------------------------------------------------------------------
/// @ingroup pbsv_computational
int64_t pbrfs(
    lapack::Uplo uplo, int64_t n, int64_t kd, int64_t nrhs,
    float const* AB, int64_t ldab,
    float const* AFB, int64_t ldafb,
    float const* B, int64_t ldb,
    float* X, int64_t ldx,
    float* ferr,
    float* berr )
{
    // check for overflow
    if (sizeof(int64_t) > sizeof(blas_int)) {
        throw_if_( std::abs(n) > std::numeric_limits<blas_int>::max() );
        throw_if_( std::abs(kd) > std::numeric_limits<blas_int>::max() );
        throw_if_( std::abs(nrhs) > std::numeric_limits<blas_int>::max() );
        throw_if_( std::abs(ldab) > std::numeric_limits<blas_int>::max() );
        throw_if_( std::abs(ldafb) > std::numeric_limits<blas_int>::max() );
        throw_if_( std::abs(ldb) > std::numeric_limits<blas_int>::max() );
        throw_if_( std::abs(ldx) > std::numeric_limits<blas_int>::max() );
    }
    char uplo_ = uplo2char( uplo );
    blas_int n_ = (blas_int) n;
    blas_int kd_ = (blas_int) kd;
    blas_int nrhs_ = (blas_int) nrhs;
    blas_int ldab_ = (blas_int) ldab;
    blas_int ldafb_ = (blas_int) ldafb;
    blas_int ldb_ = (blas_int) ldb;
    blas_int ldx_ = (blas_int) ldx;
    blas_int info_ = 0;

    // allocate workspace
    std::vector< float > work( (3*n) );
    std::vector< blas_int > iwork( (n) );

    LAPACK_spbrfs( &uplo_, &n_, &kd_, &nrhs_, AB, &ldab_, AFB, &ldafb_, B, &ldb_, X, &ldx_, ferr, berr, &work[0], &iwork[0], &info_ );
    if (info_ < 0) {
        throw Error();
    }
    return info_;
}

// -----------------------------------------------------------------------------
/// @ingroup pbsv_computational
int64_t pbrfs(
    lapack::Uplo uplo, int64_t n, int64_t kd, int64_t nrhs,
    double const* AB, int64_t ldab,
    double const* AFB, int64_t ldafb,
    double const* B, int64_t ldb,
    double* X, int64_t ldx,
    double* ferr,
    double* berr )
{
    // check for overflow
    if (sizeof(int64_t) > sizeof(blas_int)) {
        throw_if_( std::abs(n) > std::numeric_limits<blas_int>::max() );
        throw_if_( std::abs(kd) > std::numeric_limits<blas_int>::max() );
        throw_if_( std::abs(nrhs) > std::numeric_limits<blas_int>::max() );
        throw_if_( std::abs(ldab) > std::numeric_limits<blas_int>::max() );
        throw_if_( std::abs(ldafb) > std::numeric_limits<blas_int>::max() );
        throw_if_( std::abs(ldb) > std::numeric_limits<blas_int>::max() );
        throw_if_( std::abs(ldx) > std::numeric_limits<blas_int>::max() );
    }
    char uplo_ = uplo2char( uplo );
    blas_int n_ = (blas_int) n;
    blas_int kd_ = (blas_int) kd;
    blas_int nrhs_ = (blas_int) nrhs;
    blas_int ldab_ = (blas_int) ldab;
    blas_int ldafb_ = (blas_int) ldafb;
    blas_int ldb_ = (blas_int) ldb;
    blas_int ldx_ = (blas_int) ldx;
    blas_int info_ = 0;

    // allocate workspace
    std::vector< double > work( (3*n) );
    std::vector< blas_int > iwork( (n) );

    LAPACK_dpbrfs( &uplo_, &n_, &kd_, &nrhs_, AB, &ldab_, AFB, &ldafb_, B, &ldb_, X, &ldx_, ferr, berr, &work[0], &iwork[0], &info_ );
    if (info_ < 0) {
        throw Error();
    }
    return info_;
}

// -----------------------------------------------------------------------------
/// @ingroup pbsv_computational
int64_t pbrfs(
    lapack::Uplo uplo, int64_t n, int64_t kd, int64_t nrhs,
    std::complex<float> const* AB, int64_t ldab,
    std::complex<float> const* AFB, int64_t ldafb,
    std::complex<float> const* B, int64_t ldb,
    std::complex<float>* X, int64_t ldx,
    float* ferr,
    float* berr )
{
    // check for overflow
    if (sizeof(int64_t) > sizeof(blas_int)) {
        throw_if_( std::abs(n) > std::numeric_limits<blas_int>::max() );
        throw_if_( std::abs(kd) > std::numeric_limits<blas_int>::max() );
        throw_if_( std::abs(nrhs) > std::numeric_limits<blas_int>::max() );
        throw_if_( std::abs(ldab) > std::numeric_limits<blas_int>::max() );
        throw_if_( std::abs(ldafb) > std::numeric_limits<blas_int>::max() );
        throw_if_( std::abs(ldb) > std::numeric_limits<blas_int>::max() );
        throw_if_( std::abs(ldx) > std::numeric_limits<blas_int>::max() );
    }
    char uplo_ = uplo2char( uplo );
    blas_int n_ = (blas_int) n;
    blas_int kd_ = (blas_int) kd;
    blas_int nrhs_ = (blas_int) nrhs;
    blas_int ldab_ = (blas_int) ldab;
    blas_int ldafb_ = (blas_int) ldafb;
    blas_int ldb_ = (blas_int) ldb;
    blas_int ldx_ = (blas_int) ldx;
    blas_int info_ = 0;

    // allocate workspace
    std::vector< std::complex<float> > work( (2*n) );
    std::vector< float > rwork( (n) );

    LAPACK_cpbrfs( &uplo_, &n_, &kd_, &nrhs_, AB, &ldab_, AFB, &ldafb_, B, &ldb_, X, &ldx_, ferr, berr, &work[0], &rwork[0], &info_ );
    if (info_ < 0) {
        throw Error();
    }
    return info_;
}

// -----------------------------------------------------------------------------
/// Improves the computed solution to a system of linear
/// equations when the coefficient matrix is Hermitian positive definite
/// and banded, and provides error bounds and backward error estimates
/// for the solution.
///
/// Overloaded versions are available for
/// `float`, `double`, `std::complex<float>`, and `std::complex<double>`.
///
/// @param[in] uplo
///     - lapack::Uplo::Upper: Upper triangle of A is stored;
///     - lapack::Uplo::Lower: Lower triangle of A is stored.
///
/// @param[in] n
///     The order of the matrix A. n >= 0.
///
/// @param[in] kd
///     - If uplo = Upper, the number of superdiagonals of the matrix A;
///     - if uplo = Lower, the number of subdiagonals.
///     - kd >= 0.
///
/// @param[in] nrhs
///     The number of right hand sides, i.e., the number of columns
///     of the matrices B and X. nrhs >= 0.
///
/// @param[in] AB
///     The n-by-n band matrix AB, stored in an ldab-by-n array.
///     The upper or lower triangle of the Hermitian band matrix A,
///     stored in the first kd+1 rows of the array. The j-th column
///     of A is stored in the j-th column of the array AB as follows:
///     - if uplo = Upper, AB(kd+1+i-j,j) = A(i,j) for max(1,j-kd) <= i <= j;
///     - if uplo = Lower, AB(1+i-j,j) = A(i,j) for j <= i <= min(n,j+kd).
///
/// @param[in] ldab
///     The leading dimension of the array AB. ldab >= kd+1.
///
/// @param[in] AFB
///     The n-by-n band matrix AFB, stored in an ldafb-by-n array.
///     The triangular factor U or L from the Cholesky factorization
///     \f$ A = U^H U \f$ or \f$ A = L L^H \f$ of the band matrix A as computed by
///     `lapack::pbtrf`, in the same storage format as A (see AB).
///
/// @param[in] ldafb
///     The leading dimension of the array AFB. ldafb >= kd+1.
///
/// @param[in] B
///     The n-by-nrhs matrix B, stored in an ldb-by-nrhs array.
///     The right hand side matrix B.
///
/// @param[in] ldb
///     The leading dimension of the array B. ldb >= max(1,n).
///
/// @param[in,out] X
///     The n-by-nrhs matrix X, stored in an ldx-by-nrhs array.
///     On entry, the solution matrix X, as computed by `lapack::pbtrs`.
///     On exit, the improved solution matrix X.
///
/// @param[in] ldx
///     The leading dimension of the array X. ldx >= max(1,n).
///
/// @param[out] ferr
///     The vector ferr of length nrhs.
///     The estimated forward error bound for each solution vector
///     X(j) (the j-th column of the solution matrix X).
///     If XTRUE is the true solution corresponding to X(j), ferr(j)
///     is an estimated upper bound for the magnitude of the largest
///     element in (X(j) - XTRUE) divided by the magnitude of the
///     largest element in X(j). The estimate is as reliable as
///     the estimate for RCOND, and is almost always a slight
///     overestimate of the true error.
///
/// @param[out] berr
///     The vector berr of length nrhs.
///     The componentwise relative backward error of each solution
///     vector X(j) (i.e., the smallest relative change in
///     any element of A or B that makes X(j) an exact solution).
///
/// @retval = 0: successful exit
///
/// @ingroup pbsv_computational
int64_t pbrfs(
    lapack::Uplo uplo, int64_t n, int64_t kd, int64_t nrhs,
    std::complex<double> const* AB, int64_t ldab,
    std::complex<double> const* AFB, int64_t ldafb,
    std::complex<double> const* B, int64_t ldb,
    std::complex<double>* X, int64_t ldx,
    double* ferr,
    double* berr )
{
    // check for overflow
    if (sizeof(int64_t) > sizeof(blas_int)) {
        throw_if_( std::abs(n) > std::numeric_limits<blas_int>::max() );
        throw_if_( std::abs(kd) > std::numeric_limits<blas_int>::max() );
        throw_if_( std::abs(nrhs) > std::numeric_limits<blas_int>::max() );
        throw_if_( std::abs(ldab) > std::numeric_limits<blas_int>::max() );
        throw_if_( std::abs(ldafb) > std::numeric_limits<blas_int>::max() );
        throw_if_( std::abs(ldb) > std::numeric_limits<blas_int>::max() );
        throw_if_( std::abs(ldx) > std::numeric_limits<blas_int>::max() );
    }
    char uplo_ = uplo2char( uplo );
    blas_int n_ = (blas_int) n;
    blas_int kd_ = (blas_int) kd;
    blas_int nrhs_ = (blas_int) nrhs;
    blas_int ldab_ = (blas_int) ldab;
    blas_int ldafb_ = (blas_int) ldafb;
    blas_int ldb_ = (blas_int) ldb;
    blas_int ldx_ = (blas_int) ldx;
    blas_int info_ = 0;

    // allocate workspace
    std::vector< std::complex<double> > work( (2*n) );
    std::vector< double > rwork( (n) );

    LAPACK_zpbrfs( &uplo_, &n_, &kd_, &nrhs_, AB, &ldab_, AFB, &ldafb_, B, &ldb_, X, &ldx_, ferr, berr, &work[0], &rwork[0], &info_ );
    if (info_ < 0) {
        throw Error();
    }
    return info_;
}

}  // namespace lapack

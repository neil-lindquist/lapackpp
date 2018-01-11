#include "test.hh"
#include "lapack.hh"
#include "lapack_flops.hh"
#include "print_matrix.hh"
#include "error.hh"

#include <vector>

// -----------------------------------------------------------------------------
// simple overloaded wrappers around LAPACKE
static lapack_int LAPACKE_hpcon(
    char uplo, lapack_int n, float* AP, lapack_int* ipiv, float anorm, float* rcond )
{
    return LAPACKE_sspcon( LAPACK_COL_MAJOR, uplo, n, AP, ipiv, anorm, rcond );
}

static lapack_int LAPACKE_hpcon(
    char uplo, lapack_int n, double* AP, lapack_int* ipiv, double anorm, double* rcond )
{
    return LAPACKE_dspcon( LAPACK_COL_MAJOR, uplo, n, AP, ipiv, anorm, rcond );
}

static lapack_int LAPACKE_hpcon(
    char uplo, lapack_int n, std::complex<float>* AP, lapack_int* ipiv, float anorm, float* rcond )
{
    return LAPACKE_chpcon( LAPACK_COL_MAJOR, uplo, n, AP, ipiv, anorm, rcond );
}

static lapack_int LAPACKE_hpcon(
    char uplo, lapack_int n, std::complex<double>* AP, lapack_int* ipiv, double anorm, double* rcond )
{
    return LAPACKE_zhpcon( LAPACK_COL_MAJOR, uplo, n, AP, ipiv, anorm, rcond );
}

// -----------------------------------------------------------------------------
template< typename scalar_t >
void test_hpcon_work( Params& params, bool run )
{
    using namespace libtest;
    using namespace blas;
    typedef typename traits< scalar_t >::real_t real_t;
    typedef long long lld;

    // get & mark input values
    lapack::Uplo uplo = params.uplo.value();
    int64_t n = params.dim.n();
    // int64_t align = params.align.value();

    // mark non-standard output values
    params.ref_time.value();
    // params.ref_gflops.value();
    // params.gflops.value();

    if (! run)
        return;

    // ---------- setup
    real_t anorm;
    real_t rcond_tst;
    real_t rcond_ref;
    size_t size_AP = (size_t) (n*(n+1)/2);
    size_t size_ipiv = (size_t) (n);

    std::vector< scalar_t > AP( size_AP );
    std::vector< int64_t > ipiv_tst( size_ipiv );
    std::vector< lapack_int > ipiv_ref( size_ipiv );

    int64_t idist = 1;
    int64_t iseed[4] = { 0, 1, 2, 3 };
    lapack::larnv( idist, iseed, AP.size(), &AP[0] );

    // initialize anorm as the norm of the matrix
    anorm = lapack::lanhp( lapack::Norm::One, uplo, n, &AP[0] );

    // initialize ipiv_tst and ipiv_ref
    int64_t info_trf = lapack::hptrf( uplo, n, &AP[0], &ipiv_tst[0] );
    if (info_trf != 0) {
        fprintf( stderr, "lapack::hetrf returned error %lld\n", (lld) info_trf );
    }
    std::copy( ipiv_tst.begin(), ipiv_tst.end(), ipiv_ref.begin() );

    // ---------- run test
    libtest::flush_cache( params.cache.value() );
    double time = get_wtime();
    int64_t info_tst = lapack::hpcon( uplo, n, &AP[0], &ipiv_tst[0], anorm, &rcond_tst );
    time = get_wtime() - time;
    if (info_tst != 0) {
        fprintf( stderr, "lapack::hpcon returned error %lld\n", (lld) info_tst );
    }

    params.time.value() = time;
    // double gflop = lapack::Gflop< scalar_t >::hpcon( n );
    // params.gflops.value() = gflop / time;

    if (params.ref.value() == 'y' || params.check.value() == 'y') {
        // ---------- run reference
        libtest::flush_cache( params.cache.value() );
        time = get_wtime();
        int64_t info_ref = LAPACKE_hpcon( uplo2char(uplo), n, &AP[0], &ipiv_ref[0], anorm, &rcond_ref );
        time = get_wtime() - time;
        if (info_ref != 0) {
            fprintf( stderr, "LAPACKE_hpcon returned error %lld\n", (lld) info_ref );
        }

        params.ref_time.value() = time;
        // params.ref_gflops.value() = gflop / time;

        // ---------- check error compared to reference
        real_t error = 0;
        real_t eps = std::numeric_limits< real_t >::epsilon();
        if (info_tst != info_ref) {
            error = 1;
        }
        error += std::abs( rcond_tst - rcond_ref );
        params.error.value() = error;
        params.okay.value() = (error < 3*eps);  // expect lapackpp == lapacke
    }
}

// -----------------------------------------------------------------------------
void test_hpcon( Params& params, bool run )
{
    switch (params.datatype.value()) {
        case libtest::DataType::Integer:
            throw std::exception();
            break;

        case libtest::DataType::Single:
            test_hpcon_work< float >( params, run );
            break;

        case libtest::DataType::Double:
            test_hpcon_work< double >( params, run );
            break;

        case libtest::DataType::SingleComplex:
            test_hpcon_work< std::complex<float> >( params, run );
            break;

        case libtest::DataType::DoubleComplex:
            test_hpcon_work< std::complex<double> >( params, run );
            break;
    }
}

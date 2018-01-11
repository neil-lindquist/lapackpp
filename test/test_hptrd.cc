#include "test.hh"
#include "lapack.hh"
#include "lapack_flops.hh"
#include "print_matrix.hh"
#include "error.hh"

#include <vector>

// -----------------------------------------------------------------------------
// simple overloaded wrappers around LAPACKE
static lapack_int LAPACKE_hptrd(
    char uplo, lapack_int n, float* AP, float* D, float* E, float* tau )
{
    return LAPACKE_ssptrd( LAPACK_COL_MAJOR, uplo, n, AP, D, E, tau );
}

static lapack_int LAPACKE_hptrd(
    char uplo, lapack_int n, double* AP, double* D, double* E, double* tau )
{
    return LAPACKE_dsptrd( LAPACK_COL_MAJOR, uplo, n, AP, D, E, tau );
}

static lapack_int LAPACKE_hptrd(
    char uplo, lapack_int n, std::complex<float>* AP, float* D, float* E, std::complex<float>* tau )
{
    return LAPACKE_chptrd( LAPACK_COL_MAJOR, uplo, n, AP, D, E, tau );
}

static lapack_int LAPACKE_hptrd(
    char uplo, lapack_int n, std::complex<double>* AP, double* D, double* E, std::complex<double>* tau )
{
    return LAPACKE_zhptrd( LAPACK_COL_MAJOR, uplo, n, AP, D, E, tau );
}

// -----------------------------------------------------------------------------
template< typename scalar_t >
void test_hptrd_work( Params& params, bool run )
{
    using namespace libtest;
    using namespace blas;
    typedef typename traits< scalar_t >::real_t real_t;
    typedef long long lld;

    // get & mark input values
    lapack::Uplo uplo = params.uplo.value();
    int64_t n = params.dim.n();

    // mark non-standard output values
    params.ref_time.value();
    // params.ref_gflops.value();
    // params.gflops.value();

    if (! run)
        return;

    // ---------- setup
    size_t size_AP = (size_t) (n*(n+1)/2);
    size_t size_D = (size_t) (n);
    size_t size_E = (size_t) (n-1);
    size_t size_tau = (size_t) (n-1);

    std::vector< scalar_t > AP_tst( size_AP );
    std::vector< scalar_t > AP_ref( size_AP );
    std::vector< real_t > D_tst( size_D );
    std::vector< real_t > D_ref( size_D );
    std::vector< real_t > E_tst( size_E );
    std::vector< real_t > E_ref( size_E );
    std::vector< scalar_t > tau_tst( size_tau );
    std::vector< scalar_t > tau_ref( size_tau );

    int64_t idist = 1;
    int64_t iseed[4] = { 0, 1, 2, 3 };
    lapack::larnv( idist, iseed, AP_tst.size(), &AP_tst[0] );
    AP_ref = AP_tst;

    // ---------- run test
    libtest::flush_cache( params.cache.value() );
    double time = get_wtime();
    int64_t info_tst = lapack::hptrd( uplo, n, &AP_tst[0], &D_tst[0], &E_tst[0], &tau_tst[0] );
    time = get_wtime() - time;
    if (info_tst != 0) {
        fprintf( stderr, "lapack::hptrd returned error %lld\n", (lld) info_tst );
    }

    params.time.value() = time;
    // double gflop = lapack::Gflop< scalar_t >::hptrd( n );
    // params.gflops.value() = gflop / time;

    if (params.ref.value() == 'y' || params.check.value() == 'y') {
        // ---------- run reference
        libtest::flush_cache( params.cache.value() );
        time = get_wtime();
        int64_t info_ref = LAPACKE_hptrd( uplo2char(uplo), n, &AP_ref[0], &D_ref[0], &E_ref[0], &tau_ref[0] );
        time = get_wtime() - time;
        if (info_ref != 0) {
            fprintf( stderr, "LAPACKE_hptrd returned error %lld\n", (lld) info_ref );
        }

        params.ref_time.value() = time;
        // params.ref_gflops.value() = gflop / time;

        // ---------- check error compared to reference
        real_t error = 0;
        if (info_tst != info_ref) {
            error = 1;
        }
        error += abs_error( AP_tst, AP_ref );
        error += abs_error( D_tst, D_ref );
        error += abs_error( E_tst, E_ref );
        error += abs_error( tau_tst, tau_ref );
        params.error.value() = error;
        params.okay.value() = (error == 0);  // expect lapackpp == lapacke
    }
}

// -----------------------------------------------------------------------------
void test_hptrd( Params& params, bool run )
{
    switch (params.datatype.value()) {
        case libtest::DataType::Integer:
            throw std::exception();
            break;

        case libtest::DataType::Single:
            test_hptrd_work< float >( params, run );
            break;

        case libtest::DataType::Double:
            test_hptrd_work< double >( params, run );
            break;

        case libtest::DataType::SingleComplex:
            test_hptrd_work< std::complex<float> >( params, run );
            break;

        case libtest::DataType::DoubleComplex:
            test_hptrd_work< std::complex<double> >( params, run );
            break;
    }
}

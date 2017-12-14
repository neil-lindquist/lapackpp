#include "test.hh"
#include "lapack.hh"
#include "lapack_flops.hh"
#include "print_matrix.hh"
#include "error.hh"

#include <vector>
#include <omp.h>

// -----------------------------------------------------------------------------
// simple overloaded wrappers around LAPACKE
static lapack_int LAPACKE_pbtrf(
    char uplo, lapack_int n, lapack_int kd, float* AB, lapack_int ldab )
{
    return LAPACKE_spbtrf( LAPACK_COL_MAJOR, uplo, n, kd, AB, ldab );
}

static lapack_int LAPACKE_pbtrf(
    char uplo, lapack_int n, lapack_int kd, double* AB, lapack_int ldab )
{
    return LAPACKE_dpbtrf( LAPACK_COL_MAJOR, uplo, n, kd, AB, ldab );
}

static lapack_int LAPACKE_pbtrf(
    char uplo, lapack_int n, lapack_int kd, std::complex<float>* AB, lapack_int ldab )
{
    return LAPACKE_cpbtrf( LAPACK_COL_MAJOR, uplo, n, kd, AB, ldab );
}

static lapack_int LAPACKE_pbtrf(
    char uplo, lapack_int n, lapack_int kd, std::complex<double>* AB, lapack_int ldab )
{
    return LAPACKE_zpbtrf( LAPACK_COL_MAJOR, uplo, n, kd, AB, ldab );
}

// -----------------------------------------------------------------------------
template< typename scalar_t >
void test_pbtrf_work( Params& params, bool run )
{
    using namespace blas;
    typedef typename traits< scalar_t >::real_t real_t;
    typedef long long lld;

    // get & mark input values
    lapack::Uplo uplo = params.uplo.value();
    int64_t n = params.dim.n();
    int64_t kd = params.kd.value();
    int64_t align = params.align.value();

    // mark non-standard output values
    params.ref_time.value();
    //params.ref_gflops.value();
    //params.gflops.value();

    if (! run)
        return;

    // ---------- setup
    int64_t ldab = roundup( kd+1, align );
    size_t size_AB = (size_t) ldab * n;

    std::vector< scalar_t > AB_tst( size_AB );
    std::vector< scalar_t > AB_ref( size_AB );

    int64_t idist = 1;
    int64_t iseed[4] = { 0, 1, 2, 3 };
    lapack::larnv( idist, iseed, AB_tst.size(), &AB_tst[0] );

    // diagonally dominant -> positive definite
    if (uplo == lapack::Uplo::Upper) {
        for (int64_t j = 0; j < n; ++j) {
            AB_tst[ kd + j*ldab ] += n;
        }
     }
    else { // lower
        for (int64_t j = 0; j < n; ++j) {
             AB_tst[ j*ldab ] += n;
        }
    }

    AB_ref = AB_tst;

    // ---------- run test
    libtest::flush_cache( params.cache.value() );
    double time = omp_get_wtime();
    int64_t info_tst = lapack::pbtrf( uplo, n, kd, &AB_tst[0], ldab );
    time = omp_get_wtime() - time;
    if (info_tst != 0) {
        fprintf( stderr, "lapack::pbtrf returned error %lld\n", (lld) info_tst );
    }

    params.time.value() = time;
    //double gflop = lapack::Gflop< scalar_t >::pbtrf( n, kd );
    //params.gflops.value() = gflop / time;

    if (params.ref.value() == 'y' || params.check.value() == 'y') {
        // ---------- run reference
        libtest::flush_cache( params.cache.value() );
        time = omp_get_wtime();
        int64_t info_ref = LAPACKE_pbtrf( uplo2char(uplo), n, kd, &AB_ref[0], ldab );
        time = omp_get_wtime() - time;
        if (info_ref != 0) {
            fprintf( stderr, "LAPACKE_pbtrf returned error %lld\n", (lld) info_ref );
        }

        params.ref_time.value() = time;
        //params.ref_gflops.value() = gflop / time;

        // ---------- check error compared to reference
        real_t error = 0;
        if (info_tst != info_ref) {
            error = 1;
        }
        error += abs_error( AB_tst, AB_ref );
        params.error.value() = error;
        params.okay.value() = (error == 0);  // expect lapackpp == lapacke
    }
}

// -----------------------------------------------------------------------------
void test_pbtrf( Params& params, bool run )
{
    switch (params.datatype.value()) {
        case libtest::DataType::Integer:
            throw std::exception();
            break;

        case libtest::DataType::Single:
            test_pbtrf_work< float >( params, run );
            break;

        case libtest::DataType::Double:
            test_pbtrf_work< double >( params, run );
            break;

        case libtest::DataType::SingleComplex:
            test_pbtrf_work< std::complex<float> >( params, run );
            break;

        case libtest::DataType::DoubleComplex:
            test_pbtrf_work< std::complex<double> >( params, run );
            break;
    }
}
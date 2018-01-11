#include "test.hh"
#include "lapack.hh"
#include "lapack_flops.hh"
#include "print_matrix.hh"
#include "error.hh"

#include <vector>

// -----------------------------------------------------------------------------
// simple overloaded wrappers around LAPACKE
static lapack_int LAPACKE_ggrqf(
    lapack_int m, lapack_int p, lapack_int n, float* A, lapack_int lda, float* taua, float* B, lapack_int ldb, float* taub )
{
    return LAPACKE_sggrqf( LAPACK_COL_MAJOR, m, p, n, A, lda, taua, B, ldb, taub );
}

static lapack_int LAPACKE_ggrqf(
    lapack_int m, lapack_int p, lapack_int n, double* A, lapack_int lda, double* taua, double* B, lapack_int ldb, double* taub )
{
    return LAPACKE_dggrqf( LAPACK_COL_MAJOR, m, p, n, A, lda, taua, B, ldb, taub );
}

static lapack_int LAPACKE_ggrqf(
    lapack_int m, lapack_int p, lapack_int n, std::complex<float>* A, lapack_int lda, std::complex<float>* taua, std::complex<float>* B, lapack_int ldb, std::complex<float>* taub )
{
    return LAPACKE_cggrqf( LAPACK_COL_MAJOR, m, p, n, A, lda, taua, B, ldb, taub );
}

static lapack_int LAPACKE_ggrqf(
    lapack_int m, lapack_int p, lapack_int n, std::complex<double>* A, lapack_int lda, std::complex<double>* taua, std::complex<double>* B, lapack_int ldb, std::complex<double>* taub )
{
    return LAPACKE_zggrqf( LAPACK_COL_MAJOR, m, p, n, A, lda, taua, B, ldb, taub );
}

// -----------------------------------------------------------------------------
template< typename scalar_t >
void test_ggrqf_work( Params& params, bool run )
{
    using namespace libtest;
    using namespace blas;
    typedef typename traits< scalar_t >::real_t real_t;
    typedef long long lld;

    // get & mark input values
    int64_t m = params.dim.m();
    int64_t p = params.dim.k();  // TODO Using k as a stand-in for p
    // int64_t p = params.p.value();
    int64_t n = params.dim.n();
    int64_t align = params.align.value();

    // mark non-standard output values
    params.ref_time.value();
    // params.ref_gflops.value();
    // params.gflops.value();

    if (! run)
        return;

    // ---------- setup
    int64_t lda = roundup( max( 1, m ), align );
    int64_t ldb = roundup( max( 1, p ), align );
    size_t size_A = (size_t) lda * n;
    size_t size_taua = (size_t) (min(m,n));
    size_t size_B = (size_t) ldb * n;
    size_t size_taub = (size_t) (min(p,n));

    std::vector< scalar_t > A_tst( size_A );
    std::vector< scalar_t > A_ref( size_A );
    std::vector< scalar_t > taua_tst( size_taua );
    std::vector< scalar_t > taua_ref( size_taua );
    std::vector< scalar_t > B_tst( size_B );
    std::vector< scalar_t > B_ref( size_B );
    std::vector< scalar_t > taub_tst( size_taub );
    std::vector< scalar_t > taub_ref( size_taub );

    int64_t idist = 1;
    int64_t iseed[4] = { 0, 1, 2, 3 };
    lapack::larnv( idist, iseed, A_tst.size(), &A_tst[0] );
    lapack::larnv( idist, iseed, B_tst.size(), &B_tst[0] );
    A_ref = A_tst;
    B_ref = B_tst;

    // ---------- run test
    libtest::flush_cache( params.cache.value() );
    double time = get_wtime();
    int64_t info_tst = lapack::ggrqf( m, p, n, &A_tst[0], lda, &taua_tst[0], &B_tst[0], ldb, &taub_tst[0] );
    time = get_wtime() - time;
    if (info_tst != 0) {
        fprintf( stderr, "lapack::ggrqf returned error %lld\n", (lld) info_tst );
    }

    params.time.value() = time;
    // double gflop = lapack::Gflop< scalar_t >::ggrqf( m, n );
    // params.gflops.value() = gflop / time;

    if (params.ref.value() == 'y' || params.check.value() == 'y') {
        // ---------- run reference
        libtest::flush_cache( params.cache.value() );
        time = get_wtime();
        int64_t info_ref = LAPACKE_ggrqf( m, p, n, &A_ref[0], lda, &taua_ref[0], &B_ref[0], ldb, &taub_ref[0] );
        time = get_wtime() - time;
        if (info_ref != 0) {
            fprintf( stderr, "LAPACKE_ggrqf returned error %lld\n", (lld) info_ref );
        }

        params.ref_time.value() = time;
        // params.ref_gflops.value() = gflop / time;

        // ---------- check error compared to reference
        real_t error = 0;
        if (info_tst != info_ref) {
            error = 1;
        }
        error += abs_error( A_tst, A_ref );
        error += abs_error( taua_tst, taua_ref );
        error += abs_error( B_tst, B_ref );
        error += abs_error( taub_tst, taub_ref );
        params.error.value() = error;
        params.okay.value() = (error == 0);  // expect lapackpp == lapacke
    }
}

// -----------------------------------------------------------------------------
void test_ggrqf( Params& params, bool run )
{
    switch (params.datatype.value()) {
        case libtest::DataType::Integer:
            throw std::exception();
            break;

        case libtest::DataType::Single:
            test_ggrqf_work< float >( params, run );
            break;

        case libtest::DataType::Double:
            test_ggrqf_work< double >( params, run );
            break;

        case libtest::DataType::SingleComplex:
            test_ggrqf_work< std::complex<float> >( params, run );
            break;

        case libtest::DataType::DoubleComplex:
            test_ggrqf_work< std::complex<double> >( params, run );
            break;
    }
}

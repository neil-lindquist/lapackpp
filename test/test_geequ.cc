#include "test.hh"
#include "lapack.hh"
#include "lapack_flops.hh"
#include "print_matrix.hh"
#include "error.hh"
#include "lapacke_wrappers.hh"

#include <vector>

// -----------------------------------------------------------------------------
template< typename scalar_t >
void test_geequ_work( Params& params, bool run )
{
    using namespace libtest;
    using namespace blas;
    using real_t = blas::real_type< scalar_t >;
    typedef long long lld;

    // get & mark input values
    int64_t m = params.dim.m();
    int64_t n = params.dim.n();
    int64_t align = params.align();
    params.matrix.mark();

    // mark non-standard output values
    params.ref_time();
    //params.ref_gflops();
    //params.gflops();

    if (! run)
        return;

    // ---------- setup
    int64_t lda = roundup( max( 1, m ), align );
    real_t rowcnd_tst = 0;
    real_t rowcnd_ref = 0;
    real_t colcnd_tst = 0;
    real_t colcnd_ref = 0;
    real_t amax_tst;
    real_t amax_ref;
    size_t size_A = (size_t) lda * n;
    size_t size_R = (size_t) (m);
    size_t size_C = (size_t) (n);

    std::vector< scalar_t > A( size_A );
    std::vector< real_t > R_tst( size_R );
    std::vector< real_t > R_ref( size_R );
    std::vector< real_t > C_tst( size_C );
    std::vector< real_t > C_ref( size_C );

    lapack::generate_matrix( params.matrix, m, n, &A[0], lda );

    // ---------- run test
    libtest::flush_cache( params.cache() );
    double time = get_wtime();
    int64_t info_tst = lapack::geequ( m, n, &A[0], lda, &R_tst[0], &C_tst[0], &rowcnd_tst, &colcnd_tst, &amax_tst );
    time = get_wtime() - time;
    if (info_tst != 0) {
        fprintf( stderr, "lapack::geequ returned error %lld\n", (lld) info_tst );
    }

    params.time() = time;
    //double gflop = lapack::Gflop< scalar_t >::geequ( m, n );
    //params.gflops() = gflop / time;

    if (params.ref() == 'y' || params.check() == 'y') {
        // ---------- run reference
        libtest::flush_cache( params.cache() );
        time = get_wtime();
        int64_t info_ref = LAPACKE_geequ( m, n, &A[0], lda, &R_ref[0], &C_ref[0], &rowcnd_ref, &colcnd_ref, &amax_ref );
        time = get_wtime() - time;
        if (info_ref != 0) {
            fprintf( stderr, "LAPACKE_geequ returned error %lld\n", (lld) info_ref );
        }

        params.ref_time() = time;
        //params.ref_gflops() = gflop / time;

        // ---------- check error compared to reference
        real_t error = 0;
        if (info_tst != info_ref) {
            error = 1;
        }
        error += abs_error( R_tst, R_ref );
        error += abs_error( C_tst, C_ref );
        error += std::abs( rowcnd_tst - rowcnd_ref );
        error += std::abs( colcnd_tst - colcnd_ref );
        error += std::abs( amax_tst - amax_ref );
        params.error() = error;
        params.okay() = (error == 0);  // expect lapackpp == lapacke
    }
}

// -----------------------------------------------------------------------------
void test_geequ( Params& params, bool run )
{
    switch (params.datatype()) {
        case libtest::DataType::Integer:
            throw std::exception();
            break;

        case libtest::DataType::Single:
            test_geequ_work< float >( params, run );
            break;

        case libtest::DataType::Double:
            test_geequ_work< double >( params, run );
            break;

        case libtest::DataType::SingleComplex:
            test_geequ_work< std::complex<float> >( params, run );
            break;

        case libtest::DataType::DoubleComplex:
            test_geequ_work< std::complex<double> >( params, run );
            break;
    }
}

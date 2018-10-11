#include "test.hh"
#include "lapack.hh"
#include "lapack_flops.hh"
#include "print_matrix.hh"
#include "error.hh"
#include "lapacke_wrappers.hh"

#include <vector>

// -----------------------------------------------------------------------------
template< typename scalar_t >
void test_lacpy_work( Params& params, bool run )
{
    using namespace libtest;
    using namespace blas;
    using real_t = blas::real_type< scalar_t >;
    typedef long long lld;

    // get & mark input values
    lapack::MatrixType matrixtype = params.matrixtype();
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
    int64_t ldb = roundup( max( 1, m ), align );
    size_t size_A = (size_t) lda * n;
    size_t size_B = (size_t) ldb * n;

    std::vector< scalar_t > A( size_A );
    std::vector< scalar_t > B_tst( size_B );
    std::vector< scalar_t > B_ref( size_B );

    lapack::generate_matrix( params.matrix, m, n, &A[0],     lda );
    lapack::generate_matrix( params.matrix, m, n, &B_tst[0], ldb );
    B_ref = B_tst;

    // ---------- run test
    libtest::flush_cache( params.cache() );
    double time = get_wtime();
    lapack::lacpy( matrixtype, m, n, &A[0], lda, &B_tst[0], ldb );
    time = get_wtime() - time;

    params.time() = time;
    //double gflop = lapack::Gflop< scalar_t >::lacpy( m, n );
    //params.gflops() = gflop / time;

    if (params.ref() == 'y' || params.check() == 'y') {
        // ---------- run reference
        libtest::flush_cache( params.cache() );
        time = get_wtime();
        int64_t info_ref = LAPACKE_lacpy( matrixtype2char(matrixtype), m, n, &A[0], lda, &B_ref[0], ldb );
        time = get_wtime() - time;
        if (info_ref != 0) {
            fprintf( stderr, "LAPACKE_lacpy returned error %lld\n", (lld) info_ref );
        }

        params.ref_time() = time;
        //params.ref_gflops() = gflop / time;

        // ---------- check error compared to reference
        real_t error = 0;
        error += abs_error( B_tst, B_ref );
        params.error() = error;
        params.okay() = (error == 0);  // expect lapackpp == lapacke
    }
}

// -----------------------------------------------------------------------------
void test_lacpy( Params& params, bool run )
{
    switch (params.datatype()) {
        case libtest::DataType::Integer:
            throw std::exception();
            break;

        case libtest::DataType::Single:
            test_lacpy_work< float >( params, run );
            break;

        case libtest::DataType::Double:
            test_lacpy_work< double >( params, run );
            break;

        case libtest::DataType::SingleComplex:
            test_lacpy_work< std::complex<float> >( params, run );
            break;

        case libtest::DataType::DoubleComplex:
            test_lacpy_work< std::complex<double> >( params, run );
            break;
    }
}

#include "test.hh"
#include "lapack.hh"
#include "lapack_flops.hh"
#include "print_matrix.hh"
#include "error.hh"
#include "lapacke_wrappers.hh"

#include <vector>

// -----------------------------------------------------------------------------
template< typename scalar_t >
void test_pbsv_work( Params& params, bool run )
{
    using namespace libtest;
    using namespace blas;
    using real_t = blas::real_type< scalar_t >;
    typedef long long lld;

    // get & mark input values
    lapack::Uplo uplo = params.uplo();
    int64_t n = params.dim.n();
    int64_t kd = params.kd();
    int64_t nrhs = params.nrhs();
    int64_t align = params.align();

    // mark non-standard output values
    params.ref_time();
    //params.ref_gflops();
    //params.gflops();

    if (! run)
        return;

    // ---------- setup
    int64_t ldab = roundup( kd+1, align );
    int64_t ldb = roundup( max( 1, n ), align );
    size_t size_AB = (size_t) ldab * n;
    size_t size_B = (size_t) ldb * nrhs;

    std::vector< scalar_t > AB_tst( size_AB );
    std::vector< scalar_t > AB_ref( size_AB );
    std::vector< scalar_t > B_tst( size_B );
    std::vector< scalar_t > B_ref( size_B );

    int64_t idist = 1;
    int64_t iseed[4] = { 0, 1, 2, 3 };
    lapack::larnv( idist, iseed, AB_tst.size(), &AB_tst[0] );
    lapack::larnv( idist, iseed, B_tst.size(), &B_tst[0] );

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
    B_ref = B_tst;

    // ---------- run test
    libtest::flush_cache( params.cache() );
    double time = get_wtime();
    int64_t info_tst = lapack::pbsv( uplo, n, kd, nrhs, &AB_tst[0], ldab, &B_tst[0], ldb );
    time = get_wtime() - time;
    if (info_tst != 0) {
        fprintf( stderr, "lapack::pbsv returned error %lld\n", (lld) info_tst );
    }

    params.time() = time;
    //double gflop = lapack::Gflop< scalar_t >::pbsv( n, kd, nrhs );
    //params.gflops() = gflop / time;

    if (params.ref() == 'y' || params.check() == 'y') {
        // ---------- run reference
        libtest::flush_cache( params.cache() );
        time = get_wtime();
        int64_t info_ref = LAPACKE_pbsv( uplo2char(uplo), n, kd, nrhs, &AB_ref[0], ldab, &B_ref[0], ldb );
        time = get_wtime() - time;
        if (info_ref != 0) {
            fprintf( stderr, "LAPACKE_pbsv returned error %lld\n", (lld) info_ref );
        }

        params.ref_time() = time;
        //params.ref_gflops() = gflop / time;

        // ---------- check error compared to reference
        real_t error = 0;
        if (info_tst != info_ref) {
            error = 1;
        }
        error += abs_error( AB_tst, AB_ref );
        error += abs_error( B_tst, B_ref );
        params.error() = error;
        params.okay() = (error == 0);  // expect lapackpp == lapacke
    }
}

// -----------------------------------------------------------------------------
void test_pbsv( Params& params, bool run )
{
    switch (params.datatype()) {
        case libtest::DataType::Integer:
            throw std::exception();
            break;

        case libtest::DataType::Single:
            test_pbsv_work< float >( params, run );
            break;

        case libtest::DataType::Double:
            test_pbsv_work< double >( params, run );
            break;

        case libtest::DataType::SingleComplex:
            test_pbsv_work< std::complex<float> >( params, run );
            break;

        case libtest::DataType::DoubleComplex:
            test_pbsv_work< std::complex<double> >( params, run );
            break;
    }
}

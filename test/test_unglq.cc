#include "test.hh"
#include "lapack.hh"
#include "lapack_flops.hh"
#include "print_matrix.hh"
#include "error.hh"

#include <vector>
#include <omp.h>

// -----------------------------------------------------------------------------
// simple overloaded wrappers around LAPACKE
static lapack_int LAPACKE_unglq(
    lapack_int m, lapack_int n, lapack_int k, float* A, lapack_int lda, float* tau )
{
    return LAPACKE_sorglq( LAPACK_COL_MAJOR, m, n, k, A, lda, tau );
}

static lapack_int LAPACKE_unglq(
    lapack_int m, lapack_int n, lapack_int k, double* A, lapack_int lda, double* tau )
{
    return LAPACKE_dorglq( LAPACK_COL_MAJOR, m, n, k, A, lda, tau );
}

static lapack_int LAPACKE_unglq(
    lapack_int m, lapack_int n, lapack_int k, std::complex<float>* A, lapack_int lda, std::complex<float>* tau )
{
    return LAPACKE_cunglq( LAPACK_COL_MAJOR, m, n, k, A, lda, tau );
}

static lapack_int LAPACKE_unglq(
    lapack_int m, lapack_int n, lapack_int k, std::complex<double>* A, lapack_int lda, std::complex<double>* tau )
{
    return LAPACKE_zunglq( LAPACK_COL_MAJOR, m, n, k, A, lda, tau );
}

// -----------------------------------------------------------------------------
template< typename scalar_t >
void test_unglq_work( Params& params, bool run )
{
    using namespace blas;
    typedef typename traits< scalar_t >::real_t real_t;
    typedef long long lld;

    // get & mark input values
    int64_t m = params.dim.m();
    int64_t n = params.dim.n();
    int64_t k = params.dim.k();
    int64_t align = params.align.value();

    // mark non-standard output values
    params.ortho.value();
    params.time.value();
    params.gflops.value();
    params.ref_time.value();
    params.ref_gflops.value();
    params.okay.value();

    if (! run)
        return;

    // Check for problems in testing
    if (! ( m <=n && k <= m ) ) {
        printf( "skipping because unglq requires m <= n and k <= m\n" );
        return;
    }

    // ---------- setup
    int64_t lda = roundup( max( 1, m ), align );
    size_t size_A = (size_t) lda * n;
    size_t size_tau = (size_t) (k);

    std::vector< scalar_t > A_tst( size_A );
    std::vector< scalar_t > A_ref( size_A );
    std::vector< scalar_t > A_factorized( size_A );
    std::vector< scalar_t > tau( size_tau );

    int64_t idist = 1;
    int64_t iseed[4] = { 0, 1, 2, 3 };
    lapack::larnv( idist, iseed, A_tst.size(), &A_tst[0] );
    lapack::larnv( idist, iseed, tau.size(), &tau[0] );
    A_ref = A_tst;

    // ---------- factor matrix
    int64_t info_lqf = lapack::gelqf( m, n, &A_tst[0], lda, &tau[0] );
    if (info_lqf != 0) {
        fprintf( stderr, "lapack::unlqf returned error %lld\n", (lld) info_lqf );
    }
    // ---------- save matrix after factorization as the reference matrix
    A_factorized = A_tst;

    // ---------- run test
    libtest::flush_cache( params.cache.value() );
    double time = omp_get_wtime();
    int64_t info_tst = lapack::unglq( m, n, k, &A_tst[0], lda, &tau[0] );
    time = omp_get_wtime() - time;
    if (info_tst != 0) {
        fprintf( stderr, "lapack::unglq returned error %lld\n", (lld) info_tst );
    }

    params.time.value() = time;
    double gflop = lapack::Gflop< scalar_t >::unglq( m, n, k );
    params.gflops.value() = gflop / time;

    if (params.check.value() == 'y') {
        // ---------- check error
        // comparing to ref. solution doesn't work
        // Following lapack/TESTING/LIN/zlqt02.f
        real_t eps = std::numeric_limits< real_t >::epsilon();
        real_t tol = params.tol.value();

        int64_t ldq = lda;
        int64_t ldl = lda;
        std::vector< scalar_t > Q( lda * n );
        std::vector< scalar_t > L( ldl * m );

        // Copy the first k rows of the factorization to the array Q
        real_t rogue = -10000000000; // -1D+10
        lapack::laset( lapack::MatrixType::General, m, n, rogue, rogue, &Q[0], ldq );
        lapack::lacpy( lapack::MatrixType::Upper, k, n-1, &A_factorized[lda], lda, &Q[ldq], ldq );

        // Generate the first n columns of the matrix Q
        int64_t info_unglq = lapack::unglq( m, n, k, &Q[0], ldq, &tau[0] );
        if (info_unglq != 0) {
            fprintf( stderr, "lapack::unglq returned error %lld\n", (lld) info_unglq );
        }

        // Copy L(1:k,1:m)
        lapack::laset( lapack::MatrixType::General, k, m, 0.0, 0.0, &L[0], ldl );
        lapack::lacpy( lapack::MatrixType::Lower, k, m, &A_factorized[0], lda, &L[0], ldl );

        // Compute L - A*Q'  =   L(1:k,1:m) - A(1:k,1:n) * Q(1:m,1:n)'
        blas::gemm( Layout::ColMajor, Op::NoTrans, Op::ConjTrans, k, m, n,
                    -1.0, &A_ref[0], lda, &Q[0], ldq, 1.0, &L[0], ldl );

        // Compute norm( L - A*Q' ) / ( N * norm(A) * EPS ) .
        real_t Anorm = lapack::lange( lapack::Norm::One, k, n, &A_ref[0], lda );
        real_t resid1 = lapack::lange( lapack::Norm::One, k, m, &L[0], ldl );
        real_t error1 = 0;
        if (Anorm > 0)
            error1 = resid1 / ( n * Anorm );

        // Compute I - Q'*Q
        lapack::laset( lapack::MatrixType::General, m, m, 0.0, 1.0, &L[0], ldl );
        blas::herk( Layout::ColMajor, Uplo::Upper, Op::NoTrans, m, n, -1.0, &Q[0], ldq, 1.0, &L[0], ldl );

        // Compute norm( I - Q*Q' ) / ( N * EPS )
        real_t resid2 = lapack::lansy( lapack::Norm::One, lapack::Uplo::Upper, m, &L[0], ldl );
        real_t error2 = ( resid2 / n );

        params.error.value() = error1;
        params.ortho.value() = error2;
        params.okay.value() = (error1 < tol*eps) && (error2 < tol*eps);
    }

    if (params.ref.value() == 'y') {
        // ---------- run reference
        libtest::flush_cache( params.cache.value() );
        time = omp_get_wtime();
        int64_t info_ref = LAPACKE_unglq( m, n, k, &A_ref[0], lda, &tau[0] );
        time = omp_get_wtime() - time;
        if (info_ref != 0) {
            fprintf( stderr, "LAPACKE_unglq returned error %lld\n", (lld) info_ref );
        }

        params.ref_time.value() = time;
        params.ref_gflops.value() = gflop / time;
    }
}

// -----------------------------------------------------------------------------
void test_unglq( Params& params, bool run )
{
    switch (params.datatype.value()) {
        case libtest::DataType::Integer:
            throw std::exception();
            break;

        case libtest::DataType::Single:
            test_unglq_work< float >( params, run );
            break;

        case libtest::DataType::Double:
            test_unglq_work< double >( params, run );
            break;

        case libtest::DataType::SingleComplex:
            test_unglq_work< std::complex<float> >( params, run );
            break;

        case libtest::DataType::DoubleComplex:
            test_unglq_work< std::complex<double> >( params, run );
            break;
    }
}
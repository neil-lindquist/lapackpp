// Copyright (c) 2017-2020, University of Tennessee. All rights reserved.
// SPDX-License-Identifier: BSD-3-Clause
// This program is free software: you can redistribute it and/or modify it under
// the terms of the BSD 3-Clause license. See the accompanying LICENSE file.

#include "test.hh"
#include "lapack.hh"
#include "lapack/flops.hh"
#include "print_matrix.hh"
#include "error.hh"
#include "lapacke_wrappers.hh"
#include "blas_wrappers.hh"
#include "scale.hh"

#include <vector>

// -----------------------------------------------------------------------------
template< typename scalar_t >
void test_hpgv_work( Params& params, bool run )
{
    using real_t = blas::real_type< scalar_t >;
    typedef long long lld;

    // Constants
    const scalar_t zero = 0.0;
    const scalar_t one  = 1.0;
    const real_t   eps  = std::numeric_limits< real_t >::epsilon();

    // get & mark input values
    int64_t itype = params.itype();
    lapack::Job jobz = params.jobz();
    lapack::Uplo uplo = params.uplo();
    int64_t n = params.dim.n();
    int64_t align = params.align();
    int64_t verbose = params.verbose();
    real_t tol = params.tol() * eps;

    // mark non-standard output values
    params.ref_time();
    // params.ref_gflops();
    // params.gflops();
    params.error2();

    if (! run)
        return;

    // ---------- setup
    int64_t ldz = (jobz == lapack::Job::Vec
                   ? roundup( blas::max( 1, n ), align )
                   : 1 );
    size_t size_A = (size_t) (n*(n+1)/2);
    size_t size_B = (size_t) (n*(n+1)/2);
    size_t size_Z = (size_t) ldz * n;

    std::vector< scalar_t > Apack_tst( size_A );
    std::vector< scalar_t > Apack_ref( size_A );
    std::vector< scalar_t > Bpack_tst( size_B );
    std::vector< scalar_t > Bpack_ref( size_B );
    std::vector< scalar_t > Z( size_Z );  // eigenvectors
    std::vector< real_t > Lambda_tst( n );
    std::vector< real_t > Lambda_ref( n );

    int64_t idist = 1;
    int64_t iseed[4] = { 0, 1, 2, 3 };
    lapack::larnv( idist, iseed, Apack_tst.size(), &Apack_tst[0] );
    lapack::larnv( idist, iseed, Bpack_tst.size(), &Bpack_tst[0] );
    // diagonally dominant -> positive definite
    if (uplo == lapack::Uplo::Upper) {
        for (int64_t i = 0; i < n; ++i) {
            Bpack_tst[ i + (i+1)*i/2 ] += n;
        }
    }
    else { // lower
        for (int64_t i = 0; i < n; ++i) {
            Bpack_tst[ i + n*i - i*(i+1)/2 ] += n;
        }
    }
    Apack_ref = Apack_tst;
    Bpack_ref = Bpack_tst;

    // ---------- run test
    testsweeper::flush_cache( params.cache() );
    double time = testsweeper::get_wtime();
    int64_t info_tst = lapack::hpgv(
                           itype, jobz, uplo, n,
                           &Apack_tst[0],
                           &Bpack_tst[0],
                           &Lambda_tst[0], &Z[0], ldz );
    time = testsweeper::get_wtime() - time;
    if (info_tst != 0) {
        fprintf( stderr, "lapack::hpgv returned error %lld\n", (lld) info_tst );
    }

    params.time() = time;
    // double gflop = lapack::Gflop< scalar_t >::hpgv( itype, jobz, n );
    // params.gflops() = gflop / time;

    if (verbose >= 2) {
        printf( "Lambda = " );
        print_vector( n, &Lambda_tst[0], 1 );
        if (jobz == lapack::Job::Vec) {
            printf( "Z = " );
            print_matrix( n, n, &Z[0], ldz );
        }
    }

    if (params.check() == 'y' && jobz == lapack::Job::Vec) {
        // ---------- check error
        // Relative backwards error =
        //     type 1: ||A Z - B Z Lambda|| / (n * ||A|| * ||Z||)
        //     type 2: ||A B Z - Z Lambda|| / (n * ||A|| * ||Z||)
        //     type 3: ||B A Z - Z Lambda|| / (n * ||A|| * ||Z||)
        real_t Anorm = lapack::lanhp( lapack::Norm::One, uplo, n, &Apack_ref[0] );
        real_t Znorm = lapack::lange( lapack::Norm::One, n, n, &Z[0], ldz );

        real_t error = 0;
        std::vector< scalar_t > W( size_Z );  // workspace
        int64_t ldw = ldz;
        switch (itype) {
            case 1:
                // W = B Z
                blas::hpmm( uplo, n, n,
                            one,  &Bpack_ref[0],
                                  &Z[0], ldz,
                            zero, &W[0], ldw );
                // W = (B Z) Lambda
                col_scale( n, n, &W[0], ldw, &Lambda_tst[0] );
                // W = A Z - (B Z Lambda)
                blas::hpmm( uplo, n, n,
                            one,  &Apack_ref[0],
                                  &Z[0], ldz,
                            -one, &W[0], ldw );
                error = lapack::lange( lapack::Norm::One, n, n, &W[0], ldw );
                break;

            case 2:
                // W = B Z
                blas::hpmm( uplo, n, n,
                            one,  &Bpack_ref[0],
                                  &Z[0], ldz,
                            zero, &W[0], ldw );
                // Z = Z Lambda
                col_scale( n, n, &Z[0], ldz, &Lambda_tst[0] );
                // Z = A (B Z) - (Z Lambda)
                blas::hpmm( uplo, n, n,
                            one,  &Apack_ref[0],
                                  &W[0], ldw,
                            -one, &Z[0], ldz );
                error = lapack::lange( lapack::Norm::One, n, n, &Z[0], ldz );
                break;

            case 3:
                // W = A Z
                blas::hpmm( uplo, n, n,
                            one,  &Apack_ref[0],
                                  &Z[0], ldz,
                            zero, &W[0], ldw );
                // Z = Z Lambda
                col_scale( n, n, &Z[0], ldz, &Lambda_tst[0] );
                // Z = B (A Z) - (Z Lambda)
                blas::hpmm( uplo, n, n,
                            one,  &Bpack_ref[0],
                                  &W[0], ldw,
                            -one, &Z[0], ldz );
                error = lapack::lange( lapack::Norm::One, n, n, &Z[0], ldz );
                break;
        }
        if (verbose >= 2) {
            printf( "W = " );
            print_matrix( n, n, &W[0], ldw );
        }

        error /= (n * Anorm * Znorm);
        params.error() = error;
        params.okay() = (error < tol);
    }

    if (params.ref() == 'y' || params.check() == 'y') {
        // ---------- run reference
        testsweeper::flush_cache( params.cache() );
        time = testsweeper::get_wtime();
        int64_t info_ref = LAPACKE_hpgv(
                               itype, job2char(jobz), uplo2char(uplo), n,
                               &Apack_ref[0],
                               &Bpack_ref[0],
                               &Lambda_ref[0], &Z[0], ldz );
        time = testsweeper::get_wtime() - time;
        if (info_ref != 0) {
            fprintf( stderr, "LAPACKE_hpgv returned error %lld\n", (lld) info_ref );
        }

        params.ref_time() = time;
        // params.ref_gflops() = gflop / time;

        if (verbose >= 2) {
            printf( "Lambda_ref" );
            print_vector( n, &Lambda_ref[0], 1 );
            if (jobz == lapack::Job::Vec) {
                printf( "Zref" );
                print_matrix( n, n, &Z[0], ldz );
            }
        }

        // ---------- check error compared to reference
        real_t error = rel_error( Lambda_tst, Lambda_ref );
        if (info_tst != info_ref) {
            error = 1;
        }
        params.error2() = error;
        params.okay() = params.okay() && (error < tol);
    }
}

// -----------------------------------------------------------------------------
void test_hpgv( Params& params, bool run )
{
    switch (params.datatype()) {
        case testsweeper::DataType::Integer:
            throw std::exception();
            break;

        case testsweeper::DataType::Single:
            test_hpgv_work< float >( params, run );
            break;

        case testsweeper::DataType::Double:
            test_hpgv_work< double >( params, run );
            break;

        case testsweeper::DataType::SingleComplex:
            test_hpgv_work< std::complex<float> >( params, run );
            break;

        case testsweeper::DataType::DoubleComplex:
            test_hpgv_work< std::complex<double> >( params, run );
            break;
    }
}

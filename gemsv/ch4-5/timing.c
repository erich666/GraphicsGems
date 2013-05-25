/*
// timing.c
//
//
// Performance test of the sugguested ellipsoid generation method.
// The fastest execution time among 100 runs of ellipsoid generation method
// subtracted by the fastest execution time among 100 runs of gettimeofday
// system call is output.
//
// A comparison is made to the IRIS GL sphere library.
//
// Here, depth denote the degree of subdivision.
*/

#include "ellipsoid.h"

#include <gl/gl.h>
#include <gl/sphere.h>

#include <stdio.h>
#include <sys/time.h>
struct timeval t0, t1;
long microsec;

#define max_depth 105
#define max_run   100

struct {
    long init;
    long seq;
    long par;
    long libsphere;
} timing[max_depth];

int main ()
{
    object ellipsoid;
    int depth;
    int run;

    long min_syscall = -1;
    long min_init = -1;
    long min_seq = -1;
    long min_par = -1;
    long min_libsphere_draw = -1;
    long min_libsphere = -1;

    /*
    // Find the fastest execution time among 100 runs
    // of gettimeofday system call.
    */
    for (run = 1; run <= max_run; run++) {
        gettimeofday (&t0);
        gettimeofday (&t1);

        microsec = (t1.tv_sec-t0.tv_sec)*1000000 + (t1.tv_usec-t0.tv_usec);
        if (min_syscall > microsec || min_syscall < 0)
            min_syscall = microsec;
    }

    /*
    // Find the fastest execution time among 100 runs
    // of ellipsoid initialization for each depth.
    */
    for (depth = 1; depth <= max_depth; depth++) {
        for (run = 1; run <= max_run; run++) {
            gettimeofday (&t0);
            ellipsoid_init (depth);
            gettimeofday (&t1);
            ellipsoid_done ();

            microsec = (t1.tv_sec-t0.tv_sec)*1000000 + (t1.tv_usec-t0.tv_usec);
            if (min_init > microsec || min_init < 0)
                min_init = microsec;
        }
        min_init -= min_syscall;
        timing[depth-1].init = min_init;
        min_init = -1;
    }

    /*
    // Find the fastest execution time among 100 runs
    // of ellipsoid generation by ellipsoid_seq() for each depth.
    */
    ellipsoid_init (max_depth);
    for (depth = 1; depth <= max_depth; depth++) {
        for (run = 1; run <= max_run; run++) {
            gettimeofday (&t0);
            ellipsoid_seq (&ellipsoid, depth, 1.0, 2.0, 3.0);
            gettimeofday (&t1);
            ellipsoid_free (&ellipsoid);

            microsec = (t1.tv_sec-t0.tv_sec)*1000000 + (t1.tv_usec-t0.tv_usec);
            if (min_seq > microsec || min_seq < 0)
                min_seq = microsec;
        }
        min_seq -= min_syscall;
        timing[depth-1].seq = min_seq;
        min_seq = -1;
    }

    /*
    // Find the fastest execution time among 100 runs
    // of ellipsoid generation by ellipsoid_par() for each depth.
    */
    ellipsoid_init (max_depth);
    for (depth = 1; depth <= max_depth; depth++) {
        for (run = 1; run <= max_run; run++) {
            gettimeofday (&t0);
            ellipsoid_par (&ellipsoid, depth, 1.0, 2.0, 3.0);
            gettimeofday (&t1);
            ellipsoid_free (&ellipsoid);

            microsec = (t1.tv_sec-t0.tv_sec)*1000000 + (t1.tv_usec-t0.tv_usec);
            if (min_par > microsec || min_par < 0)
                min_par = microsec;
        }
        min_par -= min_syscall;
        timing[depth-1].par = min_par;
        min_par = -1;
    }

    /*
    // Find the fastest execution time among 100 runs
    // of sphere generation by IRIS GL sphere library for each depth.
    */
    foreground ();
    noport ();
    winopen ("");
    sphmode (SPH_TESS, SPH_OCT); /* octahedral subdivision */
    sphmode (SPH_PRIM, SPH_POLY);
    for (depth = 1; depth <= SPH_MAXDEPTH; depth++) {
        static float sphparams[] = { 0.0, 0.0, 0.0, 1.0 };

        /*
        // Find the fastest execution time among 100 runs
        // of sphere drawing by IRIS GL sphere library.
        */
        sphmode (SPH_DEPTH, depth);
        sphdraw (sphparams); /* generation and drawing of the sphere */
        for (run = 1; run <= max_run; run++) {
            finish (); /* block until the geometry pipeline is empty */
            gettimeofday (&t0);
            sphdraw (sphparams); /* drawing only without generation */
            gettimeofday (&t1);

            microsec = (t1.tv_sec-t0.tv_sec)*1000000 + (t1.tv_usec-t0.tv_usec);
            if (min_libsphere_draw > microsec || min_libsphere_draw < 0)
                min_libsphere_draw = microsec;
        }
        sphfree ();
        min_libsphere_draw -= min_syscall;

        /*
        // Find the fastest execution time among 100 runs
        // of sphere generation by IRIS GL sphere library.
        */
        for (run = 1; run <= max_run; run++) {
            sphmode (SPH_DEPTH, depth);
            finish ();
            gettimeofday (&t0);
            sphdraw (sphparams); /* generation and drawing of the sphere */
            gettimeofday (&t1);
            sphfree ();

            microsec = (t1.tv_sec-t0.tv_sec)*1000000 + (t1.tv_usec-t0.tv_usec);
            if (min_libsphere > microsec || min_libsphere < 0)
                min_libsphere = microsec;
        }
        min_libsphere -= min_libsphere_draw + min_syscall;
        timing[depth-1].libsphere = min_libsphere;
        min_libsphere = -1;
    }

    for (depth = 1; depth <= max_depth; depth++) {
        printf ("depth = %3d: nv = %8d, nf = %8d, init = %8d, seq = %8d, par = %8d",
            depth, 4*depth*depth + 2, 8*depth*depth,
            timing[depth-1].init, timing[depth-1].seq, timing[depth-1].par);
        if (depth <= SPH_MAXDEPTH)
            printf (", libsphere = %8d", timing[depth-1].libsphere);
        printf ("\n");
    }

    return 0;
}

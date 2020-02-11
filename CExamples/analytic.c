// SigLib Analytical Signal By Applying A Hilbert Transform
// Copyright (C) 2020 Sigma Numerix Ltd.

// Hilbert transform applied to a real input signal on the imaginary
// output path and applies a corresponding delay to the real
// output. The flow diagram is as follows :
//
//       +--     Delay     ----> R
//     --|
//       +-- Hilbert xform ----> I
//
// Analytic signals give positive output frequencies but no
// negative output frequencies.
//
// The Hilbert Transform uses an N tap FIR filter to phase shift every
// component in a signal by 90 degrees (N is odd ordered).
//
// The defining equations for the Hilbert transform are :
//
//           2        2 ( n * PI )
// h(n) = ______ * sin  (________)     for n = +-1, +-2, . . +- N/2
//        n * PI        (    2   )
//
// and h(0) = 0, for n = 0

// Include files
#include <stdio.h>
#include <siglib.h>                                 // SigLib DSP library
#include <gnuplot_c.h>                              // Gnuplot/C

// Define constants
#define SAMPLE_LENGTH   512
#define FFT_LENGTH      SAMPLE_LENGTH
#define LOG2_FFT_LENGTH 9

#define FILTER_LENGTH   31
#define FILTER_GROUP_DELAY  (FILTER_LENGTH >> 1)    // Filter group delay - Note : C array indexing

// Declare global variables and arrays
static SLData_t         SinePhase;
static SLData_t         *pImagData, *pMagnitude, *pDelay, *pFFTCoeffs, *pTempDelay;
static SLData_t         *pFilterTaps, *pFilterTapsStart, *pFilterState;
static SLArrayIndex_t   FilterIndex;
static SLData_t         *pSrc1, *pSrc2;

void    main(void);


void main(void)
{
    h_GPC_Plot  *h2DPlot;                           // Plot object

    h2DPlot =                                       // Initialize plot
        gpc_init_2d ("Analytic Signal",             // Plot title
                     "Time",                        // X-Axis label
                     "Magnitude",                   // Y-Axis label
                     GPC_AUTO_SCALE,                // Scaling mode
                     GPC_SIGNED,                    // Sign mode
                     GPC_KEY_DISABLE);              // Legend / key mode
    if (h2DPlot == NULL) {
        printf ("\nPlot creation failure.\n");
        exit (1);
    }

    printf ("Hilbert transform filter length   => %d", FILTER_LENGTH);

    pFilterTaps = SUF_VectorArrayAllocate (FILTER_LENGTH);
    pFilterState = SUF_VectorArrayAllocate (FILTER_LENGTH);
    pSrc1 = SUF_VectorArrayAllocate (SAMPLE_LENGTH);
    pSrc2 = SUF_VectorArrayAllocate (SAMPLE_LENGTH);
    pImagData = SUF_VectorArrayAllocate (SAMPLE_LENGTH);
    pMagnitude = SUF_VectorArrayAllocate (SAMPLE_LENGTH);
    pFFTCoeffs = SUF_FftCoefficientAllocate (FFT_LENGTH);
    pDelay = SUF_VectorArrayAllocate (FILTER_GROUP_DELAY);
    pTempDelay = SUF_VectorArrayAllocate (FILTER_GROUP_DELAY);

    pFilterTapsStart = pFilterTaps;
                                                    // Initialise Hilbert transformer coefficients
    SIF_HilbertTransformer (pFilterTaps,            // Pointer to filter coefficients
                            FILTER_LENGTH);         // Filter length
                                                    // Initialise FIR filter for Hilbert transformer
    SIF_Fir (pFilterState,                          // Pointer to filter state array
             &FilterIndex,                          // Pointer to filter index register
             FILTER_LENGTH);                        // Filter length
                                                    // Initialise the delay function
    SIF_FixedDelay (pDelay,                         // Pointer to filter index register
                    SIGLIB_NULL_ARRAY_INDEX_PTR,    // Unused
                    FILTER_GROUP_DELAY);            // Filter length

                                                    // Initialise FFT
    SIF_Fft (pFFTCoeffs,                            // Pointer to FFT coefficients
             SIGLIB_NULL_ARRAY_INDEX_PTR,           // Pointer to bit reverse address table - NOT USED
             FFT_LENGTH);                           // FFT length

    SinePhase = SIGLIB_ZERO;
    SDA_SignalGenerate (pSrc1,                      // Pointer to destination array
                        SIGLIB_SINE_WAVE,           // Signal type - Sine wave
                        SIGLIB_ONE,                 // Signal peak level
                        SIGLIB_FILL,                // Fill (overwrite) or add to existing array contents
                        0.05,                       // Signal frequency
                        SIGLIB_ZERO,                // D.C. Offset
                        SIGLIB_ZERO,                // Unused
                        SIGLIB_ZERO,                // Signal end value - Unused
                        &SinePhase,                 // Signal phase - maintained across array boundaries
                        SIGLIB_NULL_DATA_PTR,       // Unused
                        SAMPLE_LENGTH);             // Output dataset length

    SinePhase = SIGLIB_ZERO;
    SDA_SignalGenerate (pSrc2,                      // Pointer to destination array
                        SIGLIB_SINE_WAVE,           // Signal type - Sine wave
                        SIGLIB_ONE,                 // Signal peak level
                        SIGLIB_FILL,                // Fill (overwrite) or add to existing array contents
                        0.005,                      // Signal frequency
                        SIGLIB_ZERO,                // D.C. Offset
                        SIGLIB_ZERO,                // Unused
                        SIGLIB_ZERO,                // Signal end value - Unused
                        &SinePhase,                 // Signal phase - maintained across array boundaries
                        SIGLIB_NULL_DATA_PTR,       // Unused
                        SAMPLE_LENGTH);             // Output dataset length

    SDA_Multiply2 (pSrc1, pSrc2, pSrc1, SAMPLE_LENGTH);

    gpc_plot_2d (h2DPlot,                           // Graph handle
                 pSrc1,                             // Dataset
                 SAMPLE_LENGTH,                     // Dataset length
                 "Source signal",                   // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 (double)(SAMPLE_LENGTH - 1),       // Maximum X value
                 "lines",                           // Graph type
                 "magenta",                         // Colour
                 GPC_NEW);                          // New graph
    printf ("\nSource signal\nPlease hit <Carriage Return> to continue . . ."); getchar ();

    SDA_Copy (pSrc1,                                // Pointer to source array
              pSrc2,                                // Pointer to destination array
              SAMPLE_LENGTH);                       // Dataset length

                                                    // Perform real FFT
    SDA_Rfft (pSrc2,                                // Pointer to real array
              pImagData,                            // Pointer to imaginary array
              pFFTCoeffs,                           // Pointer to FFT coefficients
              SIGLIB_NULL_ARRAY_INDEX_PTR,          // Pointer to bit reverse address table - NOT USED
              FFT_LENGTH,                           // FFT length
              LOG2_FFT_LENGTH);                     // log2 FFT length

                                                    // Calculate real magnitude from complex
    SDA_Magnitude (pSrc2,                           // Pointer to real source array
                   pImagData,                       // Pointer to imaginary source array
                   pMagnitude,                      // Pointer to magnitude destination array
                   SAMPLE_LENGTH);                  // Dataset length

    SDA_10Log10 (pMagnitude,                        // Pointer to source array
                 pMagnitude,                        // Pointer to destination array
                 SAMPLE_LENGTH);                    // Dataset length
    gpc_plot_2d (h2DPlot,                           // Graph handle
                 pMagnitude,                        // Dataset
                 SAMPLE_LENGTH,                     // Dataset length
                 "Spectrum of source signal",       // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 (double)(SAMPLE_LENGTH - 1),       // Maximum X value
                 "lines",                           // Graph type
                 "magenta",                         // Colour
                 GPC_NEW);                          // New graph
    printf ("\nSpectrum of source signal\nPlease hit <Carriage Return> to continue . . ."); getchar ();

                                                    // Apply Hilbert transformerer
    SDA_Fir (pSrc1,                                 // Input array to be filtered
             pSrc2,                                 // Filtered output array
             pFilterState,                          // Pointer to filter state array
             pFilterTaps,                           // Pointer to filter coefficients
             &FilterIndex,                          // Pointer to filter index register
             FILTER_LENGTH,                         // Filter length
             SAMPLE_LENGTH);                        // Dataset length

                                                    // Delay real component to generate an analytical signal
    SDA_ShortFixedDelay (pSrc1,                     // Pointer to source array
                         pSrc1,                     // Pointer to destination array
                         pDelay,                    // Pointer to temporary delayed array
                         pTempDelay,                // Temporary destination array pointer
                         FILTER_GROUP_DELAY,        // Sample delay count
                         SAMPLE_LENGTH);            // Dataset length

    gpc_plot_2d (h2DPlot,                           // Graph handle
                 pSrc1,                             // Dataset
                 SAMPLE_LENGTH,                     // Dataset length
                 "Real output",                     // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 (double)(SAMPLE_LENGTH - 1),       // Maximum X value
                 "lines",                           // Graph type
                 "magenta",                         // Colour
                 GPC_NEW);                          // New graph
    gpc_plot_2d (h2DPlot,                           // Graph handle
                 pSrc2,                             // Dataset
                 SAMPLE_LENGTH,                     // Dataset length
                 "Imaginary output",                // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 (double)(SAMPLE_LENGTH - 1),       // Maximum X value
                 "lines",                           // Graph type
                 "blue",                            // Colour
                 GPC_ADD);                          // New graph
    printf ("\nAnalytic Signal\nPlease hit <Carriage Return> to continue . . ."); getchar ();

                                                    // Perform real FFT
    SDA_Cfft (pSrc1,                                // Pointer to real array
              pSrc2,                                // Pointer to imaginary array
              pFFTCoeffs,                           // Pointer to FFT coefficients
              SIGLIB_NULL_ARRAY_INDEX_PTR,          // Pointer to bit reverse address table - NOT USED
              FFT_LENGTH,                           // FFT length
              LOG2_FFT_LENGTH);                     // log2 FFT length

                                                    // Calculate real magnitude from complex
    SDA_Magnitude (pSrc1,                           // Pointer to real source array
                   pSrc2,                           // Pointer to imaginary source array
                   pMagnitude,                      // Pointer to magnitude destination array
                   SAMPLE_LENGTH);                  // Dataset length

    SDA_10Log10 (pMagnitude,                        // Pointer to source array
                 pMagnitude,                        // Pointer to destination array
                 SAMPLE_LENGTH);                    // Dataset length
    gpc_plot_2d (h2DPlot,                           // Graph handle
                 pMagnitude,                        // Dataset
                 SAMPLE_LENGTH,                     // Dataset length
                 "Spectrum of analytic signal",     // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 (double)(SAMPLE_LENGTH - 1),       // Maximum X value
                 "lines",                           // Graph type
                 "magenta",                         // Colour
                 GPC_NEW);                          // New graph
    printf ("\nSpectrum of analytic signal\n");

    printf ("\nHit <Carriage Return> to continue ....\n"); getchar (); // Wait for <Carriage Return>
    gpc_close (h2DPlot);

    SUF_MemoryFree (pFilterTaps);                   // Free memory
    SUF_MemoryFree (pFilterState);
    SUF_MemoryFree (pSrc1);
    SUF_MemoryFree (pSrc2);
    SUF_MemoryFree (pImagData);
    SUF_MemoryFree (pMagnitude);
    SUF_MemoryFree (pDelay);
    SUF_MemoryFree (pFFTCoeffs);
    SUF_MemoryFree (pTempDelay);

}


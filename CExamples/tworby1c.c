// SigLib Two Real FFTs By One Complex FFT Example
// This process calculates two real FFTs using a single complex FFT
// by utilizing the symetrical properties of the FFT process by which
// real inputs produce even real and odd imaginary outputs and imaginary
// inputs produce odd real and even imaginary outputs.
// Copyright (C) 2020 Sigma Numerix Ltd.

// Include files
#include <stdio.h>
#include <siglib.h>                                 // SigLib DSP library
#include <gnuplot_c.h>                              // Gnuplot/C

// Define constants
#define FFT_LENGTH      512
#define HALF_FFT_LENGTH 256
#define LOG2_FFT_LENGTH 9

// Declare global variables and arrays
static SLData_t     *pRealData1, *pImagData1, *pRealData2, *pImagData2, *pRealDataCopy, *pImagDataCopy;
static SLData_t     *rpImagDataCopy1, *rpImagDataCopy2, *pFFTCoeffs;
static SLData_t     *Marker;

static SLData_t     SinePhase;

void main(void)
{
    h_GPC_Plot  *h2DPlot;                           // Plot object

    SLFixData_t     i;

    h2DPlot =                                       // Initialize plot
        gpc_init_2d ("Two Real FFTs By One Complex FFT Example",  // Plot title
                     "Frequency",                   // X-Axis label
                     "Magnitude",                   // Y-Axis label
                     GPC_AUTO_SCALE,                // Scaling mode
                     GPC_SIGNED,                    // Sign mode
                     GPC_KEY_ENABLE);               // Legend / key mode
    if (h2DPlot == NULL) {
        printf ("\nPlot creation failure.\n");
        exit (1);
    }

                                                    // Allocate memory
    pRealData1 = SUF_VectorArrayAllocate (FFT_LENGTH);
    pImagData1 = SUF_VectorArrayAllocate (FFT_LENGTH);

    pRealData2 = SUF_VectorArrayAllocate (FFT_LENGTH);
    pImagData2 = SUF_VectorArrayAllocate (FFT_LENGTH);

    pRealDataCopy = SUF_VectorArrayAllocate (FFT_LENGTH);
    pImagDataCopy = SUF_VectorArrayAllocate (FFT_LENGTH);

    rpImagDataCopy1 = SUF_VectorArrayAllocate (FFT_LENGTH);
    rpImagDataCopy2 = SUF_VectorArrayAllocate (FFT_LENGTH);

    Marker = SUF_VectorArrayAllocate (FFT_LENGTH);
    pFFTCoeffs = SUF_FftCoefficientAllocate (FFT_LENGTH);


    SinePhase = SIGLIB_ZERO;                        // Generate signal 1
    SDA_SignalGenerate (pRealData1,                 // Pointer to destination array
                        SIGLIB_SINE_WAVE,           // Signal type - Sine wave
                        0.9,                        // Signal peak level
                        SIGLIB_FILL,                // Fill (overwrite) or add to existing array contents
                        0.019,                      // Signal frequency
                        SIGLIB_ZERO,                // D.C. Offset
                        SIGLIB_ZERO,                // Unused
                        SIGLIB_ZERO,                // Signal end value - Unused
                        &SinePhase,                 // Signal phase - maintained across array boundaries
                        SIGLIB_NULL_DATA_PTR ,      // Unused
                        FFT_LENGTH);                // Output dataset length

    SinePhase = SIGLIB_ZERO;                        // Generate signal 2
    SDA_SignalGenerate (pRealData2,                 // Pointer to destination array
                        SIGLIB_SINE_WAVE,           // Signal type - Sine wave
                        0.9,                        // Signal peak level
                        SIGLIB_FILL,                // Fill (overwrite) or add to existing array contents
                        0.035,                      // Signal frequency
                        SIGLIB_ZERO,                // D.C. Offset
                        SIGLIB_ZERO,                // Unused
                        SIGLIB_ZERO,                // Signal end value - Unused
                        &SinePhase,                 // Signal phase - maintained across array boundaries
                        SIGLIB_NULL_DATA_PTR ,      // Unused
                        FFT_LENGTH);                // Output dataset length

                                                    // Make duplicates of the data
    SDA_Copy (pRealData1,                           // Pointer to source array
              pRealDataCopy,                        // Pointer to destination array
              FFT_LENGTH);                          // Dataset length
    SDA_Copy (pImagData1,                           // Pointer to source array
              pImagDataCopy,                        // Pointer to destination array
              FFT_LENGTH);                          // Dataset length

                                                    // Generate marker signal
    SDA_SignalGenerate (Marker,                     // Pointer to destination array
                        SIGLIB_IMPULSE,             // Signal type - Impulse function
                        SIGLIB_HALF,                // Signal peak level
                        SIGLIB_FILL,                // Fill (overwrite) or add to existing array contents
                        SIGLIB_ZERO,                // Signal frequency - Unused
                        SIGLIB_ZERO,                // D.C. Offset
                        256.,                       // Delay (samples periods) applied to impulse
                        SIGLIB_ZERO,                // Signal end value - Unused
                        SIGLIB_NULL_DATA_PTR,       // Unused
                        SIGLIB_NULL_DATA_PTR,       // Unused
                        FFT_LENGTH);                // Output dataset length

                                                    // Initialise FFT
    SIF_Fft (pFFTCoeffs,                            // Pointer to FFT coefficients
             SIGLIB_NULL_ARRAY_INDEX_PTR,           // Pointer to bit reverse address table - NOT USED
             FFT_LENGTH);                           // FFT length

                                                    // Perform real FFT
    SDA_Rfft (pRealData1,                           // Pointer to real array
              pImagData1,                           // Pointer to imaginary array
              pFFTCoeffs,                           // Pointer to FFT coefficients
              SIGLIB_NULL_ARRAY_INDEX_PTR,          // Pointer to bit reverse address table - NOT USED
              FFT_LENGTH,                           // FFT length
              LOG2_FFT_LENGTH);                     // log2 FFT length

                                                    // Perform real FFT
    SDA_Rfft (pRealData2,                           // Pointer to real array
              pImagData2,                           // Pointer to imaginary array
              pFFTCoeffs,                           // Pointer to FFT coefficients
              SIGLIB_NULL_ARRAY_INDEX_PTR,          // Pointer to bit reverse address table - NOT USED
              FFT_LENGTH,                           // FFT length
              LOG2_FFT_LENGTH);                     // log2 FFT length

                                                    // Perform complex FFT
    SDA_Cfft (pRealDataCopy,                        // Pointer to real array
              pImagDataCopy,                        // Pointer to imaginary array
              pFFTCoeffs,                           // Pointer to FFT coefficients
              SIGLIB_NULL_ARRAY_INDEX_PTR,          // Pointer to bit reverse address table - NOT USED
              FFT_LENGTH,                           // FFT length
              LOG2_FFT_LENGTH);                     // log2 FFT length

                                                    // Scale arrays for display
    SDA_ComplexScalarDivide (pRealData1,                // Pointer to real source array
                             pImagData1,                // Pointer to imaginary source array
                             ((SLData_t)HALF_FFT_LENGTH),   // Scalar divisor
                             pRealData1,                // Pointer to real destination array
                             pImagData1,                // Pointer to imaginary destination array
                             FFT_LENGTH);               // Dataset lengths

    SDA_ComplexScalarDivide (pRealData2,                // Pointer to real source array
                             pImagData2,                // Pointer to imaginary source array
                             ((SLData_t)HALF_FFT_LENGTH),   // Scalar divisor
                             pRealData2,                // Pointer to real destination array
                             pImagData2,                // Pointer to imaginary destination array
                             FFT_LENGTH);               // Dataset lengths

    SDA_ComplexScalarDivide (pRealDataCopy,             // Pointer to real source array
                             pImagDataCopy,             // Pointer to imaginary source array
                             ((SLData_t)HALF_FFT_LENGTH),   // Scalar divisor
                             pRealDataCopy,             // Pointer to real destination array
                             pImagDataCopy,             // Pointer to imaginary destination array
                             FFT_LENGTH);               // Dataset lengths

                                                    // Extract the two separate pResults
    for (i = 0; i < HALF_FFT_LENGTH; i++) {
        *(rpImagDataCopy1+i) = SIGLIB_HALF * (*(pRealDataCopy+i) + *(pRealDataCopy+FFT_LENGTH-i));
        *(rpImagDataCopy1+HALF_FFT_LENGTH+i) = SIGLIB_HALF * (*(pImagDataCopy+i) - *(pImagDataCopy+FFT_LENGTH-i));

        *(rpImagDataCopy2+i) = SIGLIB_HALF * (*(pImagDataCopy+i) + *(pImagDataCopy+FFT_LENGTH-i));
        *(rpImagDataCopy2+HALF_FFT_LENGTH+i) = SIGLIB_HALF * (*(pRealDataCopy+FFT_LENGTH-i) - *(pRealDataCopy+i));
    }

                                                    // Merge original real and imag. data
    SDA_Copy (pImagData1,                           // Pointer to source array
              pRealData1+HALF_FFT_LENGTH,           // Pointer to destination array
              HALF_FFT_LENGTH);                     // Dataset length
    SDA_Copy (pImagData2,                           // Pointer to source array
              pRealData2+HALF_FFT_LENGTH,           // Pointer to destination array
              HALF_FFT_LENGTH);                     // Dataset length

    gpc_plot_2d (h2DPlot,                           // Graph handle
                 pRealData1,                        // Dataset
                 FFT_LENGTH,                        // Dataset length
                 "pRealData1",                      // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 (double)(FFT_LENGTH - 1),          // Maximum X value
                 "lines",                           // Graph type
                 "blue",                            // Colour
                 GPC_NEW);                          // New graph
    gpc_plot_2d (h2DPlot,                           // Graph handle
                 Marker,                            // Dataset
                 FFT_LENGTH,                        // Dataset length
                 "Marker",                          // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 (double)(FFT_LENGTH - 1),          // Maximum X value
                 "lines",                           // Graph type
                 "red",                             // Colour
                 GPC_ADD);                          // New graph
    gpc_plot_2d (h2DPlot,                           // Graph handle
                 rpImagDataCopy1,                   // Dataset
                 FFT_LENGTH,                        // Dataset length
                 "rpImagDataCopy1",                 // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 (double)(FFT_LENGTH - 1),          // Maximum X value
                 "lines",                           // Graph type
                 "yellow",                          // Colour
                 GPC_ADD);                          // New graph

    SDA_Subtract2 (pRealData1,                      // Pointer to source array 1
                   rpImagDataCopy1,                 // Pointer to source array 2
                   pRealData1,                      // Pointer to destination array
                   FFT_LENGTH);                     // Dataset length
    SDA_Abs (pRealData1,                            // Pointer to source array
             pRealData1,                            // Pointer to destination array
             FFT_LENGTH);                           // Dataset length

    printf ("\nReal and imaginary data set 1\nTotal error = %lf\n", SDA_Sum (pRealData1, FFT_LENGTH));
    printf ("Please hit <Carriage Return> to continue . . ."); getchar ();

    gpc_plot_2d (h2DPlot,                           // Graph handle
                 pRealData2,                        // Dataset
                 FFT_LENGTH,                        // Dataset length
                 "pRealData2",                      // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 (double)(FFT_LENGTH - 1),          // Maximum X value
                 "lines",                           // Graph type
                 "blue",                            // Colour
                 GPC_NEW);                          // New graph
    gpc_plot_2d (h2DPlot,                           // Graph handle
                 Marker,                            // Dataset
                 FFT_LENGTH,                        // Dataset length
                 "Marker",                          // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 (double)(FFT_LENGTH - 1),          // Maximum X value
                 "lines",                           // Graph type
                 "red",                             // Colour
                 GPC_ADD);                          // New graph
    gpc_plot_2d (h2DPlot,                           // Graph handle
                 rpImagDataCopy2,                   // Dataset
                 FFT_LENGTH,                        // Dataset length
                 "rpImagDataCopy2",                 // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 (double)(FFT_LENGTH - 1),          // Maximum X value
                 "lines",                           // Graph type
                 "yellow",                          // Colour
                 GPC_ADD);                          // New graph

    SDA_Subtract2 (pRealData2,                      // Pointer to source array 1
                   rpImagDataCopy2,                 // Pointer to source array 2
                   pRealData2,                      // Pointer to destination array
                   FFT_LENGTH);                     // Dataset length
    SDA_Abs (pRealData2,                            // Pointer to source array
             pRealData2,                            // Pointer to destination array
             FFT_LENGTH);                           // Dataset length

    printf ("\nReal and imaginary data set 2\nTotal error = %lf\n", SDA_Sum (pRealData2, FFT_LENGTH));

    printf ("\nHit <Carriage Return> to continue ....\n"); getchar (); // Wait for <Carriage Return>
    gpc_close (h2DPlot);


    SUF_MemoryFree (pRealData1);                    // Free memory
    SUF_MemoryFree (pImagData1);
    SUF_MemoryFree (pRealData2);
    SUF_MemoryFree (pImagData2);
    SUF_MemoryFree (pRealDataCopy);
    SUF_MemoryFree (pImagDataCopy);
    SUF_MemoryFree (rpImagDataCopy1);
    SUF_MemoryFree (rpImagDataCopy2);
    SUF_MemoryFree (Marker);
    SUF_MemoryFree (pFFTCoeffs);
}


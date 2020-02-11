// SigLib Welch Power Spectrum Estimation Example
// Copyright (C) 2020 Sigma Numerix Ltd.

// Include files
#include <stdio.h>
#include <siglib.h>                                 // SigLib DSP library
#include <gnuplot_c.h>                              // Gnuplot/C

// Define constants
#define FFT_LENGTH                  512             // Length of fast power spectrum FFT
#define LOG2_FFT_LENGTH             9
#define NUMBER_OF_ARRAYS_AVERAGED   4
#define OVERLAP_SIZE                32              // Size of the overlap between successive arrays
#define INPUT_ARRAY_LENGTH          (((FFT_LENGTH - OVERLAP_SIZE) * NUMBER_OF_ARRAYS_AVERAGED) + OVERLAP_SIZE)  // Size of the input array
#define RESULT_LENGTH               ((FFT_LENGTH >> SIGLIB_AI_ONE)+SIGLIB_AI_ONE)       // Note the result length is N/2+1

// Declare global variables and arrays
static SLData_t         *pSrcData, *pDstData;
static SLData_t         *pRealData, *pImagData, *pOverlap, *pWindowCoeffs, *pFFTCoeffs;
static SLArrayIndex_t   OverlapSourceIndex;
static SLData_t         SinePhase;
static SLData_t         InverseFFTSize, InverseNumberOfArraysAveraged;

void main(void)
{
    h_GPC_Plot  *h2DPlot;                           // Plot object

    SLError_t   SigLibErrorCode;

    pSrcData = SUF_VectorArrayAllocate (INPUT_ARRAY_LENGTH);    // Allocate memory
    pDstData = SUF_VectorArrayAllocate (RESULT_LENGTH);
    pRealData = SUF_VectorArrayAllocate (FFT_LENGTH);
    pImagData = SUF_VectorArrayAllocate (FFT_LENGTH);
    pOverlap = SUF_VectorArrayAllocate (OVERLAP_SIZE);
    pWindowCoeffs = SUF_VectorArrayAllocate (FFT_LENGTH);
    pFFTCoeffs = SUF_FftCoefficientAllocate (FFT_LENGTH);

    if ((pRealData == NULL) || (pImagData == NULL) ||
        (pDstData == NULL) || (pWindowCoeffs == NULL) || (pFFTCoeffs == NULL) || (pDstData == NULL)) {

        printf ("\n\nMalloc failed\n\n");
        exit (0);
    }

    h2DPlot =                                       // Initialize plot
        gpc_init_2d ("Welch Power Spectrum Estimation",   // Plot title
                     "Time / Frequency",            // X-Axis label
                     "Magnitude",                   // Y-Axis label
                     GPC_AUTO_SCALE,                // Scaling mode
                     GPC_SIGNED,                    // Sign mode
                     GPC_KEY_ENABLE);               // Legend / key mode
    if (h2DPlot == NULL) {
        printf ("\nPlot creation failure.\n");
        exit (1);
    }

                                                    // Initialise the Welch power spectrum
    SigLibErrorCode =
        SIF_WelchPowerSpectrum (&OverlapSourceIndex,            // Pointer to overlap source array index
                                pWindowCoeffs,                  // Window array pointer
                                SIGLIB_BLACKMAN,                // Window type
                                SIGLIB_ZERO,                    // Window coefficient
                                pFFTCoeffs,                     // Pointer to FFT coefficients
                                SIGLIB_NULL_ARRAY_INDEX_PTR,    // Pointer to bit reverse address table
                                &InverseFFTSize,                // Pointer to the inverse FFT length
                                FFT_LENGTH,                     // FFT length
                                &InverseNumberOfArraysAveraged, // Pointer to the inverse of the number of arrays averaged
                                NUMBER_OF_ARRAYS_AVERAGED);     // Number of arrays averaged
    if (SigLibErrorCode != SIGLIB_NO_ERROR) {
        printf ("SigLib Error Message :%s\n", SUF_StrError (SigLibErrorCode));
        exit (0);
    }

    SinePhase = SIGLIB_ZERO;
    SDA_SignalGenerate (pSrcData,                   // Pointer to destination array
                        SIGLIB_SINE_WAVE,           // Signal type - Sine wave
                        0.9,                        // Signal peak level
                        SIGLIB_FILL,                // Fill (overwrite) or add to existing array contents
                        0.01562,                    // Signal frequency
                        SIGLIB_ZERO,                // D.C. Offset
                        SIGLIB_ZERO,                // Unused
                        SIGLIB_ZERO,                // Signal end value - Unused
                        &SinePhase,                 // Signal phase - maintained across array boundaries
                        SIGLIB_NULL_DATA_PTR ,      // Unused
                        INPUT_ARRAY_LENGTH);        // Output dataset length

    SDA_SignalGenerate (pSrcData,                   // Pointer to destination array
                        SIGLIB_WHITE_NOISE,         // Signal type - random white noise
                        0.3,                        // Signal peak level
                        SIGLIB_ADD,                 // Fill (overwrite) or add to existing array contents
                        SIGLIB_ZERO,                // Signal frequency - Unused
                        SIGLIB_ZERO,                // D.C. Offset
                        SIGLIB_ZERO,                // Unused
                        SIGLIB_ZERO,                // Signal end value - Unused
                        SIGLIB_NULL_DATA_PTR,       // Unused
                        SIGLIB_NULL_DATA_PTR,       // Unused
                        INPUT_ARRAY_LENGTH);        // Output dataset length

    gpc_plot_2d (h2DPlot,                           // Graph handle
                 pSrcData,                          // Dataset
                 FFT_LENGTH,                        // Dataset length
                 "Sine Wave",                       // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 (double)(RESULT_LENGTH - 1),       // Maximum X value
                 "lines",                           // Graph type
                 "blue",                            // Colour
                 GPC_NEW);                          // New graph
    printf ("\nSine Wave\nPlease hit <Carriage Return> to continue . . ."); getchar ();


                                                                // Perform Welch power spectrum calculation
    SDA_WelchRealPowerSpectrum (pSrcData,                       // Pointer to source data
                                pDstData,                       // Pointer to destination data
                                pRealData,                      // Pointer to real internal processing array
                                pImagData,                      // Pointer to imaginary internal processing array
                                pOverlap,                       // Pointer to internal overlap array
                                &OverlapSourceIndex,            // Pointer to overlap source array index
                                OVERLAP_SIZE,                   // Size of overlap between successive partitions
                                pWindowCoeffs,                  // Pointer to window coefficients
                                pFFTCoeffs,                     // Pointer to FFT coefficients
                                SIGLIB_NULL_ARRAY_INDEX_PTR,    // Pointer to bit reverse address table
                                FFT_LENGTH,                     // FFT length
                                LOG2_FFT_LENGTH,                // log2 FFT length
                                InverseFFTSize,                 // Inverse FFT length
                                NUMBER_OF_ARRAYS_AVERAGED,      // Number of arrays averaged
                                InverseNumberOfArraysAveraged,  // Inverse of number of arrays averaged
                                INPUT_ARRAY_LENGTH);            // Source dataset length

    SDA_PositiveOffset (pDstData,                   // Pointer to source array
                        pDstData,                   // Pointer to destination array
                        RESULT_LENGTH);             // Dataset length

    gpc_plot_2d (h2DPlot,                           // Graph handle
                 pDstData,                          // Dataset
                 RESULT_LENGTH,                     // Dataset length
                 "Welch Power Spectrum",            // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 (double)(RESULT_LENGTH - 1),       // Maximum X value
                 "lines",                           // Graph type
                 "blue",                            // Colour
                 GPC_NEW);                          // New graph
    printf ("\nWelch Power Spectrum\n");

    printf ("\nHit <Carriage Return> to continue ....\n"); getchar (); // Wait for <Carriage Return>
    gpc_close (h2DPlot);


    SUF_MemoryFree (pSrcData);                      // Free memory
    SUF_MemoryFree (pDstData);
    SUF_MemoryFree (pRealData);
    SUF_MemoryFree (pImagData);
    SUF_MemoryFree (pOverlap);
    SUF_MemoryFree (pWindowCoeffs);
    SUF_MemoryFree (pFFTCoeffs);
}

